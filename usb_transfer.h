/*****************************************************************************/
/* Contains the USB protocol layer, e.g. control and interrupt transactions. */
/*****************************************************************************/

#ifndef USB_TRANSFER_H_
#define USB_TRANSFER_H_

#include <stdint.h>
#include <stddef.h>

#include "usb_device.h"


uint16_t usb_transfer_control_read(const uint8_t* buf, size_t len, size_t requested);

uint16_t usb_transfer_control_write(uint8_t* buf, size_t len);

uint16_t usb_transaction_interrupt_in(uint8_t endpoint, uint8_t* buf, size_t len);


#endif /* USB_TRANSFER_H_ */
