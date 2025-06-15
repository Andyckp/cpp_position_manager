#ifndef TRADE_RING_BUFFER_H
#define TRADE_RING_BUFFER_H

#include <atomic>
#include <cstddef>
#include <cstring>
#include <functional>

constexpr size_t BUFFER_SIZE = 1024;
constexpr size_t CACHE_LINE_SIZE = 64;

struct PositionID {
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

struct alignas(CACHE_LINE_SIZE) TradeEvent {
    long instrument_id;
    double price;
    double quantity;
    char portfolio_id[16];
    char padding[CACHE_LINE_SIZE - sizeof(long) - sizeof(double) * 2 - sizeof(portfolio_id)];
};

struct alignas(CACHE_LINE_SIZE) PositionEvent {
    PositionID position_id;
    double net_quantity;
    char padding[CACHE_LINE_SIZE - sizeof(PositionID) - sizeof(double)];
};

struct alignas(CACHE_LINE_SIZE) PaddedAtomic {
    std::atomic<size_t> pos;

    PaddedAtomic() : pos(0) {} 

    char padding[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>)];
};

struct alignas(CACHE_LINE_SIZE) TradeRingBuffer {
    alignas(CACHE_LINE_SIZE) PaddedAtomic readIndex;
    alignas(CACHE_LINE_SIZE) PaddedAtomic writeIndex;
    TradeEvent events[BUFFER_SIZE];
};

struct alignas(CACHE_LINE_SIZE) PositionRingBuffer {
    alignas(CACHE_LINE_SIZE) PaddedAtomic readIndex;
    alignas(CACHE_LINE_SIZE) PaddedAtomic writeIndex;
    PositionEvent events[BUFFER_SIZE];
};

#endif // TRADE_RING_BUFFER_H
