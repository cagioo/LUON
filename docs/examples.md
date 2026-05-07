# Examples

Annotated walkthrough of example programs included in the `examples/` directory.

---

## add42.luon — The Simplest Program

Adds 42 to the input argument.

```
∀ₛₚₑ𝒸 Ψ ∈ 𝔘[add42] ⊢_Γ {
  ∃!Φ ∈ Hom(𝒞,𝒟)[main] ⊣^{op} {
    (Ext⁰_𝔄(∂_Ω, σ₀))_{Spec ℤ}       // acc += input
    (Ext⁰_𝔄(∂_Ω, 42))_{Spec ℤ}        // acc += 42
    ⊥_{𝒯}→^{ex falso}⊤_{𝒯}            // return
  }
}
```

```bash
luon run examples/add42.luon -a 10    # → 52
luon run examples/add42.luon -a 0     # → 42
luon run examples/add42.luon -a -42   # → 0
```

---

## fibonacci.luon — Iterative Fibonacci

Computes the Nth Fibonacci number using a loop.

**Key patterns demonstrated:**
- Register save/load (`σ₂`, `σ₃`, `σ₄`)
- Loop construct (`μ_{ω₁}^{CK}⟦ ... ⟧`)
- Block + conditional branch for loop exit
- Comparison operator (`Le`)

```bash
luon run examples/fibonacci.luon -a 10   # → 55
luon run examples/fibonacci.luon -a 20   # → 6765
luon run examples/fibonacci.luon -a 1    # → 1
```

---

## arithmetic.luon — Basic Arithmetic

Demonstrates all 5 arithmetic operators: Add, Sub, Mul, Div, Mod.

```bash
luon run examples/arithmetic.luon -a 5
```

---

## bubble_sort.luon — Sorting Algorithm

Implements bubble sort using memory operations:
- `Store` to write values to linear memory
- `Load` to read values from memory
- Nested loops for comparison and swapping

```bash
luon run examples/bubble_sort.luon -a 10
```

---

## hello_wasi.luon — Hello World via WASI

Prints "Hello, World!" to stdout using WASI system calls.

**Key patterns:**
- `// @data` directive to embed string constants
- `WasiCall` operator to invoke `fd_write`
- Memory layout for iovec struct

```bash
luon build-wasi examples/hello_wasi.luon
wasmtime hello_wasi.wasm
# Output: Hello, World!
```

---

## spectral.luon — V-Dot Particle Operations

Demonstrates advanced features including float operations and the spectral computation model.

```bash
luon run examples/spectral.luon -a 1
```

---

## Running All Examples

```bash
luon test examples/
```

This runs all `.luon` files in the examples directory and reports pass/fail results.

---

*Vesege — Trust Through Transparency*
