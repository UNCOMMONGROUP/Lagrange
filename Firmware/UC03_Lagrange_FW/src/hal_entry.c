/***********************************************************************************************************************
 * UC03 "Lagrange" — USB CDC-ACM to UART bridge on Renesas RA2L2 (R7FA2L2074CNH)
 *
 * Architecture for sustained 1.5 Mbps operation:
 *   - DTC on SCI1 RX/TX removes the per-byte CPU interrupt (one interrupt per
 *     512-byte chunk on RX, one per packet on TX)
 *   - A 1 ms AGT tick performs an "idle flush": whatever has arrived so far is
 *     forwarded even if the chunk is not full, bounding latency at ~1 ms
 *     (the same idea as the FTDI latency timer)
 *   - RX uses a 512-byte ping-pong pair; USB IN transfers straight from the
 *     filled buffer with no intermediate copy
 *
 * Required FSP stacks (instance and callback names must match this file):
 *   - g_basic0 : USB PCDC (r_usb_pcdc + r_usb_basic), peripheral, Full-Speed
 *   - g_uart0  : UART (r_sci_uart) on SCI1, callback = uart0_cb
 *                + DTC driver for transmission (r_dtc, SCI1 TXI)
 *                + DTC driver for reception    (r_dtc, SCI1 RXI)
 *   - g_timer0 : Timer, Low-Power (r_agt) ch0, periodic 1 ms, callback = agt0_cb
 *
 * Suggested interrupt priorities (Cortex-M23, 0 = highest):
 *   SCI1 RXI = 0 / SCI1 TXI, TEI, ERI = 1 / USBFS INT = 2 / AGT0 = 3
 *
 * Copyright (c) Uncommon Group. SPDX-License-Identifier: MIT
 **********************************************************************************************************************/
#include "hal_data.h"
#include <string.h>

/* Provided by usb_descriptor.c */
extern void usb_descriptor_serial_init(void);

/*======================================================================================================
 * Configuration
 *====================================================================================================*/
#define RX_CHUNK            (512U)                  /* UART RX DTC chunk size (two buffers = 1 KB) */
#define USB_BULK_MPS        (64U)                   /* Full-Speed Bulk max packet size */
#define LINE_CODING_LENGTH  (7U)

#define LED_TX_PIN          (BSP_IO_PORT_01_PIN_12) /* USB to UART activity */
#define LED_RX_PIN          (BSP_IO_PORT_01_PIN_03) /* UART to USB activity */
#define LED_ON              (BSP_IO_LEVEL_LOW)      /* active-low LED; swap if the board is wired the other way */
#define LED_OFF             (BSP_IO_LEVEL_HIGH)
#define LED_HOLD_MS         (30U)                   /* LED hold time after activity [ms] */

/*======================================================================================================
 * State
 *====================================================================================================*/
/* --- UART RX ping-pong buffers, filled by DTC --- */
static uint8_t           g_rx_buf[2][RX_CHUNK];
static volatile uint16_t g_rx_len[2] = {0, 0};      /* 0 = empty, >0 = bytes waiting for USB */
static volatile uint8_t  g_rx_fill_idx = 0;         /* buffer DTC is filling (changed in ISR/critical section) */
static uint8_t           g_rx_send_idx = 0;         /* next buffer to send over USB (main loop only) */
static uint8_t           g_usb_tx_src  = 0;         /* buffer currently in flight (zero-copy source) */

/* --- diagnostic counters --- */
static volatile uint32_t g_rx_overrun_bytes = 0;    /* bytes dropped because USB could not keep up */
static volatile uint32_t g_rx_char_stray    = 0;    /* bytes arriving with no read armed; should stay 0 */

/* --- USB buffers and flags --- */
static uint8_t           g_usb_rx_buf[USB_BULK_MPS];
static volatile bool     g_usb_configured = false;
static volatile bool     g_usb_suspended  = false;
static volatile bool     g_usb_rx_armed   = false;
static volatile bool     g_usb_tx_busy    = false;
static volatile bool     g_uart_tx_busy   = false;

/* --- timer tick and LED state --- */
static volatile bool     g_tick_1ms   = false;
static volatile uint32_t g_led_tx_ms  = 0;
static volatile uint32_t g_led_rx_ms  = 0;

static usb_pcdc_linecoding_t g_line_coding =
{
    .dw_dte_rate   = 115200,
    .b_char_format = 0,
    .b_parity_type = 0,
    .b_data_bits   = 8,
};

/*======================================================================================================
 * UART callback (set g_uart0 "Callback" property to uart0_cb)
 *====================================================================================================*/
void uart0_cb (uart_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_RX_COMPLETE:            /* DTC filled RX_CHUNK: one interrupt per chunk */
        {
            uint8_t cur = g_rx_fill_idx;
            uint8_t nxt = cur ^ 1U;

            if (0U == g_rx_len[nxt])
            {
                /* hand the full buffer to the sender and re-arm on the other one (lossless: still inside the ISR) */
                g_rx_len[cur] = RX_CHUNK;
                g_rx_fill_idx = nxt;
                (void) R_SCI_UART_Read(&g_uart0_ctrl, g_rx_buf[nxt], RX_CHUNK);
            }
            else
            {
                /* both buffers pending: USB is behind, so drop this chunk and re-arm on the same buffer */
                g_rx_overrun_bytes += RX_CHUNK;
                (void) R_SCI_UART_Read(&g_uart0_ctrl, g_rx_buf[cur], RX_CHUNK);
            }
            break;
        }

        case UART_EVENT_TX_COMPLETE:
        {
            g_uart_tx_busy = false;
            break;
        }

        case UART_EVENT_RX_CHAR:                /* byte arrived with no read armed; should not happen by design */
        {
            g_rx_char_stray++;
            break;
        }

        /* receive errors are ignored; the driver clears them and the DTC read stays armed */
        default:
            break;
    }
}

/*======================================================================================================
 * AGT 1 ms callback (set g_timer0 "Callback" property to agt0_cb)
 *====================================================================================================*/
void agt0_cb (timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_tick_1ms = true;
    }
}

/*======================================================================================================
 * Start or reset the RX pipeline; call right after the UART is opened
 *====================================================================================================*/
static void uart_rx_pipeline_start (void)
{
    g_rx_len[0]   = 0;
    g_rx_len[1]   = 0;
    g_rx_fill_idx = 0;
    g_rx_send_idx = 0;
    (void) R_SCI_UART_Read(&g_uart0_ctrl, g_rx_buf[0], RX_CHUNK);
}

/*======================================================================================================
 * Idle flush: stop the in-flight DTC read, hand over whatever arrived, re-arm.
 * Interrupts are masked for a few microseconds to avoid racing the RX_COMPLETE ISR.
 * Bytes arriving in that window stay pending in RXI and are picked up by the re-armed DTC.
 *====================================================================================================*/
static void uart_rx_idle_flush (void)
{
    __disable_irq();

    uint32_t remaining = RX_CHUNK;
    (void) R_SCI_UART_ReadStop(&g_uart0_ctrl, &remaining);
    uint32_t got = RX_CHUNK - remaining;

    uint8_t cur = g_rx_fill_idx;

    if (got > 0U)
    {
        uint8_t nxt = cur ^ 1U;
        if (0U == g_rx_len[nxt])
        {
            g_rx_len[cur] = (uint16_t) got;
            g_rx_fill_idx = nxt;
            (void) R_SCI_UART_Read(&g_uart0_ctrl, g_rx_buf[nxt], RX_CHUNK);
        }
        else
        {
            g_rx_overrun_bytes += got;          /* no free buffer: discard the partial chunk */
            (void) R_SCI_UART_Read(&g_uart0_ctrl, g_rx_buf[cur], RX_CHUNK);
        }
    }
    else
    {
        (void) R_SCI_UART_Read(&g_uart0_ctrl, g_rx_buf[cur], RX_CHUNK);
    }

    __enable_irq();
}

/*======================================================================================================
 * Apply CDC line coding to the SCI UART
 *====================================================================================================*/
static void apply_line_coding (void)
{
    uart_cfg_t cfg;
    memcpy(&cfg, &g_uart0_cfg, sizeof(cfg));

    switch (g_line_coding.b_data_bits)
    {
        case 7:  cfg.data_bits = UART_DATA_BITS_7; break;
        case 9:  cfg.data_bits = UART_DATA_BITS_9; break;
        default: cfg.data_bits = UART_DATA_BITS_8; break;
    }

    switch (g_line_coding.b_parity_type)
    {
        case 1:  cfg.parity = UART_PARITY_ODD;  break;
        case 2:  cfg.parity = UART_PARITY_EVEN; break;
        default: cfg.parity = UART_PARITY_OFF;  break;
    }

    cfg.stop_bits = (2U == g_line_coding.b_char_format) ? UART_STOP_BITS_2 : UART_STOP_BITS_1;

    (void) R_SCI_UART_Close(&g_uart0_ctrl);
    g_uart_tx_busy = false;
    (void) R_SCI_UART_Open(&g_uart0_ctrl, &cfg);

    baud_setting_t baud;
    if (FSP_SUCCESS == R_SCI_UART_BaudCalculate(g_line_coding.dw_dte_rate, true, 2500U, &baud))
    {
        (void) R_SCI_UART_BaudSet(&g_uart0_ctrl, &baud);
    }

    uart_rx_pipeline_start();
}

/*======================================================================================================
 * Main
 *====================================================================================================*/
void hal_entry (void)
{
    usb_event_info_t event_info;
    usb_status_t     event;

    memset(&event_info, 0, sizeof(event_info));

    R_IOPORT_PinWrite(&g_ioport_ctrl, LED_TX_PIN, LED_OFF);
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED_RX_PIN, LED_OFF);

    /* UART and RX pipeline */
    if (FSP_SUCCESS != R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg))
    {
        __BKPT(0);
    }
    uart_rx_pipeline_start();

    /* 1 ms tick timer */
    if (FSP_SUCCESS != R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg))
    {
        __BKPT(0);
    }
    (void) R_AGT_Start(&g_timer0_ctrl);

    /* Per-unit serial number, then USB (non-RTOS: EventGet polling) */
    usb_descriptor_serial_init();

    if (FSP_SUCCESS != R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg))
    {
        __BKPT(0);
    }

    while (1)
    {
        /* ---------------------------------------------- USB events */
        (void) R_USB_EventGet(&event_info, &event);

        switch (event)
        {
            case USB_STATUS_CONFIGURED:
            {
                g_usb_configured = true;
                g_usb_suspended  = false;
                g_usb_rx_armed   = false;
                g_usb_tx_busy    = false;
                break;
            }

            case USB_STATUS_READ_COMPLETE:      /* host to UART */
            {
                g_usb_rx_armed = false;
                if (event_info.data_size > 0U)
                {
                    g_uart_tx_busy = true;
                    (void) R_SCI_UART_Write(&g_uart0_ctrl, g_usb_rx_buf, event_info.data_size);
                    R_IOPORT_PinWrite(&g_ioport_ctrl, LED_TX_PIN, LED_ON);
                    g_led_tx_ms = LED_HOLD_MS;
                }
                break;
            }

            case USB_STATUS_WRITE_COMPLETE:     /* UART to host complete: release the buffer */
            {
                g_usb_tx_busy        = false;
                g_rx_len[g_usb_tx_src] = 0;
                g_rx_send_idx        = g_usb_tx_src ^ 1U;
                break;
            }

            case USB_STATUS_REQUEST:
            {
                uint16_t breq = (uint16_t) (event_info.setup.request_type & USB_BREQUEST);

                if (USB_PCDC_SET_LINE_CODING == breq)
                {
                    (void) R_USB_PeriControlDataGet(&g_basic0_ctrl,
                                                    (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                }
                else if (USB_PCDC_GET_LINE_CODING == breq)
                {
                    (void) R_USB_PeriControlDataSet(&g_basic0_ctrl,
                                                    (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                }
                else
                {
                    (void) R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
                }
                break;
            }

            case USB_STATUS_REQUEST_COMPLETE:
            {
                if (USB_PCDC_SET_LINE_CODING ==
                    (uint16_t) (event_info.setup.request_type & USB_BREQUEST))
                {
                    apply_line_coding();
                }
                break;
            }

            case USB_STATUS_SUSPEND:            /* suspend: keep state, just pause transfers */
            {
                g_usb_suspended = true;
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_TX_PIN, LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_RX_PIN, LED_OFF);
                break;
            }

            case USB_STATUS_RESUME:             /* resume: continue without re-enumeration */
            {
                g_usb_suspended = false;
                break;
            }

            case USB_STATUS_DETACH:             /* detach: full reset */
            {
                g_usb_configured = false;
                g_usb_suspended  = false;
                g_usb_rx_armed   = false;
                g_usb_tx_busy    = false;
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_TX_PIN, LED_OFF);
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_RX_PIN, LED_OFF);
                break;
            }

            default:
                break;
        }

        /* ------------------------------- re-arm USB OUT read once UART TX has finished */
        if (g_usb_configured && (!g_usb_suspended) && (!g_usb_rx_armed) && (!g_uart_tx_busy))
        {
            if (FSP_SUCCESS == R_USB_Read(&g_basic0_ctrl, g_usb_rx_buf, USB_BULK_MPS, USB_CLASS_PCDC))
            {
                g_usb_rx_armed = true;
            }
        }

        /* ------------------------------- UART to USB: zero-copy send of a filled buffer */
        if (g_usb_configured && (!g_usb_suspended) && (!g_usb_tx_busy))
        {
            uint8_t idx = g_rx_send_idx;

            /* after an overrun the order can flip; check the other buffer */
            if ((0U == g_rx_len[idx]) && (0U != g_rx_len[idx ^ 1U]))
            {
                idx = idx ^ 1U;
            }

            uint16_t len = g_rx_len[idx];
            if (len > 0U)
            {
                if (FSP_SUCCESS == R_USB_Write(&g_basic0_ctrl, g_rx_buf[idx], len, USB_CLASS_PCDC))
                {
                    g_usb_tx_busy = true;
                    g_usb_tx_src  = idx;        /* released on WRITE_COMPLETE */
                    R_IOPORT_PinWrite(&g_ioport_ctrl, LED_RX_PIN, LED_ON);
                    g_led_rx_ms = LED_HOLD_MS;
                }
            }
        }

        /* ------------------------------- 1 ms tick: idle flush and LED timers */
        if (g_tick_1ms)
        {
            g_tick_1ms = false;

            uart_rx_idle_flush();

            if ((g_led_tx_ms > 0U) && (0U == --g_led_tx_ms))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_TX_PIN, LED_OFF);
            }
            if ((g_led_rx_ms > 0U) && (0U == --g_led_rx_ms))
            {
                R_IOPORT_PinWrite(&g_ioport_ctrl, LED_RX_PIN, LED_OFF);
            }
        }
    }
}
