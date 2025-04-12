CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = ./target/flowlog_tagger
SRC = ./src/flowlog_tagger.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
