#!/bin/bash
# testrunner.sh — runs the k0 compiler against all test cases and reports results.
# Usage: ./tests/testrunner.sh [path/to/k0]
# The compiler binary defaults to ./k0 (run from project root).

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
COMPILER="${1:-$ROOT_DIR/k0}"

PASS=0
FAIL=0
ERRORS=()

# ----------------------------
# LOG FILE FOR COMPILER OUTPUT
# ----------------------------
LOG_FILE="$SCRIPT_DIR/test_results.txt"
: > "$LOG_FILE"

run_test() {
    local file="$1"
    local expected_exit="$2"
    local label="$3"

    # Run compiler and log ALL output (stdout + stderr)
    "$COMPILER" "$file" >> "$LOG_FILE" 2>&1
    local actual_exit=$?

    if [ "$expected_exit" -eq 0 ]; then
        # Legal programs: expect exit 0
        if [ "$actual_exit" -eq 0 ]; then
            PASS=$((PASS + 1))
        else
            FAIL=$((FAIL + 1))
            ERRORS+=("FAIL [$label] $file  (expected 0, got $actual_exit)")
        fi
    else
        # Error programs: expect non-zero
        if [ "$actual_exit" -ne 0 ]; then
            PASS=$((PASS + 1))
        else
            FAIL=$((FAIL + 1))
            ERRORS+=("FAIL [$label] $file  (expected non-zero, got 0)")
        fi
    fi
}

# Move into the directory where this script lives
cd "$SCRIPT_DIR" || exit 1

echo "=== k0 test runner ==="
echo "Compiler : $COMPILER"
echo "Tests dir: $SCRIPT_DIR"
echo ""

# ---- tests/k0 : should all pass (exit 0) ----
echo "--- Legal k0 programs (expect exit 0) ---"
for f in k0/*.kt; do
    [ -f "$f" ] || continue
    run_test "$f" 0 "k0"
done

# ---- tests/errors : should all fail (exit != 0) ----
echo "--- Error programs (expect exit != 0) ---"
for f in errors/*.kt; do
    [ -f "$f" ] || continue
    run_test "$f" 1 "error"
done

# ---- tests/kotlin : Kotlin-not-k0 features ----
echo "--- Kotlin-not-k0 programs (expect exit != 0) ---"
for f in kotlin/*.kt; do
    [ -f "$f" ] || continue
    run_test "$f" 1 "kotlin"
done

echo ""
echo "=============================="
echo " Passed : $PASS"
echo " Failed : $FAIL"
echo "=============================="

if [ ${#ERRORS[@]} -gt 0 ]; then
    echo ""
    echo "Failures:"
    for msg in "${ERRORS[@]}"; do
        echo "  $msg"
    done
fi

[ "$FAIL" -eq 0 ]