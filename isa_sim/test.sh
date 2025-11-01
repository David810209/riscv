#!/bin/bash
set -e

cd mytest

CC=riscv32-unknown-elf-gcc
CFLAGS="-march=rv32im_zbb -mabi=ilp32 -nostdlib -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables -O2 -T link.ld"

$CC $CFLAGS -o zext_h_test.elf zext_h_test.c
$CC $CFLAGS -o sext_b_test.elf sext_b_test.c
$CC $CFLAGS -o sext_h_test.elf sext_h_test.c
$CC $CFLAGS -o rol_test.elf rol_test.c
$CC $CFLAGS -o ror_test.elf ror_test.c
$CC $CFLAGS -o rori_test.elf rori_test.c
$CC $CFLAGS -o orc_b_test.elf orc_b_test.c
$CC $CFLAGS -o rev8_test.elf rev8_test.c

cd ..

make

SIM=./riscv-sim
for elf in \
    mytest/zext_h_test.elf \
    mytest/sext_b_test.elf \
    mytest/sext_h_test.elf \
    mytest/rol_test.elf \
    mytest/ror_test.elf \
    mytest/rori_test.elf \
    mytest/orc_b_test.elf \
    mytest/rev8_test.elf
do
    echo "Running $elf"
    $SIM -f "$elf"
done