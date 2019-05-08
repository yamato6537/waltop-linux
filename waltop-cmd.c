/*
 * waltop-cmd.c
 *
 *  Waltop tablet driver for linux - proprietary mode's commands
 * 
 *  This driver supports "proprietary mode" of Waltop tablet
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/slab.h>
#include <linux/usb.h>
#include <asm/unaligned.h>

#define USB_REQ_GET_REPORT 0x01
#define USB_REQ_SET_REPORT 0x09
#define WAIT_MSEC 10

/*********************************************************************************
 *  Send/Recieve Control Message
 *********************************************************************************/
static int send_message(struct usb_device *udev, unsigned int endpoint,
						__u8 type, __u16 index, __u8 *data, size_t size)
{
	return usb_control_msg(	udev,
							usb_sndctrlpipe(udev, endpoint),
							USB_REQ_SET_REPORT,
							USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
							(type << 8) | data[0],
							index, data, size, USB_CTRL_SET_TIMEOUT);
}

static int recv_message(struct usb_device *udev, unsigned int endpoint,
						__u8 type, __u16 index, __u8 *data, size_t size)
{
	return usb_control_msg(	udev,
							usb_rcvctrlpipe(udev, endpoint),
							USB_REQ_GET_REPORT,
							USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
							(type << 8) | data[0],
							index, data, size, USB_CTRL_GET_TIMEOUT);
}

/*********************************************************************************
 *  Set/Get Command
 *********************************************************************************/
static int waltop_set(struct usb_interface *intf, __u8 *cmd, size_t cmdlen)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	__u8 ifnum = intf->cur_altsetting->desc.bInterfaceNumber;
	int res;
	
	msleep(WAIT_MSEC);
	res = send_message(udev, 0, 0x03, ifnum, cmd, cmdlen);
	if (res != cmdlen) { return -EIO; }
	
	return 0;
}

static int waltop_get(struct usb_interface *intf, __u8 *cmd, size_t cmdlen)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	__u8 ifnum = intf->cur_altsetting->desc.bInterfaceNumber;    
	int res;
	
	msleep(WAIT_MSEC);
	res = send_message(udev, 0, 0x03, ifnum, cmd, cmdlen);
	if (res != cmdlen) { return -EIO; }
	
	msleep(WAIT_MSEC);
	res = recv_message(udev, 0, 0x03, ifnum, cmd, cmdlen);
	if (res != cmdlen) { return -EIO; }
	
	return get_unaligned_le16(cmd + 1);
}

/*********************************************************************************
 * Waltop Tablet Commands
 * 
 * See also Linux/drivers/input/tablet/aiptek.c
 *********************************************************************************/
#define WSET(intf, d1, d2) { __u8 cmd[] = {0x02, d1, d2}; return waltop_set(intf, cmd, sizeof(cmd)); }
#define WGET(intf, d1, d2) { __u8 cmd[] = {0x02, d1, d2}; return waltop_get(intf, cmd, sizeof(cmd)); }

int waltop_set_default_mode	(struct usb_interface *intf)			WSET(intf, 0x10, 0x00)
int waltop_set_tablet_mode	(struct usb_interface *intf)			WSET(intf, 0x10, 0x01)
int waltop_set_ink_mode		(struct usb_interface *intf)			WSET(intf, 0x10, 0x02)
int waltop_set_macro_keys	(struct usb_interface *intf, __u8 x)	WSET(intf, 0x11,    x)
int waltop_set_autogain_on	(struct usb_interface *intf)			WSET(intf, 0x12, 0xFF)
int waltop_set_filter_on	(struct usb_interface *intf)			WSET(intf, 0x17, 0x00)
int waltop_set_resolution	(struct usb_interface *intf)			WSET(intf, 0x18, 0x04)

int waltop_get_max_x		(struct usb_interface *intf)			WGET(intf, 0x01, 0x00)
int waltop_get_max_y		(struct usb_interface *intf)			WGET(intf, 0x01, 0x01)
int waltop_get_model_code	(struct usb_interface *intf)			WGET(intf, 0x02, 0x00)
int waltop_get_odm_code		(struct usb_interface *intf)			WGET(intf, 0x03, 0x00)
int waltop_get_firmware_ver	(struct usb_interface *intf)			WGET(intf, 0x04, 0x00)
int waltop_get_max_pressure	(struct usb_interface *intf)			WGET(intf, 0x08, 0x00)
