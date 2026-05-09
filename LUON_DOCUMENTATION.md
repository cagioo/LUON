# Luon Programming Language — Official Documentation

**Version:** 2.1.1 Singularity Patch  
**Status:** Official Public Documentation  
**License:** Apache-2.0  
**Target Runtime:** WebAssembly / WASI  
**Last Updated:** 2026-05-07

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Design Goals](#2-design-goals)
3. [Repository Architecture](#3-repository-architecture)
4. [Execution Model](#4-execution-model)
5. [Memory Architecture](#5-memory-architecture)
6. [Language Syntax](#6-language-syntax)
7. [Operator Reference](#7-operator-reference)
8. [Type System](#8-type-system)
9. [Module and Import System](#9-module-and-import-system)
10. [Standard Library](#10-standard-library)
11. [Structs and Records](#11-structs-and-records)
12. [Enums and Algebraic Data Types](#12-enums-and-algebraic-data-types)
13. [Error Handling](#13-error-handling)
14. [Testing Framework](#14-testing-framework)
15. [Compiler and Toolchain](#15-compiler-and-toolchain)
16. [Runtime Architecture](#16-runtime-architecture)
17. [Self-Hosting Compiler](#17-self-hosting-compiler)
18. [WASI Integration](#18-wasi-integration)
19. [Project Manifest](#19-project-manifest)
20. [Examples](#20-examples)
21. [Editor Support](#21-editor-support)
22. [Security Model](#22-security-model)
23. [Public Repository Policy](#23-public-repository-policy)
24. [Current Limitations](#24-current-limitations)
25. [Roadmap](#25-roadmap)

---

## 1. Introduction

**Luon** is a military-grade security programming language designed for WebAssembly (WASM), sandboxed execution, and the Vesege ecosystem. It combines a low-level accumulator-based execution model with a distinctive mathematical syntax known as **v2 EXTREME syntax**.

Luon is built around five principles:

1. **Security Absolutism** — every design decision serves security first. The language is intentionally anti-quantum, anti-hacker, anti-AI, and anti-reverse engineering.
2. **Intentional Complexity** — Luon is designed to be impossible to learn without official documentation. The steep learning curve is a deliberate security feature, not a deficiency.
3. **Syntax-Level Obfuscation** — unlike other languages that add obfuscation at the compiler output level, Luon source code itself is already a form of obfuscation through pure mathematical logic notation.
4. **Portability** — programs compile to WASM and can run on WASI-compatible runtimes.
5. **Mathematical Identity** — Luon uses symbolic notation inspired by type theory, category theory, logic, algebra, and formal systems. Conventional keywords (`if`, `else`, `for`, `while`, `let`, `var`, `print`) are strictly forbidden.

Luon is not intended to be a generic scripting language or a developer-friendly language. Its primary direction is:

- military-grade sandboxed computation,
- smart-contract-style execution with deterministic guarantees,
- anti-reverse-engineering binary output (metamorphic + sealed executables),
- WASM-native systems programming,
- Vesege VM and blockchain infrastructure.

> **Important:** The difficulty of reading and writing Luon IS the product. The complexity IS the security. Any proposal to add conventional syntax, readable keywords, or "developer experience" improvements that reduce complexity is a violation of Luon's core vision.

---

## 2. Design Goals

### 2.1 Primary Goals

| Goal | Description |
|---|---|
| Security absolutism | Every design decision serves security first. Anti-quantum, anti-hacker, anti-AI, anti-reverse engineering. |
| Intentional complexity | The steep learning curve is a deliberate security feature. Impossible to learn without official documentation. |
| Syntax-level obfuscation | Source code itself is already obfuscated through mathematical notation. No conventional keywords. |
| WASM-first | Luon targets WebAssembly as its primary backend. |
| Deterministic execution | Programs must be predictable and suitable for consensus-sensitive environments. |
| Self-hosting | The compiler must be able to compile itself. |
| Sandboxed runtime | Execution is isolated through WASM memory and runtime limits. |
| Minimal trusted base | Core runtime is small and auditable. |
| Metamorphic output | Every compilation produces a different binary (time+urandom seed). |
| Anti-reverse engineering | 6-pass obfuscation engine + sealed executable format (.lse). |

### 2.2 Permanent Non-Goals (Will NEVER Be Implemented)

The following are **permanently rejected** because they violate Luon's security vision:

- Conventional keywords (`if`, `else`, `for`, `while`, `let`, `var`, `const`, `print`, `true`, `false`)
- Readable syntax patterns from other languages (Rust, Go, Python, TypeScript)
- Developer-friendly shortcuts, syntactic sugar, or ergonomic improvements
- REPL (interactive learning tool — destroys learning curve defense)
- Web playground (allows trying without documentation)
- String literals (`"hello"`) — manual byte writing is a security feature
- Method syntax (`.push()`) — index-based calls are a security feature
- Named function calls — `η_N` index dispatch is a security feature
- Threading, concurrency, channels, mutex (non-deterministic — destroys blockchain consensus)
- Networking/HTTP from within WASM (destroys sandbox isolation)
- Garbage collection (non-deterministic pauses destroy blockchain finality)
- Reflection/introspection (reverse engineering tool for attackers)

### 2.3 What CAN Be Added

New features may add **capability** without normalizing syntax:

- More data types (unsigned integers, slices, ranges, tuples)
- Type system features (generics, traits, ownership, lifetimes) — via mathematical notation
- Memory safety (borrow checker, arena allocator, bounds checking)
- Compiler optimizations (constant folding, dead code elimination, inlining)
- Tooling (LSP, formatter, linter, package manager) — for authorized developers only
- WASM spec features (SIMD, tail calls, component model, multi-memory)
- Security features (capability-based security, sandboxed module permissions)
- Standard library expansion (more collections, crypto, serialization)

---

## 3. Repository Architecture

The Luon repository is organized as follows:

```text
LUON/
├── bootstrap/
│   ├── compiler.luon          # Self-hosting compiler source
│   └── compiler.wasm          # Bootstrap compiler artifact
│
├── runtime/
│   ├── luon_vm.c              # Native C WASM runtime / CLI bootstrap
│   └── luon_compile.h         # Minimal v2 EXTREME → WASM compiler helper
│
├── stdlib/                    # Standard library modules
│   ├── array.luon
│   ├── buffer.luon
│   ├── convert.luon
│   ├── crypto.luon
│   ├── enum.luon
│   ├── env.luon
│   ├── error.luon
│   ├── fmt.luon
│   ├── fs.luon
│   ├── hashmap.luon
│   ├── io.luon
│   ├── math.luon
│   ├── memory.luon
│   ├── random.luon
│   ├── result.luon
│   ├── sort.luon
│   ├── stack.luon
│   ├── string.luon
│   ├── struct.luon
│   ├── testing.luon
│   ├── time.luon
│   ├── wasi.luon
│   └── deque.luon
│
├── tools/
│   ├── cli.luon               # Native CLI foundation
│   ├── emitter.luon           # WASM binary emitter
│   ├── module_resolver.luon   # Module/import resolver
│   ├── parser.luon            # Native parser
│   └── typechecker.luon       # Type-system foundation
│
├── examples/                  # Example programs
├── docs/                      # Additional documentation
├── editor/luon-vscode/        # VS Code extension
├── .github/                   # CI, release, issue templates
│
├── README.md
├── CHANGELOG.md
├── LICENSE
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── SECURITY.md
├── LUON_REFERENCE.md
└── LUON_DOCUMENTATION.md
```

---

## 4. Execution Model

Luon uses an **accumulator-based execution model**.

### 4.1 Core Concepts

| Concept | Symbol | Meaning |
|---|---|---|
| Accumulator | `∂_Ω` | Primary working value. Most operators read and write this value. |
| Input parameter | `σ₀` | Function input parameter. |
| Return value | `∂_Ω` | The accumulator is returned by `Collapse_Return`. |
| General registers | `σ₂`–`σ₁₂₇` | Temporary values and arguments. |
| Function call | `η_N` | Calls function index `N`. |

### 4.2 Function Calling Convention

Each function follows this model:

1. The input parameter is received as `σ₀`.
2. The accumulator `∂_Ω` is initialized from the input parameter.
3. Operators transform the accumulator and registers.
4. `Collapse_Return` returns the accumulator.

Example:

```luon
∃!Φ ∈ Hom(𝒞,𝒟)[add42] ⊣^{op} {
  (Ext⁰_𝔄(∂_Ω, 42))_{Spec ℤ}
  ⊥_{𝒯}→^{ex falso}⊤_{𝒯}
}
```

This function adds `42` to the accumulator and returns the result.

---

## 5. Memory Architecture

Luon programs use WASM linear memory. The default runtime configuration allocates **30 WASM pages**.

One WASM page is 64 KiB, so 30 pages provide approximately 1.875 MiB.

### 5.1 Memory Segments

| Address Range | Purpose |
|---|---|
| `0x00000–0x0FFFF` | Code/data area and low scratch area |
| `0x10000–0x1FFFF` | Input buffer |
| `0x20000–0x2FFFF` | Output buffer |
| `0x30000–0x4FFFF` | Compiler metadata and toolchain state |
| `0x50000–0x5FFFF` | Runtime state segment |
| `0x60000+` | Heap segment |
| `0xE000–0xEFFF` | WASI argument scratch |
| `0xF000–0xFFFF` | General scratch space |

### 5.2 Reserved State Addresses

| Address | Purpose | Owner |
|---:|---|---|
| `0x50000` | Heap allocation pointer | `memory.luon` |
| `0x51000` | Test total counter | `testing.luon` |
| `0x51008` | Test passed counter | `testing.luon` |
| `0x51010` | Test failed counter | `testing.luon` |
| `0x51020` | PRNG state | `random.luon` |
| `0x52000` | SHA-256 working state | `crypto.luon` |
| `0x54000` | Type descriptor table | `typechecker.luon` |
| `0x54FF8` | Module count | `module_resolver.luon` |
| `0x55000` | Module table | `module_resolver.luon` |
| `0x56000` | Struct/enum descriptor space | `struct.luon`, `enum.luon` |
| `0xF000` | String conversion scratch | `string.luon` |
| `0xF100` | WASI IOV scratch | `io.luon`, `fs.luon` |
| `0xF110` | WASI bytes-written scratch | `io.luon`, `fs.luon` |
| `0xF200` | Formatting scratch buffer | `fmt.luon` |
| `0xF300` | File descriptor result scratch | `fs.luon` |

### 5.3 Heap Allocator

The current allocator is a **free-list allocator** with first-fit search:

```text
alloc(size):
  scan free-list for block >= size (first-fit)
  if found: unlink and return
  else: bump-allocate from heap_pointer

free(ptr):
  prepend block to free-list

realloc(ptr, new_size):
  allocate new_size, copy old data, free old block
```

Properties:

- First-fit free-list with fallback to bump allocation.
- `mem_free(ptr)` returns blocks to the free-list for reuse.
- `mem_realloc(ptr, old_size, new_size)` for in-place reallocation.
- `alloc_aligned(size, alignment)` for aligned allocations.
- Block header: `[size:i64 | next:i64]` = 16 bytes per free block.

---

## 6. Language Syntax

Luon currently supports v2 EXTREME syntax as its primary syntax.

### 6.1 Module Declaration

```luon
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[module_name] ⊢_Γ {
  ...
}
```

### 6.2 Function Declaration

```luon
∃!Φ ∈ Hom(𝒞,𝒟)[function_name] ⊣^{op} {
  ...
  ⊥_{𝒯}→^{ex falso}⊤_{𝒯}
}
```

### 6.3 Constant Load

```luon
(42 ⊣_{Δ;Γ} ∂_Ω)^{axiom}
```

Meaning:

```text
acc = 42
```

### 6.4 Register Store and Load

```luon
(∂_Ω ⊢_{Γ;Δ} σ₂)^{seq}      // σ₂ = acc
(σ₂ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}   // acc = σ₂
```

### 6.5 Return

```luon
⊥_{𝒯}→^{ex falso}⊤_{𝒯}
```

Meaning:

```text
return acc
```

---

## 7. Operator Reference

### 7.1 Arithmetic Operators

| Operation | Syntax | Meaning |
|---|---|---|
| Add | `(Ext⁰_𝔄(∂_Ω, N))_{Spec ℤ}` | `acc += N` |
| Sub | `(Tor₀^𝔄(∂_Ω, -N))_{Spec ℤ}` | `acc -= N` |
| Mul | `(∂_Ω ⊗_ℤ N)^{⊗L}_{D(𝔄)}` | `acc *= N` |
| Div | `(RHom_ℤ(N, ∂_Ω))_{D^b(𝔄)}` | `acc /= N` |
| Mod | `(∂_Ω mod_{ℤ} N)^{Euclidean}` | `acc %= N` |

### 7.2 Bitwise Operators

| Operation | Syntax | Meaning |
|---|---|---|
| And | `(∂_Ω ∧_{Bool} N)^{∧-intro}` | `acc &= N` |
| Or | `(∂_Ω ∨_{Bool} N)^{∨-intro}` | `acc |= N` |
| Xor | `(∂_Ω ⊕_{Bool} N)^{⊕-intro}` | `acc ^= N` |
| Shl | `(∂_Ω ≪_{Galois} N)^{adjunction}` | `acc <<= N` |
| Shr | `(∂_Ω ≫_{Galois} N)^{adjunction}` | `acc >>= N` |

### 7.3 Comparison Operators

| Operation | Syntax | Meaning |
|---|---|---|
| Eqz | `(∂_Ω ≡_{E_∞} ⊥)^{acyclic}` | `acc == 0` |
| Eq | `(∂_Ω ≡_{E_∞} N)^{acyclic}` | `acc == N` |
| Ne | `(∂_Ω ≠_{E_∞} N)^{acyclic}` | `acc != N` |
| Lt | `(∂_Ω ≺_{ω₁} N)^{well-order}` | `acc < N` |
| Gt | `(∂_Ω ≻_{ω₁} N)^{well-order}` | `acc > N` |
| Le | `(∂_Ω ⪯_{ω₁} N)^{well-order}` | `acc <= N` |
| Ge | `(∂_Ω ⪰_{ω₁} N)^{well-order}` | `acc >= N` |

### 7.4 Memory Operators

| Operation | Syntax | Meaning |
|---|---|---|
| Store i64 | `(⊨_{𝕂,Γ}^{κ-forcing} ↦_𝒯 σ_N)` | `mem[acc] = σ_N` |
| Store byte | `(⊨_{𝕂,Γ}^{κ-forcing} ↦₈_𝒯 σ_N)` | `mem8[acc] = σ_N` |
| Load i64 | `(⊩_{𝕂,Σ}^{Γ-generic} ↤_𝒯 ∂_Ω)` | `acc = mem[acc]` |
| Load byte | `(⊩_{𝕂,Σ}^{Γ-generic} ↤₈_𝒯 ∂_Ω)` | `acc = mem8[acc]` |

### 7.5 Control Flow Operators

| Operation | Syntax | Meaning |
|---|---|---|
| Block begin | `⊢_{Γ}^{⊃I}⟦` | Start block |
| Block end | `⟧^{⊃E}_{Δ}⊣` | End block |
| Loop begin | `μ_{ω₁}^{CK}⟦` | Start loop |
| Loop end | `⟧_{ω₁}^{CK}μ` | End loop |
| Branch | `(⊬_{PA}^{Gödel} σ_N)^{ω-rule}` | Unconditional branch |
| Branch if | `(∂_Ω ⊬_{PA}^{Gödel} σ_N)^{ω-rule}` | Conditional branch |
| Return | `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` | Return accumulator |
| Call | `(η_N ∘_{2-Cat} ∂_Ω)^{Kan}` | Call function index `N` |
| Nop | `(id_{∂}^{nat})^{Yoneda}` | No operation |

### 7.6 Floating-Point Operators

Luon stores floating-point values as `i64` bit patterns at the language level. The compiler reinterprets values as `f64` only during emitted WASM operations.

| Operation | Syntax | Meaning |
|---|---|---|
| FAdd | `(st(∂_{*ℝ}) ⊕_{*ℝ} st(σ_N))^{transfer}` | f64 add |
| FSub | `(st(∂_{*ℝ}) ⊖_{*ℝ} st(σ_N))^{transfer}` | f64 subtract |
| FMul | `(st(∂_{*ℝ}) ⊗_{*ℝ} st(σ_N))^{transfer}` | f64 multiply |
| FDiv | `(st(∂_{*ℝ}) ⊘_{*ℝ} st(σ_N))^{transfer}` | f64 divide |
| FSqrt | `(√_{*ℝ} st(∂_{*ℝ}))^{shadow}` | f64 square root |

---

## 8. Type System

The type-system foundation is implemented in `tools/typechecker.luon`.

### 8.1 Type IDs

| ID | Type | Size | Description |
|---:|---|---:|---|
| 0 | `void` | 0 | No value |
| 1 | `bool` | 1 | Boolean value, represented as 0 or 1 |
| 2 | `i32` | 4 | 32-bit integer |
| 3 | `i64` | 8 | 64-bit integer |
| 4 | `f32` | 4 | 32-bit float |
| 5 | `f64` | 8 | 64-bit float |
| 6 | `ptr` | 8 | Linear memory address |
| 7 | `str` | 16 | String descriptor or pointer-length pair |
| 8 | `array` | 8 | Heap pointer to array header |
| 9 | `struct` | varies | User-defined record |
| 10 | `fn` | 8 | Function reference/index |
| 11 | `result` | 16 | Tagged result pair |
| 12 | `option` | 16 | Tagged optional pair |
| 13 | `never` | 0 | Non-returning computation |
| 14 | `i8` | 1 | 8-bit signed integer |
| 15 | `i16` | 2 | 16-bit signed integer |
| 16 | `u8` | 1 | 8-bit unsigned integer |
| 17 | `u16` | 2 | 16-bit unsigned integer |
| 18 | `u32` | 4 | 32-bit unsigned integer |
| 19 | `u64` | 8 | 64-bit unsigned integer |

### 8.2 Type Functions

| Function | Purpose |
|---|---|
| `type_size(type_id)` | Returns byte size. |
| `type_align(type_id)` | Returns ABI alignment. |
| `type_is_numeric(type_id)` | Returns whether the type is numeric. |
| `type_is_integer(type_id)` | Returns whether the type is an integer. |
| `type_is_signed_integer(type_id)` | Returns whether the type is signed integer. |
| `type_is_unsigned_integer(type_id)` | Returns whether the type is unsigned integer. |
| `type_can_assign(dst, src)` | Returns whether assignment is legal. |
| `type_error(code, line, col)` | Creates a type error record. |

### 8.3 Assignment Rules

Current assignment compatibility:

1. Exact type match is allowed.
2. Signed widening is allowed: `i8 → i16 → i32 → i64`.
3. Unsigned widening is allowed: `u8 → u16 → u32 → u64`.
4. `f32 → f64` widening is allowed.
5. Other conversions require explicit conversion helpers.

---

## 9. Module and Import System

The module resolver foundation is implemented in `tools/module_resolver.luon`.

### 9.1 Module Table

Each module entry has the following layout:

```text
[name_hash:i64 | path_ptr:i64 | status:i64 | func_base:i64 | func_count:i64 | deps_ptr:i64]
```

Each entry is 48 bytes. The current capacity is 64 modules.

### 9.2 Module Status Codes

| Code | Status | Description |
|---:|---|---|
| 0 | unloaded | Module registered but not loaded. |
| 1 | loading | Module is currently loading; used for cycle detection. |
| 2 | loaded | Module loaded successfully. |
| 3 | error | Module failed to load. |

### 9.3 Resolver Functions

| Function | Description |
|---|---|
| `module_table_init()` | Initializes the module table. |
| `module_register(name_hash, path_ptr)` | Registers a module. |
| `module_find(name_hash)` | Finds a module by hash. |
| `module_set_status(index, status)` | Updates module status. |
| `module_get_status(index)` | Reads module status. |
| `module_count()` | Returns registered module count. |

### 9.4 Planned Import Syntax

```luon
(⊢_{Γ}^{import} 𝔘[module_name])
```

The resolver exists today as a foundation. Full compiler integration is part of the next roadmap stage.

---

## 10. Standard Library

Luon ships with **23 standard library modules**.

### 10.1 Core Modules

#### `io.luon`

Basic stdout/stderr support through WASI `fd_write`.

Functions:

- `write_stdout`
- `write_stderr`
- `print_str`
- `print_int`

#### `memory.luon`

Bump allocator.

Functions:

- `heap_init`
- `alloc`

#### `string.luon`

Null-terminated string and byte-copy operations.

Functions:

- `strlen`
- `strcmp`
- `memcpy`
- `itoa`
- `atoi`

#### `wasi.luon`

Minimal WASI helpers.

Functions:

- `exit`
- `get_args_count`

### 10.2 Math and Conversion

#### `math.luon`

Integer math and f64 constants.

Functions include:

- `abs`
- `min`
- `max`
- `clamp`
- `pow`
- `sqrt`
- `log2`
- `gcd`
- `lcm`
- `factorial`
- `sign`
- `f64_pi`
- `f64_e`
- `f64_inf`
- `f64_zero`
- `f64_one`

#### `convert.luon`

Numeric conversions and bit manipulation.

Functions include:

- `i64_to_i32`
- `clamp_u8`
- `clamp_u16`
- `clamp_u32`
- `sign_extend_8`
- `sign_extend_16`
- `sign_extend_32`
- `bool_to_int`
- `hex_digit`
- `byte_to_hex`
- `swap_bytes_32`

### 10.3 Collections

#### `array.luon`

Dynamic array layout:

```text
[cap:i64 | len:i64 | data_ptr:i64]
```

Functions include:

- `array_new`
- `array_push`
- `array_pop`
- `array_get`
- `array_set`
- `array_len`
- `array_cap`
- `array_clear`
- `array_swap`
- `array_reverse`
- `array_contains`
- `array_index_of`
- `array_fill`
- `array_check_index`

#### `hashmap.luon`

Open-addressing hash map layout:

```text
Header: [cap:i64 | len:i64 | buckets_ptr:i64]
Bucket: [hash:i64 | key:i64 | value:i64 | occupied:i64]
```

Functions include:

- `map_hash`
- `map_new`
- `map_set`
- `map_get`
- `map_has`
- `map_len`
- `map_clear`

#### `stack.luon`

LIFO stack data structure.

Layout:

```text
[cap:i64 | len:i64 | data_ptr:i64]
```

Functions:

- `stack_new(capacity)` — allocate stack with minimum capacity 8
- `stack_push(stack_ptr, value)` — push value, returns new length
- `stack_pop(stack_ptr)` — pop top value (0 if empty)
- `stack_peek(stack_ptr)` — read top value without removing
- `stack_len(stack_ptr)` — current element count
- `stack_cap(stack_ptr)` — capacity
- `stack_is_empty(stack_ptr)` — 1 if empty
- `stack_clear(stack_ptr)` — reset length to 0
- `stack_check_overflow(stack_ptr)` — 0 if space, 15 (err_full) if full

#### `deque.luon`

Ring-buffer double-ended queue.

Layout:

```text
[cap:i64 | len:i64 | head:i64 | tail:i64 | data_ptr:i64]
```

Functions:

- `deque_new(capacity)` — allocate deque with minimum capacity 8
- `deque_push_back(deque_ptr, value)` — append to tail
- `deque_pop_front(deque_ptr)` — remove from head (FIFO)
- `deque_len(deque_ptr)` — current element count
- `deque_is_empty(deque_ptr)` — 1 if empty
- `deque_clear(deque_ptr)` — reset all indices to 0

#### `sort.luon`

Sorting and search utilities.

Functions:

- `insertion_sort`
- `is_sorted`
- `binary_search`

### 10.4 Error Handling and Testing

#### `result.luon`

Provides runtime representations of `Result<T,E>` and `Option<T>`.

Result layout:

```text
[tag:i64 | value:i64]
```

Tags:

- `0 = Ok`
- `1 = Err`

Option layout:

```text
[tag:i64 | value:i64]
```

Tags:

- `0 = None`
- `1 = Some`

#### `error.luon`

Defines canonical error codes:

| Code | Name |
|---:|---|
| 0 | none / success |
| 1 | not_found |
| 2 | permission |
| 3 | io |
| 4 | out_of_memory |
| 5 | out_of_bounds |
| 6 | invalid_argument |
| 7 | overflow |
| 8 | divide_by_zero |
| 9 | type_mismatch |
| 10 | parse |
| 11 | eof |
| 12 | timeout |
| 13 | already_exists |
| 14 | not_supported |
| 15 | full |
| 16 | empty |

Safety helpers:

- `is_error(code)`
- `is_success(code)`
- `error_or_default(code, default_value, success_value)`
- `check_divisor_nonzero(divisor)`
- `checked_add_i64_result(a, b, result)`
- `checked_sub_i64_result(a, b, result)`

#### `testing.luon`

Testing counters are stored at:

```text
0x51000 total_tests
0x51008 passed_tests
0x51010 failed_tests
```

Functions include:

- `test_begin`
- `test_pass`
- `test_fail`
- `assert_eq`
- `assert_ne`
- `assert_true`
- `assert_false`
- `test_summary`

### 10.5 System Modules

#### `fs.luon`

WASI filesystem wrappers.

Functions:

- `file_open`
- `file_read`
- `file_write`
- `file_close`

#### `env.luon`

WASI environment and process wrappers.

Functions:

- `args_count`
- `args_get`
- `exit`
- `clock_time`
- `clock_monotonic`
- `env_count`

#### `time.luon`

WASI clock utilities for timing and benchmarking.

Functions:

- `time_now_ns()` — nanoseconds since epoch (realtime clock)
- `time_monotonic_ns()` — nanoseconds from monotonic clock
- `time_elapsed_ns(start)` — elapsed nanoseconds since start
- `time_to_millis(ns)` — convert nanoseconds to milliseconds
- `time_to_seconds(ns)` — convert nanoseconds to seconds

### 10.6 Utility Modules

#### `fmt.luon`

Formatted output helpers.

Functions:

- `print_char`
- `print_newline`
- `print_space`
- `print_bool`
- `print_hex_byte`
- `print_separator`
- `print_tab`
- `print_ok`
- `print_fail`

#### `buffer.luon`

Byte buffer layout:

```text
[cap:i64 | len:i64 | pos:i64 | data_ptr:i64]
```

Functions:

- `buf_new`
- `buf_write_byte`
- `buf_write_i64`
- `buf_read_byte`
- `buf_read_i64`
- `buf_len`
- `buf_cap`
- `buf_reset`
- `buf_seek`
- `buf_ptr`

#### `random.luon`

Pseudo-random number generation using xorshift64*.

Functions:

- `seed`
- `next_u64`
- `range`
- `bool`

#### `crypto.luon`

Cryptographic primitives foundation.

Functions:

- `sha256_init`
- `constant_time_eq`
- `rotate_right_32`
- `memzero`

---

## 11. Structs and Records

Struct runtime support is implemented in `stdlib/struct.luon`.

### 11.1 Struct Descriptor Layout

```text
[name_hash:i64 | field_count:i64 | total_size:i64 | fields_ptr:i64]
```

### 11.2 Field Descriptor Layout

```text
[name_hash:i64 | type_id:i64 | offset:i64 | size:i64]
```

### 11.3 Functions

| Function | Purpose |
|---|---|
| `struct_define(name_hash, field_count)` | Creates a struct descriptor. |
| `struct_add_field(desc, index, name_hash, type_id, size)` | Adds a field descriptor. |
| `struct_new(desc)` | Allocates a struct instance. |
| `struct_get_field(instance, offset)` | Reads a field by offset. |
| `struct_set_field(instance, offset, value)` | Writes a field by offset. |
| `struct_size(desc)` | Returns struct byte size. |

---

## 12. Enums and Algebraic Data Types

Enum/ADT runtime support is implemented in `stdlib/enum.luon`.

### 12.1 Enum Instance Layout

```text
[tag:i64 | payload:i64...]
```

### 12.2 Enum Descriptor Layout

```text
[name_hash:i64 | variant_count:i64 | max_payload_size:i64 | variants_ptr:i64]
```

### 12.3 Variant Descriptor Layout

```text
[name_hash:i64 | tag:i64 | payload_size:i64]
```

### 12.4 Functions

| Function | Purpose |
|---|---|
| `enum_define(name_hash, variant_count)` | Creates an enum descriptor. |
| `enum_add_variant(desc, index, name_hash, payload_size)` | Adds a variant. |
| `enum_create(tag, payload)` | Creates an enum instance. |
| `enum_tag(instance)` | Reads the tag. |
| `enum_payload(instance)` | Reads the payload. |
| `enum_is_variant(instance, expected_tag)` | Checks a tag. |
| `enum_match(instance, tag0_fn, tag1_fn, tag2_fn, tag3_fn)` | Dispatches by tag. |

---

## 13. Error Handling

Luon provides two complementary error-handling mechanisms:

1. **Error codes** through `stdlib/error.luon`.
2. **Tagged result values** through `stdlib/result.luon`.

### 13.1 Recommended Pattern

For low-level runtime functions, return error codes:

```text
0 = success
nonzero = error
```

For higher-level abstractions, return `Result<T,E>`:

```text
Result layout: [tag | value]
tag 0 = Ok(value)
tag 1 = Err(error_code)
```

### 13.2 Option Values

Use `Option<T>` for optional values:

```text
Option layout: [tag | value]
tag 0 = None
tag 1 = Some(value)
```

### 13.3 Checked Arithmetic Safety Helpers

`stdlib/error.luon` provides deterministic helper functions for arithmetic safety without adding conventional syntax:

```text
check_divisor_nonzero(divisor) -> 0 | err_divide_by_zero
checked_add_i64_result(a, b, result) -> 0 | err_overflow
checked_sub_i64_result(a, b, result) -> 0 | err_overflow
```

These helpers are designed for compiler/runtime integration. They preserve Luon's explicit low-level execution model while providing standardized error codes for overflow and division-by-zero handling.

---

## 14. Testing Framework

The testing framework is implemented in `stdlib/testing.luon`.

### 14.1 Test Counters

```text
0x51000 total_tests
0x51008 passed_tests
0x51010 failed_tests
```

### 14.2 Basic Test Flow

```text
test_begin()
assert_eq(actual, expected)
assert_true(value)
assert_false(value)
test_summary() -> failed_count
```

A test program should return `test_summary()`. A return value of `0` means success.

---

## 15. Compiler and Toolchain

### 15.1 Native Parser

File:

```text
tools/parser.luon
```

Responsibilities:

- read Luon source from memory,
- scan source line by line,
- hash operator patterns,
- dispatch to opcode IDs,
- emit an intermediate IR stream.

### 15.2 WASM Emitter

File:

```text
tools/emitter.luon
```

Responsibilities:

- write WASM magic/version,
- emit type/function/memory/export/code sections,
- encode ULEB128 and SLEB128,
- translate Luon IR operations to WASM opcodes.

### 15.3 CLI Foundation

File:

```text
tools/cli.luon
```

Current status: foundation only. It provides WASI argument handling but not yet full production command dispatch.

Planned commands:

```text
luon new
luon init
luon build
luon run
luon check
luon test
luon fmt
luon lint
luon doc
luon repl
luon version
```

### 15.4 Type Checker

File:

```text
tools/typechecker.luon
```

Provides type IDs, type size/alignment, numeric checks, assignment compatibility, and error-record creation.

### 15.5 Module Resolver

File:

```text
tools/module_resolver.luon
```

Provides module registration, lookup, status management, and cycle-state tracking.

---

## 16. Runtime Architecture

The native runtime is implemented in C:

```text
runtime/luon_vm.c
```

The runtime provides:

- WASM binary parsing,
- function body execution,
- local variables,
- stack execution,
- memory model,
- exports,
- basic CLI behavior,
- loop iteration guard.

Important constants:

```c
MAX_STACK = 65536
MAX_FUNCS = 256
MAX_EXPORTS = 64
MAX_LOCALS = 256
MAX_BLOCKS = 256
MAX_CALL = 1024
MEM_PAGES = 30
MAX_ITER = 50000000
```

The runtime is intentionally small and auditable. It is not a complete general-purpose WASM runtime.

---

## 17. Self-Hosting Compiler

Luon includes a self-hosting compiler:

```text
bootstrap/compiler.luon
bootstrap/compiler.wasm
```

### 17.1 Fixed-Point Goal

The compiler is expected to satisfy:

```text
compiler.luon --compiled-by compiler.wasm -> gen1.wasm
compiler.luon --compiled-by gen1.wasm     -> gen2.wasm
SHA-256(gen1.wasm) == SHA-256(gen2.wasm)
```

This fixed point means the compiler can reproduce itself.

### 17.2 Bootstrap Artifact

`bootstrap/compiler.wasm` is included as the initial compiler artifact. Public releases should include it because users need a starting compiler.

---

## 18. WASI Integration

Luon uses WASI Preview 1 imports where needed.

Common WASI functions:

| WASI Function | Purpose |
|---|---|
| `fd_write` | Write to stdout/stderr/files. |
| `fd_read` | Read from file descriptors. |
| `fd_close` | Close file descriptors. |
| `path_open` | Open filesystem paths. |
| `proc_exit` | Terminate process. |
| `args_sizes_get` | Read argument count and buffer size. |
| `args_get` | Read arguments. |
| `clock_time_get` | Read realtime or monotonic clocks. |
| `environ_sizes_get` | Read environment count and size. |

IOV layout:

```text
[buf_ptr:i32 | buf_len:i32]
```

Luon stores packed IOV values in scratch memory such as `0xF100`.

---

## 19. Project Manifest

Luon project manifests use `luon.project.json`.

Example:

```json
{
  "name": "hello",
  "version": "0.1.0",
  "entry": "src/main.luon",
  "output": "build/hello.wasm",
  "target": "wasm32-wasi",
  "memory_pages": 30,
  "stdlib": ["io", "memory", "string", "math"]
}
```

See:

```text
docs/project-manifest.md
```

for the complete manifest specification.

---

## 20. Examples

The `examples/` directory contains 30+ programs covering:

- arithmetic,
- loops,
- comparisons,
- memory,
- strings,
- WASI output,
- floating point,
- hashing/sign/verify,
- Fibonacci,
- bubble sort,
- linked list,
- matrix multiplication,
- calculator,
- testing framework demo.

Recommended starting examples:

| File | Purpose |
|---|---|
| `add42.luon` | Minimal arithmetic program. |
| `fibonacci.luon` | Loop and register usage. |
| `calculator.luon` | Multi-function arithmetic. |
| `bubble_sort.luon` | Sorting and memory. |
| `linked_list.luon` | Heap allocation and pointers. |
| `matrix_multiply.luon` | Nested loops and array addressing. |
| `testing_demo.luon` | Test framework usage. |
| `hello_wasi.luon` | WASI output. |

---

## 21. Editor Support

Luon includes a VS Code extension:

```text
editor/luon-vscode/
```

Features:

- TextMate grammar for v2 EXTREME syntax,
- bracket configuration,
- comment configuration,
- 20 code snippets,
- module/function/operator highlighting.

Important files:

```text
editor/luon-vscode/package.json
editor/luon-vscode/language-configuration.json
editor/luon-vscode/syntaxes/luon.tmLanguage.json
editor/luon-vscode/snippets/luon.json
```

---

## 22. Security Model

### 22.1 WASM Sandboxing

Luon targets WASM because it provides:

- linear memory isolation,
- explicit imports,
- deterministic bytecode,
- small runtime surface,
- portable execution.

### 22.2 Runtime Guards

The native runtime includes:

- stack limits,
- function limits,
- call-depth limits,
- loop iteration guard,
- fixed memory page count.

### 22.3 Sealed Executables

Luon also defines a sealed executable format (`.lse`) for tamper-protected distribution of compiled modules.

---

## 23. Contributing and Repository Guidelines

Luon is an open-source project licensed under Apache 2.0. Contributions are welcome.

### 23.1 Repository Hygiene

The repository should only contain source code, documentation, examples, and build configurations. Generated binaries, environment files, credentials, and cache files should be excluded via `.gitignore`.

### 23.2 Contribution Workflow

1. Fork the repository.
2. Create a feature branch.
3. Make changes and test locally.
4. Submit a pull request with a clear description.

See `CONTRIBUTING.md` for full guidelines.

---

## 24. Current Limitations

Luon 2.1.1 is a strong foundation. The following are genuine technical limitations (NOT design choices):

Known limitations:

1. The CLI is still a foundation and needs complete command dispatch.
2. Module resolver exists but is not fully integrated into the compiler pipeline.
3. Type checker exists but requires deeper compiler integration.
4. Function calls rely on function indices (this is BY DESIGN for security — not a limitation).
5. Array and hashmap bounds checks need strengthening.
6. Allocator has no `free` or `realloc` yet.
7. Formatter, linter, and LSP are not yet implemented.
8. Package manager is specified but not implemented.
9. Generics, traits, iterators are planned but not complete.
10. Ownership/borrow checker not yet implemented.

> **Note:** The absence of conventional syntax (`if/else`, `for`, `while`, `let`, `print`, string literals) is NOT a limitation — it is a deliberate security feature. See Section 2.2.

---

## 25. Roadmap

> **Guiding Principle:** All new features MUST use Luon v2 EXTREME mathematical notation. Conventional syntax will NEVER be added.

### 25.1 Basic Completion Phase (P0)

Priority — capability additions without syntax normalization:

- complete CLI commands (`luon build/run/check/test/version`),
- integrate module resolver into build flow,
- integrate typechecker into parser/emitter pipeline,
- multiple parameters via register convention (mathematical notation),
- multiple return values via register packing,
- `free()` / `realloc()` / arena allocator,
- array/hashmap bounds checking,
- integer overflow detection,
- division by zero handling,
- unsigned integer types (`u8`, `u16`, `u32`, `u64`),
- smaller integer types (`i8`, `i16`).

### 25.2 Medium Phase (P1)

Priority — competitive capability with mathematical syntax:

- generics (via mathematical notation, NOT `<T>` syntax),
- traits/interfaces (via mathematical notation),
- full pattern matching (extend `enum_match` to N-way dispatch),
- iterators (capability, not syntax sugar),
- slice type (ptr + len),
- range type,
- ownership / borrow checker (SECURITY — memory safety),
- lifetimes (SECURITY),
- formatter (`luon fmt` — canonical style for math syntax),
- linter (`luon lint` — unreachable code, memory hazards),
- LSP server (for authorized developers only),
- package manager (`luon add`, lockfile, dependency resolver).

### 25.3 Advanced Phase (P2)

Priority — world-class without compromising vision:

- macro system (via mathematical notation),
- compile-time evaluation (comptime),
- WASM Component Model (WIT interfaces),
- WASM SIMD (128-bit performance),
- WASM Tail Calls (TCO),
- debugger and source maps (internal tooling),
- constant folding, dead code elimination, inlining,
- incremental compilation,
- link-time optimization (LTO),
- conditional compilation,
- build profiles (debug/release),
- capability-based security,
- sandboxed module permissions,
- profiler and benchmark framework,
- fuzzing (`luon fuzz`),
- code coverage (`luon cover`).

### 25.4 Permanently Rejected (Will NEVER Be Implemented)

See Section 2.2 for the complete list. Summary:

- No conventional keywords (`if`, `else`, `for`, `while`, `let`, `var`, `print`, `true`, `false`)
- No readable syntax patterns from other languages
- No REPL, no web playground
- No threading, concurrency, channels, mutex
- No networking/HTTP from within WASM sandbox
- No garbage collection
- No reflection/introspection
- No regular expressions (ReDoS vulnerability)
- No string literals, string interpolation, or method syntax

---

## Final Note

This document is the official technical reference for the Luon programming language. It must be updated whenever the language architecture, compiler pipeline, runtime, standard library, public repository policy, or roadmap changes.

Luon is open-source and community-driven. Its long-term goal is to become the most secure, auditable, WASM-first systems language — where the difficulty of the language itself is the first layer of defense. The steep learning curve is not a bug to be fixed; it is the core product.
