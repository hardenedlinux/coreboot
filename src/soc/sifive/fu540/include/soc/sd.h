/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 SiFive, Inc
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


#ifndef _LIBRARIES_SD_H
#define _LIBRARIES_SD_H

#define SD_INIT_ERROR_CMD0 1
#define SD_INIT_ERROR_CMD8 2
#define SD_INIT_ERROR_ACMD41 3
#define SD_INIT_ERROR_CMD58 4
#define SD_INIT_ERROR_CMD16 5

#define SD_COPY_ERROR_CMD18 1
#define SD_COPY_ERROR_CMD18_CRC 2

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stddef.h>

int sd_init(volatile struct spi_ctrl *spi, unsigned int input_clk_hz);
int sd_copy(volatile struct spi_ctrl *spi, void *dst, uint32_t src_lba,
		size_t size);

#endif /* !__ASSEMBLER__ */

#endif /* _LIBRARIES_SD_H */
