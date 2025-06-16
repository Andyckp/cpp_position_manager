# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread -arch arm64

# Directories
COMMON_SRC = common/src/main/cpp
TRADEQUEUE_SRC = tradequeuemanager/src/main/cpp
POSITIONMANAGER_SRC = positionmanager/src/main/cpp
TRADEPUBLISHER_SRC = tradepublisher/src/main/cpp
POSITIONQUEUE_SRC = positionqueuemanager/src/main/cpp
POSITION_SUBSCRIBER_SRC = positionsubscriber/src/main/cpp
BUILD_DIR = build
BIN_DIR = bin

# Executables
TRADEQUEUE_EXEC = $(BIN_DIR)/tradequeuemanager
POSITIONMANAGER_EXEC = $(BIN_DIR)/positionmanager
TRADEPUBLISHER_EXEC = $(BIN_DIR)/tradepublisher
POSITIONQUEUE_EXEC = $(BIN_DIR)/positionqueuemanager
POSITION_SUBSCRIBER_EXEC = $(BIN_DIR)/positionsubscriber

# Source files
TRADEQUEUE_SRCS = $(wildcard $(TRADEQUEUE_SRC)/*.cpp)
POSITIONMANAGER_SRCS = $(wildcard $(POSITIONMANAGER_SRC)/*.cpp)
TRADEPUBLISHER_SRCS = $(wildcard $(TRADEPUBLISHER_SRC)/*.cpp)
POSITIONQUEUE_SRCS = $(wildcard $(POSITIONQUEUE_SRC)/*.cpp)
POSITION_SUBSCRIBER_SRCS = $(wildcard $(POSITION_SUBSCRIBER_SRC)/*.cpp)
COMMON_HEADERS = $(wildcard $(COMMON_SRC)/*.h)  # Header-only files

# Object files
TRADEQUEUE_OBJS = $(patsubst $(TRADEQUEUE_SRC)/%.cpp, $(BUILD_DIR)/%.o, $(TRADEQUEUE_SRCS))
POSITIONMANAGER_OBJS = $(patsubst $(POSITIONMANAGER_SRC)/%.cpp, $(BUILD_DIR)/%.o, $(POSITIONMANAGER_SRCS))
TRADEPUBLISHER_OBJS = $(patsubst $(TRADEPUBLISHER_SRC)/%.cpp, $(BUILD_DIR)/%.o, $(TRADEPUBLISHER_SRCS))
POSITIONQUEUE_OBJS = $(patsubst $(POSITIONQUEUE_SRC)/%.cpp, $(BUILD_DIR)/%.o, $(POSITIONQUEUE_SRCS))
POSITION_SUBSCRIBER_OBJS = $(patsubst $(POSITION_SUBSCRIBER_SRC)/%.cpp, $(BUILD_DIR)/%.o, $(POSITION_SUBSCRIBER_SRCS))

# Default rule
all: $(TRADEQUEUE_EXEC) $(POSITIONMANAGER_EXEC) $(TRADEPUBLISHER_EXEC) $(POSITIONQUEUE_EXEC) $(POSITION_SUBSCRIBER_EXEC)

# Compile tradequeuemanager source files into object files
$(BUILD_DIR)/%.o: $(TRADEQUEUE_SRC)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile positionmanager source files into object files
$(BUILD_DIR)/%.o: $(POSITIONMANAGER_SRC)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile tradepublisher source files into object files
$(BUILD_DIR)/%.o: $(TRADEPUBLISHER_SRC)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile positionqueuemanager source files into object files
$(BUILD_DIR)/%.o: $(POSITIONQUEUE_SRC)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile positionsubscriber source files into object files
$(BUILD_DIR)/%.o: $(POSITION_SUBSCRIBER_SRC)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link tradequeuemanager with common headers
$(TRADEQUEUE_EXEC): $(TRADEQUEUE_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(TRADEQUEUE_OBJS) -o $@

# Link positionmanager with common headers
$(POSITIONMANAGER_EXEC): $(POSITIONMANAGER_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(POSITIONMANAGER_OBJS) -o $@

# Link tradepublisher with common headers
$(TRADEPUBLISHER_EXEC): $(TRADEPUBLISHER_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(TRADEPUBLISHER_OBJS) -o $@

# Link positionqueuemanager with common headers
$(POSITIONQUEUE_EXEC): $(POSITIONQUEUE_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(POSITIONQUEUE_OBJS) -o $@

# Link positionsubscriber with common headers
$(POSITION_SUBSCRIBER_EXEC): $(POSITION_SUBSCRIBER_OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(POSITION_SUBSCRIBER_OBJS) -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run executables
run-tradequeue: $(TRADEQUEUE_EXEC)
	./$(TRADEQUEUE_EXEC)

run-positionmanager: $(POSITIONMANAGER_EXEC)
	./$(POSITIONMANAGER_EXEC)

run-tradepublisher: $(TRADEPUBLISHER_EXEC)
	./$(TRADEPUBLISHER_EXEC)

run-positionqueue: $(POSITIONQUEUE_EXEC)
	./$(POSITIONQUEUE_EXEC)

run-positionsubscriber: $(POSITION_SUBSCRIBER_EXEC)
	./$(POSITION_SUBSCRIBER_EXEC)

# Print all files used in the build
print-files:
	@echo "Common Headers: $(COMMON_HEADERS)"
	@echo "TradeQueueManager Sources: $(TRADEQUEUE_SRCS)"
	@echo "PositionManager Sources: $(POSITIONMANAGER_SRCS)"
	@echo "TradePublisher Sources: $(TRADEPUBLISHER_SRCS)"
	@echo "PositionQueueManager Sources: $(POSITIONQUEUE_SRCS)"
	@echo "PositionsSubscriber Sources: $(POSITION_SUBSCRIBER_SRCS)"
