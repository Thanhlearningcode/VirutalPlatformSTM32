# Compiler and Flags 
CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -g

# Folder
ROOT_DIR := $(abspath ../)
SRC_DIR  := $(ROOT_DIR)/ENV_ARM
OUT_DIR  := $(SRC_DIR)/build
TARGET   := $(OUT_DIR)/main

# Paths 
DMA_DIR  := $(ROOT_DIR)/DMA
REG_DIR  := $(ROOT_DIR)/Common/Resigter
BUS_DIR  := $(ROOT_DIR)/Common/BusInterface
PORT_DIR := $(ROOT_DIR)/Common/Port
CORE_DIR := $(ROOT_DIR)/CorTex-M3

# Source Files 
SRCS := $(wildcard $(SRC_DIR)/*.cpp) \
        $(DMA_DIR)/DMA.cpp \
        $(DMA_DIR)/DMA_Resigter.cpp \
        $(REG_DIR)/Resigter.cpp

#Object Files
OBJS := $(patsubst %.cpp,$(OUT_DIR)/%.o,$(notdir $(SRCS)))

# Include 
INCLUDES := \
	-I$(BUS_DIR) \
	-I$(PORT_DIR) \
	-I$(REG_DIR) \
	-I$(CORE_DIR) \
	-I$(DMA_DIR)

# Libs
LIB_BUSINTERFACE_DIR := $(BUS_DIR)/lib-BusInterface
LIBS := $(LIB_BUSINTERFACE_DIR)/BusInterface.a

# Phony Targets 
.PHONY: all clean build_dirs

# Target 
all: build_dirs $(LIBS) $(TARGET)

#Build Output Directory 
build_dirs:
	@mkdir -p $(OUT_DIR)

#Link Executable 
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS) $(INCLUDES)

#  Compile Source 
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OUT_DIR)/%.o: $(DMA_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OUT_DIR)/%.o: $(REG_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build Static Library 
$(LIBS):
	$(MAKE) -C $(BUS_DIR)

clean:
	rm -rf $(OUT_DIR)
	$(MAKE) -C $(BUS_DIR) clean
