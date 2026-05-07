# Luon Project Manifest — `luon.project.json`

Every Luon project should have a `luon.project.json` file at its root. This file defines the project metadata, dependencies, build configuration, and entry points.

## Specification

```json
{
  "name": "my-project",
  "version": "0.1.0",
  "description": "A Luon project",
  "author": "Your Name",
  "license": "Apache-2.0",

  "entry": "src/main.luon",
  "output": "build/main.wasm",

  "target": "wasm32-wasi",
  "memory_pages": 30,
  "optimize": true,

  "stdlib": [
    "io",
    "memory",
    "string",
    "math"
  ],

  "dependencies": {},

  "scripts": {
    "build": "luon build",
    "run": "luon run",
    "test": "luon test",
    "check": "luon check"
  }
}
```

## Field Reference

| Field | Type | Required | Description |
|---|---|---|---|
| `name` | string | ✅ | Project name (lowercase, no spaces) |
| `version` | string | ✅ | Semantic version (MAJOR.MINOR.PATCH) |
| `description` | string | ❌ | Short project description |
| `author` | string | ❌ | Author name or organization |
| `license` | string | ❌ | SPDX license identifier |
| `entry` | string | ✅ | Path to main source file |
| `output` | string | ❌ | Path for compiled WASM output (default: `build/<name>.wasm`) |
| `target` | string | ❌ | Compilation target (default: `wasm32-wasi`) |
| `memory_pages` | number | ❌ | WASM memory pages (default: 30, each page = 64KB) |
| `optimize` | boolean | ❌ | Enable peephole optimizer (default: true) |
| `stdlib` | string[] | ❌ | Standard library modules to include |
| `dependencies` | object | ❌ | External package dependencies (future) |
| `scripts` | object | ❌ | Custom build/run scripts |

## Available Standard Library Modules

```text
io          — stdout/stderr output
memory      — heap allocation
string      — string operations
wasi        — WASI system interface
math        — mathematical functions
array       — dynamic arrays
hashmap     — hash map / dictionary
sort        — sorting algorithms
result      — Result/Option types
testing     — test assertions
random      — pseudo-random numbers
crypto      — cryptographic primitives
fs          — filesystem operations
env         — environment/process
convert     — type conversions
fmt         — formatted output
buffer      — byte buffer I/O
error       — error code constants
struct      — struct/record types
enum        — enum/ADT types
```

## Example: Minimal Project

```
my-project/
├── luon.project.json
├── src/
│   └── main.luon
└── build/
    └── main.wasm (generated)
```

```json
{
  "name": "hello",
  "version": "0.1.0",
  "entry": "src/main.luon"
}
```

## Example: Full Project

```
my-app/
├── luon.project.json
├── src/
│   ├── main.luon
│   ├── utils.luon
│   └── models.luon
├── tests/
│   └── test_main.luon
├── build/
│   └── my-app.wasm
└── README.md
```

## CLI Integration

```bash
# Create new project
luon new my-project

# Build project (reads luon.project.json)
luon build

# Run project
luon run

# Type-check without building
luon check

# Run tests
luon test
```
