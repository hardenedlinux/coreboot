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

#ifndef __SOC_SIFIVE_HIFIVE_U_SPI_FLASH_H__
#define __SOC_SIFIVE_HIFIVE_U_SPI_FLASH_H__

#include <soc/spi.h>
#include <stdint.h>

#define SPI_PROTO_S             0
#define SPI_PROTO_D             1
#define SPI_PROTO_Q             2

/**
 * Set up SPI for direct, non-memory-mapped access.
 */

static inline int initialize_spi_flash_direct(
		spi_ctrl *spictrl,
		unsigned int spi_clk_input_khz,
		unsigned int command_enable,
		unsigned int command_reset)
{
	// Max desired SPI clock is 10MHz
	spictrl->sckdiv = spi_min_clk_divisor(spi_clk_input_khz, 10000);

	spictrl->fctrl.en = 0;

	spi_txrx(spictrl, command_enable);
	spi_txrx(spictrl, command_reset);

	return 0;
}

static inline int _initialize_spi_flash_mmap(
		spi_ctrl *spictrl,
		unsigned int spi_clk_input_khz,
		uint32_t ffmt_rawbits,
		unsigned int command_enable,
		unsigned int command_reset)
{
	// Max desired SPI clock is 10MHz
	spictrl->sckdiv = spi_min_clk_divisor(spi_clk_input_khz, 10000);

	spictrl->fctrl.en = 0;

	spi_txrx(spictrl, command_enable);
	spi_txrx(spictrl, command_reset);

	spictrl->ffmt.raw_bits = ffmt_rawbits;

	spictrl->fctrl.en = 1;
	__asm__ __volatile__ ("fence io, io");
	return 0;
}


static inline int initialize_spi_flash_mmap_single(
		spi_ctrl *spictrl,
		unsigned int spi_clk_input_khz,
		uint32_t ffmt_rawbits,
		unsigned int command_enable,
		unsigned int command_reset)
{
	return _initialize_spi_flash_mmap(
		  spictrl,
		  spi_clk_input_khz,
		  ffmt_rawbits,
		  command_enable,
		  command_reset);
}


static inline int initialize_spi_flash_mmap_quad(
		spi_ctrl *spictrl,
		unsigned int spi_clk_input_khz,
		uint32_t ffmt_rawbits,
		unsigned int command_enable,
		unsigned int command_reset)
{
	return _initialize_spi_flash_mmap(
		  spictrl,
		  spi_clk_input_khz,
		  ffmt_rawbits,
		  command_enable,
		  command_reset);
}

#endif /* __SOC_SIFIVE_HIFIVE_U_SPI_FLASH_H__ */
