/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <program_loading.h>
#include <vm.h>
#include <arch/boot.h>
#include <arch/encoding.h>
#include <console/console.h>
#include <arch/smp/smp.h>
#include <mcall.h>

/*
 * A pointer to the Flattened Device Tree passed to coreboot by the boot ROM.
 * Presumably this FDT is also in ROM.
 *
 * This pointer is only used in ramstage!
 */
const void *rom_fdt;

static void do_arch_prog_run(struct prog *prog)
{
	void (*doit)(void *) = prog_entry(prog);
	void riscvpayload(const void *fdt, void *payload);
	void (*fn)(uintptr_t a0, uintptr_t a1) = prog_entry(prog);
	uintptr_t a0 = read_csr(mhartid);
	uintptr_t a1 = (uintptr_t)HLS()->fdt;

	if (ENV_RAMSTAGE && prog_type(prog) == PROG_PAYLOAD) {
		/*
		 * FIXME: This is wrong and will crash. Linux can't (in early
		 * boot) access memory that's before its own loading address.
		 * We need to copy the FDT to a place where Linux can access it.
		 */

		printk(BIOS_SPEW, "FDT is at %p\n", HLS()->fdt);
		printk(BIOS_SPEW, "OK, let's go\n");
		riscvpayload(HLS()->fdt, doit);
	}
	fn(a0, a1);
}

void arch_prog_run(struct prog *prog)
{
	smp_resume((void (*)(void *))do_arch_prog_run, prog);
}
