CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
CXXTFLAGS = -std=c++17 -I/opt/homebrew/opt/googletest/include -L/opt/homebrew/opt/googletest/lib
TARGET = ./target/flowlog_tagger
SRC = ./src/flowlog_tagger.cpp
MAIN = ./src/main.cpp
SRCTEST = ./src/test/test_load_protocol_map.cpp
TEST = ./target/test_runner
LGFLAGS = -lgtest -lgtest_main -pthread
CD = cd target/
RUN = ./flowlog_tagger
TAG_COUNT = ./output/tag_count.csv
PORT_COUNT = ./output/port_count.csv


all: $(TARGET)
	$(CD) && $(RUN)

test: $(TEST)
		$(TEST)

$(TARGET): $(MAIN) $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ -v

$(TEST): $(SRCTEST) $(SRC)
	$(CXX) $(CXXTFLAGS) $(SRCTEST) $(SRC) $(LGFLAGS) -o $@ -v

clean:
	rm -f $(TARGET) $(TEST)
	rm -f $(TAG_COUNT) $(PORT_COUNT) 

