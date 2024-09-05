CXX = g++
CXXFLAGS = -std=c++14 -g -Wall -MMD -Werror=vla
OBJECTS = piece.o display.o chessmodule.o server.o client.o main.o chessexception.o subject.o chessstrategy.o level1.o level2.o level3.o level4.o window.o graphicsdisplay.o
BUILD_DIR = build
BUILD_OBJECTS = $(OBJECTS:%=$(BUILD_DIR)/%)
BUILD_DEPENDS = $(BUILD_OBJECTS:%.o=%.d)
EXEC = chess

all: $(EXEC)

$(EXEC): $(BUILD_OBJECTS)
	$(CXX) $(CXXFLAGS) $(BUILD_OBJECTS) -o $(EXEC)

$(BUILD_DIR)/%.o: %.cc | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DEPENDS) $(BUILD_OBJECTS) $(EXEC)

-include $(BUILD_DEPENDS)
