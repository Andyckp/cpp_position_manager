#ifndef TRADE_RING_BUFFER_H
#define TRADE_RING_BUFFER_H

#include <atomic>
#include <cstddef>

// The total number of slots in the ring buffer
constexpr size_t BUFFER_SIZE = 1024;

// A trade event on which to operate.
struct TradeEvent {
    long instrument_id;
    double price;
    double quantity;
};

// Our shared ring buffer structure.
struct TradeRingBuffer {
    // Mark these atomics with cache-line alignment for minimal false-sharing
    alignas(64) std::atomic<size_t> readIndex;   // Consumer index
    alignas(64) std::atomic<size_t> writeIndex;  // Producer index

    // The ring buffer of trade events
    TradeEvent events[BUFFER_SIZE];
};

#endif // TRADE_RING_BUFFER_H