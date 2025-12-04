# router

**An IPv4 router emulator.**

______________________________________________________________________

<!--toc:start-->

- [router](#router)
  - [Functional Requirements (TODOs)](#functional-requirements-todos)
    - [Basic Requirements](#basic-requirements)
    - [File Formats](#file-formats)
      - [Interface Configuration File](#interface-configuration-file)
      - [Route Table](#route-table)
      - [Input File](#input-file)
      - [Output File](#output-file)
    - [Functionality](#functionality)
  - [Additional Constraints](#additional-constraints)
  - [Testing](#testing)

<!--toc:end-->

______________________________________________________________________

This project emulates the behavior of a standard IPv4 router. It takes in two input files: a configuration for the router in the form of a list of interfaces and associated subnets as well as a route table in the form of a list of networks and the next-hop used to reach that subnet. The program processes these files and then receives a series of packets (just a list of IP addresses that need to be processed). It then determines where the packet should be sent.

## Functional Requirements (TODOs)

### Basic Requirements

- [x] The executable should be called **router**
- [x] The program should be written in C/C++ and run on Ubuntu 22.04LTS
- [ ] The program should take two required command line arguments
  - [ ] `-c <interface configuration file`
  - [ ] `-r <route table>`
- [ ] The program should take three optional command line arguments
  - [ ] `-i <input file>` *(default: read from `stdin`)*
  - [ ] `-o <output file>` *(default: write to `stdout`)*
  - [ ] `-d <debug level>` *(default: level 4 log level)*

### File Formats

Each of the files are a multi-line `txt` file. Each line termination follows `*nix` convention (single `\n`). The contents of each line could have any number of whitespace characters before or after the line's contents. The three input files may have comments in them (first non-whitespace character is `#`) and these comments can be ignored.

#### Interface Configuration File

The format for each line is:

```
InterfaceName<SP+>IPAddress/netmask
```

As an example, the line:

```
eth0 138.67.1.2/18
```

indicates an interface named `eth0` assigned to the IP address `138.67.1.2` with a netmask that is 18 bits long. It is also important to note that there may be any amount of whitespace (either spaces or tabs) between the `InterfaceName` and the `IPAddress`. There is no whitespace between the IP address and the netmask

#### Route Table

The format for each line is:

```
Network/netmask<SP+>nexthop
```

As an example, the line:

```
192.168.12.0/24 138.67.1.4
```

indicates a network with the 24 bit prefix `192.168.12.0` can be reached through the router at `138.67.1.4`.

#### Input File

The format for each line is:

```
destinationIP
```

As an example, the line:

```
138.67.20.1
```

indicates a packet should be forwarded out the appropriate interface with the correct destination IP address.

#### Output File

The format for each line can take on two forms. The first is:

```
destinationIP:<SP+>InterfaceName<SP+>-><SP+>nextHop
```

As an example, the line:

```
138.67.20.1: eth4 -> 138.67.20.254
```

indicates a packet destined for `138.67.20.1` should be forwarded out the interface `eth4` to the node with IP address `138.67.20.254`. If the destination IP is a part of the subnet that is directly attached to the router, then the `nextHop` IP address should be the same as the `destinationIP`.

The other output line format should be used if the `destinationIP` address is not reachable. Note that this can can only happen when there is no default route in the input file. The format for this is:

```
destinationIP:<SP+>unreachable
```

### Functionality

For each line in the input file, there are three steps that must be performed:

- [ ] Read and parse the interface configuration and the route table files.
- [ ] Read one line from the input file to determine the correct next hop and find the interface that can be used to reach that next hop.
- [ ] Print a correctly formatted line to the output file

After these steps are performed for all lines in the input file, close the output file and exit.

## Additional Constraints

- Configuration files will not be broken
  - In the routing table there will not be entries with next-hop addresses that are not directly attached to one of the local subnets
  - Each interface will have a unique name and configuration. However, remember that one interface might be attached to a network that is a subnet of another network.
- No external libraries used within the executable
- Must be able to be tarballed with a Makefile to simply type `make` to

______________________________________________________________________

## Testing

Unit tests are implemented using [RapidCheck](https://github.com/emil-e/rapidcheck) for property-based testing.

1. **Enable testing during configuration:**

   ```bash
   cd build
   cmake .. -DENABLE_TESTING=ON
   ```

1. **Build the tests:**

   ```bash
   make
   ```

1. **Run the tests:**
   You can run all tests using CTest or execute the test binary directly:

   ```bash
   ctest --output-on-failure
   # OR
   ./test/TestParse
   ```
