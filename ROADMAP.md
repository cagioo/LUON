# Luon Development Roadmap

**Last Updated:** 2026-05-10  
**Current Version:** 2.2.0-dev  
**Status:** Active Development

---

## Vision & Mission (NON-NEGOTIABLE)

> **Luon is a security-oriented language. Every design decision is evaluated against its security implications.**

### Core Principles

1. **Quantum-Aware, Hardened, AI-Resistant, Reverse-Engineering Resistant** — Luon is designed to resist reverse engineering, resist automated AI analysis and incorporate quantum-aware cryptographic foundations.

2. **Steep Learning Curve = Security Design Choice** — The mathematical syntax raises the cognitive cost of code comprehension, requiring dedicated study to become proficient — a deliberate design tradeoff favoring security over accessibility.

3. **Conventional Syntax = VIOLATION** — Keywords like `if`, `else`, `for`, `while`, `let`, `var`, `print`, `true`, `false`, and "readable" syntax patterns from other languages (Rust, Go, Python, TypeScript) are **STRICTLY FORBIDDEN**. Luon uses pure mathematical logic notation (`branch_if` → `⊥_{𝒯}→^{...}`, loop → `μ`, register → `σ`, etc.).

4. **Protection from Syntax Level** — Other languages add code protection at compiler output level. Luon raises the comprehension barrier from **the syntax level itself**. Luon source code inherently resists casual reading and automated analysis.

5. **Capability May Be Added, Normal Syntax NEVER** — New features may add capability (more data types, operations, stdlib), but MUST use Luon v2 mathematical notation, never conventional syntax.

### What This Means for Development

- **DO:** Add new operators, type system features, memory safety, stdlib modules, compiler optimizations, security features — all using mathematical notation.
- **DO NOT:** Add readable keywords, syntactic sugar, developer-friendly shortcuts, REPL, playground, or anything that makes Luon "easy to learn."
- **The difficulty IS the product.** The complexity IS the security.

---

## Overview

This roadmap tracks every major component of the Luon programming language from genesis to production. Each item shows its **current status** and which release delivered it.

**Status Legend:**
- ✅ **Complete** — Implemented, tested, and shipped
- 🟡 **Foundation** — Basic implementation exists, needs deeper integration
- 🔲 **Planned** — Designed but not yet implemented
- ⛔ **Will Not Implement** — Conflicts with Luon's security-oriented vision

---

## Phase 0: Genesis (v1.0.0)

*The initial proof-of-concept — validating that a mathematical-syntax language can compile to WASM.*

| Component | Status | Description |
|---|---|---|
| Basic accumulator model | ✅ v1.0.0 | Single accumulator (`∂_Ω`) as primary register |
| LaTeX-style syntax (v1) | ✅ v1.0.0 | `\Psi`, `\Phi`, `\delta`, `\sigma`, `\kappa` notation |
| Integer arithmetic ops | ✅ v1.0.0 | Add, Sub, Mul, Div, Mod |
| WASM binary output | ✅ v1.0.0 | Valid `.wasm` binaries from source |
| Wasmtime runtime integration | ✅ v1.0.0 | Execution via Wasmtime WASI |

---

## Phase 1: Event Horizon (v2.0.0)

*Full language architecture — self-hosting compiler, mathematical syntax, security layers, and runtime.*

### 1.1 Core Language

| Component | Status | Description |
|---|---|---|
| mathematical syntax | ✅ v2.0.0 | 50+ regex patterns, 8 mathematical domains, Unicode symbols |
| Dual-mode parser (v1+v2) | ✅ v2.0.0 | Auto-detects LaTeX v1 vs Unicode v2 syntax |
| 34 operators total | ✅ v2.0.0 | Arithmetic, comparison, float, memory, control flow, crypto, bitwise, WASI |
| 127 general-purpose registers | ✅ v2.0.0 | `σ₀`–`σ₁₂₇` with Unicode subscript parsing |
| Float f64 support | ✅ v2.0.0 | Bit-reinterpret architecture for i64↔f64 |
| Block/Loop control flow | ✅ v2.0.0 | `μ` loops, `⊢/⊣` blocks, `branch_if` |
| Function calls by index | ✅ v2.0.0 | `η_N` dispatch |
| Multi-memory segmentation | ✅ v2.0.0 | 5 segments: code, input, output, state, heap |
| Peephole optimizer | ✅ v2.0.0 | `local.set N; local.get N` → `local.tee N` |
| IR optimizer | ✅ v2.0.0 | Constant folding, dead op elimination, strength reduction |

### 1.2 Self-Hosting Compiler

| Component | Status | Description |
|---|---|---|
| `compiler.luon` source | ✅ v2.0.0 | Full compiler written in Luon |
| `compiler.wasm` artifact | ✅ v2.0.0 | 27,345 bytes prebuilt bootstrap binary |
| Fixed-point verification | ✅ v2.0.0 | `gen1 == gen2` SHA-256 verified |
| Rolling hash opcode dispatch | ✅ v2.0.0 | Self-hosting parser uses hash-based matching |
| Native parser (`tools/parser.luon`) | ✅ v2.0.0 | Luon-native parser |
| WASM emitter (`tools/emitter.luon`) | ✅ v2.0.0 | Luon-native WASM binary emitter |

### 1.3 Security Layer 1: WASM Code Transformation Engine

| Component | Status | Description |
|---|---|---|
| Instruction substitution | ✅ v2.0.0 | NOP padding around arithmetic ops |
| Dead code injection | ✅ v2.0.0 | Random `i64.const; drop` sequences |
| Opaque predicates | ✅ v2.0.0 | Fake computations that always evaluate same |
| Control flow flattening | ✅ v2.0.0 | `br_table` state machine dispatcher |
| Register shuffling | ✅ v2.0.0 | Fisher-Yates permutation of locals 1-127 |
| Polymorphic output | ✅ v2.0.0 | Every compile produces unique binary (time+urandom seed) |
| `--obfuscate` flag | ✅ v2.0.0 | Opt-in via CLI flag or `LUON_OBFUSCATE=1` env var |

### 1.4 Security Layer 2: Sealed Executable (.lse)

| Component | Status | Description |
|---|---|---|
| LSE header + magic | ✅ v2.0.0 | `\x7fLSE`, version, seed |
| Opcode permutation table | ✅ v2.0.0 | Fisher-Yates 64-slot per-seed permutation |
| BLAKE2b-CTR encryption | ✅ v2.0.0 | Stream cipher for bytecode |
| Integrity verification footer | ✅ v2.0.0 | BLAKE2b-256 integrity verification |
| LSE encode/decode | ✅ v2.0.0 | Full `lse_format.py` implementation |

### 1.5 Security Layer 3: Luon VM Runtime

| Component | Status | Description |
|---|---|---|
| Accumulator interpreter | ✅ v2.0.0 | Full LSE bytecode execution |
| LSE decryption at runtime | ✅ v2.0.0 | Transparent decrypt + execute |
| Memory sandbox | ✅ v2.0.0 | Isolated linear memory, bounds checking |
| Debug-resistant detection | ✅ v2.0.0 | Debugger/tracer detection |
| JIT compilation | ✅ v2.0.0 | Just-in-time for performance |

### 1.6 Native C Runtime

| Component | Status | Description |
|---|---|---|
| `luon_vm.c` | ✅ v2.0.0 | Single-file C WASM interpreter (~1000 LOC) |
| `luon_compile.h` | ✅ v2.0.0 | Minimal v2→WASM compiler helper |
| WASM binary parser | ✅ v2.0.0 | Parses section headers, function bodies |
| Stack execution engine | ✅ v2.0.0 | i64 stack machine |
| Loop iteration guard | ✅ v2.0.0 | 50M iteration limit |
| Call depth limit | ✅ v2.0.0 | 1,024 max call depth |
| CLI (`luon build/run`) | ✅ v2.0.0 | Basic CLI behavior |

### 1.7 Test Suite

| Component | Status | Description |
|---|---|---|
| v1 syntax tests | ✅ v2.0.0 | add42, arithmetic, complex, loop, conditional, hash, sign_verify, string |
| v2 syntax tests | ✅ v2.0.0 | add42_v2, comprehensive_v2, crypto_v2, memory_v2, float_v2 |
| Obfuscation correctness tests | ✅ v2.0.0 | Output matches across obfuscated/unobfuscated |
| Metamorphic difference tests | ✅ v2.0.0 | Binary differs each compile |
| LSE tests | ✅ v2.0.0 | VM, JIT, sandbox, integrity verification, debug resistance |
| Self-hosting tests | ✅ v2.0.0 | gen1==gen2 verification |
| **Total: 78/78 passing** | ✅ v2.0.0 | All tests green |

---

## Phase 2: Singularity (v2.1.0 — v2.1.1)

*Standard library explosion, type system foundation, tooling infrastructure, and open-source readiness.*

### 2.1 Standard Library Expansion (4 → 20 modules)

| Module | Status | Version | Category |
|---|---|---|---|
| `io.luon` | ✅ Existing | v1.0.0 | Core I/O |
| `memory.luon` | ✅ Existing | v1.0.0 | Memory |
| `string.luon` | ✅ Existing | v1.0.0 | String ops |
| `wasi.luon` | ✅ Existing | v1.0.0 | WASI base |
| `math.luon` | ✅ Added | v2.1.0 | Math (abs, pow, sqrt, gcd, lcm, factorial, π, e) |
| `array.luon` | ✅ Added | v2.1.0 | Dynamic array (push, pop, get, set, reverse) |
| `hashmap.luon` | ✅ Added | v2.1.0 | FNV-1a hash map (set, get, has, delete) |
| `sort.luon` | ✅ Added | v2.1.0 | Insertion sort, binary search |
| `result.luon` | ✅ Added | v2.1.0 | Result\<T,E\> and Option\<T\> |
| `testing.luon` | ✅ Added | v2.1.0 | Assertions, test counters |
| `random.luon` | ✅ Added | v2.1.0 | xorshift64* PRNG |
| `crypto.luon` | ✅ Added | v2.1.0 | SHA-256 init, constant-time eq, memzero |
| `fs.luon` | ✅ Added | v2.1.0 | WASI file open/read/write/close |
| `env.luon` | ✅ Added | v2.1.0 | WASI args, clock, exit |
| `convert.luon` | ✅ Added | v2.1.0 | Clamp, sign extend, hex helpers |
| `struct.luon` | ✅ Added | v2.1.0 | Struct descriptors, field access |
| `enum.luon` | ✅ Added | v2.1.0 | ADT tagged unions, enum_match |
| `fmt.luon` | ✅ Added | v2.1.1 | Formatted output (char, bool, hex) |
| `buffer.luon` | ✅ Added | v2.1.1 | Byte buffer serialization |
| `error.luon` | ✅ Added | v2.1.1 | 16 standard error codes |
| `stack.luon` | ✅ Added | v2.2.0-dev | LIFO stack (push, pop, peek, overflow check) |
| `deque.luon` | ✅ Added | v2.2.0-dev | Ring-buffer deque (push_back, pop_front) |
| `time.luon` | ✅ Added | v2.2.0-dev | WASI clock (monotonic, realtime, elapsed) |

### 2.2 Compiler Infrastructure

| Component | Status | Version | Description |
|---|---|---|---|
| Type checker foundation | 🟡 Added | v2.1.0 | 13 type IDs, size/align, assignment rules — not yet integrated into parser |
| Module resolver foundation | 🟡 Added | v2.1.0 | 64-module table, cycle detection — not yet integrated into build flow |
| CLI foundation | 🟡 Added | v2.1.0 | WASI arg handling — needs full command dispatch |

### 2.3 Developer Tooling

| Component | Status | Version | Description |
|---|---|---|---|
| CI pipeline (`ci.yml`) | ✅ Added | v2.1.0 | Build, test, lint, verify across Linux/macOS |
| Release pipeline (`release.yml`) | ✅ Added | v2.1.0 | Tagged release with binary packaging |
| Test runner (`run_tests.sh`) | ✅ Added | v2.1.0 | Repo structure, stdlib, examples validation |
| VS Code extension v0.2.0 | ✅ Added | v2.1.0 | 12 grammar scopes, 20 snippets |

### 2.4 Documentation & Open Source

| Component | Status | Version | Description |
|---|---|---|---|
| `LUON_DOCUMENTATION.md` | ✅ Complete | v2.1.1 | 1,269 lines, 25 sections, complete technical reference |
| `README.md` rewrite | ✅ Complete | v2.1.1 | Architecture-accurate, no legacy references |
| `CHANGELOG.md` | ✅ Complete | v2.1.1 | Full version history |
| `CONTRIBUTING.md` | ✅ Complete | v2.1.0 | Contribution guidelines |
| `CODE_OF_CONDUCT.md` | ✅ Complete | v2.1.0 | Contributor Covenant |
| `SECURITY.md` | ✅ Complete | v2.1.0 | Security policy |
| `LUON_REFERENCE.md` | ✅ Complete | v2.1.0 | Quick reference card |
| Apache 2.0 License | ✅ Complete | v2.0.0 | Open source license |

### 2.5 Examples (33 total)

| Example | Status | Version |
|---|---|---|
| `add42.luon`, `arithmetic.luon`, `comparison.luon`, `complex.luon` | ✅ | v1.0.0 |
| `loop_test.luon`, `genesis.luon`, `multi_function.luon` | ✅ | v1.0.0 |
| `hash_test.luon`, `sign_verify_test.luon`, `string_test.luon` | ✅ | v2.0.0 |
| `fibonacci.luon`, `bubble_sort.luon`, `spectral.luon` | ✅ | v2.0.0 |
| `hello_wasi.luon`, `hello_data.luon`, `compute_wasi.luon` | ✅ | v2.0.0 |
| `add42_v2.luon`, `comprehensive_v2.luon`, `crypto_v2.luon` | ✅ | v2.0.0 |
| `memory_v2.luon`, `float_v2.luon`, `float_arith.luon` | ✅ | v2.0.0 |
| `codex.luon`, `test_cmp.luon`, `test_leb.luon`, `test_simple.luon`, `sqrt_test.luon` | ✅ | v2.0.0 |
| `linked_list.luon`, `matrix_multiply.luon` | ✅ | v2.1.0 |
| `calculator.luon`, `testing_demo.luon` | ✅ | v2.1.1 |
| `multi_param_test.luon`, `tuple_test.luon`, `scope_test.luon` | ✅ | v2.2.0-dev |

---

## Phase 3: Integration (v2.2.0) — 🔲 NEXT

*Deep compiler integration — connecting the foundations built in Phase 2 into a cohesive pipeline.*

### 3.1 Compiler Pipeline Integration (P0)

| Component | Status | Priority | Description |
|---|---|---|---|
| Integrate typechecker → parser/emitter | 🔲 | P0 | Type checking during compilation |
| Integrate module_resolver → build flow | 🔲 | P0 | Auto-resolve imports |
| Linker symbol table | ✅ | P0 | Function resolution without manual index (Call by Name) |
| Stdlib importable via syntax | 🔲 | P0 | `(⊢_{Γ}^{import} 𝔘[math])` working |
| Multiple parameters via registers | ✅ | P0 | Multi-arg via σ registers + auto-detection from subscripts |
| Multiple return values | ✅ | P0 | Zero-overhead tuples via `→^{N}` projection syntax |
| Lexical scoping | ✅ | P0 | `⊣_{scope}` / `⟧_{scope}` with register shadowing |

### 3.2 CLI Completion (P0)

| Command | Status | Description |
|---|---|---|
| `luon build` | 🟡 | Basic — needs module/type integration |
| `luon run` | 🟡 | Basic — needs improved arg handling |
| `luon check` | 🟡 | Stub implemented — pending typechecker integration |
| `luon test` | 🟡 | Stub implemented — pending test framework integration |
| `luon version` | ✅ | Prints version string via FNV-1a dispatch |
| `luon new` | 🔲 | Scaffold new project |
| `luon init` | 🔲 | Initialize in existing directory |
| `luon doc` | 🔲 | Generate documentation |

### 3.3 Runtime Safety (P0)

| Component | Status | Priority | Description |
|---|---|---|---|
| Array bounds checking | ✅ | P0 | Added `array_check_index` helper returning canonical `err_out_of_bounds` |
| HashMap bounds checking | ✅ | P0 | Added `map_check_key` returning canonical error codes, `map_delete` with tombstone |
| `free()` / dealloc | ✅ | P0 | Free-list allocator with `mem_free`, first-fit reuse |
| `realloc()` / grow | ✅ | P0 | `mem_realloc` with alloc-copy-free strategy |
| Allocator alignment | ✅ | P0 | `alloc_aligned` with power-of-2 alignment support |
| Integer overflow detection | ✅ | P0 | Added deterministic checked add/sub helper foundations in `stdlib/error.luon` |
| Division by zero handling | ✅ | P0 | Added `check_divisor_nonzero` helper returning canonical error code |

---

## Phase 4: Competitiveness (v2.3.0) — 🔲 PLANNED

*Features that make Luon competitive with modern systems languages — using Luon's mathematical syntax, NOT conventional keywords.*

### 4.1 Type System Advancement (P1)

| Component | Status | Priority | Description |
|---|---|---|---|
| Generics (parametric polymorphism) | 🔲 | P1 | Via mathematical notation |
| Traits / Interfaces | 🔲 | P1 | Behavior contracts |
| Type annotations in source | 🔲 | P1 | Via mathematical notation, NOT `fn(a: i64)` style |
| Scope / block scoping | ✅ | P1 | Lexical scoping via `⊣_{scope}` / `⟧_{scope}` in compiler |
| Unsigned integer types (u8/u16/u32/u64) | ✅ | P1 | Added to typechecker with size/alignment/numeric classification and widening rules |
| Smaller integer types (i8/i16) | ✅ | P1 | Added to typechecker with signed widening rules |

### 4.2 Advanced Language Features (P1)

| Component | Status | Priority | Description |
|---|---|---|---|
| Full pattern matching | 🔲 | P1 | Extend `enum_match` to N-way dispatch |
| Iterators / Iterator protocol | 🔲 | P1 | Lazy collection traversal |
| Higher-order functions | 🔲 | P1 | Pass function indices as values |
| Slice type (ptr + len) | 🔲 | P1 | Safe array views |
| Range type | 🔲 | P1 | Numeric ranges |
| Char type (Unicode scalar) | 🔲 | P1 | String processing |
| Basic string type (length-prefixed) | 🔲 | P1 | Modern string representation |
| Tuple type | ✅ | P1 | Zero-overhead multiple return values via `→^{N}` |

### 4.3 Memory Safety (P1)

| Component | Status | Priority | Description |
|---|---|---|---|
| Ownership model | 🔲 | P1 | Move/copy semantics |
| Borrow checker | 🔲 | P1 | Reference safety |
| Lifetimes | 🔲 | P1 | Reference validity |
| Reference types (&T, &mut T) | 🔲 | P1 | Safe references |
| Smart pointers (Box, Rc) | 🔲 | P1 | Owned heap values |
| Arena allocator | 🔲 | P1 | Region-based allocation |
| Stack-allocated arrays | 🔲 | P1 | Fixed-size arrays on stack |

### 4.4 Developer Tooling (P1)

| Component | Status | Priority | Description |
|---|---|---|---|
| Formatter (`luon fmt`) | 🔲 | P1 | Canonical style for math syntax |
| Linter (`luon lint`) | 🔲 | P1 | Unreachable code, invalid registers, memory hazards |
| LSP server | 🔲 | P1 | Diagnostics, completion, hover, go-to-def |
| Package manager (`luon add`) | 🔲 | P1 | `luon.project.json`, lockfile, dependency resolver |

---

## Phase 5: Production (v3.0.0) — 🔲 FUTURE

*World-class systems language — compiler optimizations, WASM spec features, and ecosystem maturity.*

### 5.1 Compiler Optimizations

| Component | Status | Priority |
|---|---|---|
| Constant folding (compile-time eval) | 🔲 | P2 |
| Dead code elimination (tree shaking) | 🔲 | P2 |
| Function inlining | 🔲 | P2 |
| Link-time optimization (LTO) | 🔲 | P2 |
| Incremental compilation | 🔲 | P2 |
| Parallel compilation | 🔲 | P2 |
| Tail call optimization | 🔲 | P2 |

### 5.2 WASM Spec Features

| Component | Status | Priority |
|---|---|---|
| WASM Component Model (WIT) | 🔲 | P2 |
| WASM SIMD (128-bit) | 🔲 | P2 |
| WASM Exception Handling | 🔲 | P2 |
| WASM Tail Calls | 🔲 | P2 |
| Multi-memory | 🔲 | P2 |
| FFI (host function binding) | 🔲 | P2 |

### 5.3 Advanced Tooling

| Component | Status | Priority |
|---|---|---|
| Macro system (via math notation) | 🔲 | P2 |
| Compile-time evaluation (comptime) | 🔲 | P2 |
| Debugger / source maps | 🔲 | P2 |
| Profiler (`luon profile`) | 🔲 | P2 |
| Benchmark framework | 🔲 | P2 |
| Fuzzing (`luon fuzz`) | 🔲 | P2 |
| Code coverage (`luon cover`) | 🔲 | P2 |
| Documentation generator (`luon doc`) | 🔲 | P2 |
| Conditional compilation | 🔲 | P2 |
| Build profiles (debug/release) | 🔲 | P2 |

### 5.4 Ecosystem

| Component | Status | Priority |
|---|---|---|
| Package registry | 🔲 | P2 |
| Dependency resolution (SemVer) | 🔲 | P2 |
| Workspace / monorepo support | 🔲 | P2 |
| Cross-compilation targets | 🔲 | P2 |

### 5.5 Advanced Security

| Component | Status | Priority |
|---|---|---|
| Capability-based security | 🔲 | P2 |
| Sandboxed module permissions | 🔲 | P2 |
| Attribute/annotation system | 🔲 | P2 |
| Derive macros | 🔲 | P2 |

### 5.6 Advanced Type Theory

| Component | Status | Priority |
|---|---|---|
| Type classes | 🔲 | P2 |
| Phantom types | 🔲 | P2 |
| Newtype pattern | 🔲 | P2 |
| Type aliases | 🔲 | P2 |
| Const generics | 🔲 | P2 |
| Associated types | 🔲 | P2 |
| Move semantics | 🔲 | P2 |
| Copy/Clone semantics | 🔲 | P2 |
| Drop / Destructor (RAII) | 🔲 | P2 |


---

## ⛔ Will Not Implement (ANTI-VISION — Permanently Rejected)

> These features are **permanently rejected** because they violate Luon's core security vision. Any AI agent or contributor who proposes these will be overruled.

### Rejected: Conventional Syntax (Anti-Readability)

| Feature | Reason for Rejection |
|---|---|
| Variable declaration (`let`, `var`, `const`) | Manual register `σ₀`–`σ₁₂₇` is a deliberate security design choice. Keywords lower the barrier for automated analysis. |
| Named function calls (by name) | `η_N` (call by index) is a deliberate security design choice. Readable function names simplify reverse engineering. |
| `if`/`else` statements | `branch_if` / `⊥_{𝒯}→^{...}` is a deliberate security design choice. `if/else` keywords lower comprehension barriers. |
| `while` loop keyword | `μ` loop primitive is a deliberate design choice. `while` keyword is too conventional. |
| `for` loop / `for-each` | Absence of `for` loop raises the comprehension barrier. `for i in 0..10` is too readable. |
| `print()` / `println()` | Manual WASI `fd_write` is a deliberate design choice. Built-in `print()` lowers barriers. |
| String literals (`"hello"`) | Manual byte writing to memory raises the comprehension barrier. |
| Boolean literals (`true`/`false`) | 0/1 integer representation is a deliberate design choice. |
| Array literals (`[1, 2, 3]`) | Manual memory write initialization raises the comprehension barrier. |
| String concatenation (`+`) | Manual `memcpy` is a deliberate design choice. |
| String interpolation (`fmt!`) | Manual WASI fd_write raises the comprehension barrier. |
| Type inference (`let x = 42`) | Types must be explicit via mathematical notation. |
| Implicit return | Return must be explicit via `⊥_{𝒯}→^{ex falso}⊤_{𝒯}`. |
| Operator overloading | Luon already has unique mathematical notation. |
| Method syntax (`.push()`) | `array_push(arr, 42)` via index call is a deliberate design choice. |
| Impl blocks | Rust-style `impl` syntax lowers the comprehension barrier. |
| Closures / Lambdas (`\|x\| x + 1`) | Too conventional — lowers the comprehension barrier. |
| Destructuring (`let (x, y) = p`) | Access must be via manual offset. |
| Error propagation (`?`) | Syntactic sugar lowers the error handling comprehension barrier. |
| Try/catch / panic/recover | `try/catch` keywords are too conventional. |
| Defer / cleanup | `defer` keyword lowers the comprehension barrier. Resource cleanup must be manual. |
| REPL | Lowers the learning barrier, reducing the comprehension cost for unauthorized analysis. |
| Playground (web-based) | Lowers the learning barrier, reducing the cognitive cost of code comprehension. |
| Unsafe blocks (`unsafe {}`) | Gives illusion of safety without ownership model. Safety via WASM sandbox. |

### Rejected: Anti-Security (Breaks Sandbox/Determinism)

| Feature | Reason for Rejection |
|---|---|
| Networking / HTTP | Destroys sandbox isolation. Network access from WASM = data exfiltration risk. |
| Concurrency / Threading | Threading = non-determinism = blockchain consensus destruction. |
| Channels (message passing) | Introduces non-determinism and timing side-channels. |
| Mutex / Lock | Threading model is non-deterministic. Contra single-threaded WASM sandbox. |
| WASM Threads (SharedArrayBuffer) | Spectre/Meltdown attack vector. Non-deterministic execution. |
| WASM GC | GC pause non-deterministic destroys blockchain finality. Bump allocator is deterministic. |
| Reflection / Introspection | Undermines 6-pass WASM code transformation engine. Runtime type inspection simplifies reverse engineering. |
| Regular expressions | Regex engine vulnerable to ReDoS (exponential backtracking). DoS vector in blockchain VM. |

### Rejected: Over-Engineering (Impractical for Self-Hosting 27KB Compiler)

| Feature | Reason for Rejection |
|---|---|
| Algebraic effects | CPS transformation too complex for self-hosting compiler. |
| Dependent types | Dependent type checker too complex for self-hosting compiler. |
| GADTs | Not relevant for blockchain VM. Over-engineering. |

---

## Progress Summary

| Phase | Status | Components | Complete |
|---|---|---|---|
| **Phase 0: Genesis** | ✅ Complete | 5/5 | 100% |
| **Phase 1: Event Horizon** | ✅ Complete | 41/41 | 100% |
| **Phase 2: Singularity** | ✅ Complete | 34/34 | 100% |
| **Phase 3: Integration** | 🔶 Active | 7/20 | 35% |
| **Phase 4: Competitiveness** | 🔲 Planned | 0/26 | 0% |
| **Phase 5: Production** | 🔲 Future | 0/32 | 0% |
| **TOTAL** | — | **87/158** | **~55%** |

> **87 components delivered.** Stdlib expanded to 23 modules, CLI dispatch operational. Next focus: deep compiler pipeline integration (typechecker + module resolver).

---

*Vesege — Trust Through Transparency*
