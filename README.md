# Shoreline
The problem presents us with a system of nodes, each of them being associated a numeric ID between 0 and 1023. Each node has an internal process which generates an UUID when called. A client can connect to a node and request an UUID. 
The requirement is to implement get_id and determine its interface.

As speed is mandatory when it comes to continuous generation of UUIDs, I chose to use C++ for implementing the task.

## UUID format

Generally the format of an UUID consists of a timestamp and the address of the computer on which it was generated (a node from the system).

Thus, the first step to create the custom UUID was computing a reliable numeric timestamp, which I got by converting the time since epoch (January 1st, 1970 at 00:00:00 UTC Unix epoch, which is a date given arbitrarily as an uniform unitary date for the start of time) to microseconds (0.000001 or 10−6 of a second) to ensure accuracy and a numeric value which doesn’t overflow the 64 bits long data type.

*Example*
- Time since epoch in microseconds
Tuesday, January 25, 2022 11:52:23 AM converts to 1643111543943374 microseconds;
1643111543943374 microseconds have a lenght of 51 bits (which leaves us with 13 free bits out of the 64 bits given)

The next step to ensure the uniqueness of the future UUID is attaching the node from which it was generated, as, in the previously described system, multiple nodes can generate an UUID at the same time and that would lead to multiple duplicates. In the system, each node has one particular ID and one internal process to generate one ID at a time, meaning that attaching the node ID to the converted timestamp quarantees an optimal unique result.
For reasons such as speed and memory optimization, I have taken the bitset approach by shifting the binary representation of the timestamp to the left by 10 bits (1024 is 210 and the nodes can have an ID from the [0, 1023] interval, 1023 being 210-1, so 10 bits is enough space for all the possible node IDs to be added at the end of the timestamp). After the shifting operation, I have added the nodeId, thus getting the required UUID.

*Example*
- Converting 1643111543943374 microseconds into a 64 bit representation 0000000000000101110101100110011010110111111001110011000011001110
- Shifting 10 bits to the left
0001011101011001100110101101111110011100110000110011100000000000
Node ID = 1022 (Binary representation on 10 bits: 1111111110)
Adding the node ID to the shifted timestamp
0001011101011001100110101101111110011100110000110011100000000000 +
0000000000000000000000000000000000000000000000000000001111111110 =
0001011101011001100110101101111110011100110000110011101111111110 
which gives us the UUID = 1682546220998015998

This particular format ensures uniqueness out of several reasons:

- In the case of multiple different nodes requesting an UUID at the same time (multithreading for example), the node ID added to the shifted timestamp is the solution as each of them will be globally unique having not only the timestamp as an identifier, but also the ID of the “machine” attached.
- If the same node is called multiple times almost instantly, the generate() function still guarantees uniqueness by having the timestamp in microseconds and one generation (using classes and internal functions for object) takes 10 to 23 microseconds so it is visibly included in the timestamp.

## Minimal implementation of an UUID generator for a given node ID

To test the speed and correctness of the UUIDs generator and to provide a minimal representation of the functionality and thought process of the get_id function, I have implemented a minimal generator which later was included in the multithreaded TCP server.

The generator class constructor receives a node ID for which it generates an UUID by calling its internal generate() function. For the same node ID the generate() function can be called multiple times (the microseconds format ensures the visibility of the time change in the UUID) to test the uniqueness and the reliability of the chosen UUID format.
 
Random Node ID | Execution time (seconds) for 100000 iterations
--- | --- 
1023 | 0.007994
1022 | 0.00782
512 | 0.00756
0 | 0.007565
1012 | 0.007453
890 | 0.008721
708 | 0.007844
1 | 0.010042
23 | 0.007392
1021 | 0.008361
--- | --- 
Average execution time | 0.0080752

*To optimize the execution time the generate()  function does not print the number, it only returns it, the correctness (non-existence of duplicates) was tested using sets.

For additional tests I have also added the printBinary() function which receives an integer on 64 bits as a parameter and prints it’s binary representation.

## Distributed systems? 

A system of nodes, each node with a particular numeric ID assigned from the [0, 1023] interval is, by definition, a distributed system, where each node is a machine entity with an assigned machine ID and internal operations. 

## Multithreaded TCP server with queue to select available nodes 

One minimal representation of a distributed system is a TCP(*)  server to which multiple clients can connect at the same time and the way to do that is by using multithreading, each client socket connection being handled on a particular thread. Multithreading allows simultaneous requests from multiple clients which also increases the overall speed. 

*Using an UDP might be faster, but packets of data might be lost and it can also send the same data pacet multiple times and that would create problems for the globally unique IDs. TCP is safer to use in this particular case.
![Fig.2](https://www.researchgate.net/profile/Jingyu-Zhou-3/publication/221244103/figure/fig2/AS:669067807563778@1536529604229/Multi-threaded-server-design.png)
