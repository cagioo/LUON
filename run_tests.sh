#!/bin/bash
# Luon Test Runner — runs all examples and validates outputs

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
LUON="$SCRIPT_DIR/runtime/luon"
PASS=0
FAIL=0

run_test() {
  local file="$1"
  local expected="$2"
  local args="$3"
  local name
  name=$(basename "$file")
  local result
  result=$("$LUON" run "$file" $args 2>/dev/null | tr -d '\r' | grep -E '^-?[0-9]+$' | tail -1) || true
  if [ "$result" = "$expected" ]; then
    echo "  ✅ $name → $result"
    PASS=$((PASS + 1))
  else
    echo "  ❌ $name → '$result' (expected $expected)"
    FAIL=$((FAIL + 1))
  fi
}

echo "╔══════════════════════════════════════╗"
echo "║     Luon Test Suite v2.2.0-dev       ║"
echo "╚══════════════════════════════════════╝"
echo ""

DIR="$SCRIPT_DIR/examples"

echo "── Core Tests ──"
run_test "$DIR/global_test.luon" "42" ""
run_test "$DIR/match_test.luon" "200" "-a 1"
run_test "$DIR/type_test.luon" "42" ""
run_test "$DIR/mem_ops_test.luon" "32" ""
run_test "$DIR/test_math.luon" "97" ""

echo ""
echo "── Feature Tests ──"
[ -f "$DIR/multi_param_test.luon" ] && run_test "$DIR/multi_param_test.luon" "10" ""
[ -f "$DIR/null_safety_test.luon" ] && run_test "$DIR/null_safety_test.luon" "42" ""
[ -f "$DIR/asm_test.luon" ] && run_test "$DIR/asm_test.luon" "42" ""

echo ""
echo "══════════════════════════════════════"
echo "  Results: $PASS passed, $FAIL failed"
echo "══════════════════════════════════════"

if [ "$FAIL" -eq 0 ]; then
  echo "  🎯 All tests passed!"
  exit 0
else
  exit 1
fi
