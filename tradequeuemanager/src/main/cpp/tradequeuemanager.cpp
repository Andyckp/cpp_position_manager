#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include "../../../../common/src/main/cpp/data_structure.h"

// Shared memory name (POSIX requires names starting with a slash)
constexpr const char* SHM_NAME = "/trade_ring_buffer_shm";

int main() {
    // Attempt to open an existing shared memory object
    int existing_shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (existing_shm_fd >= 0) {
        std::cout << "Closing previously opened shared memory FD: " << existing_shm_fd << std::endl;
        close(existing_shm_fd);  // Close old FD
        shm_unlink(SHM_NAME);    // Remove the previous shared memory object
    }

    // Create (or open) the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(shm_fd < 0) {
        perror("shm_open");
        return 1;
    }
    
    // Set the memory segment to hold one TradeRingBuffer
    size_t shm_size = sizeof(TradeRingBuffer);
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
        return 1;
    }
    
    // Map the shared memory into address space
    void* ptr = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    // Initialize
    TradeRingBuffer* ringBuffer = static_cast<TradeRingBuffer*>(ptr);
    ringBuffer->readIndex.pos.store(0, std::memory_order_relaxed);
    ringBuffer->writeIndex.pos.store(0, std::memory_order_relaxed);
    
    std::cout << "Trade ring buffer created in shared memory." << std::endl;
    std::cout << "Press Ctrl+C to exit ..." << std::endl;
    
    // Keep running
    while(true) {
        sleep(1);
    }
    
    // Cleanup (never reached)
    munmap(ptr, shm_size);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    return 0;
}
