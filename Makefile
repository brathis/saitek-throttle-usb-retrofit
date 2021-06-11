TARGET=saitek_throttle_usb_retrofit
MCU=atmega16u2
SOURCES=src/main.c src/usb_device.c src/usb_hid.c src/usb_standard.c src/usb_transfer.c src/usb.c

PROGRAMMER=atmelice_isp

OBJECTS=$(SOURCES:.c=.o)
CFLAGS=-c -Os
LDFLAGS=

all: hex

hex: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -j .data -j .text $(TARGET).elf $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	avr-gcc $(LDFLAGS) -mmcu=$(MCU) $(OBJECTS) -o $(TARGET).elf

.c.o:
	avr-gcc $(CFLAGS) -mmcu=$(MCU) $< -o $@

size:
	avr-size --mcu=$(MCU) -C $(TARGET).elf

program:
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -Uflash:w:$(TARGET).hex:a

clean:
	rm -rf src/*.o
	rm -rf *.elf
	rm -rf *.hex
