/************************************************************************/
/* Contains all USB & HID Class descriptors, as well as the HID report. */
/************************************************************************/

#ifndef USB_H_
#define USB_H_

#include <stdint.h>

#define _HB(b) (((b) & 0xff00) >> 8)
#define _LB(b) ((b) & 0xff)

#define FIRMWARE_REVISION								0x0001
#define VID												0x06A3	// Saitek
#define PID												0x0C2D	// Pro Flight Quadrant

// USB standard device request types
#define USB_REQUEST_GET_STATUS							0
#define USB_REQUEST_CLEAR_FEATURE						1
#define USB_REQUEST_SET_FEATURE							3
#define USB_REQUEST_SET_ADDRESS							5
#define USB_REQUEST_GET_DESCRIPTOR						6
#define USB_REQUEST_SET_DESCRIPTOR						7
#define USB_REQUEST_GET_CONFIGURATION					8
#define USB_REQUEST_SET_CONFIGURATION					9
#define USB_REQUEST_GET_INTERFACE						10
#define USB_REQUEST_SET_INTERFACE						11
#define USB_REQUEST_SYNCH_FRAME							12

// USB descriptor types (USB 2.0 Specification, Table 9-5)
#define USB_DESCRIPTOR_TYPE_DEVICE						1
#define USB_DESCRIPTOR_TYPE_CONFIGURATION				2
#define USB_DESCRIPTOR_TYPE_STRING						3
#define USB_DESCRIPTOR_TYPE_INTERFACE					4
#define USB_DESCRIPTOR_TYPE_ENDPOINT					5
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER			6
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION	7
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER				8

// USB class codes
// https://www.usb.org/defined-class-codes
#define USB_CLASS_CODE_USE_INTERFACE					0x00
#define USB_CLASS_CODE_HID								0x03

// Bit masks for bmRequestType
// USB 2.0 Specification, Table 9-2
#define USB_REQUEST_TYPE_DIRECTION						0b10000000
#define USB_REQUEST_TYPE_TYPE							0b01100000
#define USB_REQUEST_TYPE_RECIPIENT						0b00011111

// USB Device Request Types, corresponds to bits 5-6 of bmRequestType
// USB 2.0 Specification, Table 9-2
#define USB_REQUEST_TYPE_STANDARD						0
#define USB_REQUEST_TYPE_CLASS							1
#define USB_REQUEST_TYPE_VENDOR							2

// USB Device Request recipients, corresponds to bits 0-4 of bmRequestType
// USB 2.0 Specification, Table 9-2
#define USB_REQUEST_RECIPIENT_DEVICE					0
#define USB_REQUEST_RECIPIENT_INTERFACE					1
#define USB_REQUEST_RECIPIENT_ENDPOINT					2
#define USB_REQUEST_RECIPIENT_OTHER						3

// USB Standard Feature Selectors
// USB 2.0 Specification, Table 9-6
#define USB_FEATURE_SELECTOR_ENDPOINT_HALT				0
#define USB_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP		1
#define USB_FEATURE_SELECTOR_TEST_MODE					2

// USB HID class request types
#define USB_HID_REQUEST_GET_REPORT						0x01
#define USB_HID_REQUEST_GET_IDLE						0x02
#define USB_HID_REQUEST_SET_REPORT						0x09
#define USB_HID_REQUEST_SET_IDLE						0x0a

// USB HID class descriptor types
// USB HID Specification V1.11, section 7.1 Standard Requests
#define USB_HID_DESCRIPTOR_TYPE_HID						0x21
#define USB_HID_DESCRIPTOR_TYPE_REPORT					0x22
#define USB_HID_DESCRIPTOR_TYPE_PHYSICAL				0x23

// USB HID class report types
#define USB_HID_REPORT_TYPE_INPUT						0x01
#define USB_HID_REPORT_TYPE_OUTPUT						0x02
#define USB_HID_REPORT_TYPE_FEATURE						0x03

#define USB_MAX_INTERFACE_NUM							0
#define USB_MAX_ENDPOINT_NUM							1
#define USB_MAX_CONFIGURATION_NUM						1
#define USB_REMOTE_WAKEUP								0
#define USB_SELF_POWERED								0


typedef union {
	uint8_t bytes[8];
	struct {
		uint8_t bmRequestType;
		uint8_t bRequest;
		uint16_t wValue;
		uint16_t wIndex;
		uint16_t wLength;
	} packet;
} usb_setup_packet_u;

extern uint8_t usb_hid_report[5];

extern const uint8_t usb_device_status[2];

extern const uint8_t usb_interface_status[2];

extern const uint8_t usb_hid_report_descriptor[49];

extern const uint8_t usb_device_descriptor[18];

extern const uint8_t usb_configuration_descriptor[34];

extern const uint8_t usb_string_descriptors[94];

extern const uint16_t usb_string_descriptor_offsets[4];


#endif /* USB_H_ */
