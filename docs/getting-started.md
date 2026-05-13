# Getting Started with Luon

This guide walks you through installing Luon and running your first program.

## Prerequisites

- **GCC** (any version) or Clang
- **Linux** or **macOS**

## Installation

```bash
git clone https://github.com/cagioo/LUON.git
cd LUON
bash install.sh
```

After installation, restart your shell or run:

```bash
source ~/.bashrc
```

Verify:

```bash
luon version
```

You should see:

```
Luon Compiler v2.2.0-dev
Vesege — Security-Oriented WASM Language
Runtime: Native C (zero dependencies)
```

## Quick Build (without install)

If you prefer not to install system-wide:

```bash
gcc -O2 -o luon runtime/luon_vm.c -lm
./luon version
```

## Your First Program

Create a file `hello.luon`:

```luon
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[hello] ⊢_Γ {

  ∃!Φ ∈ Hom(𝒞,𝒟)[main] ⊣^{op} {
    // Add 42 to the input argument
    (Ext⁰_𝔄(∂_Ω, 42))_{Spec ℤ}
    ⊥_{𝒯}→^{ex falso}⊤_{𝒯}
  }

}
```

## Compile and Run

```bash
luon build hello.luon
luon run hello.luon -a 10
```

Output:

```
52
```

What happened:
1. `∀ₛₚₑ𝒸 Ψ ∈ 𝔘[hello]` — Declares module `hello`
2. `∃!Φ ∈ Hom(𝒞,𝒟)[main]` — Declares function `main`
3. `(Ext⁰_𝔄(∂_Ω, 42))_{Spec ℤ}` — Adds 42 to the accumulator (input was 10)
4. `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` — Returns the result (52)

## Fibonacci Example

```luon
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[fib] ⊢_Γ {

  ∃!Φ ∈ Hom(𝒞,𝒟)[main] ⊣^{op} {
    // Iterative Fibonacci(N)
    // σ₂=prev, σ₃=curr, σ₄=counter
    (∂_Ω ⊢_{Γ;Δ} σ₄)^{seq}
    (0 ⊣_{Δ;Γ} ∂_Ω)^{axiom}
    (∂_Ω ⊢_{Γ;Δ} σ₂)^{seq}
    (1 ⊣_{Δ;Γ} ∂_Ω)^{axiom}
    (∂_Ω ⊢_{Γ;Δ} σ₃)^{seq}
    ⊢_{Γ}^{⊃I}⟦
    μ_{ω₁}^{CK}⟦
      (σ₄ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}
      (∂_Ω ≼_{E₂^{p,q}} 1)^{⊆filt}
      (∂_Ω ⊬_{PA}^{Gödel} σ₁)^{ω-rule}
      (σ₃ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}
      (∂_Ω ⊢_{Γ;Δ} σ₅)^{seq}
      (σ₃ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}
      (Ext⁰_𝔄(∂_Ω, σ₂))_{Spec ℤ}
      (∂_Ω ⊢_{Γ;Δ} σ₃)^{seq}
      (σ₅ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}
      (∂_Ω ⊢_{Γ;Δ} σ₂)^{seq}
      (σ₄ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}
      (Tor₀^𝔄(∂_Ω, -1))_{Spec ℤ}
      (∂_Ω ⊢_{Γ;Δ} σ₄)^{seq}
      (⊬_{PA}^{Gödel} σ₀)^{ω-rule}
    ⟧_{ω₁}^{CK}μ
    ⟧^{⊃E}_{Δ}⊣
    (σ₃ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}
    ⊥_{𝒯}→^{ex falso}⊤_{𝒯}
  }

}
```

```bash
luon run fib.luon -a 10
```

Output: `55`

## Key Concepts

| Concept | Syntax | Meaning |
|---------|--------|---------|
| Module | `∀ₛₚₑ𝒸 Ψ ∈ 𝔘[name] ⊢_Γ { }` | Declare a module |
| Function | `∃!Φ ∈ Hom(𝒞,𝒟)[name] ⊣^{op} { }` | Declare a function |
| Accumulator | `∂_Ω` | Primary working register |
| Constant | `(N ⊣_{Δ;Γ} ∂_Ω)^{axiom}` | Load constant N into acc |
| Add | `(Ext⁰_𝔄(∂_Ω, N))_{Spec ℤ}` | acc += N |
| Subtract | `(Tor₀^𝔄(∂_Ω, -N))_{Spec ℤ}` | acc -= N |
| Store register | `(∂_Ω ⊢_{Γ;Δ} σ₂)^{seq}` | σ₂ = acc |
| Load register | `(σ₂ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}` | acc = σ₂ |
| Return | `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` | Return accumulator |
| Loop | `μ_{ω₁}^{CK}⟦ ... ⟧_{ω₁}^{CK}μ` | Loop block |
| Block | `⊢_{Γ}^{⊃I}⟦ ... ⟧^{⊃E}_{Δ}⊣` | Block (for branch targets) |
| Branch if | `(∂_Ω ⊬_{PA}^{Gödel} σ_N)^{ω-rule}` | Branch to depth N if acc != 0 |
| Branch | `(⊬_{PA}^{Gödel} σ_N)^{ω-rule}` | Unconditional branch to depth N |
| Call | `η_{Kan}[func_name]` | Call function by name |

## Next Steps

- See `examples/` for 50+ example programs
- See `LUON_DOCUMENTATION.md` for the full language reference
- See `LUON_REFERENCE.md` for a quick reference card
- See `stdlib/` for available standard library modules
