/************************************************************************/
/* Low-level functions for accessing the USB peripheral registers.      */
/************************************************************************/

#ifndef USB_DRV_H_
#define USB_DRV_H_

#include <stdbool.h>

#include <avr/io.h>

#if defined(__AVR_ATmega8U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__)
	#define __AVR_SERIES_2__
#elif defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
	#define __AVR_SERIES_4__
#else
	#error "This code only works on ATmegaXXU2 / ATmegaXXU4"
#endif


__attribute__((always_inline)) static inline void usb_poll_vbus() {
	#if defined(__AVR_SERIES_4__)
		while (!(USBSTA & (1 << VBUS)));
	#endif
}

__attribute__((always_inline)) static inline void usb_enable() {
	USBCON |= (1 << USBE);
}

__attribute__((always_inline)) static inline void usb_disable() {
	USBCON &= ~(1 << USBE);
}

__attribute__((always_inline)) static inline void usb_clock_freeze() {
	USBCON |= (1 << FRZCLK);
}

__attribute__((always_inline)) static inline void usb_clock_unfreeze() {
	USBCON &= ~(1 << FRZCLK);
}

__attribute__((always_inline)) static inline void usb_regulator_enable() {
	#if defined(__AVR_SERIES_4__)
		UHWCON |= (1 << UVREGE);
		USBCON |= (1 << OTGPADE);
	#else
		REGCR &= ~(1 << REGDIS);
	#endif
}

__attribute__((always_inline)) static inline void usb_regulator_disable() {
	#if defined(__AVR_SERIES_4__)
		USBCON &= ~(1 << OTGPADE);
		UHWCON &= ~(1 << UVREGE);
	#else
		REGCR |= (1 << REGDIS);
	#endif
}

__attribute__((always_inline)) static inline void usb_attach() {
	UDCON &= (~(1 << DETACH));
}

__attribute__((always_inline)) static inline void usb_detach() {
	UDCON |= (1 << DETACH);
}

__attribute__((always_inline)) static inline void usb_pll_enable() {
	#ifndef PINDIV
		// PINDIV seems to be missing from headers
		#define PINDIV 2
	#endif
	PLLCSR |= (1 << PINDIV); // 16MHz system clock, enable divide-by-2
	PLLCSR |= (1 << PLLE);
}

__attribute__((always_inline)) static inline void usb_pll_disable() {
	PLLCSR &= ~(1 << PLLE);
}

__attribute__((always_inline)) static inline void usb_pll_poll() {
	while (!(PLLCSR & (1 << PLOCK)));
}

__attribute__((always_inline)) static inline void usb_stall() {
	UECONX |= (1 << STALLRQ);
}

__attribute__((always_inline)) static inline void usb_setup_clear() {
	UEINTX &= ~(1 << RXSTPI);
}

__attribute__((always_inline)) static inline bool usb_setup_check() {
	return (bool) (UEINTX & (1 << RXSTPI));
}

__attribute__((always_inline)) static inline void usb_out_clear() {
	UEINTX &= ~(1 << RXOUTI);
}

__attribute__((always_inline)) static inline bool usb_out_check() {
	return (bool) (UEINTX & (1 << RXOUTI));
}

__attribute__((always_inline)) static inline void usb_out_poll() {
	while (!(UEINTX & (1 << RXOUTI)));
}

__attribute__((always_inline)) static inline void usb_in_clear() {
	UEINTX &= ~(1 << TXINI);
}

__attribute__((always_inline)) static inline bool usb_in_check() {
	return (bool) (UEINTX & (1 << TXINI));
}

__attribute__((always_inline)) static inline void usb_in_poll() {
	while (!(UEINTX & (1 << TXINI)));
}

__attribute__((always_inline)) static inline void usb_fifo_clear() {
	UEINTX &= ~(1 << FIFOCON);
}

__attribute__((always_inline)) static inline void usb_endpoint_select(uint8_t endpoint_number) {
	UENUM = (0b111 & endpoint_number);
}

__attribute__((always_inline)) static inline uint16_t usb_endpoint_byte_count(uint8_t endpoint) {
	usb_endpoint_select(endpoint);
	#if defined(__AVR_SERIES_4__)
		return (uint16_t) ((UEBCHX << 8) | UEBCLX);
	#else
		return (uint16_t) UEBCLX;
	#endif
}

__attribute__((always_inline)) static inline uint8_t usb_read(uint8_t endpoint) {
	usb_endpoint_select(endpoint);
	return UEDATX;
}

__attribute__((always_inline)) static inline void usb_write(uint8_t endpoint, uint8_t byte) {
	usb_endpoint_select(endpoint);
	UEDATX = byte;
}

__attribute__((always_inline)) static inline void usb_wakeup_clear() {
	UDINT &= ~(1 << WAKEUPI);
}

__attribute__((always_inline)) static inline void usb_wakeup_enable() {
	usb_wakeup_clear();
	UDIEN |= (1 << WAKEUPE);
}

__attribute__((always_inline)) static inline void usb_wakeup_disable() {
	UDIEN &= ~(1 << WAKEUPE);
}

__attribute__((always_inline)) static inline bool usb_wakeup_check() {
	return (bool) (UDINT & (1 << WAKEUPI));
}

__attribute__((always_inline)) static inline void usb_suspend_clear() {
	UDINT &= ~(1 << SUSPI);
}

__attribute__((always_inline)) static inline void usb_suspend_enable() {
	usb_suspend_clear();
	UDIEN |= (1 << SUSPE);
}

__attribute__((always_inline)) static inline void usb_suspend_disable() {
	UDIEN &= ~(1 << SUSPE);
}

__attribute__((always_inline)) static inline bool usb_suspend_check() {
	return (bool) (UDINT & (1 << SUSPI));
}

__attribute__((always_inline)) static inline void usb_end_of_reset_clear() {
	UDINT &= ~(1 << EORSTI);
}

__attribute__((always_inline)) static inline void usb_end_of_reset_enable() {
	usb_end_of_reset_clear();
	UDIEN |= (1 << EORSTE);
}

__attribute__((always_inline)) static inline void usb_end_of_reset_disable() {
	UDIEN &= ~(1 << EORSTE);
}

__attribute__((always_inline)) static inline bool usb_end_of_reset_check() {
	return (bool) (UDINT & (1 << EORSTI));
}

__attribute__((always_inline)) static inline void usb_start_of_frame_clear() {
	UDINT &= ~(1 << SOFI);
}

__attribute__((always_inline)) static inline void usb_start_of_frame_enable() {
	usb_start_of_frame_clear();
	UDIEN |= (1 << SOFE);
}

__attribute__((always_inline)) static inline void usb_start_of_frame_disable() {
	UDIEN &= ~(1 << SOFE);
}

__attribute__((always_inline)) static inline bool usb_start_of_frame_check() {
	return (bool) (UDINT & (1 << SOFI));
}


#endif /* USB_DRV_H_ */
