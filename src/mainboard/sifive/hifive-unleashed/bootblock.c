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

extern void flash_init(void);
extern void bootblock_mainboard_init(void);

void bootblock_mainboard_init(void)
{
	flash_init();
}
