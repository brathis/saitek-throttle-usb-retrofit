#include "usb_standard.h"

void usb_standard_set_address(usb_setup_packet_u packet) {
	if (usb_device_get_state() == USB_DEVICE_STATE_CONFIGURED) {
		usb_stall();
		return;
	}
	
	// set device address
	UDADDR = (packet.packet.wValue & 0b01111111);
	
	// status stage
	usb_in_poll();
	usb_in_clear();
	
	// wait until the end of the status stage before enabling the address, as per
	// USB Specification 2.0, Section 9.4.6
	// per datasheet, Section 21.12.1, the end of the status stage is characterized
	// by the TXINI bit being reset to 1
	usb_in_poll();
	
	// set device to "Address" state
	UDADDR |= (1 << ADDEN);
}

void usb_standard_get_descriptor(usb_setup_packet_u packet) {
	switch ((packet.packet.wValue & 0xff00) >> 8) { // descriptor type
		case USB_DESCRIPTOR_TYPE_DEVICE:
			usb_transfer_control_read(usb_device_descriptor,
									 sizeof(usb_device_descriptor),
									 packet.packet.wLength);
			break;
		case USB_DESCRIPTOR_TYPE_CONFIGURATION:
			usb_transfer_control_read(usb_configuration_descriptor,
									  sizeof(usb_configuration_descriptor),
									  packet.packet.wLength);
			break;
		case USB_DESCRIPTOR_TYPE_STRING:
			;
			uint16_t offset = usb_string_descriptor_offsets[packet.packet.wValue & 0x00ff];
			usb_transfer_control_read((uint8_t*) (usb_string_descriptors + offset),
									  usb_string_descriptors[offset],
									  packet.packet.wLength);
			break;
		case USB_HID_DESCRIPTOR_TYPE_REPORT:
			usb_transfer_control_read(usb_hid_report_descriptor, sizeof(usb_hid_report_descriptor), packet.packet.wLength);
			break;
		case USB_HID_DESCRIPTOR_TYPE_HID:
			usb_transfer_control_read(&usb_configuration_descriptor[18], usb_configuration_descriptor[18], packet.packet.wLength);
			break;
		default: // other descriptor types are unsupported
			usb_stall();
			break;
	}
}

void usb_standard_get_status(usb_setup_packet_u packet) {
	usb_device_state_t device_state = usb_device_get_state();
	if (device_state == USB_DEVICE_STATE_DEFAULT) {
		usb_stall();
		return;
	}
	
	if (device_state == USB_DEVICE_STATE_ADDRESS && packet.packet.wIndex != 0) {
		usb_stall();
		return;
	}
	
	switch (packet.packet.bmRequestType & USB_REQUEST_TYPE_RECIPIENT) {
		case USB_REQUEST_RECIPIENT_DEVICE:
			usb_transfer_control_read(usb_device_status, 2, 2);
			break;
		case USB_REQUEST_RECIPIENT_INTERFACE:
			if (packet.packet.wIndex > USB_MAX_INTERFACE_NUM) {
				usb_stall();
				return;
			}
			usb_transfer_control_read(usb_interface_status, 2, 2);
			break;
		case USB_REQUEST_RECIPIENT_ENDPOINT:
			if (packet.packet.wIndex > USB_MAX_ENDPOINT_NUM) {
				usb_stall();
				return;
			}
			
			uint8_t endpoint_status[2] = {
				usb_device_endpoint_is_halted(packet.packet.wIndex) ? 1U : 0U,
				0U,
			};
			usb_transfer_control_read(endpoint_status, 2, 2);
			break;
		case USB_REQUEST_RECIPIENT_OTHER:
		default:
			usb_stall();
			break;
	}
}

void usb_standard_clear_feature(usb_setup_packet_u packet) {
	usb_device_state_t device_state = usb_device_get_state();
	
	// Behavior unspecified in Default state -> request error
	if (device_state == USB_DEVICE_STATE_DEFAULT) {
		usb_stall();
		return;
	}
	
	// Request only valid in Address state if interface / endpoint is zero
	if (device_state == USB_DEVICE_STATE_ADDRESS && packet.packet.wIndex != 0) {
		usb_stall();
		return;
	}
	
	switch (packet.packet.bmRequestType & USB_REQUEST_TYPE_RECIPIENT) {
		case USB_REQUEST_RECIPIENT_DEVICE:
			// only supported request is DEVICE_REMOTE_WAKEUP
			if (packet.packet.wValue == USB_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP) {
				// do nothing, just acknowledge
				usb_in_poll();
				usb_in_clear();
			} else {
				usb_stall();
			}
			break;
		case USB_REQUEST_RECIPIENT_ENDPOINT:
			// only supported request is ENDPOINT_HALT
			if (packet.packet.wValue == USB_FEATURE_SELECTOR_ENDPOINT_HALT) {
				usb_device_endpoint_resume(packet.packet.wIndex);
				
				usb_in_poll();
				usb_in_clear();
			} else {
				usb_stall();
			}
			break;
		case USB_REQUEST_RECIPIENT_INTERFACE:
		case USB_REQUEST_RECIPIENT_OTHER:
		default:
			usb_stall();
			break;
	}
}

void usb_standard_get_configuration() {
	// Behavior in Default state is unspecified -> request error
	if (usb_device_get_state() == USB_DEVICE_STATE_DEFAULT) {
		usb_stall();
		return;
	}
	
	usb_transfer_control_read(&usb_device_configuration, 1, 1);
}

void usb_standard_set_configuration(usb_setup_packet_u packet) {
	usb_device_state_t device_state = usb_device_get_state();
	if (device_state == USB_DEVICE_STATE_DEFAULT) {
		usb_stall();
		return;
	}
	
	uint16_t new_configuration = packet.packet.wValue & 0x00ff;
	if (new_configuration > USB_MAX_CONFIGURATION_NUM) {
		usb_stall();
		return;
	}
	
	usb_device_configuration = new_configuration;
				
	// status stage
	usb_in_poll();
	usb_in_clear();
}

void usb_standard_get_interface() {
	if (usb_device_get_state() != USB_DEVICE_STATE_CONFIGURED) {
		usb_stall();
		return;	
	}
	
	usb_transfer_control_read(&usb_device_interface, 1, 1);
}

void usb_standard_handle_request(usb_setup_packet_u packet_u) {
	switch (packet_u.packet.bRequest) {
		case USB_REQUEST_GET_STATUS:
			usb_standard_get_status(packet_u);
			break;
		case USB_REQUEST_CLEAR_FEATURE:
			usb_standard_clear_feature(packet_u);
			break;
		case USB_REQUEST_SET_ADDRESS:
			usb_standard_set_address(packet_u);
			break;
		case USB_REQUEST_GET_DESCRIPTOR:
			usb_standard_get_descriptor(packet_u);
			break;
		case USB_REQUEST_GET_CONFIGURATION:
			usb_standard_get_configuration();
			break;
		case USB_REQUEST_SET_CONFIGURATION:
			usb_standard_set_configuration(packet_u);
			break;
		case USB_REQUEST_GET_INTERFACE:
			usb_standard_get_interface();
			break;
		case USB_REQUEST_SET_FEATURE: // unsure if required or not, currently unsupported
		case USB_REQUEST_SET_DESCRIPTOR: // optional request, unsupported
		case USB_REQUEST_SET_INTERFACE: // unsupported request, no alternate configuration
		case USB_REQUEST_SYNCH_FRAME: // unsupported request, high-speed only
		default:
			usb_stall();
			break;
	}
}
