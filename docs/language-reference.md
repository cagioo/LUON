# Luon Language Reference

**Version 2.0.0 — Vesege Ecosystem**

Complete reference for all operators, syntax, and conventions in the Luon programming language.

---

## Table of Contents

- [Program Structure](#program-structure)
- [Operators](#operators-34-total)
  - [Arithmetic](#arithmetic-derived-functors)
  - [Comparison](#comparison-spectral-sequences)
  - [Float](#float-non-standard-analysis)
  - [Memory](#memory-kripke-forcing)
  - [Bitwise](#bitwise-boolean-algebra)
  - [Control Flow](#control-flow-gödel--proof-theory)
  - [Cryptographic](#cryptographic-étale-cohomology)
  - [Category Theory](#category-theory)
  - [WASI System Calls](#wasi-system-calls)
- [Operands](#operands)
- [Data Directives](#data-directives)
- [Comments](#comments)

---

## Program Structure

Every Luon program follows a consistent structure:

### Program Declaration

```
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[program_name] ⊢_Γ {
  // functions go here
}
```

The program declaration wraps all functions in the compilation unit. `program_name` is a label for the module.

### Function Declaration

```
∃!Φ ∈ Hom(𝒞,𝒟)[function_name] ⊣^{op} {
  // operations go here
}
```

Each function:
- Takes one `i64` parameter (accessible as `σ₀`)
- Returns one `i64` value (the accumulator `σ₁`)
- Has 127 general-purpose registers (`σ₂`–`σ₁₂₇`)

The **accumulator** (`σ₁`, also written `∂_Ω`) is the primary working register. Most operators read from and write to it implicitly.

### Minimal Example

```
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[example] ⊢_Γ {

  ∃!Φ ∈ Hom(𝒞,𝒟)[main] ⊣^{op} {
    (Ext⁰_𝔄(∂_Ω, σ₀))_{Spec ℤ}       // acc += input
    (Ext⁰_𝔄(∂_Ω, 42))_{Spec ℤ}        // acc += 42
    ⊥_{𝒯}→^{ex falso}⊤_{𝒯}            // return
  }

}
```

---

## Operators (34 Total)

### Arithmetic (Derived Functors)

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **Add** | `(Ext⁰_𝔄(∂_Ω, N))_{Spec ℤ}` | `acc += N` |
| **Sub** | `(Tor₀^𝔄(∂_Ω, N))_{Spec ℤ}` | `acc -= N` |
| **Mul** | `(∂_Ω ⊗_ℤ N)^{⊗L}_{D(𝔄)}` | `acc *= N` |
| **Div** | `(RHom_ℤ(N, ∂_Ω))_{D^b(𝔄)}` | `acc /= N` |
| **Mod** | `(∂_Ω /_{𝔪} N)_{Spec(ℤ/nℤ)}` | `acc %= N` |

Where `N` is either a literal integer or a register reference (`σ₄`).

### Comparison (Spectral Sequences)

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **Eqz** | `(∂_Ω ≡_{E_∞} ⊥)^{acyclic}` | `acc = (acc == 0) ? 1 : 0` |
| **Eq** | `(∂_Ω ≡_{E_∞} N)^{d_r=0}` | `acc = (acc == N) ? 1 : 0` |
| **Ne** | `(∂_Ω ≢_{E_∞} N)^{d_r≠0}` | `acc = (acc != N) ? 1 : 0` |
| **Lt** | `(∂_Ω ≺_{E₂^{p,q}} N)^{filtration}` | `acc = (acc < N) ? 1 : 0` |
| **Gt** | `(∂_Ω ≻_{E₂^{p,q}} N)^{cofiltration}` | `acc = (acc > N) ? 1 : 0` |
| **Le** | `(∂_Ω ≼_{E₂^{p,q}} N)^{⊆filt}` | `acc = (acc <= N) ? 1 : 0` |
| **Ge** | `(∂_Ω ≽_{E₂^{p,q}} N)^{⊇filt}` | `acc = (acc >= N) ? 1 : 0` |

Comparison results are always `1` (true) or `0` (false), extended to `i64`.

### Float (Non-Standard Analysis)

Luon uses a **bit-reinterpret** architecture for floats: `i64` values are reinterpreted as `f64` for the operation, then the result is reinterpreted back to `i64`.

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **FAdd** | `(st(∂_{*ℝ}) ⊕_{*ℝ} st(N))^{transfer}` | `f64(acc) + f64(N)` |
| **FSub** | `(st(∂_{*ℝ}) ⊖_{*ℝ} st(N))^{transfer}` | `f64(acc) - f64(N)` |
| **FMul** | `(st(∂_{*ℝ}) ⊗_{*ℝ} st(N))^{transfer}` | `f64(acc) * f64(N)` |
| **FDiv** | `(st(∂_{*ℝ}) ⊘_{*ℝ} st(N))^{transfer}` | `f64(acc) / f64(N)` |
| **FGt** | `(st(∂_{*ℝ}) ≻_{*ℝ} st(N))^{Loeb}` | `f64(acc) > f64(N) → 1 : 0` |
| **FLt** | `(st(∂_{*ℝ}) ≺_{*ℝ} st(N))^{Loeb}` | `f64(acc) < f64(N) → 1 : 0` |
| **FSqrt** | `(√_{*ℝ} st(∂_{*ℝ}))^{shadow}` | `sqrt(f64(acc))` |
| **ToFloat** | `(∂_Ω ↪_{ℤ→*ℝ} ∂_{*ℝ})^{embed}` | `i64 → f64 (embed)` |
| **ToInt** | `(∂_{*ℝ} ↠_{*ℝ→ℤ} ∂_Ω)^{floor}` | `f64 → i64 (floor)` |

### Memory (Kripke Forcing)

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **Store** | `(⊨_{𝕂,Γ}^{κ-forcing} ↦_𝒯 addr)` | `mem[addr] = acc` (i64) |
| **Store8** | `(⊨_{𝕂,Γ}^{κ-forcing} ↦₈_𝒯 addr)` | `mem[addr] = acc` (byte) |
| **Load** | `(⊩_{𝕂,Σ}^{Γ-generic} ↤_𝒯 ∂_Ω)` | `acc = mem[acc]` (i64) |
| **Load8** | `(⊩_{𝕂,Σ}^{Γ-generic} ↤₈_𝒯 ∂_Ω)` | `acc = mem[acc]` (byte) |
| **Save_State** | `(∂_Ω ⊢_{Γ;Δ} σ_N)^{seq}` | `σ_N = acc` |
| **Load_State** | `(σ_N ⊣_{Δ;Γ} ∂_Ω)^{co-seq}` | `acc = σ_N` |

### Bitwise (Boolean Algebra / Galois Field)

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **And** | `(∂_Ω ∧_{𝔹} N)^{∧}` | `acc &= N` |
| **Or** | `(∂_Ω ∨_{𝔹} N)^{∨}` | `acc \|= N` |
| **Xor** | `(∂_Ω ⊕_{GF} N)^{⊕}` | `acc ^= N` |
| **Shl** | `(∂_Ω ≪_{shift} N)^{<<}` | `acc <<= N` |
| **Shr** | `(∂_Ω ≫_{shift} N)^{>>}` | `acc >>= N` (unsigned) |
| **Shr_S** | `(∂_Ω ≫ₛ_{shift} N)^{s>>}` | `acc >>= N` (signed) |

### Control Flow (Gödel + Proof Theory)

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **Block** | `⊢_{Γ}^{⊃I}⟦ ... ⟧^{⊃E}_{Δ}⊣` | Begin/end block scope |
| **Loop** | `μ_{ω₁}^{CK}⟦ ... ⟧_{ω₁}^{CK}μ` | Begin/end loop scope |
| **Branch** | `(⊬_{PA}^{Gödel} N)^{ω-rule}` | Unconditional branch (depth N) |
| **BranchIf** | `(∂_Ω ⊬_{PA}^{Gödel} N)^{ω-rule}` | Branch if acc ≠ 0 (depth N) |
| **Return** | `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` | Return accumulator value |

### Cryptographic (Étale Cohomology)

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **Hash** | `(H^n_{ét}(Spec(∂_Ω), 𝔾_m))^{Galois}` | FNV-1a hash of accumulator |
| **Sign** | `(π₁^{ét}(Spec(∂_Ω)) ⋉_{Gal} N)^{Frob}` | Hash-sign acc with key N |
| **Verify** | `(π₁^{ét}(Spec(∂_Ω)) ⋊_{Gal} N)^{Frob⁻¹}` | Verify acc equals N |

### Category Theory

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **Call** | `(η_target ∘_{2-Cat} ∂_Ω)^{Kan}` | Call function by index |
| **Nop** | `(id_{∂}^{nat})^{Yoneda}` | No operation |

### WASI System Calls

| Operation | Syntax | Semantics |
|-----------|--------|-----------|
| **WasiCall** | `(ε_N^{WASI} ∂_Ω)^{syscall(K)}` | Call WASI import N with K args |

WASI enables standalone execution via `wasmtime` without a custom runtime. Arguments are loaded from registers `σ₂`–`σ₂₊ₖ`.

---

## Operands

| Type | Syntax | Example |
|------|--------|---------|
| Literal integer | Plain number | `42`, `-7`, `0` |
| Register | `σ_N` or `σ₄` | `σ₀` = parameter |

### Special Registers

| Register | Role |
|----------|------|
| `σ₀` | Function parameter (input argument) |
| `σ₁` | Accumulator (`∂_Ω`) — implicit |
| `σ₂`–`σ₁₂₇` | General-purpose registers |

---

## Data Directives

Embed constant data (strings) directly into WASM memory:

```
// @data <offset> "<string>"
// @data 65536 "Hello, World!\n"
```

Data is placed in the `input` memory segment at the specified byte offset.

---

## Comments

```
// This is a single-line comment
// Comments start with double forward slash
```

Luon only supports single-line comments with `//`.

---

## Memory Layout

The WASM linear memory is divided into 5 segments (30 pages = 1,966,080 bytes):

| Segment | Base Address | Size | Purpose |
|---------|-------------|------|---------|
| `code` | `0x00000` | 64KB | Compiler state / scratch |
| `input` | `0x10000` | 128KB | Input buffer |
| `output` | `0x30000` | 128KB | Output buffer |
| `state` | `0x50000` | 64KB | Persistent state |
| `heap` | `0x60000` | ~888KB | General-purpose heap |

---

## Compound Expressions

Multiple operations can appear on a single line separated by semicolons:

```
(Ext⁰_𝔄(∂_Ω, 10))_{Spec ℤ}; (∂_Ω ⊗_ℤ 2)^{⊗L}_{D(𝔄)}
```

This is equivalent to:
```
(Ext⁰_𝔄(∂_Ω, 10))_{Spec ℤ}
(∂_Ω ⊗_ℤ 2)^{⊗L}_{D(𝔄)}
```

---

*Vesege — Trust Through Transparency*
