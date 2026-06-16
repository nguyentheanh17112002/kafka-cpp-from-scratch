[![progress-banner](https://backend.codecrafters.io/progress/kafka/61f7146b-344c-4c73-8dae-0cb27f837916)](https://app.codecrafters.io/users/nguyentheanh17112002?r=2qF)

# Kafka from scratch (C++) — my self-study repo

This is my personal **self-study** project where I implement a toy
[Apache Kafka](https://kafka.apache.org/) broker **from scratch in C++**,
following the ["Build Your Own Kafka" Challenge](https://codecrafters.io/challenges/kafka)
on [CodeCrafters](https://codecrafters.io).

The goal is learning, not just passing tests: along the way I dig into the
Kafka wire protocol (encoding/decoding messages), TCP sockets, and how to
structure a C++ codebase that stays clean as it grows. The code is refactored
incrementally into small, single-responsibility modules.

## Architecture

The codebase is organized in layers — each layer only depends on the one below
it, and `main` is just the composition root that wires everything together:

```
src/
├── main.cpp              # composition root: build server, set handler, run
│
├── common/
│   └── byte_buffer       # Reader / Writer — decode & encode wire primitives
│
├── net/
│   ├── connection        # one client connection (read_exact/write_exact, RAII fd)
│   └── tcp_server        # socket lifecycle + accept loop (Kafka-agnostic)
│
└── protocol/
    ├── request           # RequestHeader + parse_request_header
    └── handler           # turn a request into a response (business logic)
```

Key boundaries:

- `net/` knows nothing about Kafka — it only moves bytes and calls a handler.
- `protocol/` knows nothing about sockets — it only decodes/encodes.
- Resources (socket fds) are owned by classes and released via RAII destructors.

### Request flow

```
TcpServer.run(): accept ──► Connection.read_request() ──► Reader
                                                            │
                                  handle_request(reader) ◄──┘
                                        │
                                parse_request_header() → RequestHeader
                                        │
                                Writer → response bytes ──► Connection.send()
```

## Build & run locally

Requires `cmake` and a C++23 compiler.

```sh
# Build
cmake -S . -B build && cmake --build build

# Run the broker (listens on localhost:9092)
./build/kafka
```

Or use the CodeCrafters helper which compiles and runs in one step:

```sh
./your_program.sh
```

## Testing

```sh
# Quick local smoke test (sends one request, checks the echoed correlation_id)
python3 scripts/smoke_test.py

# Run the official CodeCrafters tests locally
codecrafters test

# Submit a solution to CodeCrafters
codecrafters submit
```

## Status

Work in progress — refactoring the starter code into the layered structure
above while progressing through the challenge stages.
