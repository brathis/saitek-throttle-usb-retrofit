/**************************************************************************************/
/* Contains all low-level code for interacting with the throttle and the status LEDs. */
/**************************************************************************************/
#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#define PIND_THR_DAT  2			// Leonardo: D0
#define PIND_THR_CLK  1			// Leonardo: D2
#define PIND_THR_RDY  0			// Leonardo: D3
#define PIND_LED_GRN  3			// Leonardo: D1
#define PIND_LED_RED  4			// Leonardo: D4


__attribute__((always_inline)) static inline void board_init() {
	// Throttle and LED pins
	DDRD = (1 << PIND_THR_CLK) | (1 << PIND_LED_GRN) | (1 << PIND_LED_RED);
	PORTD = (1 << PIND_LED_RED) | (1 << PIND_THR_CLK);

	// INT0 on falling edge
	EICRA = (1 << ISC01);
	EIMSK = (1 << INT0);
}

__attribute__((always_inline)) static inline void board_mask_int_0() {
	EIMSK = 0;
	EIFR = (1 << INTF0);
}

__attribute__((always_inline)) static inline void board_unmask_int_0() {
	EIFR = (1 << INTF0);
	EIMSK = (1 << INT0);
}

__attribute__((always_inline)) static inline void board_led_connected() {
	PORTD |= (1 << PIND_LED_GRN);
	PORTD &= ~(1 << PIND_LED_RED);
}

__attribute__((always_inline)) static inline void board_led_disconnected() {
	PORTD &= ~(1 << PIND_LED_GRN);
	PORTD |= (1 << PIND_LED_RED);
}

__attribute__((always_inline)) static inline void board_throttle_set_clock_high() {
	PORTD |= (1 << PIND_THR_CLK);
}

__attribute__((always_inline)) static inline void board_throttle_set_clock_low() {
	PORTD &= ~(1 << PIND_THR_CLK);
}

__attribute__((always_inline)) static inline uint8_t board_throttle_read_bit() {
	return (PIND & (1 << PIND_THR_DAT)) ? 1 : 0;
}

__attribute__((always_inline)) static inline void timer_0_stop() {
	TIMSK0 = 0;
	TCCR0B = 0;
	TCNT0 = 0;
	TIFR0 = (1 << OCF0A);
}

/************************************************************************/
/* Start Timer/Counter0 in CTC mode with frequency 525 Hz (1.9 ms).     */
/************************************************************************/
__attribute__((always_inline)) static inline void timer_0_start_holdoff() {
	timer_0_stop();

	OCR0A = 119;
	TIMSK0 = (1 << OCIE0A); // enable Timer/Counter0 Output Compare Match A interrupt
	TCCR0A = (1 << WGM01); // CTC, normal operation
	TCCR0B = (1 << CS02); // /256 pre-scaler
}

/************************************************************************/
/* Start Timer/Counter0 in CTC mode with frequency 50 kHz (20 us).      */
/************************************************************************/
__attribute__((always_inline)) static inline void timer_0_start_read() {
	timer_0_stop();

	OCR0A = 5;
	TIMSK0 = (1 << OCIE0A); // enable Timer/Counter0 Output Compare Match A interrupt
	TCCR0A = (1 << WGM01); // CTC, normal operation
	TCCR0B = (1 << CS01) | (1 << CS00); // /64 pre-scaler
}

__attribute__((always_inline)) static inline void timer_1_stop() {
	TIMSK1 = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	TIFR1 = (1 << TOV1);
}

/************************************************************************/
/* Start Timer/Counter1 in overflow mode with frequency 3.8 Hz (262 ms).*/
/************************************************************************/
__attribute__((always_inline)) static inline void timer_1_start() {
	TIFR1 = (1 << TOV1);
	TIMSK1 = (1 << TOIE1);
	TCCR1B = (1 << CS11) | (1 << CS10);
}


#endif /* BOARD_H_ */
