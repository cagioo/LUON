# Security Policy


## Reporting a Vulnerability

If you discover a security vulnerability in Luon, please report it responsibly.

**Do NOT open a public issue for security vulnerabilities.**

Instead, please email the maintainers directly or use GitHub's private vulnerability reporting feature.

### What to Include

- Description of the vulnerability
- Steps to reproduce
- Potential impact
- Suggested fix (if any)

### Response Timeline

- **Acknowledgment**: Within 48 hours
- **Assessment**: Within 7 days
- **Fix**: Depending on severity, typically within 30 days

## Security Architecture

Luon includes several security layers:

### WASM Obfuscation Engine
The compiler includes a multi-pass obfuscation pipeline that produces metamorphic binaries — each compilation generates structurally different but functionally identical output.

### Luon Sealed Executable (.lse)
A proprietary container format with:
- BLAKE2b-CTR stream cipher encryption
- Per-compilation opcode permutation tables
- Anti-tamper integrity verification

### Sandboxed VM
The LSE runtime includes:
- Memory isolation with configurable bounds
- Instruction count limits (DoS prevention)
- Call depth limits (stack overflow prevention)
- Wall-clock execution timeout

## Supported Versions

| Version | Supported |
|---------|-----------|
| 2.0.x   | ✅ Current |
| < 2.0   | ❌ End of life |

## Scope

This security policy applies to:
- The Luon compiler (`luon_assembler.py`, `luon_cli.py`)
- The Rust runtime (`luon-horizon`)
- The LSE format and VM (`lse_format.py`)
- All standard library modules (`stdlib/`)
