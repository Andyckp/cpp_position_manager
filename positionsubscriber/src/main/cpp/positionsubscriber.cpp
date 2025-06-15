#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "../../../../common/src/main/cpp/trade_ring_buffer.h"

constexpr const char* SHM_NAME = "/position_ring_buffer_shm";

int main() {
    // Open the shared memory object.
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        return 1;
    }

    size_t shm_size = sizeof(PositionRingBuffer);
    void* ptr = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    PositionRingBuffer* ringBuffer = static_cast<PositionRingBuffer*>(ptr);

    while (true) {
        size_t currentRead = ringBuffer->readIndex.pos.load(std::memory_order_relaxed);
        size_t currentWrite = ringBuffer->writeIndex.pos.load(std::memory_order_acquire);

        if (currentRead == currentWrite) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        // Read the position event at the current read position.
        PositionEvent position = ringBuffer->events[currentRead];

        std::cout << "Received Position Update: instrument_id = " << position.position_id.instrument_id
                  << ", portfolio_id = " << position.position_id.portfolio_id
                  << ", net_quantity = " << position.net_quantity << "\n";
        std::cout.flush();

        // Advance the read index.
        size_t nextRead = (currentRead + 1) % BUFFER_SIZE;
        ringBuffer->readIndex.pos.store(nextRead, std::memory_order_release);
    }

    munmap(ptr, shm_size);
    close(shm_fd);
    return 0;
}
