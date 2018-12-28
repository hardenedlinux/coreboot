/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux
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

#include <soc/addressmap.h>
#include <soc/spi.h>
#include <soc/spi_flash.h>
#include <soc/clock.h>
#include "flash.h"

/* 4-byte address fast read quad output */
#define IS25WP256D_4FRQO 0x6c
/* software reset enable */
#define IS25WP256D_RSTEN 0x66
/* software reset */
#define IS25WP256D_RST   0x99

const static spi_reg_ffmt ffmt = {
	.cmd_en = 1,
	.addr_len = 4,
	.pad_cnt = 8,
	.command_proto = SPI_PROTO_S,
	.addr_proto = SPI_PROTO_S,
	.data_proto = SPI_PROTO_Q,
	.command_code = IS25WP256D_4FRQO
};


void flash_init(void)
{
	initialize_spi_flash_mmap(
			(volatile struct spi_ctrl *)FU540_QSPI0,
			clock_get_tlclk_khz(),
			ffmt.raw_bits,
			IS25WP256D_RSTEN,
			IS25WP256D_RST);
}
