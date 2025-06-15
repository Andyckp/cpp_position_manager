#ifndef TRADE_RING_BUFFER_H
#define TRADE_RING_BUFFER_H

#include <atomic>
#include <cstddef>

// The total number of slots in the ring buffer
constexpr size_t BUFFER_SIZE = 1024;

// A trade event on which to operate.
struct alignas(64) TradeEvent {
    long instrument_id;
    double price;
    double quantity;
    char portfolio_id[16]; 
};

// Define a composite key for position ID
struct alignas(64) PositionID {
    long instrument_id;
    char portfolio_id[16];

    // Equality operator for unordered_map
    bool operator==(const PositionID& other) const {
        return instrument_id == other.instrument_id && std::memcmp(portfolio_id, other.portfolio_id, 16) == 0;
    }
};

// Custom hash function for PositionID
struct PositionIDHash {
    std::size_t operator()(const PositionID& pos) const {
        std::size_t hash1 = std::hash<long>()(pos.instrument_id);
        std::size_t hash2 = std::hash<std::string>()(std::string(pos.portfolio_id, 16)); // Convert char[16] to string for hashing
        return hash1 ^ (hash2 << 1);  // Combine hashes
    }
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