/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc
 * Copyright (C) 2018 HardenedLinux
 * Copyright (C) 2018 Jonathan Neuschäfer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/boot.h>
#include <arch/encoding.h>
#include <console/console.h>

void run_payload(
		void (*fn)(uintptr_t arg0, uintptr_t arg1),
			uintptr_t arg0,
			uintptr_t arg1)
{
	uintptr_t status = read_csr(mstatus);
	status &= ~MSTATUS_MPIE;
	status &= ~MSTATUS_MPP;
	switch (CONFIG_RISCV_PAYLOAD_MODE) {
	case 0:
		break;
	case 1:
		status |= MSTATUS_SPP;
		break;
	case 3:
		fn(arg0, arg1);
		return;
	default:
		die("wrong privilege level for payload");
		break;
	}
	write_csr(mstatus, status);
	write_csr(mepc, fn);
	asm volatile("mv a0, %0"::"r"(arg0):"a0");
	asm volatile("mv a1, %0"::"r"(arg1):"a1");
	asm volatile("mret");
}
