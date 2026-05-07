# Changelog


All notable changes to the Luon programming language are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

---

## [2.1.1] — Singularity Patch — 2026-05-07

### Added — Standard Library Completion (20 modules total)
- **`stdlib/fmt.luon`** — Formatted output: print_char, print_newline, print_space, print_bool, print_hex_byte, print_separator, print_tab, print_ok, print_fail
- **`stdlib/buffer.luon`** — Byte buffer: buf_new, buf_write_byte, buf_write_i64, buf_read_byte, buf_read_i64, buf_len, buf_cap, buf_reset, buf_seek, buf_ptr
- **`stdlib/error.luon`** — 16 standard error codes (none, not_found, permission, io, out_of_memory, out_of_bounds, invalid_argument, overflow, divide_by_zero, type_mismatch, parse, eof, timeout, already_exists, not_supported, full, empty) + is_error, is_success, error_or_default

### Added — Documentation Overhaul
- **`README.md`** — Complete rewrite reflecting current architecture (no more Python/Rust references)
- **`docs/architecture.md`** — Accurate compilation pipeline, type system, module system, memory layout
- **`docs/getting-started.md`** — Updated installation (GCC-based), syntax tutorial, VS Code setup
- **`docs/project-manifest.md`** — `luon.project.json` specification for project management

### Added — Examples
- **`examples/calculator.luon`** — Multi-function arithmetic: (10+20)*3 - 100/5 = 70
- **`examples/testing_demo.luon`** — Testing framework demonstration with assertions

### Fixed
- `.gitignore` updated for clean public repository (excludes binaries, secrets, AI traces, legacy artifacts)
- GitHub repo synced with internal architecture (removed legacy Cargo.toml, crates/, compiler/)

---

## [2.1.0] — Singularity — 2026-05-07

### Added — Standard Library Expansion (15 modules total)
- **`stdlib/math.luon`** — abs, min, max, clamp, pow, sqrt, log2, gcd, lcm, factorial, sign, f64 constants (π, e, ∞)
- **`stdlib/array.luon`** — Dynamic array: new, push, pop, get, set, len, cap, clear, swap, reverse, contains, index_of, fill
- **`stdlib/hashmap.luon`** — Hash map: new, set, get, has, delete, len, clear (FNV-1a, open addressing, linear probing)
- **`stdlib/sort.luon`** — Insertion sort, binary search, is_sorted
- **`stdlib/result.luon`** — Result<T,E> and Option<T> algebraic types: ok, err, some, none, unwrap, unwrap_or, is_ok/is_err/is_some/is_none
- **`stdlib/testing.luon`** — Test framework: assert_eq, assert_ne, assert_true, assert_false, test_begin, test_pass, test_fail, test_summary
- **`stdlib/random.luon`** — xorshift64* PRNG: seed, next_u64, range, bool
- **`stdlib/crypto.luon`** — SHA-256 init, constant_time_eq, rotate_right_32, memzero
- **`stdlib/fs.luon`** — WASI filesystem: file_open, file_read, file_write, file_close
- **`stdlib/env.luon`** — WASI environment: args_count, args_get, exit, clock_time, clock_monotonic, env_count
- **`stdlib/convert.luon`** — Type conversions: i64_to_i32, clamp_u8/u16/u32, sign_extend_8/16/32, bool_to_int, hex_digit, byte_to_hex, swap_bytes_32

### Added — Compiler Infrastructure
- **`tools/typechecker.luon`** — Static type system foundation: type_size, type_align, type_is_numeric, type_can_assign, type_error (13 type IDs: void, bool, i32, i64, f32, f64, ptr, str, array, struct, fn, result, option, never)
- **`tools/module_resolver.luon`** — Module resolution: register, find, set_status, get_status, module_count (cycle detection, 64-module table)

### Added — Tooling & CI/CD
- **`.github/workflows/ci.yml`** — Full CI pipeline: build runtime (Linux/macOS), compile examples, test stdlib, verify bootstrap, lint docs
- **`.github/workflows/release.yml`** — Automated release with binary packaging for Linux/macOS
- **`tests/run_tests.sh`** — Comprehensive test runner: repo structure, stdlib coverage, example compilation, VS Code extension validation

### Added — VS Code Extension v0.2.0
- **Enhanced TextMate grammar** — 12 scope categories: module/function declarations, arithmetic/bitwise/comparison operators, control flow, memory ops, WASI syscalls, registers, constants, types, brackets
- **20 code snippets** — module, func, main, block, loop, return, const, store, load, add, sub, mul, div, branch, call, mstore, mload, wasi, program template
- **Marketplace metadata** — keywords, repository, icon support, Apache-2.0 license

### Added — Real-World Examples
- **`examples/linked_list.luon`** — Singly linked list: node allocation, prepend, length, sum traversal
- **`examples/matrix_multiply.luon`** — 3×3 matrix multiplication with identity verification

### Architecture
- Standard library expanded from 4 to 15 modules
- Type system foundation with 13 primitive type IDs and assignment compatibility rules
- Module resolver with cycle detection and 64-module capacity
- Test infrastructure with automated CI/CD

---

## [2.0.0] — Event Horizon — 2026-05-06

### Added
- **Self-hosting compiler** — `compiler.luon` can compile itself (`gen1 == gen2` verified)
- **v2 EXTREME syntax** — Pure mathematical logic notation across 8 domains
- **34 operators** — Arithmetic, comparison, float, memory, control flow, crypto, bitwise, WASI
- **Float f64 support** — Bit-reinterpret architecture for floating-point operations
- **Standard library** — `stdlib/io.luon`, `stdlib/memory.luon`, `stdlib/string.luon`, `stdlib/wasi.luon`
- **WASI integration** — Standalone execution via `wasmtime` without custom runtime
- **WASM obfuscation engine** — 6-pass metamorphic pipeline (instruction substitution, dead code injection, opaque predicates, control flow flattening, register shuffling)
- **Luon Sealed Executable (.lse)** — Encrypted container with BLAKE2b-CTR cipher and anti-tamper verification
- **Sandboxed VM** — Memory isolation, instruction limits, call depth limits, execution timeout
- **Peephole optimizer** — `local.set 1; local.get 1` → `local.tee 1` optimization
- **CLI tool** — `luon init`, `luon build`, `luon run`, `luon check`, `luon test`
- **VS Code extension** — Syntax highlighting for v1 and v2 syntax
- **Self-hosting toolchain** — Native parser and emitter written in Luon
- **Multi-memory segmentation** — 5-segment memory layout (code, input, output, state, heap)
- **IR optimizer** — Constant folding, dead op elimination, strength reduction

### Architecture
- Accumulator-based model with 127 general-purpose registers (σ₀–σ₁₂₇)
- Rolling hash opcode dispatch for self-hosting compiler
- 3-layer compilation pipeline: Source → Assembler → WASM → Runtime

---

## [1.0.0] — Genesis

### Added
- Initial compiler implementation
- Basic accumulator model
- LaTeX-style syntax (v1)
- Integer arithmetic operators
- WASM binary output
- Wasmtime runtime integration

---

*Vesege — Trust Through Transparency*
