/* generated HAL source file - do not edit */
#include "hal_data.h"
agt_instance_ctrl_t g_timer0_ctrl;
const agt_extended_cfg_t g_timer0_extend =
{ .count_source = AGT_CLOCK_PCLKB,
  .agto = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtoa = AGT_PIN_CFG_DISABLED,
  .agtoab_settings_b.agtob = AGT_PIN_CFG_DISABLED,
  .measurement_mode = AGT_MEASURE_DISABLED,
  .agtio_filter = AGT_AGTIO_FILTER_NONE,
  .enable_pin = AGT_ENABLE_PIN_NOT_USED,
  .trigger_edge = AGT_TRIGGER_EDGE_RISING,
  .counter_bit_width = AGT_COUNTER_BIT_WIDTH_32, };
const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 0.002730666666666667 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x10000,
  .duty_cycle_counts = 0x8000, .source_div = (timer_source_div_t) 0, .channel = 0, .p_callback = agt0_cb,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer0_extend,
  .cycle_end_ipl = (3),
#if defined(VECTOR_NUMBER_AGT0_INT)
    .cycle_end_irq       = VECTOR_NUMBER_AGT0_INT,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{ .p_ctrl = &g_timer0_ctrl, .p_cfg = &g_timer0_cfg, .p_api = &g_timer_on_agt };
dtc_instance_ctrl_t g_transfer0_ctrl;

#if (BSP_CFG_DCACHE_ENABLED) && (1 == 1)
const transfer_info_t g_transfer0_user_config_info =
{
    .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
    .transfer_settings_word_b.repeat_area    = TRANSFER_REPEAT_AREA_DESTINATION,
    .transfer_settings_word_b.irq            = TRANSFER_IRQ_END,
    .transfer_settings_word_b.chain_mode     = TRANSFER_CHAIN_MODE_DISABLED,
    .transfer_settings_word_b.src_addr_mode  = TRANSFER_ADDR_MODE_FIXED,
    .transfer_settings_word_b.size           = TRANSFER_SIZE_1_BYTE,
    .transfer_settings_word_b.mode           = TRANSFER_MODE_NORMAL,
    .p_dest                                  = (void *) NULL,
    .p_src                                   = (void const *) NULL,
    .num_blocks                              = (uint16_t) 0,
    .length                                  = (uint16_t) 0,
};
#endif

#if BSP_CFG_DCACHE_ENABLED
    #if (1 > 0)
    transfer_info_t g_transfer0_info_fsp_nocache[1] DTC_TRANSFER_INFO_ALIGNMENT;
    #else
    /* User must call api::reconfigure before enable DTC transfer. */
    #endif
#else
#if (1 == 1)
transfer_info_t g_transfer0_info DTC_TRANSFER_INFO_ALIGNMENT =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = (uint16_t) 0,
  .length = (uint16_t) 0, };
#elif (1 > 1)
    /* User is responsible to initialize the array. */
    transfer_info_t g_transfer0_info[1] DTC_TRANSFER_INFO_ALIGNMENT;
    #else
    /* User must call api::reconfigure before enable DTC transfer. */
    #endif
#endif

const dtc_extended_cfg_t g_transfer0_cfg_extend =
{ .activation_source = VECTOR_NUMBER_SCI0_RXI,

#if BSP_CFG_DCACHE_ENABLED
    #if (1 == 1)
        .p_user_config_info =  &g_transfer0_user_config_info,
    #else
        .p_user_config_info = NULL,
    #endif
#else
        /* p_user_config_info not present. */
#endif
        };

const transfer_cfg_t g_transfer0_cfg =
{
#if BSP_CFG_DCACHE_ENABLED
    #if (1 > 0)
        .p_info              = g_transfer0_info_fsp_nocache,
    #else
        .p_info = NULL,
    #endif
#else
#if (1 == 1)
  .p_info = &g_transfer0_info,
#elif (1 > 1)
        .p_info              = g_transfer0_info,
    #else
        .p_info = NULL,
    #endif
#endif
  .p_extend = &g_transfer0_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
dtc_instance_ctrl_t g_transfer1_ctrl;

#if (BSP_CFG_DCACHE_ENABLED) && (1 == 1)
const transfer_info_t g_transfer1_user_config_info =
{
    .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
    .transfer_settings_word_b.repeat_area    = TRANSFER_REPEAT_AREA_SOURCE,
    .transfer_settings_word_b.irq            = TRANSFER_IRQ_END,
    .transfer_settings_word_b.chain_mode     = TRANSFER_CHAIN_MODE_DISABLED,
    .transfer_settings_word_b.src_addr_mode  = TRANSFER_ADDR_MODE_INCREMENTED,
    .transfer_settings_word_b.size           = TRANSFER_SIZE_1_BYTE,
    .transfer_settings_word_b.mode           = TRANSFER_MODE_NORMAL,
    .p_dest                                  = (void *) NULL,
    .p_src                                   = (void const *) NULL,
    .num_blocks                              = (uint16_t) 0,
    .length                                  = (uint16_t) 0,
};
#endif

#if BSP_CFG_DCACHE_ENABLED
    #if (1 > 0)
    transfer_info_t g_transfer1_info_fsp_nocache[1] DTC_TRANSFER_INFO_ALIGNMENT;
    #else
    /* User must call api::reconfigure before enable DTC transfer. */
    #endif
#else
#if (1 == 1)
transfer_info_t g_transfer1_info DTC_TRANSFER_INFO_ALIGNMENT =
{ .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE,
  .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void*) NULL,
  .p_src = (void const*) NULL,
  .num_blocks = (uint16_t) 0,
  .length = (uint16_t) 0, };
#elif (1 > 1)
    /* User is responsible to initialize the array. */
    transfer_info_t g_transfer1_info[1] DTC_TRANSFER_INFO_ALIGNMENT;
    #else
    /* User must call api::reconfigure before enable DTC transfer. */
    #endif
#endif

const dtc_extended_cfg_t g_transfer1_cfg_extend =
{ .activation_source = VECTOR_NUMBER_SCI0_TXI,

#if BSP_CFG_DCACHE_ENABLED
    #if (1 == 1)
        .p_user_config_info =  &g_transfer1_user_config_info,
    #else
        .p_user_config_info = NULL,
    #endif
#else
        /* p_user_config_info not present. */
#endif
        };

const transfer_cfg_t g_transfer1_cfg =
{
#if BSP_CFG_DCACHE_ENABLED
    #if (1 > 0)
        .p_info              = g_transfer1_info_fsp_nocache,
    #else
        .p_info = NULL,
    #endif
#else
#if (1 == 1)
  .p_info = &g_transfer1_info,
#elif (1 > 1)
        .p_info              = g_transfer1_info,
    #else
        .p_info = NULL,
    #endif
#endif
  .p_extend = &g_transfer1_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t g_transfer1 =
{ .p_ctrl = &g_transfer1_ctrl, .p_cfg = &g_transfer1_cfg, .p_api = &g_transfer_on_dtc };
sci_uart_instance_ctrl_t g_uart0_ctrl;

baud_setting_t g_uart0_baud_setting =
        {
        /* Baud rate calculated with 0.160% error. */.semr_baudrate_bits_b.abcse = 0,
          .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 12, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme =
                  false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart0_cfg_extend =
{ .clock = SCI_UART_CLOCK_INT, .rx_edge_start = SCI_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart0_baud_setting, .flow_control =
          SCI_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
  .rs485_setting =
  { .enable = SCI_UART_RS485_DISABLE, .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
#if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
    .de_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
          },
  .irda_setting =
  { .ircr_bits_b.ire = 0, .ircr_bits_b.irrxinv = 0, .ircr_bits_b.irtxinv = 0, }, };

/** UART interface configuration */
const uart_cfg_t g_uart0_cfg =
{ .channel = 0, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          NULL,
  .p_context = NULL, .p_extend = &g_uart0_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_transfer1)
                .p_transfer_tx       = NULL,
#else
  .p_transfer_tx = &g_transfer1,
#endif
#if (RA_NOT_DEFINED == g_transfer0)
                .p_transfer_rx       = NULL,
#else
  .p_transfer_rx = &g_transfer0,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (2),
  .txi_ipl = (2), .tei_ipl = (2), .eri_ipl = (2),
#if defined(VECTOR_NUMBER_SCI0_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI0_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI0_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI0_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI0_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart0 =
{ .p_ctrl = &g_uart0_ctrl, .p_cfg = &g_uart0_cfg, .p_api = &g_uart_on_sci };
usb_instance_ctrl_t g_basic0_ctrl;

#if !defined(g_usb_descriptor)
extern usb_descriptor_t g_usb_descriptor;
#endif
#define RA_NOT_DEFINED (1)
const usb_cfg_t g_basic0_cfg =
{ .usb_mode = USB_MODE_PERI,
  .usb_speed = USB_SPEED_FS,
  .module_number = 0,
  .type = USB_CLASS_PCDC,
#if defined(g_usb_descriptor)
                .p_usb_reg = g_usb_descriptor,
#else
  .p_usb_reg = &g_usb_descriptor,
#endif
  .usb_complience_cb = NULL,
#if defined(VECTOR_NUMBER_USBFS_INT)
                .irq       = VECTOR_NUMBER_USBFS_INT,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBFS_RESUME)
                .irq_r     = VECTOR_NUMBER_USBFS_RESUME,
#else
  .irq_r = FSP_INVALID_VECTOR,
#endif
  .hsirq = FSP_INVALID_VECTOR,
  .hsirq_d0 = FSP_INVALID_VECTOR,
  .hsirq_d1 = FSP_INVALID_VECTOR,
  .ipl = (2),
  .ipl_r = (2),
  .ipl_d0 = (2),
  .ipl_d1 = (2),
  .hsipl = BSP_IRQ_DISABLED,
  .hsipl_d0 = BSP_IRQ_DISABLED,
  .hsipl_d1 = BSP_IRQ_DISABLED,
#if (BSP_CFG_RTOS == 0) && defined(USB_CFG_HMSC_USE)
                .p_usb_apl_callback = NULL,
#else
  .p_usb_apl_callback = NULL,
#endif
#if defined(NULL)
                .p_context = NULL,
#else
  .p_context = (void*) &NULL,
#endif
#if defined(VECTOR_NUMBER_USBFS_FIFO_0)
                .irq_d0    = VECTOR_NUMBER_USBFS_FIFO_0,
#else
  .irq_d0 = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBFS_FIFO_1)
                .irq_d1    = VECTOR_NUMBER_USBFS_FIFO_1,
#else
  .irq_d1 = FSP_INVALID_VECTOR,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
#else
                .p_transfer_tx = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
#else
                .p_transfer_rx = &RA_NOT_DEFINED,
#endif
        };
#undef RA_NOT_DEFINED

/* Instance structure to use this module. */
const usb_instance_t g_basic0 =
{ .p_ctrl = &g_basic0_ctrl, .p_cfg = &g_basic0_cfg, .p_api = &g_usb_on_usb, };

void g_hal_init(void)
{
    g_common_init ();
}
