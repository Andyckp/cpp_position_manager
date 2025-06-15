# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread -arch arm64

# Directories
COMMON_SRC = common/src/main/cpp
TRADEQUEUE_SRC = tradequeuemanager/src/main/cpp
PROCESSOR_SRC = processor/src/main/cpp
PUBLISHER_SRC = publisher/src/main/cpp
BUILD_DIR = build
BIN_DIR = bin

# Executables
TRADEQUEUE_EXEC = $(BIN_DIR)/tradequeuemanager
PROCESSOR_EXEC = $(BIN_DIR)/processor
PUBLISHER_EXEC = $(BIN_DIR)/publisher

# Source files
TRADEQUEUE_SRCS = $(wildcard $(TRADEQUEUE_SRC)/*.cpp)
PROCESSOR_SRCS = $(wildcard $(PROCESSOR_SRC)/*.cpp)
PUBLISHER_SRCS = $(wildcard $(PUBLISHER_SRC)/*.cpp)
COMMON_HEADERS = $(wildcard $(COMMON_SRC)/*.h)  # Header-only files

# Object files
TRADEQUEUE_OBJS = $(patsubst $(TRADEQUEUE_SRC)/%.cpp, $(BUILD_DIR)/%.o, $(TRADEQUEUE_SRCS))
PROCESSOR_OBJS = $(patsubst $(PROCESSOR_SRC)/%.cpp, $(BUILD_DIR)/%.o, $(PROCESSOR_SRCS))
PUBLISHER_OBJS = $(patsubst $(PUBLISHER_SRC)/%.cpp, $(BUILD_DIR)/%.o, $(PUBLISHER_SRCS))

# Default rule
all: $(TRADEQUEUE_EXEC) $(PROCESSOR_EXEC) $(PUBLISHER_EXEC)

# Compile tradequeuemanager source files into object files
$(BUILD_DIR)/%.o: $(TRADEQUEUE_SRC)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile processor source files into object files
$(BUILD_DIR)/%.o: $(PROCESSOR_SRC)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile publisher source files into object files
$(BUILD_DIR)/%.o: $(PUBLISHER_SRC)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link tradequeuemanager with common headers
$(TRADEQUEUE_EXEC): $(TRADEQUEUE_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(TRADEQUEUE_OBJS) -o $@

# Link processor with common headers
$(PROCESSOR_EXEC): $(PROCESSOR_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(PROCESSOR_OBJS) -o $@

# Link publisher with common headers
$(PUBLISHER_EXEC): $(PUBLISHER_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(PUBLISHER_OBJS) -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run executables
run-tradequeue: $(TRADEQUEUE_EXEC)
	./$(TRADEQUEUE_EXEC)

run-processor: $(PROCESSOR_EXEC)
	./$(PROCESSOR_EXEC)

run-publisher: $(PUBLISHER_EXEC)
	./$(PUBLISHER_EXEC)

# Print all files used in the build
print-files:
	@echo "Common Headers: $(COMMON_HEADERS)"
	@echo "TradeQueueManager Sources: $(TRADEQUEUE_SRCS)"
	@echo "Processor Sources: $(PROCESSOR_SRCS)"
	@echo "Publisher Sources: $(PUBLISHER_SRCS)"

