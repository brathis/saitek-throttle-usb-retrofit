#include "usb_transfer.h"

uint16_t usb_transfer_control_read(const uint8_t* buf, size_t len, size_t requested) {
	usb_endpoint_select(0);
	
	if (requested < len) {
		// don't send more data than has been requested
		len = requested;
	} else if (len == 0) {
		// send ZLP
		usb_in_clear();
	}
	
	uint16_t sent = 0;
	bool last_packet_full = false;
	while (sent < len || last_packet_full) {
		// wait for the USB controller to accept new data
		usb_in_poll();
		
		// fill up the endpoint buffer
		while (sent < len && usb_endpoint_byte_count(0) < 64) {
			usb_write(0, buf[sent++]);
		}
		
		// if there is no more data to be sent but the last packet was "full" nonetheless, a ZLP must be sent
		last_packet_full = usb_endpoint_byte_count(0) == 64;
		
		// send
		usb_in_clear();
	}
	
	// status stage
	usb_out_poll();
	usb_out_clear();
	
	return sent;
}

uint16_t usb_transfer_control_write(uint8_t* buf, size_t len) {
	usb_endpoint_select(0);
	
	// read at most len bytes from the host
	uint16_t read = 0;
	while (read < len) {
		usb_out_poll();
		
		// if a ZLP is received, the transfer is over
		if (usb_endpoint_byte_count(0) == 0) {
			break;
		}
		
		// read until there is no more data in the endpoint or len bytes have been read
		while (usb_endpoint_byte_count(0) > 0 && read < len) {
			buf[read++] = usb_read(0);
		}
		
		usb_out_clear();
	}
	
	// status stage
	usb_in_poll();
	usb_in_clear();
	
	return read;
}

uint16_t usb_transaction_interrupt_in(uint8_t endpoint, uint8_t* buf, size_t len) {
	usb_endpoint_select(endpoint);
	
	// wait for buffer to become available
	usb_in_poll();
	usb_in_clear();
	
	// write data into endpoint buffer
	uint16_t sent = 0;
	while (sent < len) {
		usb_write(endpoint, buf[sent++]);
	}

	// send
	usb_fifo_clear();
	
	return sent;
}
