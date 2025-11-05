#!/bin/bash

cd mytest

if command -v riscv32-unknown-elf-gcc >/dev/null 2>&1; then
    CC=riscv32-unknown-elf-gcc
else 
    CC=riscv64-unknown-elf-gcc
fi
CFLAGS="-march=rv32im_zicsr_zba_zbb -mabi=ilp32 -nostdlib -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables -O2 -T link.ld"

test_set=(
    andn
    orn
    xnor
    sh1add
    sh2add
    sh3add
    clz
    ctz
    cpop
)
    
for test in "${test_set[@]}"; do
    $CC $CFLAGS -o ${test}_test.elf ${test}_test.c
done

cd ..

make

SIM=./riscv-sim
pass_count=0
fail_count=0

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

for test in "${test_set[@]}"; do
    elf_path="mytest/${test}_test.elf"
    echo "----------------------------------------"
    echo "Running ${elf_path}"

    if [[ ! -f "$elf_path" ]]; then
        echo -e "${YELLOW}⚠️  ELF not found, skipping ${test}${NC}"
        ((fail_count++))
        continue
    fi

    output=$($SIM -f "${elf_path}" 2>&1)
    result=$?

    echo "$output"

    if [ $result -ne 0 ]; then
        echo -e "${RED}❌ ${test} FAILED (exit code ${result})${NC}"
        ((fail_count++))
    elif echo "$output" | grep -q "ERR"; then
        echo -e "${RED}❌ ${test} FAILED (found ERR in output)${NC}"
        ((fail_count++))
    else
        echo -e "${GREEN}✅ ${test} PASSED${NC}"
        ((pass_count++))
    fi
    echo
done

echo "========================================"
echo -e "${GREEN}${pass_count} test(s) passed${NC}, ${RED}${fail_count} test(s) failed${NC}"
echo "========================================"
