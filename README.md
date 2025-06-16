# **cpp_position_manager**

## **Overview**
This project explores **C++ capabilities** using a **position manager** as an example. It processes **enriched trades** and calculates **positions** based on **portfolio ID**.

It also tests **message passing** using **shared memory ring buffers** for inter-component communication on the same machine.

---

## **Components**
The system consists of **five components**, each utilizing **shared memory queues** for efficient data exchange:

1. **Trade Queue Manager**  
   - Uses a **shared memory ring buffer** to transport **enriched trades**.

2. **Position Queue Manager**  
   - Uses a **shared memory ring buffer** to transport **calculated positions**.

3. **Test Trade Publisher**  
   - Submits **enriched trades** to the **Trade Queue Manager**.

4. **Position Manager (Main Component)**  
   - Calculates **positions** based on **trades** and publishes them to the **Position Queue Manager**.

5. **Test Position Subscriber**  
   - Subscribes to the **Position Queue Manager** to receive **position updates**.

---

## **Usage**
```sh
make # build the project
./bin/tradequeuemanager &   # Start Trade Queue Manager
./bin/positionqueuemanager & # Start Position Queue Manager
./bin/processor &            # Start Position Manager
./bin/positionsubscriber &   # Start Position Subscriber
./bin/publisher &            # Start Trade Publisher
```

## **Usage**
1. Compare Aeron/SBE with custom-built shared memory queues
2. Optimize memory layout and cache efficiency
3. Enhance multi-threading performance
