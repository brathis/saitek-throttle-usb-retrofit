/************************************************************************/
/* High-level functions for interacting with the USB device controller. */
/************************************************************************/

#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_

#include <stdbool.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include "usb_drv.h"


extern uint8_t usb_device_configuration;

extern uint8_t usb_device_interface;

typedef enum {
	USB_DEVICE_STATE_DEFAULT,
	USB_DEVICE_STATE_ADDRESS,
	USB_DEVICE_STATE_CONFIGURED,
} usb_device_state_t;

usb_device_state_t usb_device_get_state();

void usb_device_endpoint_halt(uint8_t endpoint_number);

void usb_device_endpoint_resume(uint8_t endpoint_number);

bool usb_device_endpoint_is_halted(uint8_t endpoint_number);

void usb_device_on();

void usb_device_off();

void usb_device_suspend();

void usb_device_resume();

void usb_device_configure_endpoints();


#endif /* USB_DEVICE_H_ */
