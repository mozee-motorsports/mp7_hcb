#!/bin/sh
set -eu
repo=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
core="$repo/mp7_hcb/CM7/Core"
test_dir="$repo/tests/sevcon_pdo"
build=$(mktemp -d "${TMPDIR:-/tmp}/hcb-sevcon-test.XXXXXX")
trap 'rm -rf "$build"' EXIT HUP INT TERM

for mode in unassigned fdcan2 fdcan1; do
  case "$mode" in
    unassigned) vehicle=1; sevcon=0 ;;
    fdcan2) vehicle=1; sevcon=2 ;;
    fdcan1) vehicle=2; sevcon=1 ;;
  esac
  "${CC:-cc}" -std=c11 -Wall -Wextra -Werror -pedantic \
    -fsanitize=address,undefined -fno-omit-frame-pointer \
    -DHCB_VEHICLE_FDCAN_INDEX="$vehicle" -DHCB_SEVCON_FDCAN_INDEX="$sevcon" \
    -I"$test_dir/support" -I"$core/Inc" \
    "$core/Src/canopen_pdo.c" "$core/Src/sevcon_pdo.c" \
    "$core/Src/sevcon_stm32.c" "$core/Src/sevcon_rx.c" \
    "$core/Src/mp7_hcb.c" "$test_dir/test_sevcon.c" -o "$build/$mode"
  "$build/$mode"
done

for source in canopen_pdo sevcon_pdo; do
  "${CC:-cc}" -std=c11 -Wall -Wextra -Werror -pedantic -I"$core/Inc" \
    -c "$core/Src/$source.c" -o "$build/$source.o"
done
"${CXX:-c++}" -std=c++11 -Wall -Wextra -Werror -pedantic -I"$core/Inc" \
  "$test_dir/test_headers.cpp" "$build/canopen_pdo.o" "$build/sevcon_pdo.o" \
  -o "$build/headers"
"$build/headers"
printf '%s\n' 'PASS: C++ headers and C linkage'
