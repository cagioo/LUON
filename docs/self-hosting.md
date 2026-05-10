# Self-Hosting Guide

How the Luon compiler compiles itself — and why it matters.

---

## What is Self-Hosting?

A **self-hosting compiler** is a compiler written in the language it compiles. The Luon compiler (`compiler.luon`) is written in Luon and can compile itself to produce a working WASM binary.

This is significant because it proves:
1. **Completeness** — The language can express its own compiler
2. **Correctness** — The compiler is self-consistent
3. **Independence** — No external toolchain is needed for future development

---

## The Bootstrap Chain

```
Step 1 (Bootstrap):
  compiler.luon ──▶ [Python Assembler] ──▶ compiler.wasm (gen1)

Step 2 (Self-compile):
  compiler.luon ──▶ [compiler.wasm gen1] ──▶ compiler.wasm (gen2)

Step 3 (Verify):
  sha256(gen1) == sha256(gen2) ✓  → Fixed Point Achieved
```

### Step 1: Bootstrap

The Python assembler (`luon_assembler.py`) compiles `compiler.luon` for the first time:

```bash
luon build compiler.luon
# Produces: compiler.wasm (gen1)
```

### Step 2: Self-Compile

The gen1 compiler compiles its own source code:

```bash
wasmtime compiler.wasm --mem-in compiler.luon --mem-out compiler_gen2.wasm
```

### Step 3: Verify Fixed Point

```bash
sha256sum compiler.wasm compiler_gen2.wasm
# Both hashes must match
```

If `gen1 == gen2`, the compiler has reached a **fixed point** — it produces itself identically.

---

## How the Self-Hosting Compiler Works

The `compiler.luon` file implements:

1. **Tokenizer** — Reads source bytes from memory at `0x10000`
2. **Parser** — Recognizes v2 syntax via rolling hash dispatch
3. **Emitter** — Generates WASM binary sections byte-by-byte

### Rolling Hash Dispatch

Instead of string comparison (which would require complex string handling), the compiler uses a **rolling hash** to identify operator patterns:

```
hash = 0
for each byte in pattern:
    hash = (hash * 31 + byte) & 0xFFFFFFFF
```

Each known operator has a precomputed hash. The compiler matches input against these hashes using conditional branches.

### Memory-Mapped I/O

The compiler reads source from the `input` segment (`0x10000`) and writes WASM output to the `output` segment (`0x30000`). The host loads/extracts data via `--mem-in` and `--mem-out`.

---

## Self-Hosting Toolchain

Beyond the compiler, Luon has native implementations of:

| Tool | File | Purpose |
|------|------|---------|
| Compiler | `compiler.luon` | Full compilation pipeline |
| Parser | `tools/parser.luon` | Source → IR |
| Emitter | `tools/emitter.luon` | IR → WASM binary |
| CLI | `tools/cli.luon` | Command-line interface |

These form a complete toolchain written entirely in Luon.

---

*Vesege — Trust Through Transparency*
