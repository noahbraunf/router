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
- [x] The program should be written in C/C++ and run on Ubuntu 22.04 LTS
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

______________________________________________________________________

## Implementation Tasks

This section breaks down the remaining work into smaller, manageable tasks.

### Phase 1: Command Line Argument Parsing

**Status:** Not started

- [ ] **Task 1.1:** Create `ArgParser` class/module in `src/ArgParser.hpp` and `src/ArgParser.cpp`
  - Parse `-c <config_file>` (required)
  - Parse `-r <route_table>` (required)
  - Parse `-i <input_file>` (optional, default: stdin)
  - Parse `-o <output_file>` (optional, default: stdout)
  - Parse `-d <debug_level>` (optional, default: 4)
  - Return error and usage message if required args missing

- [ ] **Task 1.2:** Add argument validation
  - Check that specified files exist and are readable
  - Validate debug level is within acceptable range

- [ ] **Task 1.3:** Write property-based tests for argument parsing

### Phase 2: Data Structures

**Status:** Partially complete (IP parsing exists)

- [ ] **Task 2.1:** Create `Interface` struct/class

  ```cpp
  struct Interface {
      std::string   name;        // e.g., "eth0"
      std::uint32_t ip_addr;     // interface IP address
      std::uint8_t  prefix_len;  // netmask length (e.g., 18)
      std::uint32_t network;     // computed network address
      std::uint32_t netmask;     // computed netmask (e.g., 0xFFFFC000 for /18)
  };
  ```

- [ ] **Task 2.2:** Create `Route` struct/class

  ```cpp
  struct Route {
      std::uint32_t network;     // network address
      std::uint8_t  prefix_len;  // netmask length
      std::uint32_t netmask;     // computed netmask
      std::uint32_t next_hop;    // next hop IP address
  };
  ```

- [ ] **Task 2.3:** Create `Router` class to hold configuration

  ```cpp
  class Router {
      std::vector<Interface> interfaces;
      std::vector<Route>     routes;
  };
  ```

- [ ] **Task 2.4:** Implement helper function `prefix_to_netmask(uint8_t prefix) -> uint32_t`

- [ ] **Task 2.5:** Implement helper function `ip_num_to_str(uint32_t addr) -> std::string`

- [ ] **Task 2.6:** Write tests for netmask computation and IP string conversion

### Phase 3: File Parsing

**Status:** Not started

- [ ] **Task 3.1:** Create generic line parser utilities
  - `trim_whitespace(std::string_view)` - remove leading/trailing whitespace
  - `is_comment_or_empty(std::string_view)` - check if line should be skipped
  - `split_on_whitespace(std::string_view)` - split line into tokens

- [ ] **Task 3.2:** Implement interface configuration file parser
  - Parse format: `InterfaceName<SP+>IPAddress/netmask`
  - Handle arbitrary whitespace before/after content
  - Skip comment lines (starting with `#`)
  - Return `std::vector<Interface>`

- [ ] **Task 3.3:** Implement route table file parser
  - Parse format: `Network/netmask<SP+>nexthop`
  - Handle arbitrary whitespace
  - Skip comment lines
  - Return `std::vector<Route>`

- [ ] **Task 3.4:** Implement input file parser (destination IPs)
  - Parse one IP address per line
  - Handle whitespace and comments
  - Support reading from stdin or file

- [ ] **Task 3.5:** Write property-based tests for each parser

### Phase 4: Routing Logic

**Status:** Not started

- [ ] **Task 4.1:** Implement `is_in_subnet(uint32_t ip, uint32_t network, uint32_t netmask) -> bool`
  - Check if an IP address belongs to a given subnet

- [ ] **Task 4.2:** Implement `find_matching_route(uint32_t dest_ip, routes) -> optional<Route>`
  - Find the route with the longest prefix match for a destination IP
  - Must implement longest-prefix matching (most specific route wins)

- [ ] **Task 4.3:** Implement `find_interface_for_nexthop(uint32_t next_hop, interfaces) -> optional<Interface>`
  - Given a next-hop IP, find which interface can reach it
  - The next-hop must be in the subnet of one of the interfaces

- [ ] **Task 4.4:** Implement `route_packet(uint32_t dest_ip, router) -> RoutingResult`
  - Combine route lookup and interface lookup
  - Handle direct delivery (dest is in a directly connected subnet)
  - Handle indirect delivery (dest requires routing via next-hop)
  - Return "unreachable" if no route exists

- [ ] **Task 4.5:** Write comprehensive tests for routing logic
  - Test longest-prefix matching
  - Test direct vs indirect delivery
  - Test unreachable destinations

### Phase 5: Output Formatting

**Status:** Not started

- [ ] **Task 5.1:** Implement output formatter
  - Format: `destinationIP: InterfaceName -> nextHop`
  - Format: `destinationIP: unreachable`
  - Support writing to stdout or file

- [ ] **Task 5.2:** Write tests for output formatting

### Phase 6: Main Program Integration

**Status:** Skeleton exists

- [ ] **Task 6.1:** Integrate argument parsing in `main()`

- [ ] **Task 6.2:** Load and parse configuration files
  - Load interface configuration
  - Load route table
  - Handle file errors gracefully

- [ ] **Task 6.3:** Process input and generate output
  - Read destination IPs (from file or stdin)
  - Route each packet
  - Write results (to file or stdout)

- [ ] **Task 6.4:** Implement debug/logging based on `-d` flag
  - Level 0: Errors only
  - Level 1-3: Increasing verbosity
  - Level 4: Full debug output (default)

- [ ] **Task 6.5:** End-to-end testing with sample data
  - Test with `data/sample1/` files
  - Test with `data/sample2/` files
  - Compare output against expected results

### Phase 7: Polish and Edge Cases

**Status:** Not started

- [ ] **Task 7.1:** Handle edge cases
  - Empty input file
  - No default route (0.0.0.0/0) and unreachable destinations
  - Multiple interfaces on overlapping subnets

- [ ] **Task 7.2:** Error handling improvements
  - Meaningful error messages for malformed input
  - Proper exit codes

- [ ] **Task 7.3:** Code cleanup
  - Run `make format` to ensure consistent style
  - Run `make analyze` to fix any static analysis warnings
  - Update CMakeLists.txt with new source files

- [ ] **Task 7.4:** Documentation
  - Add inline comments for complex logic
  - Update README if needed
