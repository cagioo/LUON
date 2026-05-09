# Changelog




All notable changes to the Luon programming language are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

---

## Unreleased

### Added — Standard Library Expansion (20 → 25 modules)
- **`stdlib/slice.luon`** — Zero-copy memory view (slice_new, slice_len, slice_ptr, slice_get, slice_subslice).
- **`stdlib/iter.luon`** — Array iterator protocol (iter_new, iter_has_next, iter_next) preventing standard loop keywords.
- **`stdlib/lstring.luon`** — Length-prefixed string type (lstr_new, lstr_len, lstr_ptr, lstr_get_byte, lstr_eq, lstr_concat).
- **`stdlib/char.luon`** — Unicode scalar value type (char_from_code, char_is_ascii/digit/alpha/upper/lower, char_to_upper/lower).
- **`stdlib/range.luon`** — Range type (range_new, range_start, range_end, range_step, range_len, range_contains, range_get).
- **`stdlib/arena.luon`** — Region-based arena allocator (arena_new, arena_alloc, arena_reset, arena_used, arena_remaining).
- **`stdlib/stack.luon`** — LIFO stack: stack_new, stack_push, stack_pop, stack_peek, stack_len, stack_is_empty, stack_clear, stack_check_overflow
- **`stdlib/deque.luon`** — Ring-buffer double-ended queue: deque_new, deque_push_back, deque_pop_front, deque_len, deque_is_empty, deque_clear
- **`stdlib/time.luon`** — WASI clock utilities: time_now_ns, time_monotonic_ns, time_elapsed_ns, time_to_millis, time_to_seconds

### Added — CLI Expansion (Phase 3)
- Added `skip_block_comment` to `tools/parser.luon` — enables `/* */` multi-line block comments.
- Expanded `tools/cli.luon` from skeleton to full subcommand dispatch with FNV-1a rolling hash matching.
- Added `luon version` — prints version string.
- Added `luon check` — type checking stub (pending typechecker integration).
- Added `luon test` — test runner stub (pending test framework integration).
- Added `luon pkg` — package manager skeleton (routes to `tools/pkg.luon`).
- Created **`tools/pkg.luon`** stub for package manager architecture (`pkg_init`, `pkg_add`).
- Added `cmd_hash` utility function for command string hashing.
- Added `write_str` utility for WASI fd_write output.

### Added — Examples
- **`examples/iter_demo.luon`** — Slice and manual Iterator loop demonstration (`μ` block).
- **`examples/stack_demo.luon`** — Stack push/pop demonstration (push 10, 20, 42 → pop returns 42)
- **`examples/deque_demo.luon`** — Deque FIFO demonstration (push_back 100, 200 → pop_front returns 100)
- **`examples/time_demo.luon`** — WASI clock elapsed time measurement (monotonic clock, busy loop, ns→ms conversion)

### Fixed — Stdlib Bugs (via line-by-line audit)
- **`stdlib/deque.luon`** — Removed dead `acc *= cap` multiply in `deque_push_back` (line 88)
- **`stdlib/time.luon`** — Fixed register collision in `time_elapsed_ns`: start_ns moved from σ₂ to σ₁₀ to avoid WASI param conflict
- **`stdlib/time.luon`** — Fixed division operator in `time_to_millis`/`time_to_seconds`: changed from f64 divide (`⊘_{𝕂}`) to i64 integer divide (`RHom_ℤ`)

### Changed — Documentation
- Updated `LUON_DOCUMENTATION.md` — Added stack, deque, time module docs; updated allocator to free-list; updated repo tree; stdlib count 20→23
- Updated `ROADMAP.md` — Phase 3 progress 20%→35%, 87 components delivered

### Added — P0 Free-List Allocator (Phase 3)
- Upgraded `stdlib/memory.luon` from bump-only to free-list allocator with deallocation support.
- Added `mem_free(ptr)` — returns freed block to linked free-list for reuse.
- Added `mem_realloc(old_ptr, new_size)` — allocate-copy-free reallocation.
- Added `alloc_aligned(size, alignment)` — power-of-2 aligned allocation.
- `alloc()` now searches free-list (first-fit) before falling back to bump allocation.
- All allocations include 16-byte block header `[size:i64 | next_ptr:i64]` for free-list management.
- Minimum block size enforced at 24 bytes; 8-byte alignment guaranteed.

### Added — P0 HashMap Bounds Checking (Phase 3)
- Added `map_check_key(map_ptr, key)` to `stdlib/hashmap.luon` — validates key existence before access, returns canonical `err_not_found` (1) or `err_invalid_argument` (6) for null map.
- Added `map_delete(map_ptr, key)` — tombstone-based key deletion with automatic `len` decrement, returns canonical error codes.

### Added — P0 Array Bounds Safety
- Added `array_check_index` to `stdlib/array.luon`, returning canonical `err_out_of_bounds` for invalid negative or out-of-range indices.

### Added — P0 Checked Arithmetic Safety
- Added deterministic arithmetic safety helpers to `stdlib/error.luon`:
  - `check_divisor_nonzero`
  - `checked_add_i64_result`
  - `checked_sub_i64_result`
- Added canonical `err_overflow` / `err_divide_by_zero` integration points for future compiler/runtime checked arithmetic.

### Added — P0 Type System Capability
- Added expanded integer type IDs to `tools/typechecker.luon`: `i8`, `i16`, `u8`, `u16`, `u32`, and `u64`.
- Added size/alignment rules for narrow signed and unsigned integer types.
- Added integer classification helpers: `type_is_integer`, `type_is_signed_integer`, and `type_is_unsigned_integer`.
- Added safe widening assignment rules for signed and unsigned integer families.

### Added — Professional GitHub Delivery Workflow
- Added mandatory GitHub Project-based delivery workflow to the handover process: Project item → Issue → Branch → Pull Request → Changelog → Merge.
- Upgraded Pull Request template with required Summary, Security Alignment, Files Changed, Validation, Handover Checklist, Issues/Constraints, and Status sections.
- Upgraded Issue templates for feature tasks and bug reports with security alignment, validation, and repository hygiene checkpoints.

### Security
- Added explicit public repository hygiene requirements: no API keys, credentials, personal data, local binaries, sealed artifacts, AI logs, or machine-specific configuration.
- Added PR-level checks to ensure Luon's no-conventional-syntax policy and deterministic sandbox model are preserved.

### Validation
- Audited the public GitHub mirror for common secret/token/key/artifact patterns.

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
