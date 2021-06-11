/************************************************************************/
/* Handlers for standard device requests.                               */
/************************************************************************/

#ifndef USB_STANDARD_H_
#define USB_STANDARD_H_

#include <avr/io.h>

#include "usb.h"
#include "usb_device.h"
#include "usb_hid.h"
#include "usb_transfer.h"


void usb_standard_set_address(usb_setup_packet_u packet);

void usb_standard_get_descriptor(usb_setup_packet_u packet);

void usb_standard_get_status(usb_setup_packet_u packet);

void usb_standard_clear_feature(usb_setup_packet_u packet);

void usb_standard_get_configuration();

void usb_standard_set_configuration(usb_setup_packet_u packet);

void usb_standard_get_interface();

void usb_standard_handle_request(usb_setup_packet_u packet);


#endif /* USB_STANDARD_H_ */
