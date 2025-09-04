# Home directory
HOME_PATH ?= $(shell pwd | sed "s/common.*$$//")
# Model name
MODEL ?= BusInterface
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
# Compile flags
FLAGS ?=
# Source list
SCRS = BusInterface.cpp
# Object list
OBJS = lib-${MODEL}/BusInterface.o
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

${OBJS}: ${SCRS}
	${GCC} ${FLAGS} -c $< -o $@
# Clean rule
clean:
	rm -rf lib-${MODEL} *.a