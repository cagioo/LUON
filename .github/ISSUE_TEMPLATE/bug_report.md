---
name: Bug Report
description: Report a reproducible Luon bug or regression
title: "[BUG] "
labels: ["bug", "needs-triage"]
assignees: ''
---

## Description
<!-- Clear and concise bug description. -->


## Security Impact
<!-- Does this affect determinism, sandboxing, obfuscation, memory safety, or secret handling? -->
- [ ] Determinism affected
- [ ] Sandbox boundary affected
- [ ] Memory safety affected
- [ ] Obfuscation / reverse-engineering resistance affected
- [ ] Secret / repository hygiene affected
- [ ] No known security impact

## Steps to Reproduce
1. Create or use this `.luon` file:
   ```luon
   // paste minimal reproduction here
   ```
2. Run:
   ```bash
   ./luon run file.luon -a <argument>
   ```
3. Observe the result.

## Expected Behavior


## Actual Behavior
<!-- Include full error output if safe. Do not paste secrets or tokens. -->


## Environment
- Luon version / commit:
- OS:
- C compiler:
- WASM runtime, if applicable:

## Validation Already Tried
- [ ] Rebuilt runtime
- [ ] Ran test suite
- [ ] Checked recent changes
