# Home directory
HOME_PATH     ?= $(shell pwd | sed "s/ProjectSTM32Virtual.*$$/ProjectSTM32Virtual/")
COMMON_PATH   ?= ${HOME_PATH}/common
BUS_PATH      ?= ${COMMON_PATH}/BusInterface
NAME_PATH     ?= ${COMMON_PATH}/NameController
PORT_PATH     ?= ${COMMON_PATH}/Port
REGISTER_PATH ?= ${COMMON_PATH}/Register
PYTHON_PATH   ?= /mnt/d/DATA_TRAIN_2025/Build_Template_Model/Lib/Python-2.7.18/Include
# Model name
MODEL ?= ARM_M3
# Target module
TARGET ?= lib-${MODEL}/${MODEL}.a
# Make version
MAKE ?= make
# Compiler
GCC ?= g++
# Archive command
AR ?= ar
# Archive option
AR_OPT ?= cqs
# Include flags
INC_FLAGS ?= -I. -I${BUS_PATH} -I${NAME_PATH} -I${PORT_PATH} -I${REGISTER_PATH} -I${PYTHON_PATH}
# Compile flags
FLAGS ?= ${INC_FLAGS}
# Source list
SCRS = Cortex_M3.cpp \
	   Python_Cortex_M3.cpp
# Object list
OBJS = lib-${MODEL}/Cortex_M3.o \
	   lib-${MODEL}/Python_Cortex_M3.o
# Phony rule
.PHONY: all clean compile

all:
	@echo ${HOME_PATH}
	if [ -e lib-${MODEL} ]; then rm -rf lib-${MODEL}; mkdir lib-${MODEL}; else mkdir lib-${MODEL}; fi && \
	$(MAKE) Makefile compile
# compile rule
compile: ${TARGET}
	@echo "Compile done!!!"

${TARGET}: ${OBJS}
	$(AR) $(AR_OPT) $@ $^

lib-${MODEL}/%.o: %.cpp
	${GCC} ${FLAGS} -c $< -o $@
# Clean rule
clean:
	rm -rf lib-${MODEL} *.a