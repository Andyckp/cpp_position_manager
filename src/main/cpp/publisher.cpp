// publisher.cpp
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "trade_ring_buffer.h"

constexpr const char* SHM_NAME = "/trade_ring_buffer_shm";

int main() {
    // Open the shared memory object.
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(shm_fd < 0) {
        perror("shm_open");
        return 1;
    }
    
    size_t shm_size = sizeof(TradeRingBuffer);
    void* ptr = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    TradeRingBuffer* ringBuffer = static_cast<TradeRingBuffer*>(ptr);
    
    while(true) {
        // Wait for 1ms between generating trades.
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        // Read the current write and read indices.
        size_t currentWrite = ringBuffer->writeIndex.load(std::memory_order_relaxed);
        size_t nextWrite = (currentWrite + 1) % BUFFER_SIZE;
        size_t currentRead = ringBuffer->readIndex.load(std::memory_order_acquire);

        // Check for buffer full condition: do not overwrite an unread slot.
        if (nextWrite == currentRead) {
            // Buffer is full, so skip publishing until the consumer catches up.
            continue;
        }
        
        // Generate a random trade.
        TradeEvent trade;
        trade.instrument_id = rand() % 1000;           // Random instrument id.
        trade.price = (rand() % 10000) / 100.0;         // Random price between 0.0 and 100.00.
        trade.quantity = (rand() % 1000) / 10.0;        // Random quantity.
        
        // Write the trade into the current slot.
        ringBuffer->events[currentWrite] = trade;
        
        // Publish: update the write index (release order ensures the write is visible).
        ringBuffer->writeIndex.store(nextWrite, std::memory_order_release);
    }
    
    munmap(ptr, shm_size);
    close(shm_fd);
    return 0;
}
