[![progress-banner](https://backend.codecrafters.io/progress/kafka/61f7146b-344c-4c73-8dae-0cb27f837916)](https://app.codecrafters.io/users/nguyentheanh17112002?r=2qF)

# Kafka from scratch (C++) — my self-study repo

A toy [Apache Kafka](https://kafka.apache.org/) broker implemented **from scratch
in C++23**, following the ["Build Your Own Kafka" Challenge](https://codecrafters.io/challenges/kafka)
on [CodeCrafters](https://codecrafters.io).

The goal is learning, not just passing tests. Along the way I dug into the Kafka
wire protocol (binary encoding/decoding), the KRaft metadata log format, TCP
sockets, and how to keep a C++ codebase clean as it grows — small,
single-responsibility modules split into layers.

## What it does

The broker speaks enough of the Kafka protocol to handle real `kafka` clients for
these APIs:

| API | Key | What's implemented |
|-----|-----|--------------------|
| **ApiVersions** | 18 | Advertises supported APIs and version ranges |
| **DescribeTopicPartitions** | 75 | Lists real topics & partitions (read from the metadata log), sorted; error `3` for unknown topics |
| **Fetch** | 1 | Consumer read path — identifies topics by UUID, streams `RecordBatch` bytes straight from the partition log; error `100` for unknown topic ids |
| **Produce** | 0 | Producer write path — appends the incoming `RecordBatch` bytes to the partition log; error `3` for unknown topic/partition |

Topic and partition metadata is not hard-coded — it is parsed at startup from the
**KRaft metadata log** (`__cluster_metadata-0/00000000000000000000.log`), decoding
the nested `RecordBatch → Record → metadata record` layers (Topic / Partition
records).

## Architecture

The codebase is organized in layers — each layer only depends on the ones below
it, and `main` is just the composition root that wires everything together:

```
src/
├── main.cpp                      # composition root: load metadata, build server, run
│
├── common/
│   └── byte_buffer               # Reader / Writer — decode & encode wire primitives
│                                 #   (ints, varints, zigzag varints, compact strings)
├── net/                          # generic byte transport — knows NOTHING about Kafka
│   ├── connection                # one client connection (read_exact/write_exact, RAII fd)
│   └── tcp_server                # socket lifecycle + accept loop, one thread per client
│
├── metadata/                     # the broker's view of topics & partitions
│   ├── metadata_types            # Topic / Partition structs
│   ├── metadata_log              # parse the KRaft __cluster_metadata log
│   └── metadata_store            # load once, look up topics by name or UUID
│
└── protocol/                     # Kafka wire logic — knows NOTHING about sockets
    ├── request                   # RequestHeader + parse_request_header
    ├── api_keys / errors         # protocol enums
    ├── handler                   # dispatch a request to the right API handler
    ├── api_versions
    ├── describe_topic_partitions
    ├── fetch
    └── produce
```

Key boundaries:

- **`net/` knows nothing about Kafka** — it only moves bytes and calls a handler
  (a `std::function<std::vector<char>(Reader&)>`).
- **`protocol/` knows nothing about sockets** — it only decodes/encodes.
- The `MetadataStore` is loaded **once** in `main` and injected into the handlers
  via a closure, so the metadata layer never leaks into `net/` (dependency
  injection).
- Resources (socket fds) are owned by classes and released via RAII destructors.

### Request flow

```
TcpServer.run(): accept ──► Connection.read_request() ──► Reader
                                                            │
                       handle_request(reader, store) ◄──────┘
                              │
                    parse_request_header() → RequestHeader
                              │
                    switch(api_key) → handle_produce / handle_fetch /
                                      handle_describe_topic_partitions /
                                      handle_api_versions
                              │
                    Writer → response bytes ──► Connection.send()
```

## Wire protocol notes

A few encoding details the whole codebase relies on:

- **UNSIGNED_VARINT** (LEB128) for lengths and tag buffers.
- **Compact collections**: a length is stored as `N + 1` (so `0` means null,
  `1` means empty). Strings/arrays throughout use this.
- **Signed (zigzag) varints** inside `RecordBatch` records: `(v >> 1) ^ -(v & 1)`.
- **Tagged fields** ("tag buffer") close most flexible structures — usually an
  empty `0`.
- Multi-byte integers are big-endian (network order).

## Build & run locally

Requires `cmake` and a C++23 compiler.

```sh
# Build
cmake -S . -B build && cmake --build build

# Run the broker (listens on localhost:9092)
./build/kafka
```

Or use the CodeCrafters helper, which compiles and runs in one step:

```sh
./your_program.sh
```

> Note: the broker reads its metadata and topic logs from
> `/tmp/kraft-combined-logs/`, matching the CodeCrafters test harness layout.

## Testing

```sh
# Run the official CodeCrafters tests locally
codecrafters test

# Submit a solution to CodeCrafters
codecrafters submit
```

## Status

All challenge stages completed ✅ — ApiVersions, DescribeTopicPartitions, Fetch
(including reading real messages from the log) and Produce, backed by a real
KRaft metadata-log parser.
