flash:
	avrdude -B 5 -c usbasp -p m328p -U flash:w:/home/dirk/emb_workspace/Atmega328_oscilloscope/.pio/build/ATmega328/firmware.hex
