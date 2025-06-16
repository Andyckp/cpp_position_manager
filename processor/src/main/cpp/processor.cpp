#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <cstring>
#include "../../../../common/src/main/cpp/data_structure.h"

constexpr const char* SHM_NAME = "/trade_ring_buffer_shm";
constexpr const char* POSITION_SHM_NAME = "/position_ring_buffer_shm";

int main() {
    // Open TradeRingBuffer shared memory object.
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        return 1;
    }

    size_t shm_size = sizeof(TradeRingBuffer);
    void* ptr = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Open PositionRingBuffer shared memory
    int position_shm_fd = shm_open(POSITION_SHM_NAME, O_RDWR, 0666);
    if (position_shm_fd < 0) {
        perror("shm_open (PositionRingBuffer)");
        return 1;
    }

    size_t position_shm_size = sizeof(PositionRingBuffer);
    void* position_ptr = mmap(nullptr, position_shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, position_shm_fd, 0);
    if (position_ptr == MAP_FAILED) {
        perror("mmap (PositionRingBuffer)");
        return 1;
    }

    TradeRingBuffer* ringBuffer = static_cast<TradeRingBuffer*>(ptr);
    PositionRingBuffer* positionRingBuffer = static_cast<PositionRingBuffer*>(position_ptr);
    std::unordered_map<PositionID, double, PositionIDHash> positionMap;

    while (true) {
        size_t currentRead = ringBuffer->readIndex.pos.load(std::memory_order_relaxed);
        size_t currentWrite = ringBuffer->writeIndex.pos.load(std::memory_order_acquire);

        if (currentRead == currentWrite) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        // Process the trade at the current read position.
        TradeEvent trade = ringBuffer->events[currentRead];
        PositionID posKey = {trade.instrument_id, {}};
        std::memcpy(posKey.portfolio_id, trade.portfolio_id, 16);
        positionMap[posKey] += trade.quantity;

        std::cout << "Processing Trade: instrument_id = " << trade.instrument_id
                  << ", price = " << trade.price
                  << ", quantity = " << trade.quantity
                  << ", portfolio_id = " << trade.portfolio_id << "\n";
        std::cout << "Updated Position Quantity: instrument_id = " << posKey.instrument_id
                  << ", portfolio_id = " << posKey.portfolio_id
                  << ", quantity = " << positionMap[posKey] << "\n";
        std::cout.flush();

        // Ensure PositionRingBuffer is not full before writing
        while (true) {
            size_t positionReadIndex = positionRingBuffer->readIndex.pos.load(std::memory_order_acquire);
            size_t positionWriteIndex = positionRingBuffer->writeIndex.pos.load(std::memory_order_relaxed);

            size_t nextPositionWriteIndex = (positionWriteIndex + 1) % BUFFER_SIZE;
            if (nextPositionWriteIndex == positionReadIndex) {
                std::cerr << "Warning: PositionRingBuffer is full, waiting before retrying...\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;  // Retry indefinitely
            }

            // Publish to PositionRingBuffer
            positionRingBuffer->events[positionWriteIndex].position_id = posKey;
            positionRingBuffer->events[positionWriteIndex].net_quantity = positionMap[posKey];
            positionRingBuffer->writeIndex.pos.store(nextPositionWriteIndex, std::memory_order_release);

            std::cout << "Published Position Update: instrument_id = " << posKey.instrument_id
                      << ", portfolio_id = " << posKey.portfolio_id
                      << ", net_quantity = " << positionMap[posKey] << "\n";
            std::cout.flush();
            break;  // Exit retry loop after successful write
        }

        // Advance the read index in TradeRingBuffer
        size_t nextRead = (currentRead + 1) % BUFFER_SIZE;
        ringBuffer->readIndex.pos.store(nextRead, std::memory_order_release);
    }

    munmap(ptr, shm_size);
    close(shm_fd);
    munmap(position_ptr, position_shm_size);
    close(position_shm_fd);
    return 0;
}
