#include "usb.h"

uint8_t usb_hid_report[] = {0};

const uint8_t usb_device_status[] = {
	(USB_REMOTE_WAKEUP << 1) | (USB_SELF_POWERED << 0),
	0U,
};

const uint8_t usb_interface_status[] = {
	0U,
	0U,
};

const uint8_t usb_hid_report_descriptor[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x15, 0x00,                    // LOGICAL_MINIMUM (0)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x32,                    //     USAGE (Z)
    0x26, 0xff, 0x00,              //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0xa2,                    //     INPUT (Data,Var,Abs,NPrf,Vol)
    0xc0,                          //   END_COLLECTION
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x09,                    //   USAGE_MAXIMUM (Button 9)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x09,                    //   REPORT_COUNT (9)
    0x81, 0x82,                    //   INPUT (Data,Var,Abs,Vol)
    0x75, 0x07,                    //   REPORT_SIZE (7)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x83,                    //   INPUT (Cnst,Var,Abs,Vol)
    0xc0                           // END_COLLECTION
};

const uint8_t usb_device_descriptor[] = {
	18,																				// bLength
	USB_DESCRIPTOR_TYPE_DEVICE,														// bDescriptorType
	0x00, 0x02,																		// bcdUSB
	USB_CLASS_CODE_USE_INTERFACE,													// bDeviceClass
	0x00,																			// bDeviceSubClass
	0x00,																			// bDeviceProtocol
	64,																				// bMaxPacketSize0
	_LB(VID), _HB(VID),																// idVendor
	_LB(PID), _HB(PID),																// idProduct
	_LB(FIRMWARE_REVISION), _HB(FIRMWARE_REVISION),									// bcdDevice
	1,																				// iManufacturer
	2,																				// iProduct
	3,																				// iSerialNumber
	1,																				// bNumConfigurations
};

const uint8_t usb_configuration_descriptor[] = {
	// Configuration Descriptor
	9,										// bLength
	USB_DESCRIPTOR_TYPE_CONFIGURATION,		// bDescriptorType
	34, 0,									// wTotalLength
	1,										// bNumInterfaces
	1,										// bConfigurationValue
	0,										// iConfiguration
	0b10000000,								// bmAttributes
	50,										// bMaxPower (50 * 2mA = 100mA)

	// Interface Descriptor
	9,										// bLength
	USB_DESCRIPTOR_TYPE_INTERFACE,			// bDescriptorType
	0,										// bInterfaceNumber
	0,										// bAlternateSetting
	1,										// bNumEndpoints
	USB_CLASS_CODE_HID,						// bInterfaceClass
	0x00,									// bInterfaceSubclass
	0x00,									// bInterfaceProtocol
	0,										// iInterface

	// HID Descriptor
	9,										// bLength
	USB_HID_DESCRIPTOR_TYPE_HID,			// bDescriptorType
	0x11, 0x01,								// bcdHID
	0,										// bCountryCode
	1,										// bNumDescriptors
	USB_HID_DESCRIPTOR_TYPE_REPORT,			// bDescriptorType
	49,	0,									// wDescriptorLength
	
	// Endpoint Descriptor
	7,										// bLength
	USB_DESCRIPTOR_TYPE_ENDPOINT,			// bDescriptorType
	0b10000001,								// bEndpointAddress (1 IN)
	0b00000011,								// bmAttributes (Interrupt)
	64, 0,									// wMaxPacketSize (64 bytes)
	100,									// bInterval (100ms)
};

const uint8_t usb_string_descriptors[] = {
	// Offset 0
	// string descriptor zero
	4U,
	USB_DESCRIPTOR_TYPE_STRING,
	0x09, 0x04, // 0x0409 = en-US
	
	// Offset 4, Manufacturer
	14U,
	USB_DESCRIPTOR_TYPE_STRING,
	0x53, 0x00, // S
	0x61, 0x00, // a
	0x69, 0x00, // i
	0x74, 0x00, // t
	0x65, 0x00, // e
	0x6b, 0x00, // k
	
	// Offset 18, Product
	40U,
	USB_DESCRIPTOR_TYPE_STRING,
	0x50, 0x00, // P
	0x72, 0x00, // r
	0x6f, 0x00, // o
	0x20, 0x00, //
	0x46, 0x00, // F
	0x6c, 0x00, // l
	0x69, 0x00, // i
	0x67, 0x00, // g
	0x68, 0x00, // h
	0x74, 0x00, // t
	0x20, 0x00, //
	0x51, 0x00, // Q
	0x75, 0x00, // u
	0x61, 0x00, // a
	0x64, 0x00, // d
	0x72, 0x00, // r
	0x61, 0x00, // a
	0x6e, 0x00, // n
	0x74, 0x00, // t
	
	// Offset 58, Serial No.
	4U,
	USB_DESCRIPTOR_TYPE_STRING,
	0x30, 0x00, // 0
};

const uint16_t usb_string_descriptor_offsets[] = {
	0,
	4,
	18,
	58,
};
