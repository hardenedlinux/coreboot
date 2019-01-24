/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2019 HardenedLinux
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

#include <cbmem.h>
#include <console/console.h>
#include <console/streams.h>
#include <console/uart.h>
#include <program_loading.h>
#include <soc/clock.h>
#include <soc/sdram.h>
#include <mcall.h>
#include <fdt.h>
#include <string.h>
#include <symbols.h>
#include <cbfs.h>
#include <soc/otp.h>
#include <soc/addressmap.h>

static void update_dtb(void)
{
	uintptr_t dtb_maskrom = (uintptr_t)cbfs_boot_map_with_leak(
		"fallback/DTB", CBFS_TYPE_RAW, NULL);
	uint32_t  dtb_size = fdt_size(dtb_maskrom);
	uintptr_t dtb_target = (uintptr_t)cbmem_add(CBMEM_ID_DEVICETREE,
		dtb_size);

	memcpy((void *)dtb_target, (void *)dtb_maskrom, dtb_size);
	fdt_reduce_mem(dtb_target, sdram_size_mb() * 1024 * 1024);

	uint32_t serial = otp_read_serial();
	unsigned char mac[6] = { 0x70, 0xb3, 0xd5, 0x92, 0xf0, 0x00 };
	if (serial != ~0) {
		mac[5] |= (serial >>  0) & 0xff;
		mac[4] |= (serial >>  8) & 0xff;
		mac[3] |= (serial >> 16) & 0xff;
	}
	fdt_set_prop(dtb_target, "local-mac-address", &mac[0]);

	for (int i = 0; i < CONFIG_MAX_CPUS; i++)
		OTHER_HLS(i)->fdt = (void *)dtb_target;
}

static void nsleep(long nsec)
{
	long setp = 600;
	while (*(volatile long *)&nsec > 0)
		*(volatile long *)&nsec -= setp;
}

#define GPIO_REG(n) (*(uint32_t *)(FU540_GPIO + (n)))
#define GPIO_OUTPUT_EN  0x08
#define GPIO_OUTPUT_VAL 0x0c

static void phy_init(void)
{
#define PHY_NRESET 0x1000
	nsleep(2000000);
	__sync_fetch_and_or(&GPIO_REG(GPIO_OUTPUT_VAL),  PHY_NRESET);
	__sync_fetch_and_or(&GPIO_REG(GPIO_OUTPUT_EN),   PHY_NRESET);
	nsleep(100);
	__sync_fetch_and_and(&GPIO_REG(GPIO_OUTPUT_VAL), ~PHY_NRESET);
	nsleep(100);
	__sync_fetch_and_or(&GPIO_REG(GPIO_OUTPUT_VAL),  PHY_NRESET);
	nsleep(15000000);
}

void main(void)
{
	console_init();

	/* TODO: Follow Section 6.3 (FSBL) of the FU540 manual */

	/*
	 * Flush console before changing clock/UART divisor to prevent garbage
	 * being printed.
	 */
	console_tx_flush();

	clock_init();

	// re-initialize UART
	if (IS_ENABLED(CONFIG_CONSOLE_SERIAL))
		uart_init(CONFIG_UART_FOR_CONSOLE);

	sdram_init();
	phy_init();

	cbmem_initialize_empty();

	update_dtb();

	run_ramstage();
}
