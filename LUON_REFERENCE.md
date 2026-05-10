# Luon Language Reference




**Version 2.2.0-dev — Vesege**

Luon is a security-first systems programming language designed for maximum security. All programs are compiled to obfuscated WASM bytecode.

---

## Installation

```bash
bash install.sh
```

After install, restart your shell or run `source ~/.bashrc`.

Or build from source:
```bash
gcc -O2 -o luon runtime/luon_vm.c
```

---

## CLI Commands

| Command | Description | Example |
|---------|-------------|---------|
| `luon init [path]` | Initialize new project | `luon init myproject` |
| `luon build [file]` | Compile to .wasm | `luon build src/main.luon` |
| `luon run [file] [-a N]` | Compile & execute | `luon run prog.luon -a 42` |
| `luon check [file]` | Validate syntax | `luon check src/main.luon` |
| `luon test [dir]` | Run all tests | `luon test examples/` |
| `luon version` | Show version | `luon version` |

---

## Program Structure

### Program Declaration
```
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[program_name] ⊢_Γ {
  // functions go here
}
```

### Function Declaration
```
// Single return (default)
∃!Φ ∈ Hom(𝒞,𝒟)[function_name] ⊣^{op} {
  // operations go here
}

// Multiple return values (Tuple of N)
∃!Φ ∈ Hom(𝒞,𝒟)[function_name] →^{3} ⊣^{op} {
  // returns σ₁, σ₂, σ₃
}
```

---

## Operators (36 Total)

### Arithmetic (Derived Functors)

| Operation | Syntax | Description |
|-----------|--------|-------------|
| Add | `(Ext⁰_𝔄(∂_Ω, N))_{Spec ℤ}` | acc += N |
| Sub | `(Tor₀^𝔄(∂_Ω, -N))_{Spec ℤ}` | acc -= N |
| Mul | `(∂_Ω ⊗_ℤ N)^{⊗L}_{D(𝔄)}` | acc *= N |
| Div | `(RHom_ℤ(N, ∂_Ω))_{D^b(𝔄)}` | acc /= N |
| Mod | `(∂_Ω /_{𝔪} N)_{Spec(ℤ/nℤ)}` | acc %= N |

### Bitwise (Heyting Algebra)

| Operation | Syntax | Description |
|-----------|--------|-------------|
| And | `(∂_Ω ∧_{𝒽} N)^{Sub(Ω)}` | acc &= N |
| Or | `(∂_Ω ∨_{𝒽} N)^{Sub(Ω)}` | acc \|= N |
| Xor | `(∂_Ω △_{𝔹} N)^{Aut(𝔽₂)}` | acc ^= N |
| Shl | `(∂_Ω ◁_{End} 2^N)^{𝔊𝔩}` | acc <<= N |
| Shr | `(∂_Ω ▷_{End} 2^{-N})^{𝔊𝔩}` | acc >>= N |

### Comparison (Spectral Sequences)

| Operation | Syntax | Description |
|-----------|--------|-------------|
| Eqz | `(∂_Ω ≡_{E_∞} ⊥)^{acyclic}` | acc == 0 |
| Eq | `(∂_Ω ≡_{E_∞} N)^{d_r=0}` | acc == N |
| Ne | `(∂_Ω ≢_{E_∞} N)^{d_r≠0}` | acc != N |
| Lt | `(∂_Ω ≺_{E₂^{p,q}} N)^{filtration}` | acc < N |
| Gt | `(∂_Ω ≻_{E₂^{p,q}} N)^{cofiltration}` | acc > N |
| Le | `(∂_Ω ≼_{E₂^{p,q}} N)^{⊆filt}` | acc <= N |
| Ge | `(∂_Ω ≽_{E₂^{p,q}} N)^{⊇filt}` | acc >= N |

### Memory (Kripke Forcing)

| Operation | Syntax | Description |
|-----------|--------|-------------|
| Store | `(⊨_{𝕂,Γ}^{κ-forcing} ↦_𝒯 addr)` | mem[addr] = acc |
| Store8 | `(⊨_{𝕂,Γ}^{κ-forcing} ↦₈_𝒯 addr)` | mem[addr] = acc (byte) |
| Load | `(⊩_{𝕂,Σ}^{Γ-generic} ↤_𝒯 ∂_Ω)` | acc = mem[acc] |
| Load8 | `(⊩_{𝕂,Σ}^{Γ-generic} ↤₈_𝒯 ∂_Ω)` | acc = mem[acc] (byte) |
| Save_State | `(∂_Ω ⊢_{Γ;Δ} σ₄)^{seq}` | σ₄ = acc |
| Load_State | `(σ₄ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}` | acc = σ₄ |

### Control Flow (Gödel + Proof Theory)

| Operation | Syntax | Description |
|-----------|--------|-------------|
| Block | `⊢_{Γ}^{⊃I}⟦ ... ⟧^{⊃E}_{Δ}⊣` | Block scope |
| Loop | `μ_{ω₁}^{CK}⟦ ... ⟧_{ω₁}^{CK}μ` | Loop scope |
| Branch | `(⊬_{PA}^{Gödel} N)^{ω-rule}` | Jump to depth N |
| BranchIf | `(∂_Ω ⊬_{PA}^{Gödel} N)^{ω-rule}` | Jump if acc ≠ 0 |
| Return | `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` | Return acc |

### Function Call (Category Theory)

| Operation | Syntax | Description |
|-----------|--------|-------------|
| Call | `η_{Kan}N` | Call function N |
| Nop | `(id_{∂}^{nat})^{Yoneda}` | No operation |

### Lexical Scoping

| Operation | Syntax | Description |
|-----------|--------|-------------|
| Scope begin | `[σ₂, σ₃] ⊣_{scope} {` | Shadow registers |
| Scope end | `⟧_{scope}` | Restore registers |

---

## Operands

| Type | Syntax | Example |
|------|--------|---------|
| Literal integer | Plain number | `42`, `-7` |
| Register σ | `σ₄` or `σ_4` | `σ₀` = parameter |

### Special Registers

| Register | Meaning |
|----------|---------|
| `σ₀` | Function parameter (input) |
| `σ₁` | Accumulator (implicit) |
| `σ₂`–`σ₁₂₇` | General purpose |

---

## Project Structure

```
myproject/
├── luon.project.json    # Project manifest
├── src/
│   └── main.luon        # Entry point
├── build/
│   └── main.wasm        # Compiled output
└── examples/
```

---

## Error Messages

Luon provides structured error feedback:

```
  ✗ [SYNTAX] → src/main.luon:5
    No v2 syntax detected
    ↳ Functions must start with ∃!Φ ∈ Hom(𝒞,𝒟)[name]

  ✗ [BRACE] → src/main.luon
    Unbalanced braces: 3 open, 2 close
    ↳ Every { must have a matching }
```

---

*Vesege — Quantum-Computational Language Division*
