#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <random>

constexpr size_t CACHE_LINE_SIZE = 64;
constexpr size_t RINGBUFFER_SIZE = 1024;

struct alignas(CACHE_LINE_SIZE) TradeEvent {
    long instrumentId;
    double price;
    double netQuantity;

    char padding[CACHE_LINE_SIZE - sizeof(long) - 2 * sizeof(double)];
};

struct alignas(CACHE_LINE_SIZE) PaddedAtomic {
    std::atomic<size_t> pos;

    PaddedAtomic() : pos(0) {} 

    char padding[CACHE_LINE_SIZE - sizeof(std::atomic<size_t>)];
};

class alignas(CACHE_LINE_SIZE) RingBuffer {
    private:
        TradeEvent buffer[RINGBUFFER_SIZE];

        PaddedAtomic writePos; 
        PaddedAtomic readPos;  

public:
    bool pushTrade(long instrumentId, double price, double netQuantity) {
        size_t nextWrite = (writePos.pos + 1) % RINGBUFFER_SIZE;

        if (nextWrite == readPos.pos.load()) { 
            return false;
        }

        buffer[writePos.pos].instrumentId = instrumentId;
        buffer[writePos.pos].price = price;
        buffer[writePos.pos].netQuantity = netQuantity;

        writePos.pos.store(nextWrite);
        return true;
    }

    bool processTrade() {
        if (readPos.pos.load() == writePos.pos.load()) { 
            return false;
        }

        const TradeEvent& trade = buffer[readPos.pos];
        std::cout << "Processing Trade: "
                << "InstrumentID=" << trade.instrumentId
                << ", Price=" << trade.price
                << ", Quantity=" << trade.netQuantity
                << std::endl;

        readPos.pos.store((readPos.pos + 1) % RINGBUFFER_SIZE);
        return true;
    }
};

void tradePublisher(RingBuffer& ringBuffer) {
    std::default_random_engine generator;
    std::uniform_int_distribution<long> instrumentDist(1000, 9999);
    std::uniform_real_distribution<double> priceDist(10.0, 1000.0);
    std::uniform_real_distribution<double> quantityDist(1.0, 500.0);

    while (true) {
        long instrumentId = instrumentDist(generator);
        double price = priceDist(generator);
        double netQuantity = quantityDist(generator);

        if (!ringBuffer.pushTrade(instrumentId, price, netQuantity)) {
            std::cout << "Buffer full, skipping trade submission!" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void tradeProcessor(RingBuffer& ringBuffer) {
    while (true) {
        if (!ringBuffer.processTrade()) {
            std::cout << "Buffer empty, waiting for trades..." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void runTest() {
    RingBuffer ringBuffer;

    std::thread publisher(tradePublisher, std::ref(ringBuffer));
    std::thread processor(tradeProcessor, std::ref(ringBuffer));

    std::this_thread::sleep_for(std::chrono::seconds(5)); 

    publisher.detach();
    processor.detach();
}

// int main() {
//    runTest();
//    return 0;
//}

