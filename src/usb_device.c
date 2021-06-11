#include "usb_device.h"

uint8_t usb_device_configuration = 0;

uint8_t usb_device_interface = 0;

static bool endpoints_halted[2] = {
	false,
	false,	
};

usb_device_state_t usb_device_get_state() {
	if (UDADDR & (1 << ADDEN)) {
		if (usb_device_configuration != 0U) {
			return USB_DEVICE_STATE_CONFIGURED;
		}
		return USB_DEVICE_STATE_ADDRESS;
	}
	return USB_DEVICE_STATE_DEFAULT;
}

void usb_device_endpoint_halt(uint8_t endpoint_number) {
	endpoints_halted[endpoint_number] = true;
}

void usb_device_endpoint_resume(uint8_t endpoint_number) {
	endpoints_halted[endpoint_number] = false;
}

bool usb_device_endpoint_is_halted(uint8_t endpoint_number) {
	return endpoints_halted[endpoint_number];
}

void usb_device_on() {
	// enable USB pad regulator
	usb_regulator_enable();
	
	// configure and enable PLL
	usb_pll_enable();
	
	// wait for PLL lock
	usb_pll_poll();
	
	// enable USB controller
	usb_enable();
	
	// unfreeze USB clock
	usb_clock_unfreeze();
	
	// configure endpoints
	usb_device_configure_endpoints();
	
	// wait for VBUS
	usb_poll_vbus();
	
	// attach to USB bus
	usb_attach();
}

void usb_device_off() {
	// detach from USB bus
	usb_detach();
	
	// disable USB interface
	usb_disable();
	
	// disable PLL
	usb_pll_disable();
	
	// disable USB pad regulator
	usb_regulator_disable();
}

void usb_device_suspend() {	
	// freeze USB clock
	usb_clock_freeze();
	
	// disable PLL
	usb_pll_disable();
	
	// enable wakeup interrupt
	usb_wakeup_enable();
	
	// enter sleep mode
	SMCR = (1 << SM2) | (1 << SM1) | (1 << SM0);
	sleep_cpu();
}

void usb_device_resume() {
	// disable wakeup interrupt
	usb_wakeup_disable();
	
	// enable PLL
	usb_pll_enable();
	
	// wait for PLL lock
	usb_pll_poll();

	// unfreeze USB clock
	usb_clock_unfreeze();
}

void usb_device_configure_endpoints() {
	// EP0
	// Control, 64 bytes, one bank
	usb_endpoint_select(0);
	UECONX |= (1 << EPEN); // enable endpoint
	UECFG0X |= (0 << EPTYPE1) | (0 << EPTYPE0) | (0 << EPDIR);	// control type
	UECFG1X |= (0 << EPSIZE2) | (1 << EPSIZE1) | (1 << EPSIZE0) | (0 << EPBK1) | (0 << EPBK0) | (1 << ALLOC);
	
	// EP1
	// Interrupt, 64 bytes, one bank
	usb_endpoint_select(1);
	UECONX |= (1 << EPEN); // enable endpoint
	UECFG0X |= (1 << EPTYPE1) | (1 << EPTYPE0) | (1 << EPDIR); // interrupt type, IN
	UECFG1X |= (0 << EPSIZE2) | (1 << EPSIZE1) | (1 << EPSIZE0) | (0 << EPBK1) | (0 << EPBK0) | (1 << ALLOC);
}
