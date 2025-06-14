// processor.cpp
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <chrono>
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
        size_t currentRead = ringBuffer->readIndex.load(std::memory_order_relaxed);
        size_t currentWrite = ringBuffer->writeIndex.load(std::memory_order_acquire);
        
        if (currentRead == currentWrite) {
            // No new trade; wait briefly.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        
        // Process the trade at the current read position.
        TradeEvent trade = ringBuffer->events[currentRead];
        std::cout << "Processing Trade: instrument_id = " << trade.instrument_id
                  << ", price = " << trade.price
                  << ", quantity = " << trade.quantity << "\n";
        std::cout.flush();
        
        // Advance the read index.
        size_t nextRead = (currentRead + 1) % BUFFER_SIZE;
        ringBuffer->readIndex.store(nextRead, std::memory_order_release);
    }
    
    munmap(ptr, shm_size);
    close(shm_fd);
    return 0;
}