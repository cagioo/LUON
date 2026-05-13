#!/usr/bin/env bash
# ═══════════════════════════════════════════════════════════════
# Luon Runtime Correctness Tests
# Verifies that compiled programs produce CORRECT output,
# not just that they compile successfully.
# ═══════════════════════════════════════════════════════════════

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
LUON="${ROOT_DIR}/runtime/luon"

# Build runtime if not present
if [ ! -f "$LUON" ]; then
    echo "Building runtime..."
    gcc -O2 -o "$LUON" "$ROOT_DIR/runtime/luon_vm.c"
fi

PASS=0
FAIL=0
TOTAL=0

# Test helper: run_test <description> <file> <arg> <expected_output>
run_test() {
    local desc="$1"
    local file="$2"
    local arg="$3"
    local expected="$4"
    TOTAL=$((TOTAL+1))

    local output
    output=$("$LUON" run "$ROOT_DIR/$file" -a "$arg" 2>/dev/null | tail -1)

    if [ "$output" = "$expected" ]; then
        echo "  ✅ PASS: $desc (got $output)"
        PASS=$((PASS+1))
    else
        echo "  ❌ FAIL: $desc — expected '$expected', got '$output'"
        FAIL=$((FAIL+1))
    fi
}

echo "Luon Runtime Correctness Tests"
echo "==============================="
echo ""

# ─── Basic Arithmetic ───
echo "── Arithmetic ──"
run_test "add42(10) = 52" "examples/add42.luon" 10 "52"
run_test "add42(0) = 42" "examples/add42.luon" 0 "42"
run_test "add42(100) = 142" "examples/add42.luon" 100 "142"
run_test "arithmetic(10) = 190" "examples/arithmetic.luon" 10 "190"

# ─── Fibonacci ───
echo ""
echo "── Fibonacci ──"
run_test "fibonacci(1) = 1" "examples/fibonacci.luon" 1 "1"
run_test "fibonacci(2) = 1" "examples/fibonacci.luon" 2 "1"
run_test "fibonacci(5) = 5" "examples/fibonacci.luon" 5 "5"
run_test "fibonacci(10) = 55" "examples/fibonacci.luon" 10 "55"
run_test "fibonacci(20) = 6765" "examples/fibonacci.luon" 20 "6765"

# ─── Power ───
echo ""
echo "── Power (4^3=64) ──"
run_test "test_pow = 64" "examples/test_pow.luon" 0 "64"

# ─── Multi-function ───
echo ""
echo "── Multi-function ──"
run_test "calculator(5) = 70" "examples/calculator.luon" 5 "70"
run_test "named_call(3) = 20" "examples/named_call_test.luon" 3 "20"

# ─── Scoping ───
echo ""
echo "── Scoping & Tuples ──"
run_test "scope_test(5) = 200 10 20" "examples/scope_test.luon" 5 "200 10 20"
run_test "tuple_test(7) = 11 12 13" "examples/tuple_test.luon" 7 "11 12 13"

# ─── Comparison ───
echo ""
echo "── Comparison ──"
run_test "comparison(5) = 0" "examples/comparison.luon" 5 "0"
run_test "comparison(0) = 0" "examples/comparison.luon" 0 "0"

# ─── Loop ───
echo ""
echo "── Loop ──"
run_test "loop_test(10) = 0" "examples/loop_test.luon" 10 "0"

# ─── Global Variables ───
echo ""
echo "── Globals ──"
run_test "global_test = 42" "examples/global_test.luon" 5 "42"

# ─── Summary ───
echo ""
echo "═══════════════════════════════"
echo "  Total:  $TOTAL"
echo "  Passed: $PASS"
echo "  Failed: $FAIL"
echo "═══════════════════════════════"

if [ $FAIL -gt 0 ]; then
    exit 1
fi
exit 0
