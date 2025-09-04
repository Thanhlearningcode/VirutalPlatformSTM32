
HOME_PATH     ?= $(shell pwd | sed "s#/DMA##")
BUS_PATH      ?= ${HOME_PATH}/Common/BusInterface
NAME_PATH     ?= ${HOME_PATH}/NameController
PORT_PATH     ?= ${HOME_PATH}/Common/Port
REGISTER_PATH ?= ${HOME_PATH}/Common/Resigter

INC_FLAGS ?= -I. -I${BUS_PATH} -I${NAME_PATH} -I${PORT_PATH} -I${REGISTER_PATH}
GCC ?= g++
OBJS = DMA.o DMA_Resigter.o Uart.o Timer.o I2C.o SPI.o SRAM.o \
	BusInterface.o PeripheralRegistry.o Resigter.o

all: $(OBJS)

test: $(OBJS) test_dma.cpp
	$(GCC) $(INC_FLAGS) -std=c++14 -pthread test_dma.cpp $(OBJS) -o test_dma

run-test: test
	./test_dma

%.o: %.cpp
	$(GCC) $(INC_FLAGS) -std=c++14 -pthread -c $< -o $@

clean:
	rm -f *.o

# Build common objects into current directory
BusInterface.o: ${BUS_PATH}/BusInterface.cpp
	$(GCC) $(INC_FLAGS) -std=c++14 -pthread -c $< -o $@

PeripheralRegistry.o: ${BUS_PATH}/PeripheralRegistry.cpp
	$(GCC) $(INC_FLAGS) -std=c++14 -pthread -c $< -o $@

Resigter.o: ${REGISTER_PATH}/Resigter.cpp
	$(GCC) $(INC_FLAGS) -std=c++14 -pthread -c $< -o $@
