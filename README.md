<p align="center">
  <img src="docs/vesege_logo.png" alt="Vesege" width="120">
</p>

<h1 align="center">Luon Programming Language</h1>

<p align="center">
  <strong>A security-first, systems-level WASM language with mathematical identity — built for the post-quantum era.</strong>
</p>

<p align="center">
  <a href="#why-luon">Why Luon?</a> •
  <a href="#security-architecture">Security</a> •
  <a href="#installation">Installation</a> •
  <a href="#syntax-reference">Syntax</a> •
  <a href="#standard-library">Standard Library</a> •
  <a href="ROADMAP.md">Roadmap</a> •
  <a href="CONTRIBUTING.md">Contributing</a>
</p>

<p align="center">
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-Apache_2.0-blue.svg" alt="License"></a>
  <a href="CHANGELOG.md"><img src="https://img.shields.io/badge/Version-2.1.1_Singularity-brightgreen.svg" alt="Version"></a>
  <a href="https://github.com/cagioo/LUON"><img src="https://img.shields.io/badge/Target-WebAssembly-blueviolet.svg" alt="Target"></a>
</p>

---

## Why Luon?

Most programming languages optimize for **developer ergonomics** — readability, convenience, low learning curves. Luon takes a fundamentally different approach.

**Luon optimizes for security.**

Every design decision — from the mathematical syntax to the register-based execution model — exists to make Luon programs **resistant to reverse engineering, AI analysis, and quantum-era attacks**. The steep learning curve is not a limitation; it is the **first layer of defense**.

### Design Philosophy

| Principle | Description |
|---|---|
| **Security First** | Every language feature is evaluated against its security implications. Convenience never overrides safety. |
| **Anti-Reverse Engineering** | Source code uses mathematical logic notation that is inherently resistant to automated analysis, pattern matching, and AI comprehension without official documentation. |
| **Post-Quantum Ready** | Cryptographic foundations designed with quantum computing threats in mind. BLAKE2b, constant-time operations, and sealed executable format. |
| **Deterministic Execution** | Single-threaded, no GC, no non-determinism. Every execution path is fully predictable — essential for blockchain consensus and auditable computation. |
| **WASM Sandboxing** | Programs run inside WebAssembly's linear memory sandbox with strict isolation, making escape impossible by design. |
| **Self-Hosting** | The compiler compiles itself (`gen1 == gen2` verified). Zero dependency on external toolchains for compilation. |
| **Minimal Attack Surface** | Small, auditable C runtime (~1000 LOC). No external dependencies. No standard library bloat. |

### What Makes Luon Different

```
┌─────────────────────────────────────────────────────────────────┐
│                    TYPICAL LANGUAGE                              │
│  if (x > 0) { print("positive"); }    ← Readable by anyone     │
│  AI can analyze, decompile, reverse    ← Security liability     │
├─────────────────────────────────────────────────────────────────┤
│                    LUON                                          │
│  (∂_Ω ≻_{ω₁} 0)^{well-order}         ← Mathematical notation  │
│  Anti-AI, anti-reverse engineering     ← Security feature       │
└─────────────────────────────────────────────────────────────────┘
```

Luon's syntax is derived from **category theory**, **formal logic**, **algebraic geometry**, and **set theory**. This is not aesthetic — it is a security mechanism. Source code that looks like a mathematics paper cannot be casually read, copied, or reverse-engineered.

---

## Security Architecture

Luon implements security at **every layer** of the stack:

### Layer 1: Syntax-Level Obfuscation
Source code uses Unicode mathematical symbols (∀, ∃, ⊢, ⊣, ∂, Ω, σ, η, μ, ⊥, ⊤) instead of conventional keywords. The source itself is a form of obfuscation.

### Layer 2: WASM Obfuscation Engine (6 Passes)
The compiler includes an opt-in metamorphic obfuscation pipeline:

| Pass | Technique | Effect |
|---|---|---|
| 1 | Instruction Substitution | NOP padding around arithmetic ops |
| 2 | Dead Code Injection | Random `i64.const; drop` sequences |
| 3 | Opaque Predicates | Fake computations that always evaluate the same way |
| 4 | Control Flow Flattening | Splits linear code into shuffled `br_table` state machine |
| 5 | Register Shuffling | Fisher-Yates permutation of local indices 1-127 |
| 6 | Metamorphic Output | Every compilation produces a **different** binary (seeded from time + urandom) |

### Layer 3: Sealed Executable Format (.lse)
Luon defines a proprietary encrypted container format:

```
┌─────────────────────────────────────┐
│ Magic: \x7fLSE                      │
│ Version + Seed                      │
│ Integrity Hash (BLAKE2b-256)        │
├─────────────────────────────────────┤
│ Opcode Permutation Table            │
│ (Fisher-Yates, 64-slot, per-seed)   │
├─────────────────────────────────────┤
│ Encrypted Bytecode                  │
│ (BLAKE2b-CTR Stream Cipher)         │
├─────────────────────────────────────┤
│ Anti-Tamper Footer                  │
│ (BLAKE2b-256 over header+cipher)    │
└─────────────────────────────────────┘
```

### Layer 4: Runtime Sandbox
The Luon VM enforces strict execution limits:

| Guard | Limit |
|---|---|
| Stack depth | 65,536 entries |
| Function count | 256 max |
| Call depth | 1,024 max |
| Local variables | 256 per function |
| Memory | 30 WASM pages (~1.9 MiB) |
| Loop iterations | 50,000,000 max |

### Layer 5: WASM Memory Isolation
Every Luon program runs inside WASM's linear memory model — no access to host filesystem, network, or system calls beyond explicitly imported WASI functions.

---

## Quick Demo

```
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[hello] ⊢_Γ {

  ∃!Φ ∈ Hom(𝒞,𝒟)[main] ⊣^{op} {
    // Add 42 to the input parameter
    (Ext⁰_𝔄(∂_Ω, 42))_{Spec ℤ}
    ⊥_{𝒯}→^{ex falso}⊤_{𝒯}
  }

}
```

```bash
$ luon run hello.luon -a 10
52
```

What happened:
1. `∀ₛₚₑ𝒸 Ψ ∈ 𝔘[hello]` — Declares module `hello`
2. `∃!Φ ∈ Hom(𝒞,𝒟)[main]` — Declares function `main`
3. `(Ext⁰_𝔄(∂_Ω, 42))_{Spec ℤ}` — Adds 42 to accumulator (input was 10)
4. `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` — Returns the accumulator (52)

---

## Installation

### Prerequisites

- **GCC** or any C compiler (for building the native runtime)

### Build from Source

```bash
git clone https://github.com/cagioo/LUON.git
cd LUON
gcc -O2 -o luon runtime/luon_vm.c
```

### Quick Install (Linux/macOS)

```bash
bash install.sh
```

After installation, restart your shell or run `source ~/.bashrc`.

### Verify

```bash
./luon build examples/fibonacci.luon
./luon run examples/fibonacci.luon -a 10
```

---

## Architecture

### Compilation Pipeline

```
┌─────────────┐      ┌──────────────┐      ┌────────────────┐
│  .luon       │─────▶│  Parser +    │─────▶│  .wasm         │
│  (source)    │      │  Emitter     │      │  (binary)      │
└─────────────┘      └──────────────┘      └───────┬────────┘
                                                    │
                      ┌──────────────┐              │
                      │  Runtime     │◀─────────────┘
                      │  (C / WASI)  │
                      └──────────────┘
```

1. **Parser** — Reads v2 EXTREME syntax, hashes operator patterns via rolling hash, generates IR
2. **Emitter** — Translates IR to valid WASM binary (i64 accumulator model)
3. **Runtime** — Executes via native C runtime or any WASI-compatible runtime (Wasmtime, etc.)

### Self-Hosting Compiler

The compiler (`bootstrap/compiler.luon`) can compile itself, producing byte-identical output across generations:

```
compiler.luon  ──[compiled by compiler.wasm]──▶  gen1.wasm
compiler.luon  ──[compiled by gen1.wasm]──────▶  gen2.wasm
SHA-256(gen1.wasm) == SHA-256(gen2.wasm)  ✅
```

### Execution Model

Luon uses an **accumulator-based model** with 127 general-purpose registers:

| Concept | Symbol | Purpose |
|---|---|---|
| Accumulator | `∂_Ω` | Primary working value — most operations read/write this |
| Input parameter | `σ₀` | Function input |
| Registers | `σ₁`–`σ₁₂₇` | Temporary storage |
| Function call | `η_N` | Calls function by index `N` |
| Return | `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` | Returns the accumulator |

### Memory Layout

Luon programs use WASM linear memory (30 pages ≈ 1.9 MiB):

```
Address Range        Purpose
─────────────        ────────────────────────────────
0x00000–0x0FFFF      Code/data area
0x10000–0x1FFFF      Input buffer
0x20000–0x2FFFF      Output buffer
0x30000–0x4FFFF      Compiler metadata
0x50000–0x5FFFF      Runtime state (heap ptr, test counters, PRNG, crypto)
0x60000+             Heap segment (bump allocator)
0xE000–0xEFFF        WASI argument scratch
0xF000–0xFFFF        General scratch space
```

---

## Syntax Reference

### Module Declaration
```
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[module_name] ⊢_Γ {
  ...
}
```

### Function Declaration
```
∃!Φ ∈ Hom(𝒞,𝒟)[function_name] ⊣^{op} {
  ...
  ⊥_{𝒯}→^{ex falso}⊤_{𝒯}
}
```

### Arithmetic Operators

| Operation | Syntax | Equivalent |
|---|---|---|
| Add | `(Ext⁰_𝔄(∂_Ω, N))_{Spec ℤ}` | `acc += N` |
| Subtract | `(Tor₀^𝔄(∂_Ω, -N))_{Spec ℤ}` | `acc -= N` |
| Multiply | `(∂_Ω ⊗_ℤ N)^{⊗L}_{D(𝔄)}` | `acc *= N` |
| Divide | `(RHom_ℤ(N, ∂_Ω))_{D^b(𝔄)}` | `acc /= N` |
| Modulo | `(∂_Ω mod_{ℤ} N)^{Euclidean}` | `acc %= N` |

### Bitwise Operators

| Operation | Syntax | Equivalent |
|---|---|---|
| AND | `(∂_Ω ∧_{Bool} N)^{∧-intro}` | `acc &= N` |
| OR | `(∂_Ω ∨_{Bool} N)^{∨-intro}` | `acc \|= N` |
| XOR | `(∂_Ω ⊕_{Bool} N)^{⊕-intro}` | `acc ^= N` |
| Shift Left | `(∂_Ω ≪_{Galois} N)^{adjunction}` | `acc <<= N` |
| Shift Right | `(∂_Ω ≫_{Galois} N)^{adjunction}` | `acc >>= N` |

### Comparison Operators

| Operation | Syntax | Equivalent |
|---|---|---|
| Equal zero | `(∂_Ω ≡_{E_∞} ⊥)^{acyclic}` | `acc == 0` |
| Equal | `(∂_Ω ≡_{E_∞} N)^{acyclic}` | `acc == N` |
| Not equal | `(∂_Ω ≠_{E_∞} N)^{acyclic}` | `acc != N` |
| Less than | `(∂_Ω ≺_{ω₁} N)^{well-order}` | `acc < N` |
| Greater than | `(∂_Ω ≻_{ω₁} N)^{well-order}` | `acc > N` |

### Control Flow

| Operation | Syntax |
|---|---|
| Block begin | `⊢_{Γ}^{⊃I}⟦` |
| Block end | `⟧^{⊃E}_{Δ}⊣` |
| Loop begin | `μ_{ω₁}^{CK}⟦` |
| Loop end | `⟧_{ω₁}^{CK}μ` |
| Branch (unconditional) | `(⊬_{PA}^{Gödel} σ_N)^{ω-rule}` |
| Branch if (conditional) | `(∂_Ω ⊬_{PA}^{Gödel} σ_N)^{ω-rule}` |
| Return | `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` |
| Call function | `(η_N ∘_{2-Cat} ∂_Ω)^{Kan}` |

### Memory Operations

| Operation | Syntax | Equivalent |
|---|---|---|
| Store i64 | `(⊨_{𝕂,Γ}^{κ-forcing} ↦_𝒯 σ_N)` | `mem[acc] = σ_N` |
| Store byte | `(⊨_{𝕂,Γ}^{κ-forcing} ↦₈_𝒯 σ_N)` | `mem8[acc] = σ_N` |
| Load i64 | `(⊩_{𝕂,Σ}^{Γ-generic} ↤_𝒯 ∂_Ω)` | `acc = mem[acc]` |
| Load byte | `(⊩_{𝕂,Σ}^{Γ-generic} ↤₈_𝒯 ∂_Ω)` | `acc = mem8[acc]` |

### Floating-Point Operations

| Operation | Syntax | Equivalent |
|---|---|---|
| FAdd | `(st(∂_{*ℝ}) ⊕_{*ℝ} st(σ_N))^{transfer}` | f64 add |
| FSub | `(st(∂_{*ℝ}) ⊖_{*ℝ} st(σ_N))^{transfer}` | f64 subtract |
| FMul | `(st(∂_{*ℝ}) ⊗_{*ℝ} st(σ_N))^{transfer}` | f64 multiply |
| FDiv | `(st(∂_{*ℝ}) ⊘_{*ℝ} st(σ_N))^{transfer}` | f64 divide |
| FSqrt | `(√_{*ℝ} st(∂_{*ℝ}))^{shadow}` | f64 square root |

### Register Operations

| Operation | Syntax | Equivalent |
|---|---|---|
| Store to register | `(∂_Ω ⊢_{Γ;Δ} σ₂)^{seq}` | `σ₂ = acc` |
| Load from register | `(σ₂ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}` | `acc = σ₂` |
| Constant load | `(42 ⊣_{Δ;Γ} ∂_Ω)^{axiom}` | `acc = 42` |

> **Full reference:** See [LUON_DOCUMENTATION.md](LUON_DOCUMENTATION.md) for the complete 1,200+ line technical specification including type system, module resolver, struct/enum runtime, and all 34 operators.

---

## Standard Library

Luon ships with **20 standard library modules**:

### Core

| Module | Description | Key Functions |
|--------|-------------|---------------|
| `io` | WASI stdout/stderr | `write_stdout`, `print_str`, `print_int` |
| `memory` | Bump heap allocator | `heap_init`, `alloc` |
| `string` | String operations | `strlen`, `strcmp`, `memcpy`, `itoa`, `atoi` |
| `wasi` | WASI system interface | `exit`, `get_args_count` |

### Math & Conversion

| Module | Description | Key Functions |
|--------|-------------|---------------|
| `math` | Integer/f64 math | `abs`, `min`, `max`, `pow`, `sqrt`, `gcd`, `lcm`, `factorial` |
| `convert` | Type conversions | `clamp_u8/u16/u32`, `sign_extend`, `hex_digit`, `swap_bytes` |
| `fmt` | Formatted output | `print_char`, `print_bool`, `print_hex_byte` |

### Collections

| Module | Description | Key Functions |
|--------|-------------|---------------|
| `array` | Dynamic array (vector) | `push`, `pop`, `get`, `set`, `reverse`, `contains` |
| `hashmap` | Hash map (FNV-1a) | `set`, `get`, `has`, `delete`, `clear` |
| `sort` | Sorting algorithms | `insertion_sort`, `binary_search`, `is_sorted` |
| `buffer` | Byte buffer I/O | `write_byte`, `write_i64`, `read_byte`, `seek` |

### Safety & Error Handling

| Module | Description | Key Functions |
|--------|-------------|---------------|
| `result` | Result\<T,E\> / Option\<T\> | `ok`, `err`, `some`, `none`, `unwrap`, `is_ok` |
| `error` | 16 standard error codes | `is_error`, `is_success`, `error_or_default` |
| `testing` | Test assertions | `assert_eq`, `assert_ne`, `assert_true`, `test_summary` |

### System & Security

| Module | Description | Key Functions |
|--------|-------------|---------------|
| `fs` | WASI filesystem | `file_open`, `file_read`, `file_write`, `file_close` |
| `env` | WASI environment | `args_count`, `args_get`, `clock_time`, `exit` |
| `random` | xorshift64* PRNG | `seed`, `next_u64`, `range`, `bool` |
| `crypto` | Cryptographic primitives | `sha256_init`, `constant_time_eq`, `memzero` |

### Type System

| Module | Description | Key Functions |
|--------|-------------|---------------|
| `struct` | Struct/record types | `struct_define`, `struct_new`, `get_field`, `set_field` |
| `enum` | Enum/ADT tagged unions | `enum_define`, `enum_create`, `enum_tag`, `enum_match` |

---

## Toolchain

| Tool | File | Description |
|------|------|-------------|
| **Parser** | `tools/parser.luon` | Hash-based opcode dispatch parser (written in Luon) |
| **Emitter** | `tools/emitter.luon` | WASM binary emitter with ULEB128/SLEB128 encoding |
| **CLI** | `tools/cli.luon` | Command-line interface foundation |
| **Type Checker** | `tools/typechecker.luon` | 13 type IDs, assignment compatibility, error reporting |
| **Module Resolver** | `tools/module_resolver.luon` | Import resolution, cycle detection, 64-module capacity |

### Type System (13 Primitive Types)

| ID | Type | Size | Description |
|---:|---|---:|---|
| 0 | `void` | 0 | No value |
| 1 | `bool` | 1 | Boolean (0/1 integer) |
| 2 | `i32` | 4 | 32-bit signed integer |
| 3 | `i64` | 8 | 64-bit signed integer |
| 4 | `f32` | 4 | 32-bit float |
| 5 | `f64` | 8 | 64-bit float |
| 6 | `ptr` | 8 | Linear memory pointer |
| 7 | `str` | 16 | String descriptor |
| 8 | `array` | 8 | Heap array pointer |
| 9 | `struct` | varies | User-defined record |
| 10 | `fn` | 8 | Function reference |
| 11 | `result` | 16 | Tagged result pair |
| 12 | `option` | 16 | Tagged optional pair |
| 13 | `never` | 0 | Non-returning |

---

## Examples

The `examples/` directory contains **31 programs**:

| Example | Description | Concepts |
|---------|-------------|----------|
| `add42.luon` | Add 42 to input | Basic arithmetic |
| `fibonacci.luon` | Iterative Fibonacci | Loops, registers |
| `bubble_sort.luon` | Sorting algorithm | Memory, arrays, loops |
| `calculator.luon` | Multi-function arithmetic | Multiple functions, calls |
| `linked_list.luon` | Singly linked list | Heap allocation, pointers |
| `matrix_multiply.luon` | 3×3 matrix multiplication | Nested loops, memory |
| `testing_demo.luon` | Testing framework demo | Assertions, test counters |
| `hello_wasi.luon` | Hello World via WASI | WASI fd_write |
| `spectral.luon` | V-Dot particle filter | Float operations |
| `crypto_v2.luon` | Sign/verify crypto | Étale cohomology notation |
| `comprehensive_v2.luon` | Full v2 syntax showcase | All operator domains |
| `codex.luon` | Complex multi-module demo | Advanced patterns |

---

## Project Structure

```
LUON/
├── bootstrap/             # Self-hosting compiler
│   ├── compiler.luon      # Compiler source (written in Luon)
│   └── compiler.wasm      # Prebuilt bootstrap binary (27 KB)
│
├── runtime/               # Native C runtime
│   ├── luon_vm.c          # WASM interpreter + CLI (single-file, ~1000 LOC)
│   └── luon_compile.h     # Minimal v2→WASM compiler helper
│
├── stdlib/                # Standard library (20 modules)
│   ├── io.luon            ├── math.luon
│   ├── memory.luon        ├── array.luon
│   ├── string.luon        ├── hashmap.luon
│   ├── wasi.luon          ├── sort.luon
│   ├── result.luon        ├── testing.luon
│   ├── random.luon        ├── crypto.luon
│   ├── fs.luon            ├── env.luon
│   ├── convert.luon       ├── fmt.luon
│   ├── buffer.luon        ├── error.luon
│   ├── struct.luon        └── enum.luon
│
├── tools/                 # Compiler toolchain (written in Luon)
│   ├── parser.luon        # Hash-based opcode dispatch parser
│   ├── emitter.luon       # WASM binary emitter
│   ├── cli.luon           # CLI foundation
│   ├── typechecker.luon   # 13-type static type system
│   └── module_resolver.luon # Import resolution + cycle detection
│
├── examples/              # 31 example programs
├── editor/luon-vscode/    # VS Code extension (v0.2.0)
├── docs/                  # Additional documentation
├── .github/workflows/     # CI/CD pipelines
│
├── README.md              # This file
├── ROADMAP.md             # Detailed development roadmap
├── LICENSE                # Apache 2.0
├── CHANGELOG.md           # Version history
├── CONTRIBUTING.md        # Contribution guidelines
├── CODE_OF_CONDUCT.md     # Community standards
├── SECURITY.md            # Security policy
├── LUON_DOCUMENTATION.md  # Complete 1,200+ line technical reference
└── LUON_REFERENCE.md      # Quick reference card
```

---

## Editor Support

### VS Code Extension (v0.2.0)

Install from `editor/luon-vscode/`:

```bash
cd editor/luon-vscode
code --install-extension .
```

Features:
- Syntax highlighting for v2 EXTREME mathematical notation
- 20 code snippets (module, func, loop, block, operators, etc.)
- Bracket matching and auto-closing
- 12 TextMate grammar scope categories

---

## Contributing

We welcome contributions from everyone. See [CONTRIBUTING.md](CONTRIBUTING.md) for full guidelines.

### Proof of Contribution Program

Vesege operates a **Proof of Contribution** incentive system. Contributors who provide value to the ecosystem — bug reports, code PRs, feature proposals, documentation — are eligible for **VVF (Vesege Coin)** distribution when Mainnet launches.

### How to Contribute

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## Community & Support

| Channel | Link |
|---|---|
| **Discord** | [discord.gg/47q8J5PgD](https://discord.gg/47q8J5PgD) |
| **X (Twitter)** | [@CagiR8649](https://x.com/CagiR8649) |
| **GitHub** | [github.com/cagioo](https://github.com/cagioo) |
| **Instagram** | [@cagi.r12](https://www.instagram.com/cagi.r12) |
| **Threads** | [@cagi.r12](https://www.threads.com/@cagi.r12) |
| **Facebook** | [Cagi R](https://www.facebook.com/share/1BMv3UaTMY/) |
| **Sponsorship** | cagi@vesege.com |

---

## License

This project is licensed under the **Apache License 2.0** — see the [LICENSE](LICENSE) file for details.

---

<p align="center">
  <strong>Vesege</strong> — Trust Through Transparency
</p>
