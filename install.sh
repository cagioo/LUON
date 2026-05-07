#!/usr/bin/env bash
# ═══════════════════════════════════════════════════════════════
# Vesege — Luon Compiler Installer
# Installs the `luon` command system-wide or per-user
# ═══════════════════════════════════════════════════════════════

set -e

VERSION="2.0.0"
CODENAME="Event Horizon"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

echo ""
echo -e "${CYAN}${BOLD}  ╔══════════════════════════════════════════╗${NC}"
echo -e "${CYAN}${BOLD}  ║  Luon Compiler v${VERSION} — ${CODENAME}  ║${NC}"
echo -e "${CYAN}${BOLD}  ║  Vesege                       ║${NC}"
echo -e "${CYAN}${BOLD}  ╚══════════════════════════════════════════╝${NC}"
echo ""

# Detect script directory (where luon source lives)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check Python 3.9+
if ! command -v python3 &>/dev/null; then
    echo -e "${RED}  ✗ Python 3 not found. Install Python 3.9+ first.${NC}"
    exit 1
fi

PYTHON_VER=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
PYTHON_MAJOR=$(echo "$PYTHON_VER" | cut -d. -f1)
PYTHON_MINOR=$(echo "$PYTHON_VER" | cut -d. -f2)

if [ "$PYTHON_MAJOR" -lt 3 ] || ([ "$PYTHON_MAJOR" -eq 3 ] && [ "$PYTHON_MINOR" -lt 9 ]); then
    echo -e "${RED}  ✗ Python $PYTHON_VER found, but 3.9+ required.${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ Python $PYTHON_VER${NC}"

# Verify core files
REQUIRED_FILES=("compiler/luon_cli.py" "compiler/luon_assembler.py" "compiler/luon_runtime.py")
for f in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$SCRIPT_DIR/$f" ]; then
        echo -e "${RED}  ✗ Missing: $f${NC}"
        exit 1
    fi
done
echo -e "${GREEN}  ✓ Core files verified${NC}"

# Determine install location
INSTALL_DIR="${LUON_HOME:-$HOME/.luon}"
BIN_DIR="$INSTALL_DIR/bin"
LIB_DIR="$INSTALL_DIR/lib"

echo ""
echo -e "  Install directory: ${BOLD}$INSTALL_DIR${NC}"

# Create directories
mkdir -p "$BIN_DIR" "$LIB_DIR"

# Copy core library files
CORE_FILES=(
    "compiler/luon_core.py"
    "compiler/luon_cli.py"
    "compiler/luon_assembler.py"
    "compiler/luon_runtime.py"
    "compiler/luon_stdlib.py"
    "compiler/luon_optimizer.py"
    "compiler/luon_extensions.py"
    "compiler/luon_advanced.py"
    "compiler/luon_advanced2.py"
    "compiler/luon_mitigations.py"
    "compiler/luon_final.py"
    "compiler/lse_format.py"
    "compiler/__main__.py"
    "compiler/build_standalone.py"
    "compiler/test_standalone.py"
)

for f in "${CORE_FILES[@]}"; do
    if [ -f "$SCRIPT_DIR/$f" ]; then
        cp "$SCRIPT_DIR/$f" "$LIB_DIR/"
    fi
done
echo -e "${GREEN}  ✓ Library files installed${NC}"

# Copy docs
if [ -f "$SCRIPT_DIR/LUON_REFERENCE.md" ]; then
    cp "$SCRIPT_DIR/LUON_REFERENCE.md" "$INSTALL_DIR/"
fi

# Create launcher script
cat > "$BIN_DIR/luon" << LAUNCHER
#!/usr/bin/env bash
# Luon Compiler v${VERSION} — Vesege
export PYTHONPATH="${LIB_DIR}:\$PYTHONPATH"
exec python3 "${LIB_DIR}/luon_cli.py" "\$@"
LAUNCHER

chmod +x "$BIN_DIR/luon"
echo -e "${GREEN}  ✓ Launcher created: $BIN_DIR/luon${NC}"

# Add to PATH if not already there
SHELL_RC=""
if [ -f "$HOME/.bashrc" ]; then
    SHELL_RC="$HOME/.bashrc"
elif [ -f "$HOME/.zshrc" ]; then
    SHELL_RC="$HOME/.zshrc"
fi

if [[ ":$PATH:" != *":$BIN_DIR:"* ]]; then
    if [ -n "$SHELL_RC" ]; then
        # Check if already added
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
    echo -e "${GREEN}${BOLD}  ══════════════════════════════════════${NC}"
    echo -e "${GREEN}${BOLD}  Installation complete!${NC}"
    echo -e "${GREEN}${BOLD}  ══════════════════════════════════════${NC}"
    echo ""
    "$BIN_DIR/luon" version
    echo ""
    echo "  Usage:"
    echo "    luon init myproject"
    echo "    luon build src/main.luon"
    echo "    luon run src/main.luon -a 42"
    echo ""
    echo -e "  Restart your shell or run: ${BOLD}source $SHELL_RC${NC}"
else
    echo -e "${RED}  ✗ Installation verification failed${NC}"
    exit 1
fi
