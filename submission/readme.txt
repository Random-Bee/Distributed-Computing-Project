Dependencies:

Make sure your system has `openssl`, the development headers, make and zeromq installed. On Ubuntu, you can install these with:

```bash
sudo apt-get install libssl-dev openssl make
```

Install the cpp zeromq library as per instructions given on its site.

Running the project:

For a single system, a makefile is provided to build and run the project

```bash
make all
```

This will build the project and run the nodes on a single system. The terminal will display a parent node which can be used to insert, and find keys in the DHT.

Changing the number of nodes: Just edit the number of nodes in the `Makefile` and run `make all` again.

Changing the DHT: Go to `src/main.cpp` and change include header to `kademlia.hpp` or `pastry.hpp` to switch between the two DHTs and run `make all` again.

Running the project on multiple systems:

To run the project on multiple systems, you need to manually compile the project on each system and run the generated binary. The IP addresses of the nodes need to be given in the `inp.txt` file in the following format:

```
<Number of Nodes>
<IP Address of Machine 0>
<IP Address of Machine 1>

...

Make sure that there is a newline at the end of the file.

To run the generated binary, use the following command:

```bash
./main <Machine Number>
```

Note that the machine number should be between 0 and the number of machines - 1. Also the order of the IP addresses in the `inp.txt` file should be the same as the order of the machine numbers.

When running the project on multiple systems, the parent node on each system will be the node with the lowest node id assigned to that machine number.