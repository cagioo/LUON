#!/usr/bin/env bash
# ═══════════════════════════════════════════════════════════════
# Vesege — Luon Compiler Installer
# Builds and installs the native C runtime
# ═══════════════════════════════════════════════════════════════

set -e

VERSION="2.2.0"

RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

echo ""
echo -e "${CYAN}${BOLD}  Luon Compiler v${VERSION}${NC}"
echo -e "${CYAN}  Vesege — Security-Oriented WASM Language${NC}"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check GCC
if ! command -v gcc &>/dev/null; then
    echo -e "${RED}  ✗ GCC not found. Install GCC first.${NC}"
    echo "    Ubuntu/Debian: sudo apt install gcc"
    echo "    macOS: xcode-select --install"
    exit 1
fi
echo -e "${GREEN}  ✓ GCC found: $(gcc --version | head -1)${NC}"

# Verify source files
if [ ! -f "$SCRIPT_DIR/runtime/luon_vm.c" ]; then
    echo -e "${RED}  ✗ Missing: runtime/luon_vm.c${NC}"
    exit 1
fi
if [ ! -f "$SCRIPT_DIR/runtime/luon_compile.h" ]; then
    echo -e "${RED}  ✗ Missing: runtime/luon_compile.h${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ Source files verified${NC}"

# Determine install location
INSTALL_DIR="${LUON_HOME:-$HOME/.luon}"
BIN_DIR="$INSTALL_DIR/bin"
LIB_DIR="$INSTALL_DIR/lib"
STDLIB_DIR="$INSTALL_DIR/stdlib"

echo ""
echo -e "  Install directory: ${BOLD}$INSTALL_DIR${NC}"

# Create directories
mkdir -p "$BIN_DIR" "$LIB_DIR" "$STDLIB_DIR"

# Build native runtime
echo ""
echo -e "  Building native runtime..."
gcc -O2 -o "$BIN_DIR/luon" "$SCRIPT_DIR/runtime/luon_vm.c" -lm
if [ $? -ne 0 ]; then
    echo -e "${RED}  ✗ Build failed${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ Built: $BIN_DIR/luon${NC}"

# Copy stdlib
if [ -d "$SCRIPT_DIR/stdlib" ]; then
    cp "$SCRIPT_DIR"/stdlib/*.luon "$STDLIB_DIR/" 2>/dev/null || true
    echo -e "${GREEN}  ✓ Standard library installed ($(ls "$STDLIB_DIR"/*.luon 2>/dev/null | wc -l) modules)${NC}"
fi

# Copy bootstrap compiler
if [ -f "$SCRIPT_DIR/bootstrap/compiler.wasm" ]; then
    mkdir -p "$INSTALL_DIR/bootstrap"
    cp "$SCRIPT_DIR/bootstrap/compiler.wasm" "$INSTALL_DIR/bootstrap/"
    cp "$SCRIPT_DIR/bootstrap/compiler.luon" "$INSTALL_DIR/bootstrap/" 2>/dev/null || true
    echo -e "${GREEN}  ✓ Bootstrap compiler installed${NC}"
fi

# Add to PATH if not already there
SHELL_RC=""
if [ -f "$HOME/.bashrc" ]; then
    SHELL_RC="$HOME/.bashrc"
elif [ -f "$HOME/.zshrc" ]; then
    SHELL_RC="$HOME/.zshrc"
fi

if [[ ":$PATH:" != *":$BIN_DIR:"* ]]; then
    if [ -n "$SHELL_RC" ]; then
        if ! grep -q "LUON_HOME" "$SHELL_RC" 2>/dev/null; then
            echo "" >> "$SHELL_RC"
            echo "# Luon Compiler" >> "$SHELL_RC"
            echo "export LUON_HOME=\"$INSTALL_DIR\"" >> "$SHELL_RC"
            echo "export PATH=\"$BIN_DIR:\$PATH\"" >> "$SHELL_RC"
            echo -e "${GREEN}  ✓ PATH updated in $SHELL_RC${NC}"
        fi
    fi
    export PATH="$BIN_DIR:$PATH"
fi

# Verify installation
echo ""
if "$BIN_DIR/luon" version &>/dev/null; then
    echo -e "${GREEN}${BOLD}  Installation complete!${NC}"
    echo ""
    "$BIN_DIR/luon" version
    echo ""
    echo "  Usage:"
    echo "    luon build <file.luon>        Compile to .wasm"
    echo "    luon run <file.luon> -a N     Compile and execute"
    echo "    luon version                  Show version"
    echo ""
    if [ -n "$SHELL_RC" ]; then
        echo -e "  Restart your shell or run: ${BOLD}source $SHELL_RC${NC}"
    fi
else
    echo -e "${RED}  ✗ Installation verification failed${NC}"
    exit 1
fi
