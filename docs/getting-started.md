# Getting Started with Luon

## Prerequisites

You need a C compiler (GCC or Clang) to build the Luon runtime.

```bash
# Check if you have GCC
gcc --version

# Or Clang
clang --version
```

## Installation

### Option 1: Build from Source

```bash
git clone https://github.com/cagioo/LUON.git
cd LUON
gcc -O2 -o luon runtime/luon_vm.c
```

### Option 2: Quick Install Script

```bash
bash install.sh
```

### Verify Installation

```bash
./luon build examples/add42.luon
./luon run examples/add42.luon -a 10
# Expected output: 52
```

## Your First Program

### Step 1: Create a file

Create `hello.luon`:

```
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[hello] ⊢_Γ {

  ∃!Φ ∈ Hom(𝒞,𝒟)[main] ⊣^{op} {
    // Load 42 into accumulator
    (42 ⊣_{Δ;Γ} ∂_Ω)^{axiom}
    // Return it
    ⊥_{𝒯}→^{ex falso}⊤_{𝒯}
  }

}
```

### Step 2: Build

```bash
./luon build hello.luon
```

This produces `hello.wasm`.

### Step 3: Run

```bash
./luon run hello.luon
# Output: 42
```

## Understanding the Syntax

### Module Declaration

Every Luon file starts with a module declaration:

```
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[module_name] ⊢_Γ {
  ...
}
```

### Function Declaration

Functions are declared with:

```
∃!Φ ∈ Hom(𝒞,𝒟)[function_name] ⊣^{op} {
  ...
  ⊥_{𝒯}→^{ex falso}⊤_{𝒯}    // return
}
```

### Core Operations

| Operation | Syntax | Meaning |
|-----------|--------|---------|
| Load constant | `(42 ⊣_{Δ;Γ} ∂_Ω)^{axiom}` | acc = 42 |
| Store to register | `(∂_Ω ⊢_{Γ;Δ} σ₂)^{seq}` | σ₂ = acc |
| Load from register | `(σ₂ ⊣_{Δ;Γ} ∂_Ω)^{co-seq}` | acc = σ₂ |
| Add | `(Ext⁰_𝔄(∂_Ω, 42))_{Spec ℤ}` | acc += 42 |
| Subtract | `(Tor₀^𝔄(∂_Ω, -42))_{Spec ℤ}` | acc -= 42 |
| Multiply | `(∂_Ω ⊗_ℤ 10)^{⊗L}_{D(𝔄)}` | acc *= 10 |
| Divide | `(RHom_ℤ(5, ∂_Ω))_{D^b(𝔄)}` | acc /= 5 |
| Return | `⊥_{𝒯}→^{ex falso}⊤_{𝒯}` | return acc |
| Call function | `(η_0 ∘_{2-Cat} ∂_Ω)^{Kan}` | call func[0] |

### Control Flow

**Block:**
```
⊢_{Γ}^{⊃I}⟦
  ...
⟧^{⊃E}_{Δ}⊣
```

**Loop:**
```
⊢_{Γ}^{⊃I}⟦
  μ_{ω₁}^{CK}⟦
    ...
    (⊬_{PA}^{Gödel} σ₀)^{ω-rule}    // continue (branch to loop start)
  ⟧_{ω₁}^{CK}μ
⟧^{⊃E}_{Δ}⊣
```

**Branch if zero:**
```
(∂_Ω ≡_{E_∞} ⊥)^{acyclic}           // test if acc == 0
(∂_Ω ⊬_{PA}^{Gödel} σ₁)^{ω-rule}    // if true, branch out
```

## VS Code Support

Install the Luon VS Code extension for syntax highlighting and snippets:

```bash
cd editor/luon-vscode
# Copy to VS Code extensions directory, or use:
code --install-extension .
```

Type `program` + Tab to scaffold a full program template.

## Next Steps

- Browse [examples/](../examples/) for more programs
- Read the [Language Reference](language-reference.md) for all operators
- See [Architecture](architecture.md) for system internals
- Check the [Standard Library modules](../stdlib/) for available functions
