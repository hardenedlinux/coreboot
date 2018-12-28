/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 SiFive, Inc
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

#include <soc/spi.h>

/**
 * Wait until SPI is ready for transmission and transmit byte.
 */
void spi_tx(spi_ctrl *spictrl, uint8_t in)
{
#if __riscv_atomic
	int32_t r;
	do {
		asm volatile (
			"amoor.w %0, %2, %1\n"
			: "=r" (r), "+A" (spictrl->txdata.raw_bits)
			: "r" (in)
		);
	} while (r < 0);
#else
	while ((int32_t) spictrl->txdata.raw_bits < 0)
		;
	spictrl->txdata.data = in;
#endif
}


/**
 * Wait until SPI receive queue has data and read byte.
 */
uint8_t spi_rx(spi_ctrl *spictrl)
{
	int32_t out;
	while ((out = (int32_t) spictrl->rxdata.raw_bits) < 0)
		;
	return (uint8_t) out;
}


/**
 * Transmit a byte and receive a byte.
 */
uint8_t spi_txrx(spi_ctrl *spictrl, uint8_t in)
{
	spi_tx(spictrl, in);
	return spi_rx(spictrl);
}
