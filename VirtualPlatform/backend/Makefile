CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
TARGET = vp_backend
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
