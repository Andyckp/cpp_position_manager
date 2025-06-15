#ifndef TRADE_RING_BUFFER_H
#define TRADE_RING_BUFFER_H

#include <atomic>
#include <cstddef>

constexpr size_t BUFFER_SIZE = 1024;

struct alignas(64) TradeEvent {
    long instrument_id;
    double price;
    double quantity;
    char portfolio_id[16]; 
};

struct alignas(64) PositionID {
    long instrument_id;
    char portfolio_id[16];

    bool operator==(const PositionID& other) const {
        return instrument_id == other.instrument_id && std::memcmp(portfolio_id, other.portfolio_id, 16) == 0;
    }
};

struct PositionIDHash {
    std::size_t operator()(const PositionID& pos) const {
        std::size_t hash1 = std::hash<long>()(pos.instrument_id);
        std::size_t hash2 = std::hash<std::string>()(std::string(pos.portfolio_id, 16)); 
        return hash1 ^ (hash2 << 1);  
    }
};

struct TradeRingBuffer {
    alignas(64) std::atomic<size_t> readIndex;   
    alignas(64) std::atomic<size_t> writeIndex;  

    TradeEvent events[BUFFER_SIZE];
};

#endif // TRADE_RING_BUFFER_H