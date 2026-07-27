/***********************************************************************************************************************
 * usb_descriptor.c — USB CDC-ACM descriptors for UC03 "Lagrange"
 *
 * The FSP r_usb_basic stack requires the application to supply the symbol
 * g_usb_descriptor (referenced as extern from the generated hal_data.c).
 * Placing this file in src/ resolves that link dependency.
 *
 * Endpoint layout (standard Renesas PCDC arrangement):
 *   EP1 IN  (0x81) : Bulk IN      64B  — device to host data
 *   EP2 OUT (0x02) : Bulk OUT     64B  — host to device data
 *   EP3 IN  (0x83) : Interrupt IN 16B  — CDC notifications
 *
 * VID/PID
 *   0x1209 is the pid.codes vendor ID for open-source hardware.
 *   0x0001..0x0010 under that VID are shared prototyping IDs and MUST NOT ship
 *   in a released product. Replace USB_PRODUCT_ID with the allocated PID once
 *   the pid.codes pull request is merged.
 **********************************************************************************************************************/
#include "hal_data.h"

#define USB_BCDNUM              (0x0200U)  /* bcdUSB 2.00 */
#define USB_VENDOR_ID           (0x1209U)  /* pid.codes — open-source hardware VID */
#define USB_PRODUCT_ID          (0x0001U)  /* TODO: replace with allocated pid.codes PID */
#define USB_RELEASE_NUM         (0x0100U)  /* bcdDevice 1.00 */

#define USB_DCP_MAXP            (64U)      /* EP0 max packet size */
#define USB_BULK_MAXP           (64U)      /* Full-Speed bulk max packet size */
#define USB_INT_MAXP            (16U)

#define USB_CONFIG_TOTAL_LEN    (9U + 9U + 5U + 5U + 4U + 5U + 7U + 9U + 7U + 7U)  /* 67 */

/*------------------------------------------------------------------------------------------------------
 * Device descriptor
 *----------------------------------------------------------------------------------------------------*/
static uint8_t g_device_descriptor[] =
{
    18U,                                   /* bLength */
    0x01U,                                 /* bDescriptorType : DEVICE */
    (uint8_t) (USB_BCDNUM & 0xFFU),        /* bcdUSB (L) */
    (uint8_t) (USB_BCDNUM >> 8),           /* bcdUSB (H) */
    0x02U,                                 /* bDeviceClass    : CDC */
    0x00U,                                 /* bDeviceSubClass */
    0x00U,                                 /* bDeviceProtocol */
    USB_DCP_MAXP,                          /* bMaxPacketSize0 */
    (uint8_t) (USB_VENDOR_ID & 0xFFU),     /* idVendor (L) */
    (uint8_t) (USB_VENDOR_ID >> 8),        /* idVendor (H) */
    (uint8_t) (USB_PRODUCT_ID & 0xFFU),    /* idProduct (L) */
    (uint8_t) (USB_PRODUCT_ID >> 8),       /* idProduct (H) */
    (uint8_t) (USB_RELEASE_NUM & 0xFFU),   /* bcdDevice (L) */
    (uint8_t) (USB_RELEASE_NUM >> 8),      /* bcdDevice (H) */
    1U,                                    /* iManufacturer */
    2U,                                    /* iProduct */
    3U,                                    /* iSerialNumber */
    1U,                                    /* bNumConfigurations */
};

/*------------------------------------------------------------------------------------------------------
 * Configuration descriptor (Full-Speed): CDC communication IF + CDC data IF
 * Array padded to an even length to suit the FSP USB FIFO 16-bit access path.
 *----------------------------------------------------------------------------------------------------*/
static uint8_t g_configuration_descriptor[USB_CONFIG_TOTAL_LEN + (USB_CONFIG_TOTAL_LEN % 2U)] =
{
    /* Configuration */
    9U, 0x02U,
    (uint8_t) (USB_CONFIG_TOTAL_LEN & 0xFFU),
    (uint8_t) (USB_CONFIG_TOTAL_LEN >> 8),
    2U,                                    /* bNumInterfaces */
    1U,                                    /* bConfigurationValue */
    0U,                                    /* iConfiguration */
    0x80U,                                 /* bmAttributes : bus powered */
    (uint8_t) (100U / 2U),                 /* bMaxPower : 100 mA */

    /* Interface 0 : CDC communications class (ACM) */
    9U, 0x04U,
    0U,                                    /* bInterfaceNumber */
    0U,                                    /* bAlternateSetting */
    1U,                                    /* bNumEndpoints */
    0x02U,                                 /* bInterfaceClass    : CDC-Comm */
    0x02U,                                 /* bInterfaceSubClass : ACM */
    0x01U,                                 /* bInterfaceProtocol : AT commands */
    0U,                                    /* iInterface */

    /* Header functional descriptor */
    5U, 0x24U, 0x00U, 0x10U, 0x01U,        /* bcdCDC 1.10 */

    /* Call management functional descriptor */
    5U, 0x24U, 0x01U, 0x03U, 0x01U,        /* bmCapabilities = 3, bDataInterface = 1 */

    /* Abstract control management functional descriptor */
    4U, 0x24U, 0x02U, 0x02U,               /* bmCapabilities : line coding + serial state */

    /* Union functional descriptor */
    5U, 0x24U, 0x06U, 0x00U, 0x01U,        /* master IF0, slave IF1 */

    /* Endpoint : interrupt IN (EP3) */
    7U, 0x05U, 0x83U, 0x03U,
    USB_INT_MAXP, 0x00U,
    0x10U,                                 /* bInterval 16 ms */

    /* Interface 1 : CDC data class */
    9U, 0x04U,
    1U,                                    /* bInterfaceNumber */
    0U,                                    /* bAlternateSetting */
    2U,                                    /* bNumEndpoints */
    0x0AU,                                 /* bInterfaceClass : CDC-Data */
    0x00U, 0x00U, 0U,

    /* Endpoint : bulk IN (EP1) */
    7U, 0x05U, 0x81U, 0x02U,
    USB_BULK_MAXP, 0x00U,
    0x00U,

    /* Endpoint : bulk OUT (EP2) */
    7U, 0x05U, 0x02U, 0x02U,
    USB_BULK_MAXP, 0x00U,
    0x00U,
};

/*------------------------------------------------------------------------------------------------------
 * String descriptors (UTF-16LE)
 *----------------------------------------------------------------------------------------------------*/
/* index 0 : LANGID (English, US) */
static uint8_t g_string_descriptor_0[] =
{
    4U, 0x03U, 0x09U, 0x04U,
};

/* index 1 : manufacturer "Uncommon Group" */
static uint8_t g_string_descriptor_1[] =
{
    30U, 0x03U,
    'U', 0, 'n', 0, 'c', 0, 'o', 0, 'm', 0, 'm', 0, 'o', 0, 'n', 0, ' ', 0,
    'G', 0, 'r', 0, 'o', 0, 'u', 0, 'p', 0,
};

/* index 2 : product "Lagrange USB-UART Bridge" */
static uint8_t g_string_descriptor_2[] =
{
    50U, 0x03U,
    'L', 0, 'a', 0, 'g', 0, 'r', 0, 'a', 0, 'n', 0, 'g', 0, 'e', 0, ' ', 0,
    'U', 0, 'S', 0, 'B', 0, '-', 0, 'U', 0, 'A', 0, 'R', 0, 'T', 0, ' ', 0,
    'B', 0, 'r', 0, 'i', 0, 'd', 0, 'g', 0, 'e', 0,
};

/* index 3 : serial number — overwritten at runtime from the MCU unique ID */
static uint8_t g_string_descriptor_3[] =
{
    34U, 0x03U,
    '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0,
    '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '1', 0,
};

static uint8_t * g_string_table[] =
{
    g_string_descriptor_0,
    g_string_descriptor_1,
    g_string_descriptor_2,
    g_string_descriptor_3,
};

/*------------------------------------------------------------------------------------------------------
 * Descriptor bundle required by FSP
 *----------------------------------------------------------------------------------------------------*/
usb_descriptor_t g_usb_descriptor =
{
    .p_device    = g_device_descriptor,
    .p_config_f  = g_configuration_descriptor,
    .p_config_h  = NULL,                   /* Full-Speed only */
    .p_qualifier = NULL,                   /* Full-Speed only */
    .p_string    = g_string_table,
    .num_string  = sizeof(g_string_table) / sizeof(g_string_table[0]),
};

/*------------------------------------------------------------------------------------------------------
 * Build a per-unit serial number from the MCU unique ID.
 *
 * Call this once before R_USB_Open(). Giving every unit a distinct serial makes
 * Windows bind a stable COM port number per device instead of per USB port,
 * which matters when several bridges are used on one machine.
 *----------------------------------------------------------------------------------------------------*/
void usb_descriptor_serial_init (void)
{
    static const char hex[] = "0123456789ABCDEF";
    bsp_unique_id_t const * p_uid = R_BSP_UniqueIdGet();

    /* 8 ID bytes -> 16 hex characters, written as UTF-16LE after the 2-byte header */
    for (uint32_t i = 0; i < 8U; i++)
    {
        uint8_t b = p_uid->unique_id_bytes[i];
        g_string_descriptor_3[2U + (i * 4U)]      = (uint8_t) hex[(b >> 4) & 0x0FU];
        g_string_descriptor_3[2U + (i * 4U) + 1U] = 0U;
        g_string_descriptor_3[2U + (i * 4U) + 2U] = (uint8_t) hex[b & 0x0FU];
        g_string_descriptor_3[2U + (i * 4U) + 3U] = 0U;
    }
}
