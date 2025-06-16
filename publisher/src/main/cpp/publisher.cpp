#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "../../../../common/src/main/cpp/data_structure.h"

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
    
    const char* sample_portfolio_ids[] = {
        "PORTFOLIO_001", "PORTFOLIO_002", "PORTFOLIO_ABC", "PORTFOLIO_XYZ"
    };

    while(true) {
        // Wait for 1ms between generating trades.
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        // Check for buffer full condition: do not overwrite an unread slot.
        size_t currentWrite = ringBuffer->writeIndex.pos.load(std::memory_order_relaxed);
        size_t nextWrite = (currentWrite + 1) % BUFFER_SIZE;
        size_t currentRead = ringBuffer->readIndex.pos.load(std::memory_order_acquire);
        if (nextWrite == currentRead) {
            continue;
        }
        
        // Generate a random trade.
        TradeEvent trade;
        trade.instrument_id = rand() % 1000;           
        trade.price = (rand() % 10000) / 100.0;         
        trade.quantity = (rand() % 1000) / 10.0;        
        std::strncpy(trade.portfolio_id, sample_portfolio_ids[rand() % 4], sizeof(trade.portfolio_id) - 1);
        trade.portfolio_id[15] = '\0';  
        
        // Publish: update the write index (release order ensures the write is visible).
        ringBuffer->events[currentWrite] = trade;
        ringBuffer->writeIndex.pos.store(nextWrite, std::memory_order_release);
    }
    
    munmap(ptr, shm_size);
    close(shm_fd);
    return 0;
}
