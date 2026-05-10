# Contributing to Luon

Thank you for your interest in contributing to Luon! Every contribution — whether it's fixing a bug, improving documentation, or proposing a new feature — helps strengthen the Vesege ecosystem.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Development Setup](#development-setup)
- [Code Style](#code-style)
- [Pull Request Process](#pull-request-process)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)
- [Proof of Contribution](#proof-of-contribution)

---

## Code of Conduct

This project follows the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you agree to uphold a respectful, inclusive, and harassment-free environment.

---

## How Can I Contribute?

### 🐛 Bug Fixes
Found a bug? Check [existing issues](../../issues) first. If it hasn't been reported, [open a new issue](../../issues/new?template=bug_report.md).

### 📝 Documentation
Improvements to documentation are always welcome — from fixing typos to adding guides.

### 💡 New Features
Have an idea? [Open a feature request](../../issues/new?template=feature_request.md) to discuss before implementing.

### 🧪 Tests
Adding test cases for existing features helps improve reliability.

### 🌐 Examples
New example programs that showcase Luon's capabilities are highly valued.

---

## Development Setup

### Prerequisites

- **GCC** or any C compiler
- **Git**

### Setup

```bash
# Clone the repository
git clone https://github.com/cagioo/LUON.git
cd LUON

# Build the native runtime
gcc -O2 -o luon runtime/luon_vm.c

# Try compiling and running an example
./luon build examples/add42.luon
./luon run examples/add42.luon -a 10
# Expected output: 52

# Run more examples
./luon run examples/fibonacci.luon -a 10
./luon run examples/calculator.luon -a 0
```

### Project Structure

```
LUON/
├── bootstrap/          # Self-hosting compiler (compiler.luon + compiler.wasm)
├── runtime/            # Native C runtime (luon_vm.c)
├── stdlib/             # 20 standard library modules
├── tools/              # Compiler toolchain (parser, emitter, typechecker, etc.)
├── examples/           # 31 example programs
├── editor/luon-vscode/ # VS Code extension
├── docs/               # Additional documentation
└── .github/            # CI/CD workflows
```

---

## Code Style

### Luon (.luon files)
- Use **v2 syntax exclusively** — conventional keywords are not permitted
- Add comments explaining the purpose of each function
- Use meaningful register names in comments (e.g., `// σ₂ = counter`, `// σ₃ = loop limit`)
- One module declaration per file
- Functions should be focused and concise

### C (runtime)
- Follow K&R style
- Keep functions small and auditable
- Document any WASM spec compliance notes
- No external dependencies

### General
- All code, comments, and documentation must be in **English**
- Keep commit messages clear and descriptive
- One logical change per commit

---

## Pull Request Process

1. **Fork** the repository and create your branch from `main`
2. **Make** your changes with clear, focused commits
3. **Test** your changes — ensure examples compile and run correctly
4. **Update** documentation if your changes affect the public API
5. **Submit** a Pull Request with a clear description of what and why

### PR Guidelines

- Title should clearly describe the change
- Include context for _why_ the change is needed
- Reference related issues (e.g., "Fixes #42")
- Keep PRs focused — one feature or fix per PR
- Be open to feedback and discussion

---

## Reporting Bugs

When reporting a bug, please include:

1. **Luon version** (`./luon version` or check `CHANGELOG.md`)
2. **Operating system** and C compiler version
3. **Steps to reproduce** — minimal .luon program that triggers the bug
4. **Expected behavior** vs. **actual behavior**
5. **Error output** — full error message

---

## Suggesting Features

Feature requests should include:

1. **Problem description** — What problem does this solve?
2. **Proposed solution** — How should it work?
3. **Alternatives considered** — What other approaches were evaluated?
4. **Use cases** — Who benefits and how?

> **Important:** Luon intentionally does not implement conventional syntax keywords (`if`, `else`, `for`, `while`, `let`, `print`, etc.). Feature proposals that add conventional syntax will be declined. See [ROADMAP.md](ROADMAP.md) for the list of permanently rejected features and the reasoning behind these decisions.

---

## Proof of Contribution

Vesege operates a **Proof of Contribution** incentive system. Contributors who provide value to the ecosystem are eligible for **VVF (Vesege Coin)** distribution when Mainnet launches.

Eligible contributions include:

| Contribution Type | Description |
|---|---|
| 🐛 **Bug Bounty** | Report verified security vulnerabilities or bugs |
| 💻 **Code PRs** | Merged pull requests that improve the codebase |
| 💡 **Feature Proposals** | Feature ideas that get implemented |
| 📝 **Documentation** | Documentation improvements |
| 🧪 **Testing** | New test cases and test coverage improvements |
| 🌐 **Community** | Active participation in Discord community |

---

## Community

| Channel | Link |
|---|---|
| **Discord** | [discord.gg/47q8J5PgD](https://discord.gg/47q8J5PgD) |
| **X (Twitter)** | [@CagiR8649](https://x.com/CagiR8649) |
| **GitHub** | [github.com/cagioo](https://github.com/cagioo) |
| **Sponsorship** | cagi@vesege.com |

---

Thank you for helping build the future of Vesege! 🚀
