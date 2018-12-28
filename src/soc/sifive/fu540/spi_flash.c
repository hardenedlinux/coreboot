/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 SiFive, Inc
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

#include <soc/spi_flash.h>

/**
 * Set up SPI for direct, non-memory-mapped access.
 */
int initialize_spi_flash_direct(
		volatile struct spi_ctrl *spictrl,
		unsigned int spi_clk_input_khz,
		unsigned int command_enable,
		unsigned int command_reset)
{
	spictrl->sckdiv = spi_min_clk_divisor(spi_clk_input_khz, 10000);

	spictrl->fctrl.en = 0;

	spi_txrx(spictrl, command_enable);
	spi_txrx(spictrl, command_reset);

	return 0;
}

/**
 * Set up SPI for memory-mapped access.
 */
int initialize_spi_flash_mmap(
		volatile struct spi_ctrl *spictrl,
		unsigned int spi_clk_input_khz,
		uint32_t ffmt_rawbits,
		unsigned int command_enable,
		unsigned int command_reset)
{
	spictrl->sckdiv = spi_min_clk_divisor(spi_clk_input_khz, 10000);

	spictrl->fctrl.en = 0;

	spi_txrx(spictrl, command_enable);
	spi_txrx(spictrl, command_reset);

	spictrl->ffmt.raw_bits = ffmt_rawbits;

	spictrl->fctrl.en = 1;
	__asm__ __volatile__ ("fence io, io");
	return 0;
}
