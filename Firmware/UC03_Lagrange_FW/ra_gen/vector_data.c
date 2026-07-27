/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = usbfs_interrupt_handler, /* USBFS INT (USBFS interrupt) */
            [1] = usbfs_resume_handler, /* USBFS RESUME (USBFS resume interrupt) */
            [2] = usbfs_d1fifo_handler, /* USBFS FIFO 1 (DMA/DTC transfer request 1) */
            [4] = sci_uart_rxi_isr, /* SCI0 RXI (Receive data full) */
            [5] = usbfs_d0fifo_handler, /* USBFS FIFO 0 (DMA/DTC transfer request 0) */
            [6] = sci_uart_tei_isr, /* SCI0 TEI (Transmit end) */
            [7] = sci_uart_eri_isr, /* SCI0 ERI (Receive error) */
            [9] = sci_uart_txi_isr, /* SCI0 TXI (Transmit data empty) */
            [11] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_USBFS_INT,GROUP0), /* USBFS INT (USBFS interrupt) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_USBFS_RESUME,GROUP1), /* USBFS RESUME (USBFS resume interrupt) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_USBFS_FIFO_1,GROUP2), /* USBFS FIFO 1 (DMA/DTC transfer request 1) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI0_RXI,GROUP4), /* SCI0 RXI (Receive data full) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_USBFS_FIFO_0,GROUP5), /* USBFS FIFO 0 (DMA/DTC transfer request 0) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TEI,GROUP6), /* SCI0 TEI (Transmit end) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI0_ERI,GROUP7), /* SCI0 ERI (Receive error) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI0_TXI,GROUP1), /* SCI0 TXI (Transmit data empty) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_AGT0_INT,GROUP3), /* AGT0 INT (AGT interrupt) */
        };
        #endif
        #endif
