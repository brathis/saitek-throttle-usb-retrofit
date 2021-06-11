#include "usb_hid.h"


bool usb_hid_data_fresh = true;

uint8_t usb_hid_idle_rate = 0;

void usb_hid_get_report(usb_setup_packet_u packet_u) {	
	switch (packet_u.packet.wValue & 0xf0) {
		case USB_HID_REPORT_TYPE_INPUT:
			usb_transfer_control_read((const uint8_t*) usb_hid_report, sizeof(usb_hid_report), packet_u.packet.wLength);
			break;
		case USB_HID_REPORT_TYPE_OUTPUT:
		case USB_HID_REPORT_TYPE_FEATURE:
		default:
			usb_stall();
			break;
	}
}

void usb_hid_get_idle() {
	usb_transfer_control_read((const uint8_t*) &usb_hid_idle_rate, 1, 1);
	
	// status stage
	usb_out_poll();
	usb_out_clear();
}

void usb_hid_set_idle(usb_setup_packet_u packet_u) {
	usb_hid_idle_rate = packet_u.packet.wValue & (0xff00);
	
	// status stage
	usb_in_poll();
	usb_in_clear();
}

void usb_hid_handle_request(usb_setup_packet_u packet_u) {
	switch (packet_u.packet.bRequest) {
		case USB_HID_REQUEST_GET_REPORT:
			usb_hid_get_report(packet_u);
			break;
		case USB_HID_REQUEST_GET_IDLE:
			usb_hid_get_idle();
			break;
		case USB_HID_REQUEST_SET_IDLE:
			usb_hid_set_idle(packet_u);
			break;
		case USB_HID_REQUEST_SET_REPORT:
		default:
			usb_stall();
			break;
	}
}
