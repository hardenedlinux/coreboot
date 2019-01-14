/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Jonathan Neuschäfer
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

#include <boot_device.h>
#include <symbols.h>
#include <cbfs.h>
#include <arch/mmio.h>
#include <soc/addressmap.h>
#include <soc/spi.h>
#include <soc/sd.h>
#include <soc/clock.h>
#include <console/console.h>
#include <string.h>
#include "flash.h"

#define SPI2_CTRL ((volatile struct spi_ctrl*)FU540_QSPI2)

/* At 0x20000000: A 256MiB long memory-mapped view of the flash at QSPI0 */
static struct mem_region_device spi_mdev =
	MEM_REGION_DEV_RO_INIT((void *)0x20000000, CONFIG_ROM_SIZE);

static ssize_t unleashed_sd_readat(const struct region_device *rdev, void *dest,
					size_t offset, size_t count)
{
	if (offset / 512 == (offset + count - 1) / 512) {
		size_t lba = offset / 512;
		size_t o = offset % 512;
		size_t l = count;
		uint8_t tmp[512];
		if (sd_copy(SPI2_CTRL, tmp, lba, 1) != 0)
			return -1;
		memcpy(dest, tmp + o, l);
	} else {
		int has_begin = !!(offset % 512);
		int has_end = !!((offset + count) % 512);
		size_t first_lba = offset / 512;
		size_t last_lba = (offset + count - 1) / 512;
		if (has_begin) {
			size_t lba = first_lba;
			size_t o = offset % 512;
			size_t l = 512 - o;
			uint8_t tmp[512];
			if (sd_copy(SPI2_CTRL, tmp, lba, 1) != 0)
				return -1;
			memcpy(dest, tmp + o, l);
		}
		if (first_lba + has_begin <= last_lba - has_end) {
			size_t lba = first_lba + has_begin;
			size_t size = (last_lba - has_end) - lba + 1;
			size_t o = 512 - offset % 512;
			if (sd_copy(SPI2_CTRL, dest + o, lba, size) != 0)
				return -1;
		}
		if (has_end) {
			size_t lba = last_lba;
			size_t o = 0;
			size_t l = (offset + count) % 512;
			uint8_t tmp[512];
			if (sd_copy(SPI2_CTRL, tmp, lba, 1) != 0)
				return -1;
			memcpy(dest + count - l, tmp + o, l);
		}
	}
	return count;
}

static const struct region_device_ops unleashed_sd_ops = {
	.mmap   = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = unleashed_sd_readat,
};


static struct mmap_helper_region_device sd_mdev =
	MMAP_HELPER_REGION_INIT(&unleashed_sd_ops, 0, CONFIG_ROM_SIZE);


const struct region_device *boot_device_ro(void)
{
	switch (read32((uint32_t *)FU540_MSEL)) {
	case  6:
	case 10:
	case 15:
		return &spi_mdev.rdev;
	case 11:
		return &sd_mdev.rdev;
	}
	return NULL;
}

void boot_device_init(void)
{
	switch (read32((uint32_t *)FU540_MSEL)) {
	case  6:
	case 10:
	case 15:
		flash_init();
		break;
	case 11:
		sd_init(SPI2_CTRL, clock_get_tlclk_khz());
		mmap_helper_device_init(&sd_mdev,
				_cbfs_cache, REGION_SIZE(cbfs_cache));
		break;
	default:
		die("Wrong configuration of MSEL");
	}
}
