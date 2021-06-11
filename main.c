#include <stdbool.h>

#include <avr/interrupt.h>

#include "board.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_drv.h"
#include "usb_hid.h"
#include "usb_standard.h"
#include "usb_transfer.h"


static enum {
	IDLE,
	HOLDOFF,
	READ,
} throttle_read_state = IDLE;
uint8_t throttle_data[5] = {};
bool throttle_connected = false;

static volatile uint8_t flag_timer_0 = 0;
static volatile uint8_t flag_timer_1 = 0;
static volatile uint8_t flag_int_0 = 0;

// USB General Interrupt
// This ISR is unused but still required, since the wakeup interrupt is triggered to recover the device from a "Suspend" state.
ISR(USB_GEN_vect) {
	
}

// Timer/Counter0 Output Compare Match A
ISR(TIMER0_COMPA_vect) {
	flag_timer_0 = 1;
}

// Timer/Counter1 Overflow
// Serves as a watchdog which overflows if INT0 does not fire for too long.
ISR(TIMER1_OVF_vect) {
	flag_timer_1 = 1;
}

// External Interrupt 0
// Triggered when signal THR_RDY transitions to 0.
ISR(INT0_vect) {
	flag_int_0 = 1;
}

int main(void)
{
	board_init();
	
	sei();
	
	while (1) {
		cli();
		
		// /////////////////
		// BEGIN USB_GENERAL
		// /////////////////
		// Suspend Interrupt
		if (usb_suspend_check()) {
			usb_suspend_clear();
				
			usb_device_suspend();
		}
			
		// Wakeup Interrupt
		if (usb_wakeup_check()) {
			usb_wakeup_clear();
				
			usb_device_resume();
		}
			
		// End of Reset
		if (usb_end_of_reset_check()) {
			usb_end_of_reset_clear();
				
			// Re-configure endpoints after "End of Reset" event
			// According to the datasheet, sec. 21.4, << The default control endpoint remains configured >>.
			// However, this appears to be incorrect. Thus, ALL endpoints, even EP0, must be re-configured after a reset event.
			usb_device_configure_endpoints();
		}
		// ///////////////
		// END USB_GENERAL
		// ///////////////
		
		
		// //////////////////
		// BEGIN USB_ENDPOINT
		// //////////////////
		// EP0: Control Transfers
		usb_endpoint_select(0);
		if (usb_setup_check()) {
			
			// Read the raw data into a USB Setup packet
			usb_setup_packet_u packet_u;
			for (uint8_t i = 0; i < 8; ++i) {
				packet_u.bytes[i] = usb_read(0);
			}
			usb_setup_clear();

			switch ((packet_u.packet.bmRequestType & USB_REQUEST_TYPE_TYPE) >> 5) {
				case USB_REQUEST_TYPE_STANDARD:
					usb_standard_handle_request(packet_u);
					break;
				case USB_REQUEST_TYPE_CLASS:
					usb_hid_handle_request(packet_u);
					break;
				default:
					usb_stall();
					break;
			}
		}
		
		// EP1: HID Reports
		usb_endpoint_select(1);
		if (usb_in_check()) {
			usb_transaction_interrupt_in(1, usb_hid_report, sizeof(usb_hid_report));
			usb_hid_data_fresh = false;
		}
		// ////////////////
		// END USB_ENDPOINT
		// ////////////////
		
		
		// /////////////////////
		// BEGIN TIMER/COUNTER 0
		// /////////////////////
		if (flag_timer_0) {
			flag_timer_0 = 0;

			// advance to "READ"
			if (throttle_read_state == HOLDOFF) {
				throttle_read_state = READ;
			}
		}
		// ///////////////////
		// END TIMER/COUNTER 0
		// ///////////////////
		
		
		// /////////////////////
		// BEGIN TIMER/COUNTER 1
		// /////////////////////
		if (flag_timer_1) {
			flag_timer_1 = 0;
			
			if (throttle_connected) {
				throttle_connected = false;
				
				board_led_disconnected();

				timer_1_stop();
				
				usb_device_off();
			}
		}
		// ///////////////////
		// END TIMER/COUNTER 1
		// ///////////////////
		
		
		// //////////
		// BEGIN INT0
		// //////////
		if (flag_int_0) {
			flag_int_0 = 0;

			// advance to "HOLDOFF"
			if (throttle_read_state == IDLE) {
				throttle_read_state = HOLDOFF;
				timer_0_start_holdoff();
			}

			if (!throttle_connected) {
				throttle_connected = true;
			
				board_led_connected();
				
				usb_device_on();
			}

			// restart connection status watchdog
			timer_1_stop();
			timer_1_start();
		}
		// ////////
		// END INT0
		// ////////
		
		
		// ///////////////////
		// BEGIN THROTTLE_READ
		// ///////////////////
		if (throttle_read_state == READ) {
			// bit-bang throttle read
			timer_0_start_read();

			board_mask_int_0();
			sei();

			for (int i = 0; i < 5; ++i) {
				throttle_data[i] = 0;
				for (int j = 0; j < 8; ++j) {
					board_throttle_set_clock_low();
					while (!flag_timer_0);
					flag_timer_0 = 0;

					board_throttle_set_clock_high();
					throttle_data[i] |= (board_throttle_read_bit() << j);
					while (!flag_timer_0);
					flag_timer_0 = 0;
				}
			}

			board_unmask_int_0();
			cli();
			
			// copy the fresh data into the USB HID report buffer
			for (uint8_t i = 0; i < 5; ++i) {
				usb_hid_report[i] = throttle_data[i];
			}

			// advance to "IDLE"
			throttle_read_state = IDLE;
			timer_0_stop();
		}
		// /////////////////
		// END THROTTLE_READ
		// /////////////////
		
		sei();
	}
}
