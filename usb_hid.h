/************************************************************************/
/* Contains handlers for HID class-specific device requests.			*/
/************************************************************************/

#ifndef USB_HID_H_
#define USB_HID_H_

#include "usb.h"
#include "usb_device.h"
#include "usb_transfer.h"


extern bool usb_hid_data_fresh;

extern uint8_t usb_hid_idle_rate;

void usb_hid_get_report(usb_setup_packet_u packet_u);

void usb_hid_get_idle();

void usb_hid_set_idle(usb_setup_packet_u packet_u);

void usb_hid_handle_request(usb_setup_packet_u packet_u);


#endif /* USB_HID_H_ */
