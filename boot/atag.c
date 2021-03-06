/*
 *  A module for creating rebootless custom image boot support.
 *
 *  Copyright (C) 2010 XVilka <xvilka at gmail.com>
 *
 *  Inspired by 2ndboot by dimich: http://hg.ezxdev.org/2ndboot/
 * 
 *  This file is part of 2ndboot-ng.
 *
 *  2ndboot-ng is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Foobar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *   along with 2ndboot-ng.  If not, see <http://www.gnu.org/licenses/>.
 * 
*/

#include "types.h"
#include "memory.h"
#include "atag_priv.h"
#include "images.h"
#include "stdio.h"
#include "atag.h"
#include "common.h"

void *atag_build() {
  struct memory_image image;
  struct tag *tag = (struct tag*)ATAG_BASE_ADDR;
	
  tag->hdr.tag = ATAG_CORE;
  tag->hdr.size = tag_size (tag_core);
  tag->u.core.flags = 0;
  tag->u.core.pagesize = 0x00001000;
  tag->u.core.rootdev = 0x0000;
  tag = tag_next(tag);

  if (image_find(IMG_CMDLINE, &image) != NULL) {
    char *atag_cmdline = tag->u.cmdline.cmdline;

    tag->hdr.tag = ATAG_CMDLINE;
    tag->hdr.size = (sizeof(struct tag_header) + image.size + 1 + 3) >> 2;
    memcpy(atag_cmdline, image.data, image.size);
    if (atag_cmdline[image.size-1] == '\xa') {
      atag_cmdline[image.size-1] = '\0';
    } else {
      atag_cmdline[image.size] = '\0';
    }
    tag = tag_next(tag);
  }

  tag->hdr.tag = ATAG_MBM_VERSION;
  tag->hdr.size = tag_size(tag_mbm_version);
  tag->u.mbm_version.mbm_version = 0x1234;
  tag = tag_next(tag);

  if (image_find(IMG_MOTFLATTREE, &image) != NULL) {
    tag->hdr.tag = ATAG_FLAT_DEV_TREE_ADDRESS;
    tag->hdr.size = tag_size (tag_flat_dev_tree_address);
    tag->u.flat_dev_tree_address.flat_dev_tree_address = (u32)image.data;
    tag->u.flat_dev_tree_address.flat_dev_tree_size = (u32)image.size;
    tag = tag_next(tag);
  }

  if (image_find(IMG_INITRAMFS, &image) != NULL) {
    tag->hdr.tag = ATAG_INITRD2;
    tag->hdr.size = tag_size(tag_initrd);
    tag->u.initrd.start = (u32)image.data;
    tag->u.initrd.size = (u32)image.size;
    tag = tag_next(tag);
  }

  if (image_find(IMG_USBFW, &image) != NULL) {
    tag->hdr.tag = ATAG_USB_FIRMWARE_ADDRESS;
    tag->hdr.size = tag_size (tag_usb_firmware_address);
    tag->u.usb_firmware_address.usb_firmware_address = (u32)image.data;
    tag = tag_next(tag);

    tag->hdr.tag = ATAG_USB_FIRMWARE_SIZE;
    tag->hdr.size = tag_size (tag_usb_firmware_size);
    tag->u.usb_firmware_size.usb_firmware_size = (u32)image.size;
    tag = tag_next(tag);
  }

  tag->hdr.tag = ATAG_NONE;
  tag->hdr.size = 0;
  return (void*)ATAG_BASE_ADDR;
}


