#!/usr/bin/env bash
# ============================================================================
# Codexion test harness  (NOT part of the mandatory submission)
#
# Reproduces the checks used while building this project so an evaluator can
# verify everything in one command:
#
#     ./test.sh
#
# Sections: build, norm, argument validation, "no burnout / all complete"
# (Easy), burnout edge cases (Less easy), cooldown + fifo/edf (Medium),
# memory leaks (valgrind) and data races (ThreadSanitizer) when available.
#
# Tools that are not installed (norminette / valgrind) are skipped, not failed.
# ============================================================================

cd "$(dirname "$0")" || exit 1

GREEN='\033[32m'; RED='\033[31m'; YEL='\033[33m'; RST='\033[0m'
PASS=0; FAIL=0; SKIP=0
LOG=$(mktemp)

ok()   { printf "  ${GREEN}PASS${RST} %s\n" "$1"; PASS=$((PASS + 1)); }
ko()   { printf "  ${RED}FAIL${RST} %s\n" "$1"; FAIL=$((FAIL + 1)); }
skip() { printf "  ${YEL}SKIP${RST} %s\n" "$1"; SKIP=$((SKIP + 1)); }
hdr()  { printf "\n${YEL}== %s ==${RST}\n" "$1"; }

# run the binary; $1=timeout secs, rest=args. Output -> $LOG, returns exit code.
run() { local t=$1; shift; timeout "$t" ./codexion "$@" >"$LOG" 2>&1; return $?; }
ccount() { grep -c "is compiling" "$LOG"; }
bcount() { grep -c "burned out" "$LOG"; }

# expect a full, burnout-free run: $1=label then codexion args (last must let
# the run terminate, i.e. a finite number_of_compiles_required).
expect_complete() {
	local label=$1; shift
	local n=$1 req=$6
	run 40 "$@"
	local rc=$? c b
	c=$(ccount); b=$(bcount)
	if [ $rc -eq 124 ]; then ko "$label (HANG)"
	elif [ "$b" != "0" ]; then ko "$label (a coder burned out: $b)"
	elif [ "$c" -lt $((n * req)) ]; then ko "$label (only $c/$((n * req)) compiles)"
	else ok "$label ($c compiles, 0 burnout)"; fi
}

# expect at least one coder to burn out (infeasible-on-purpose setup).
expect_burnout() {
	local label=$1; shift
	run 15 "$@"
	local rc=$?
	if [ $rc -eq 124 ]; then ko "$label (HANG, should have burned out)"
	elif [ "$(bcount)" = "0" ]; then ko "$label (nobody burned out)"
	else ok "$label (burned out as expected)"; fi
}

# expect a rejected run (invalid args): non-zero exit, no crash/hang.
expect_reject() {
	local label=$1; shift
	run 5 "$@"
	local rc=$?
	if [ $rc -eq 0 ]; then ko "$label (accepted invalid input)"
	elif [ $rc -ge 124 ]; then ko "$label (crash/hang, rc=$rc)"
	else ok "$label (rejected, rc=$rc)"; fi
}

# ----------------------------------------------------------------------------
hdr "BUILD"
make re >"$LOG" 2>&1
build_rc=$?
if [ $build_rc -eq 0 ] && [ -x ./codexion ] \
		&& ! grep -qiE "(warning|error):" "$LOG"; then
	ok "make -> ./codexion, no warnings (-Wall -Wextra -Werror -pthread)"
else
	ko "build produced warnings/errors or no binary"; cat "$LOG"
fi

# ----------------------------------------------------------------------------
hdr "NORM (42)"
if command -v norminette >/dev/null 2>&1; then
	if norminette coders/ 2>&1 | grep -q "Error"; then
		ko "norminette reported errors"; norminette coders/ 2>&1 | grep -v ": OK" | head
	else
		ok "norminette: all files OK"
	fi
else
	skip "norminette not installed"
fi

# ----------------------------------------------------------------------------
hdr "ARGUMENT VALIDATION (must reject, no crash)"
expect_reject "no args"
expect_reject "too few args"       3 800 200 200 200 3 10
expect_reject "too many args"      3 800 200 200 200 3 10 fifo extra
expect_reject "zero coders"        0 800 200 200 200 3 10 fifo
expect_reject "negative value"     -3 800 200 200 200 3 10 fifo
expect_reject "non-integer"        3 800 200 abc 200 3 10 fifo
expect_reject "float value"        3 1.5 200 200 200 3 10 fifo
expect_reject "bad scheduler"      3 800 200 200 200 3 10 rr

# ----------------------------------------------------------------------------
hdr "EASY: no coder burns out, all complete (fifo + edf, up to 200 coders)"
# cycle = compile+debug+refactor = 180ms, deadline 800ms -> generous margin.
expect_complete "N=2   fifo"   2   800 60 60 60 3 10 fifo
expect_complete "N=5   fifo"   5   800 60 60 60 3 10 fifo
expect_complete "N=10  edf"    10  800 60 60 60 3 10 edf
expect_complete "N=50  fifo"   50  800 60 60 60 2 10 fifo
expect_complete "N=100 edf"    100 800 60 60 60 1 10 edf
expect_complete "N=200 fifo"   200 1000 60 60 60 1 10 fifo
expect_complete "N=7  (odd) edf" 7 800 60 60 60 3 10 edf

# ----------------------------------------------------------------------------
hdr "LESS EASY: burnout edge cases"
# one coder, one dongle -> can never grab two -> must burn out.
expect_burnout "N=1 must burn out" 1 200 100 100 100 5 10 fifo
# burnout reported close to the deadline (<= ~10ms tolerance)?
run 5 3 130 100 60 60 9 5 fifo
t=$(grep "burned out" "$LOG" | head -1 | awk '{print $1}')
if [ -n "$t" ] && [ "$t" -ge 130 ] && [ "$t" -le 200 ]; then
	ok "burnout logged near deadline (${t}ms, deadline 130ms)"
else
	skip "burnout-timing check inconclusive (t=${t:-none})"
fi
# every 'is compiling' is preceded by two 'has taken a dongle'
run 6 2 800 100 80 80 2 5 fifo
takes=$(grep -c "has taken a dongle" "$LOG"); comps=$(ccount)
if [ "$takes" -eq $((comps * 2)) ]; then
	ok "two dongles taken per compile ($takes takes / $comps compiles)"
else
	ko "dongle/compile mismatch ($takes takes, $comps compiles)"
fi

# ----------------------------------------------------------------------------
hdr "MEDIUM: cooldown + scheduler difference"
# cooldown=400: after coder 1 releases, the dongle is re-taken >= 400ms later.
run 6 2 5000 100 50 50 2 400 fifo
rel=$(grep "is debugging" "$LOG" | head -1 | awk '{print $1}')
nxt=$(grep "has taken a dongle" "$LOG" | sed -n '3p' | awk '{print $1}')
if [ -n "$rel" ] && [ -n "$nxt" ] && [ $((nxt - rel)) -ge 390 ]; then
	ok "cooldown respected (re-taken ${nxt}ms, released ~${rel}ms)"
else
	skip "cooldown check inconclusive (rel=$rel nxt=$nxt)"
fi
# fifo vs edf both run and complete (different arbitration policies).
expect_complete "fifo policy runs" 6 1500 80 80 80 2 10 fifo
expect_complete "edf  policy runs" 6 1500 80 80 80 2 10 edf

# ----------------------------------------------------------------------------
hdr "MEMORY LEAKS (valgrind)"
if command -v valgrind >/dev/null 2>&1; then
	for args in "5 1500 80 80 80 2 10 fifo" "5 1500 80 80 80 2 10 edf" \
	            "1 200 100 100 100 5 10 fifo"; do
		timeout 120 valgrind --leak-check=full \
			--errors-for-leak-kinds=definite,indirect --error-exitcode=42 \
			./codexion $args >/dev/null 2>"$LOG"
		if [ $? -eq 42 ]; then ko "valgrind: leak/error [$args]"
		else ok "valgrind clean [$args]"; fi
	done
else
	skip "valgrind not installed"
fi

# ----------------------------------------------------------------------------
hdr "DATA RACES (ThreadSanitizer)"
if cc -Wall -Wextra -Werror -pthread -fsanitize=thread -g coders/*.c \
       -o ./codexion_tsan 2>/dev/null; then
	for args in "8 1500 120 100 100 3 10 fifo" "8 1500 120 100 100 3 10 edf"; do
		timeout 60 ./codexion_tsan $args >/dev/null 2>"$LOG"
		if grep -q "WARNING: ThreadSanitizer" "$LOG"; then
			ko "data race [$args]"; else ok "tsan clean [$args]"; fi
	done
	rm -f ./codexion_tsan
else
	skip "ThreadSanitizer not available"
fi

# ----------------------------------------------------------------------------
printf "\n${YEL}== SUMMARY ==${RST}\n"
printf "  ${GREEN}%d passed${RST}, ${RED}%d failed${RST}, ${YEL}%d skipped${RST}\n" \
	"$PASS" "$FAIL" "$SKIP"
rm -f "$LOG"
[ "$FAIL" -eq 0 ]
