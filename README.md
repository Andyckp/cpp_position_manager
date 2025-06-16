# cpp_position_manager
The purpose of the project to learn the C++ capability by using a position manager as example. 
It takes enriched trades and calculates the positions according to portfolio id. 

Tests the capability of message passing with shared memory queues for components on the same machines, e.g. trade enricher, position manager and pnl calculations. 

There are 5 components.
(a) trade queue manager, which uses shared memory as ring buffer to transport enriched trades 
(b) position queue manager, which uses shared memory as ring buffer to transport positions
(c) test trade publisher, submits enriched trades to trade queue manager
(d) position manager (the main component being tested), calculates and publishes positions to the position queue manager
(e) test position subscriber, subscribes to the position queue manager 

# next steps
(1) compare Aeron/SBE with custom build shared memory queues and codes