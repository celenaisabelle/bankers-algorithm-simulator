# Banker's Algorithm Simulator

A command-line resource allocation simulator written in C that implements the Banker's Algorithm for deadlock avoidance.

The program manages multiple customers competing for a limited set of resources and evaluates each resource request to determine whether granting it would leave the system in a safe state.

Originally developed as part of my Operating Systems coursework at Florida International University.

## Features

- Banker's Algorithm safety check
- Deadlock avoidance through safe-state detection
- Resource request and release handling
- Automatic rollback of unsafe resource allocations
- Tracking of available, maximum, allocated, and needed resources
- Maximum resource demands loaded from an external file
- Interactive command-line interface
- Input validation for resource requests and releases
- Current system-state display

## How It Works

The simulator manages five customers and four resource types.

The system tracks four primary data structures:

- **Available** — resources currently available for allocation
- **Maximum** — maximum resources each customer may request
- **Allocation** — resources currently allocated to each customer
- **Need** — remaining resources each customer may still require

The remaining need for each customer is determined by:

`Need = Maximum - Allocation`

When a customer requests resources, the simulator first verifies that the request does not exceed the customer's remaining need or the resources currently available.

The resources are then tentatively allocated and the Banker's safety algorithm determines whether the system can still complete all customers in a safe order.

If the resulting state is safe, the allocation is kept. If the state is unsafe, the allocation is rolled back and the request is denied.

## Safety Algorithm

The safety check creates a temporary representation of the available resources and determines whether each customer could complete with the resources currently available.

When a customer can finish, its allocated resources are returned to the temporary resource pool.

This process continues until either:

- All customers can finish, indicating a **safe state**, or
- No additional customer can finish, indicating an **unsafe state**

Unsafe resource requests are rejected to prevent the system from entering a state that could lead to deadlock.

## Building the Project

Compile the program with GCC:

```bash
gcc -o banker banker.c
```

## Running the Simulator

The program accepts the initial quantity of each of the four resource types as command-line arguments:

```bash
./banker A B C D
```

For example:

```bash
./banker 10 5 7 8
```

By default, the program loads maximum customer resource demands from:

```text
data.txt
```

A different maximum-demand file can also be supplied:

```bash
./banker 10 5 7 8 custom-data.txt
```

## Commands

Once the simulator starts, resource requests and releases can be entered interactively.

### Request Resources

```text
RQ customer_id r1 r2 r3 r4
```

Example:

```text
RQ 0 1 0 2 1
```

This requests resources for customer `0`.

Before granting the request, the simulator verifies that the requested resources are available and that the resulting allocation leaves the system in a safe state.

### Release Resources

```text
RL customer_id r1 r2 r3 r4
```

Example:

```text
RL 0 1 0 1 0
```

The simulator verifies that the customer currently holds the requested resources before releasing them back to the available resource pool.

### Display System State

```text
*
```

This displays the current:

- Available resources
- Maximum resource demands
- Resource allocations
- Remaining resource needs

### Exit

```text
exit
```

Terminates the simulator.

## Example Data

The included `data.txt` file contains the maximum resource requirements for five customers across four resource types.

```text
7 5 3 4
7 5 7 5
5 5 5 5
7 4 5 7
7 4 5 7
```

Each row represents one customer, while each column represents one resource type.

## Project Structure

```text
bankers-algorithm-simulator/
├── README.md
├── banker.c
├── data.txt
└── .gitignore
```

## Input Validation

The simulator validates several conditions before modifying system resources, including:

- Customer IDs must reference a valid customer
- Resource quantities cannot be negative
- Requests cannot exceed a customer's remaining need
- Requests cannot exceed currently available resources
- Releases cannot exceed resources currently allocated to a customer
- Commands must contain the expected number of arguments

Invalid or unsafe requests are rejected without changing the current system state.

## What I Learned

This project gave me hands-on experience implementing operating system resource-management concepts in C.

I gained experience with:

- Deadlock avoidance
- Banker's Algorithm
- Safe-state detection
- Resource allocation and release
- Multi-dimensional arrays
- File I/O
- Command-line arguments
- Interactive command parsing
- Input validation
- State management
- Rollback logic

Implementing the safety check helped me understand how an operating system can evaluate resource requests before granting them rather than responding to a deadlock after it has already occurred.