#!/usr/bin/env bash

#cp --parents arch/arm/include/asm/unistd.h /work/achromix_kernel/arch/arm/include/asm/


KERNEL_ROOT=/work/achroimx_kernel
SOURCE_DIR="$(pwd)"/achroimx_kernel



#cp --parents arch/arm/include/asm/unistd.h $TARGET_DIR

cp -f achroimx_kernel/arch/arm/include/asm/unistd.h $KERNEL_ROOT/arch/arm/include/asm/
cp -f achroimx_kernel/arch/arm/kernel/calls.S $KERNEL_ROOT/arch/arm/kernel/
cp -f achroimx_kernel/include/linux/syscalls.h $KERNEL_ROOT/include/linux/syscalls.h
cp -f achroimx_kernel/kernel/Makefile $KERNEL_ROOT/kernel
cp -f achroimx_kernel/kernel/proj2_call.c $KERNEL_ROOT/kernel

