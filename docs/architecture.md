# Luon Architecture

## Overview

Luon is a systems-level programming language that compiles to WebAssembly (WASM). It uses an accumulator-based execution model with 127 general-purpose registers.

## Compilation Pipeline

```
.luon source → Parser → IR → Emitter → .wasm binary → Runtime
```

1. **Parser** (`tools/parser.luon`) — Reads v2 EXTREME syntax line-by-line, computes FNV-1a hashes of operator patterns, dispatches to opcode numbers, and emits an IR stream to memory.

2. **Emitter** (`tools/emitter.luon`) — Reads the IR stream and generates a valid WASM binary with proper section headers, function bodies, memory declarations, and export entries.

3. **Runtime** (`runtime/luon_vm.c`) — A minimal native WASM interpreter written in C that can parse and execute Luon-compiled WASM modules. Also supports execution via Wasmtime or any WASI-compatible runtime.

## Execution Model

- **Accumulator** (`∂_Ω`) — Primary working register, used by most operations.
- **Registers** (`σ₀`–`σ₁₂₇`) — 128 registers. `σ₀` is the function parameter, `σ₁` is the accumulator alias, `σ₂`–`σ₁₂₇` are general-purpose.
- **Memory** — 30 WASM pages (1.875 MB) divided into 5 segments:
  - `0x00000–0x0FFFF` — Code/data segment
  - `0x10000–0x1FFFF` — Input buffer
  - `0x20000–0x2FFFF` — Output buffer
  - `0x50000–0x5FFFF` — State segment (allocator pointers, test counters, RNG state, etc.)
  - `0x60000+` — Heap (bump-allocated)

## Type System

13 primitive type IDs defined in `tools/typechecker.luon`:

| ID | Type | Size | Align |
|---:|------|-----:|------:|
| 0 | void | 0 | 0 |
| 1 | bool | 1 | 1 |
| 2 | i32 | 4 | 4 |
| 3 | i64 | 8 | 8 |
| 4 | f32 | 4 | 4 |
| 5 | f64 | 8 | 8 |
| 6 | ptr | 8 | 8 |
| 7 | str | 16 | 8 |
| 8 | array | 8 | 8 |
| 9 | struct | varies | varies |
| 10 | fn | 8 | 8 |
| 11 | result | 16 | 8 |
| 12 | option | 16 | 8 |
| 13 | never | 0 | 0 |

## Module System

Module resolver (`tools/module_resolver.luon`) maintains a table of up to 64 modules with cycle detection:

```
Module entry: [name_hash | path_ptr | status | func_base | func_count | deps_ptr]
Status: 0=unloaded, 1=loading, 2=loaded, 3=error
```

## Standard Library

20 modules covering: I/O, memory, strings, math, collections (array, hashmap), sorting, error handling (Result/Option), testing, random, crypto, filesystem, environment, formatting, byte buffers, type conversion, structs, and enums.

## Self-Hosting

The bootstrap compiler (`bootstrap/compiler.luon`) can compile itself:

```
compiler.luon → [bootstrap compiler.wasm] → gen1.wasm
gen1.wasm → [compile compiler.luon] → gen2.wasm
SHA-256(gen1.wasm) == SHA-256(gen2.wasm)  ✓ Fixed point
```

## Security

- **WASM Sandboxing** — Memory isolation, instruction limits, call depth limits
- **Sealed Executable (.lse)** — Encrypted container with anti-tamper verification
- **Iteration Guard** — `MAX_ITER = 50,000,000` prevents infinite loops
