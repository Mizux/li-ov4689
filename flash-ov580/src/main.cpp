/*
 * Copyright (c) 2006-2009 Logitech.
 *
 * This file is part of libwebcam.
 *
 * libwebcam is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libwebcam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libwebcam.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <iconv.h>
#include <limits.h>
#include <linux/usb/video.h>
#include <linux/uvcvideo.h>
#include <linux/videodev2.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>

#define OV580_SensorRegisterRW_UNIT 0x0200
#define OV580_COMMAND_SUB_TYPE_WRITE 0x02
#define OV580_COMMAND_SUB_TYPE_READ 0x03
#define OV580_COMMAND_SUB_TYPE_ERASE_SECTOR 0x20

#define OV580_COMMAND_ID 0x51
#define OV580_COMMAND_TYPE_FLASH 0xA1

#define SPI_PAGE_SIZE 256
#define SPI_FLASH_SIZE 0x13AD8
#define SPI_FLASH_ERASE_MAX 0x14000
#define SPI_FLASH_SECTOR_SIZE 0x1000

/**
 * Open the V4L2 device node with the given name.
 *
 * @param device_name	A device name as accepted by c_open_device()
 *
 * @return
 * 		- 0 if the device could not be opened
 * 		- a device handle > 0 on success
 */

static __u8 value[512] = {0};
static __u8* data = (__u8*)value;
static __u32 v4l2_dev;

struct uvc_xu_control_query xu_query = {
    .unit = 4,      // has to be unit 4
    .selector = 1,  // TD
    .query = UVC_SET_CUR,
    .size = 4,  // TD
    .data = value,
};

int
open_v4l2_device(char* device_name) {
  int v4l2_dev;
  char* dev_node;

  if (device_name == NULL)
    // return C_INVALID_ARG;
    return -5;

  dev_node = (char*)malloc(5 + strlen(device_name) + 1);
  if (!dev_node)
    return 0;
  sprintf(dev_node, "/dev/%s", device_name);
  v4l2_dev = open(dev_node, 0);
  free(dev_node);
  return v4l2_dev;
}

void
error_handle() {
  int res = errno;

  const char* err;
  switch (res) {
    case ENOENT:
      err = "Extension unit or control not found";
      break;
    case ENOBUFS:
      err = "Buffer size does not match control size";
      break;
    case EINVAL:
      err = "Invalid request code";
      break;
    case EBADRQC:
      err = "Request not supported by control";
      break;
    default:
      err = strerror(res);
      break;
  }
  printf("failed %s. (System code: %d) \n\r", err, res);

  return;
}

int
ISP_RegisterRead(__u32 isp_address) {
  xu_query.selector = OV580_SensorRegisterRW_UNIT >> 8;
  xu_query.query = UVC_SET_CUR;
  xu_query.size = 384;

  value[0] = OV580_COMMAND_ID;
  value[1] = 0xa2;
  value[2] = 0x20;
  value[3] = 0x04;
  value[4] = 0x01;
  value[5] = (isp_address >> 24) & 0xff;
  value[6] = (isp_address >> 16) & 0xff;
  value[7] = (isp_address >> 8) & 0xff;
  value[8] = isp_address & 0xff;
  value[9] = 0x90;
  value[10] = 0x01;
  value[11] = 0x00;
  value[12] = 0x01;

  if (ioctl(v4l2_dev, UVCIOC_CTRL_QUERY, &xu_query) != 0) {
    error_handle();
    return -1;
  }

  usleep(5000);

  xu_query.query = UVC_GET_CUR;
  if (ioctl(v4l2_dev, UVCIOC_CTRL_QUERY, &xu_query) != 0) {
    error_handle();
    return -1;
  }

  return value[17];
}

int
ISP_RegisterWrite(__u32 isp_address, __u8 isp_val) {
  xu_query.selector = OV580_SensorRegisterRW_UNIT >> 8;
  xu_query.query = UVC_SET_CUR;
  xu_query.size = 384;

  memset(value, 0, 512);

  value[0] = 0x50;
  value[1] = 0xa2;
  value[2] = 0x6c;
  value[3] = 0x04;
  value[4] = 0x01;
  value[5] = (isp_address >> 24) & 0xff;
  value[6] = (isp_address >> 16) & 0xff;
  value[7] = (isp_address >> 8) & 0xff;
  value[8] = isp_address & 0xff;
  value[9] = 0x90;
  value[10] = 0x01;
  value[11] = 0x00;
  value[12] = 0x01;
  value[16] = isp_val;

  if (ioctl(v4l2_dev, UVCIOC_CTRL_QUERY, &xu_query) != 0) {
    error_handle();
    return -1;
  }

  return 0;
}

int
ISP_spi_flash_errase_page(__u32 isp_address) {
  xu_query.selector = OV580_SensorRegisterRW_UNIT >> 8;
  xu_query.query = UVC_SET_CUR;
  xu_query.size = 384;

  memset(value, 0, 512);
  value[0] = OV580_COMMAND_ID;
  value[1] = OV580_COMMAND_TYPE_FLASH;
  value[2] = OV580_COMMAND_SUB_TYPE_ERASE_SECTOR;
  /* reserved */
  value[3] = 0x00;
  value[4] = 0x00;
  /* address */
  value[5] = isp_address >> 3 * 8;
  value[6] = isp_address >> 2 * 8;
  value[7] = isp_address >> 1 * 8;
  value[8] = isp_address >> 0 * 8;

  if (ioctl(v4l2_dev, UVCIOC_CTRL_QUERY, &xu_query) != 0) {
    error_handle();
    return -1;
  }

  usleep(5000);

  return 0;
}

#define OV580_FLASH_FLAGS_FIRST_PKG 1 << 7
#define OV580_FLASH_FLAGS_LAST_PKG 1 << 4

int
ISP_spi_flash_page(__u32 isp_address, const __u8* buffer, __u16 len) {
  if (len > SPI_PAGE_SIZE) {
    fprintf(stderr, "len is too large max 256 got: %d\n", len);
    return -EINVAL;
  }

  xu_query.selector = OV580_SensorRegisterRW_UNIT >> 8;
  xu_query.query = UVC_SET_CUR;
  xu_query.size = 384;

  memset(value, 0, 512);

  value[0] = OV580_COMMAND_ID;
  value[1] = OV580_COMMAND_TYPE_FLASH;
  value[2] = OV580_COMMAND_SUB_TYPE_WRITE;
  /* reserved */
  value[3] = 0x00;
  value[4] = 0x00;
  /* address */
  value[5] = isp_address >> 3 * 8;
  value[6] = isp_address >> 2 * 8;
  value[7] = isp_address >> 1 * 8;
  value[8] = isp_address >> 0 * 8;
  /* flags */
  value[9] =
      OV580_FLASH_FLAGS_FIRST_PKG | OV580_FLASH_FLAGS_LAST_PKG | (len >> 1 * 8);
  value[10] = len & 0xFF;
  /* len */
  value[11] = len >> 1 * 8;
  value[12] = len >> 0 * 8;

  value[13] = 0x00;
  value[14] = 0x00;
  value[15] = 0x00;

  memcpy(value + 16, buffer, len);

  if (ioctl(v4l2_dev, UVCIOC_CTRL_QUERY, &xu_query) != 0) {
    error_handle();
    return -1;
  }

  usleep(5000);

  printf("Flashed page %d\n", isp_address);

  return 0;
}

int
ISP_spi_read_page(__u32 isp_address, __u8* buffer, __u16 len) {
  if (len > 256) {
    fprintf(stderr, "len is too large max 256 got: %d\n", len);
    return -EINVAL;
  }
  memset(value, 0, len);

  xu_query.selector = OV580_SensorRegisterRW_UNIT >> 8;
  xu_query.query = UVC_SET_CUR;
  xu_query.size = 384;

  value[0] = OV580_COMMAND_ID;
  value[1] = OV580_COMMAND_TYPE_FLASH;
  value[2] = OV580_COMMAND_SUB_TYPE_READ;
  /* reserved */
  value[3] = 0x00;
  value[4] = 0x00;
  /* address */
  value[5] = isp_address >> 3 * 8;
  value[6] = isp_address >> 2 * 8;
  value[7] = isp_address >> 1 * 8;
  value[8] = isp_address >> 0 * 8;
  /* flags */
  value[9] =
      OV580_FLASH_FLAGS_FIRST_PKG | OV580_FLASH_FLAGS_LAST_PKG | (len >> 1 * 8);
  value[10] = len & 0xFF;
  /* len */
  value[11] = len >> 1 * 8;
  value[12] = len >> 0;

  value[13] = 0x00;
  value[14] = 0x00;
  value[15] = 0x00;

  if (ioctl(v4l2_dev, UVCIOC_CTRL_QUERY, &xu_query) != 0) {
    error_handle();
    return -EIO;
  }

  usleep(5000);

  xu_query.query = UVC_GET_CUR;
  if (ioctl(v4l2_dev, UVCIOC_CTRL_QUERY, &xu_query) != 0) {
    error_handle();
    return -1;
  }

  memcpy(buffer, value + 17, SPI_PAGE_SIZE);

  return SPI_PAGE_SIZE;
}

/* This erase the spi flash sector,
 * one sector is 4k so 0x1000
 */
int
erase_spi_flash() {
  for (__u32 address = 0; address < SPI_FLASH_ERASE_MAX;
       address += SPI_FLASH_SECTOR_SIZE) {
    if (ISP_spi_flash_errase_page(address) != 0)
      return -EIO;
  }

  return 0;
}

int
fwcmp(const char* fw_path) {
  __u8 filebuffer[SPI_PAGE_SIZE];
  __u8 flashbuffer[SPI_PAGE_SIZE];
  int fd, n, err;
  __u32 address = 0;

  fd = open(fw_path, O_RDONLY);
  if (fd < 0) {
    printf("can't open file: %s: %s\n", fw_path, strerror(errno));
    return -errno;
  }

  while ((n = read(fd, filebuffer, SPI_PAGE_SIZE)) > 0) {
    err = ISP_spi_read_page(address, flashbuffer, n);
    if (err < 0)
      return err;

    if (memcmp(filebuffer, flashbuffer, n) != 0) {
      for (int j = 0; j < SPI_PAGE_SIZE; j++) {
        if (filebuffer[j] != flashbuffer[j])
          printf(
              "data %d at address %d is different from flash (%x) and file "
              "(%x)\n",
              j, address, flashbuffer[j], filebuffer[j]);
      }
      return -1;
    }
    address += n;
  }
  return 0;
}

int
flash_fw(const char* fw_path) {
  int fd, n, err = 0;
  __u8 buffer[SPI_PAGE_SIZE];
  __u32 address = 0;

  fd = open(fw_path, O_RDONLY);
  if (fd < 0) {
    printf("can't open file: %s: %s\n", fw_path, strerror(errno));
    return -errno;
  }

  /* official tools seems to wipe the data first,
   * let's do that.
   */
  erase_spi_flash();

  while ((n = read(fd, buffer, SPI_PAGE_SIZE)) > 0) {
    err = ISP_spi_flash_page(address, buffer, n);
    if (err)
      printf("Failed to flash page %d\n", address);
    address += n;
  }

  if (n < 0)
    printf("can't read from file: %s: %s\n", fw_path, strerror(errno));

  close(fd);
}

int
read_fw(const char* fw_path) {
  __u8 buffer[SPI_PAGE_SIZE];
  __u32 address = 0;
  int fd, n;

  fd = open(fw_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

  if (fd < 0) {
    printf("can't open file: %s: %s\n", fw_path, strerror(errno));
    return -errno;
  }

  while ((n = ISP_spi_read_page(address, buffer, SPI_PAGE_SIZE)) > 0) {
    int err = write(fd, buffer, n);
    if (err < 0) {
      printf("can't write file: %s: %s\n", fw_path, strerror(errno));
      close(fd);
      return -errno;
    }
    address += n;
    if (address >= SPI_FLASH_SIZE)
      break;
  }

  if (n < 0)
    printf("can't read from file: %s: %s\n", fw_path, strerror(errno));
  close(fd);
  return 0;
}

long
get_version() {
  int a, b;
  long version;

  // Firmware version reading
  a = ISP_RegisterRead(0x8018fff1);
  b = ISP_RegisterRead(0x8018fff0);
  version = (a << 8) | b;
}

void
usage(const char* prog) {
  printf("usage:\n");
  printf("%s flash file\n", prog);
  printf("%s read file\n", prog);
  printf("%s fwcmp file\n", prog);
  printf("%s version\n", prog);
}

int
main(int argc, char* argv[]) {
  char* cmd = NULL;
  int err = 0;

  v4l2_dev = open_v4l2_device("video0");
  if (v4l2_dev < 0) {
    printf("open camera failed,err code:%d\n\r", v4l2_dev);
    return v4l2_dev;
  }

  if (argc > 1) {
    cmd = argv[1];
    if (strcmp("version", cmd) == 0) {
      printf("firmware version: %x\n\r", get_version());
      goto close;
    } else if (argc < 3)
      usage(argv[0]);
    else if (strcmp("flash", cmd) == 0)
      flash_fw(argv[2]);
    else if (strcmp("read", cmd) == 0)
      read_fw(argv[2]);
    else if (strcmp("fwcmp", cmd) == 0) {
      int err = fwcmp(argv[2]);
      if (err == -1)
        printf("Fw on OV580 is different from file %s\n", argv[2]);
      else if (err)
        printf("error \n");
      else
        printf("Fw file %s and OV580 flashed firmware are identical\n",
               argv[2]);
    }
  } else
    usage(argv[0]);

close:
  close(v4l2_dev);
  return err;
}
