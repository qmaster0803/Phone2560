CONTROLLER_MODEL = atmega2560

.PHONY: all clean rebuild flash

all: build/main.o build/Display.o build/SIM_A6.o build/SD.o
	@echo "[LINK] all"
	@avr-g++ -mmcu=$(CONTROLLER_MODEL) -o build/main.elf build/*.o
	@#avr-objdump -h build/main.elf
	@avr-objcopy -R .eeprom -O ihex build/main.elf build/main.hex

build/main.o:
	@mkdir -p build/
	@echo "[ CC ] main.cc"
	@avr-g++ -std=c++17 -Os -mmcu=$(CONTROLLER_MODEL) -c main.cc -o build/main.o

build/Display.o:
	@mkdir -p build/
	@echo "[ CC ] Display.cc"
	@avr-g++ -std=c++17 -Os -mmcu=$(CONTROLLER_MODEL) -c Display.cc -o build/Display.o

build/SIM_A6.o:
	@mkdir -p build/
	@echo "[ CC ] SIM_A6.cc"
	@avr-g++ -std=c++17 -Os -mmcu=$(CONTROLLER_MODEL) -c SIM_A6.cc -o build/SIM_A6.o

build/SD.o:
	@mkdir -p build/
	@echo "[ CC ] SD.cc"
	@avr-g++ -std=c++17 -Os -mmcu=$(CONTROLLER_MODEL) -c SD.cc -o build/SD.o

clean:
	@echo "Cleaning..."
	@rm -f build/*

flash:
ifdef port
	@avrdude -c stk500v1 -P $(port) -p m2560 -U flash:w:build/main.hex:i
else
	@echo "Please specify port with 'port=/dev/xxxXXX'"
endif

rebuild: clean all