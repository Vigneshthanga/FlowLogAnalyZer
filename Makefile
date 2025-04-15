CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
CXXTFLAGS = -std=c++17 -I/opt/homebrew/opt/googletest/include -L/opt/homebrew/opt/googletest/lib
TARGET = ./target/flowlog_tagger
SRC = ./src/flowlog_tagger.cpp
MAIN = ./src/main.cpp
SRCTEST = ./src/test/test_flowlog_tagger.cpp
TEST = ./target/test_runner
LGFLAGS = -lgtest -lgtest_main -pthread
CD = cd target/
RUN = ./flowlog_tagger
TAG_COUNT = ./output/tag_count.csv
PORT_COUNT = ./output/port_count.csv
ARGS = ../src/specifications/protocol-numbers-1.csv ../src/generators/unique_random_lookup_table_100k.csv ../src/generators/vpc_sample_logs_500k.txt
FTARGS = ../src/specifications/protocol-numbers-1.csv ../src/generators/ft_lookup_table.csv ../src/generators/ft_sample_logs.csv

all: $(TARGET)
	$(CD) && $(RUN) $(ARGS)

test: $(TEST)
		$(TEST)

functest: $()
		$(CD) && $(RUN) $(FTARGS)

$(TARGET): $(MAIN) $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ -v

$(TEST): $(SRCTEST) $(SRC)
	$(CXX) $(CXXTFLAGS) $(SRCTEST) $(SRC) $(LGFLAGS) -o $@ -v

clean:
	rm -f $(TARGET) $(TEST)
	rm -f $(TAG_COUNT) $(PORT_COUNT) 

