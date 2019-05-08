/*
 * waltop-dev.c
 *
 *  Waltop tablet driver for linux - device-dependent code
 * 
 *  This driver supports "proprietary mode" of Waltop tablet
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/usb/input.h>
#include <asm/unaligned.h>
#include "waltop.h"

/*
 *	from xf86-input-wacom/src/xf86WacomDefs
 */
#define WACOM_STYLUS_DEVICE_ID	0x02
#define WACOM_TOUCH_DEVICE_ID	0x03
#define WACOM_CURSOR_DEVICE_ID	0x06
#define WACOM_ERASER_DEVICE_ID	0x0A
#define WACOM_PAD_DEVICE_ID		0x0F

/*********************************************************************************
 * Input Device Capability
 *
 * ATTENTION:
 *  ***_BTN needs KEY_ESC. 
 *  PAD_*** needs ABS_X and ABS_Y. 
 *
 * xf86-input-wacom supports following buttons:
 *	(See xf86-input-wacom/src/wcmUSB.c: static unsigned short padkey_codes[])
 *	BTN_0, BTN_1, BTN_2, BTN_3, BTN_4,
 *	BTN_5, BTN_6, BTN_7, BTN_8, BTN_9,
 *	BTN_A, BTN_B, BTN_C, BTN_X, BTN_Y, BTN_Z,
 *	BTN_BASE, BTN_BASE2, BTN_BASE3,
 *	BTN_BASE4, BTN_BASE5, BTN_BASE6,
 *	BTN_TL, BTN_TR, BTN_TL2, BTN_TR2, BTN_SELECT
 *********************************************************************************/
static int CURSOR_EVT[] = {EV_SYN, EV_KEY, EV_REL, -1};
static int CURSOR_BTN[] = {BTN_LEFT, BTN_MIDDLE, BTN_RIGHT, KEY_ESC, -1};
static int CURSOR_REL[] = {REL_X, REL_Y, -1};
static int CURSOR_REL_WHEEL[] = {REL_X, REL_Y, REL_WHEEL, REL_HWHEEL, -1};

static int STYLUS_EVT[] = {EV_SYN, EV_KEY, EV_ABS, -1};
static int STYLUS_BTN[] = {BTN_TOOL_PEN, BTN_TOUCH, BTN_STYLUS, BTN_STYLUS2, KEY_ESC, -1 };
static int STYLUS_ABS[] = {ABS_X, ABS_Y, ABS_PRESSURE, -1};
static int STYLUS_ABS_TILT[] = {ABS_X, ABS_Y, ABS_PRESSURE, ABS_TILT_X, ABS_TILT_Y, -1};
static int STYLUS_BTN_MYNOTE[] = {BTN_TOOL_PEN, BTN_TOUCH, -1};

static int PAD_EVT[] = {EV_SYN, EV_KEY, EV_ABS, -1};
static int PAD_BTN_16[] = {BTN_0, BTN_1, BTN_2, BTN_3, BTN_4,
							BTN_5, BTN_6, BTN_7, BTN_8, BTN_9,
							BTN_A, BTN_B, BTN_C, BTN_X, BTN_Y, BTN_Z,
							KEY_ESC, -1};
static int PAD_BTN_26[] = {BTN_0, BTN_1, BTN_2, BTN_3, BTN_4,
							BTN_5, BTN_6, BTN_7, BTN_8, BTN_9,
							BTN_A, BTN_B, BTN_C, BTN_X, BTN_Y, BTN_Z,
							BTN_BASE,  BTN_BASE2, BTN_BASE3,
							BTN_BASE4, BTN_BASE5, BTN_BASE6, 
							BTN_TL, BTN_TR, BTN_TL2, BTN_TR2,
							KEY_ESC, -1};
static int PAD_ABS[] = {ABS_X, ABS_Y, -1};

static int MACRO_EVT[] = {EV_SYN, EV_ABS, -1};
static int MACRO_ABS[] = {ABS_MISC, -1};

/* Standard Tablet */
struct waltop_icaps WC_STD[WALTOP_IDEV_LEN] =
{
	{CURSOR_EVT, CURSOR_BTN, CURSOR_REL, NULL,       NULL},
	{STYLUS_EVT, STYLUS_BTN, NULL,       STYLUS_ABS, NULL},
	{NULL,       NULL,       NULL,       NULL,       NULL},
	{NULL,       NULL,       NULL,       NULL,       NULL},
};

/* MyNote and MyNote2 */
struct waltop_icaps WC_MYN[WALTOP_IDEV_LEN] =
{
	{CURSOR_EVT, CURSOR_BTN,        CURSOR_REL, NULL,       NULL},
	{STYLUS_EVT, STYLUS_BTN_MYNOTE, NULL,       STYLUS_ABS, NULL},
	{NULL,       NULL,              NULL,       NULL,       NULL},
	{NULL,       NULL,              NULL,       NULL,       NULL},
};
/* Mars A and Mars BR Tablet */
struct waltop_icaps WC_MAR[WALTOP_IDEV_LEN] =
{
	{CURSOR_EVT, CURSOR_BTN, CURSOR_REL, NULL,       NULL},
	{STYLUS_EVT, STYLUS_BTN, NULL,       STYLUS_ABS, NULL},
	{NULL,       NULL,       NULL,       NULL,       NULL},
	{MACRO_EVT,  NULL,       NULL,       MACRO_ABS,  NULL},
};
/* Sirius Tablet */
struct waltop_icaps WC_SIR[WALTOP_IDEV_LEN] =
{
	{CURSOR_EVT, CURSOR_BTN, CURSOR_REL_WHEEL, NULL,            NULL},
	{STYLUS_EVT, STYLUS_BTN, NULL,             STYLUS_ABS_TILT, NULL},
	{PAD_EVT,    PAD_BTN_26, NULL,             PAD_ABS,         NULL},
	{MACRO_EVT,  NULL,       NULL,             MACRO_ABS,       NULL},
};
/* Venus Tablet */
struct waltop_icaps WC_VNS[WALTOP_IDEV_LEN] =
{
	{CURSOR_EVT, CURSOR_BTN, CURSOR_REL_WHEEL, NULL,            NULL},
	{STYLUS_EVT, STYLUS_BTN, NULL,             STYLUS_ABS_TILT, NULL},
	{NULL,       NULL,       NULL,             NULL,            NULL},
	{NULL,       NULL,       NULL,             NULL,            NULL},
};

/* Vega Tablet */
struct waltop_icaps WC_VEG[WALTOP_IDEV_LEN] =
{
	{CURSOR_EVT, CURSOR_BTN, CURSOR_REL_WHEEL, NULL,            NULL},
	{STYLUS_EVT, STYLUS_BTN, NULL,             STYLUS_ABS_TILT, NULL},
	{PAD_EVT,    PAD_BTN_16, NULL,             PAD_ABS,         NULL},
	{NULL,       NULL,       NULL,             NULL,            NULL},
};

/*********************************************************************************
 * Convinient Functions
 *********************************************************************************/
static inline void waltop_report_rel(struct input_dev *idev, int code, int value)
{
	input_report_rel(idev, code, value);
	input_sync(idev);
}

static inline void waltop_report_key(struct input_dev *idev, int keycode)
{
	input_report_key(idev, keycode, 1);
	input_sync(idev);
	input_report_key(idev, keycode, 0);
	input_sync(idev);
}

static inline void waltop_set_button_state(struct waltop_data *wdata, int button)
{
	switch (button) {
		case 0x0400:
			if (wdata->fn_btn != WALTOP_FNBTN_SCROLL) {
				wdata->fn_btn = WALTOP_FNBTN_SCROLL;
				wdata->sw_btn = 0;
			}
			break;
		case 0x0800:
			if (wdata->fn_btn != WALTOP_FNBTN_ZOOM_VOL) {
				wdata->fn_btn = WALTOP_FNBTN_ZOOM_VOL;
				wdata->sw_btn = 0;
			}
			break;
		case 0x1000:
			if (wdata->fn_btn != WALTOP_FNBTN_KEYBOARD) {
				wdata->fn_btn = WALTOP_FNBTN_KEYBOARD;
				wdata->sw_btn = 0;
			}
			break;
		case 0x2000:
			wdata->sw_btn ^= 1;
			break;
	}
}

/*********************************************************************************
 * Mouse, Pen, Macro report handler
 *********************************************************************************/
static inline void WH_MOUSE(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_STYLUS].idev;
	int data1 = data[1];
	
	input_report_key(idev, BTN_LEFT,   data1 & 0x1);
	input_report_key(idev, BTN_MIDDLE, data1 & 0x2);
	input_report_key(idev, BTN_RIGHT,  data1 & 0x4);    
	input_report_rel(idev, REL_X,      data[2]);
	input_report_rel(idev, REL_Y,      data[3]);
	input_sync(idev);
}

static inline void WH_STYLUS(__u8 *data, struct waltop_data *wdata)
{
	int data5 = data[5];
	
	if ((wdata->in_range = data5 & 0x01)) { /* NOT "==" */
		struct input_dev *idev = wdata->widevs[WALTOP_STYLUS].idev;
		input_report_abs(idev, ABS_X,        get_unaligned_le16(data + 1));
		input_report_abs(idev, ABS_Y,        get_unaligned_le16(data + 3));
		input_report_key(idev, BTN_TOOL_PEN, 1);
		input_report_key(idev, BTN_TOUCH,    data5 & 0x04);
		input_report_key(idev, BTN_STYLUS,   data5 & 0x08);
		input_report_key(idev, BTN_STYLUS2,  data5 & 0x10);
		input_report_abs(idev, ABS_PRESSURE, get_unaligned_le16(data + 6));
		input_sync(idev);
	}
}

static inline void WH_STYLUS_TILT(__u8 *data, struct waltop_data *wdata)
{
	int data5 = data[5];
	
	if ((wdata->in_range = data5 & 0x01)) { /* NOT "==" */
		struct input_dev *idev = wdata->widevs[WALTOP_STYLUS].idev;
		input_report_abs(idev, ABS_X, 			get_unaligned_le16(data + 1));
		input_report_abs(idev, ABS_Y, 			get_unaligned_le16(data + 3));
		input_report_key(idev, BTN_TOOL_PEN, 	1);
		input_report_key(idev, BTN_TOUCH,		data5 & 0x04);
		input_report_key(idev, BTN_STYLUS,		data5 & 0x08);
		input_report_key(idev, BTN_STYLUS2,		data5 & 0x10);        
		input_report_abs(idev, ABS_PRESSURE,	get_unaligned_le16(data + 6));
		input_report_abs(idev, ABS_TILT_X,		 (s8)data[8]);
		input_report_abs(idev, ABS_TILT_Y,		-(s8)data[9]);
		input_sync(idev);
	}   
}

static inline void WH_STYLUS_MYNOTE1(__u8 *data, struct waltop_data *wdata)
{
	int data5 = data[5];
	
	if ((wdata->in_range = data5 & 0x01)) { /* NOT "==" */
		struct input_dev *idev = wdata->widevs[WALTOP_STYLUS].idev;
		input_report_abs(idev, ABS_X,        get_unaligned_le16(data + 1));
		input_report_abs(idev, ABS_Y,        get_unaligned_le16(data + 3));
		input_report_key(idev, BTN_TOOL_PEN, 1);
		input_report_key(idev, BTN_TOUCH,    data5 & 0x04);
		input_report_abs(idev, ABS_PRESSURE, get_unaligned_le16(data + 6));
		input_sync(idev);
	}
}

static inline void WH_STYLUS_MYNOTE2(__u8 *data, struct waltop_data *wdata)
{
	int data6 = data[6];
	
	if ((wdata->in_range = data6 & 0x01)) { /* NOT "==" */
		struct input_dev *idev = wdata->widevs[WALTOP_STYLUS].idev;
		input_report_abs(idev, ABS_X,        get_unaligned_le16(data + 1));
		input_report_abs(idev, ABS_Y,        get_unaligned_le16(data + 3));
		input_report_key(idev, BTN_TOOL_PEN, 1);
		input_report_key(idev, BTN_TOUCH,    data6 & 0x04);
		input_report_abs(idev, ABS_PRESSURE, data[7] & 0x0FFF);
		input_sync(idev);
	}
}

static inline void WH_MACRO(__u8 *data, struct waltop_data *wdata)
{
	if (!wdata->in_range) {
		struct input_dev *idev = wdata->widevs[WALTOP_MACRO].idev;
		input_report_abs(idev, ABS_MISC, get_unaligned_le32(data + 1));
		input_sync(idev);
	}
}

/*********************************************************************************
 * Standard Tablet
 *********************************************************************************/
void WH_STD(__u8 *data, struct waltop_data *wdata)
{
	switch (data[0]) {
		case 0x01: WH_MOUSE(data, wdata); return;
		case 0x02: WH_STYLUS(data, wdata); return;
	}
}

/*********************************************************************************
 * MyNote
 *********************************************************************************/
void WH_MY1(__u8 *data, struct waltop_data *wdata)
{
	switch (data[0]) {
		case 0x01: WH_MOUSE(data, wdata); return;
		case 0x02: WH_STYLUS_MYNOTE1(data, wdata); return;
	}
}

/*********************************************************************************
 * MyNote2
 *********************************************************************************/
void WH_MY2(__u8 *data, struct waltop_data *wdata)
{
	switch (data[0]) {
		case 0x01: WH_MOUSE(data, wdata); return;
		case 0x02: WH_STYLUS_MYNOTE2(data, wdata); return;
	}
}

/*********************************************************************************
 * Waltop Mars A and Mars BR Tablet
 *********************************************************************************/
void WH_MAR(__u8 *data, struct waltop_data *wdata)
{  
	switch (data[0]) {
		case 0x01: WH_MOUSE(data, wdata); return;
		case 0x02: WH_STYLUS(data, wdata); return;
		case 0x05: WH_MACRO(data, wdata); return;
	}
}

/*********************************************************************************
 * Sirius Bettery Free Tablet
 *********************************************************************************/
static inline void WH_0A02_SIRIUS_WHEEL(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_CURSOR].idev;
	int ch = (s8)data[2];	/* Wheel changed  (-1:CCW, 0:None, 1:CW) */
	int rp = data[6];		/* Right wheel position */
	int lp = data[7];		/* Left  wheel position */
	int sw = wdata->sw_btn;  

	if      (lp && ch > 0 && !sw) waltop_report_rel(idev, REL_WHEEL,  -1);
	else if (lp && ch < 0 && !sw) waltop_report_rel(idev, REL_WHEEL,   1);
	else if (rp && ch > 0 && !sw) waltop_report_rel(idev, REL_HWHEEL,  1);
	else if (rp && ch < 0 && !sw) waltop_report_rel(idev, REL_HWHEEL, -1);
	else if (lp && ch > 0 &&  sw) waltop_report_rel(idev, REL_HWHEEL,  1);
	else if (lp && ch < 0 &&  sw) waltop_report_rel(idev, REL_HWHEEL, -1);
	else if (rp && ch > 0 &&  sw) waltop_report_rel(idev, REL_WHEEL,  -1);
	else if (rp && ch < 0 &&  sw) waltop_report_rel(idev, REL_WHEEL,   1);

	wdata->fn_btn = WALTOP_FNBTN_SCROLL;
}

static inline void WH_0A02_SIRIUS(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	int ch = (s8)data[2];	/* Wheel changed  (-1:CCW, 0:None, 1:CW) */
	int rp = data[6];		/* Right wheel position */
	int lp = data[7];		/* Left  wheel position */
	int sw = wdata->sw_btn;  

	if      (lp && ch > 0 && !sw) waltop_report_key(idev, BTN_A);
	else if (lp && ch < 0 && !sw) waltop_report_key(idev, BTN_B);
	else if (rp && ch > 0 && !sw) waltop_report_key(idev, BTN_C);
	else if (rp && ch < 0 && !sw) waltop_report_key(idev, BTN_X);
	else if (lp && ch > 0 &&  sw) waltop_report_key(idev, BTN_C);
	else if (lp && ch < 0 &&  sw) waltop_report_key(idev, BTN_X);
	else if (rp && ch > 0 &&  sw) waltop_report_key(idev, BTN_A);
	else if (rp && ch < 0 &&  sw) waltop_report_key(idev, BTN_B);

	wdata->fn_btn = WALTOP_FNBTN_SCROLL;
}

static inline void WH_0A03_SIRIUS(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	int ch = (s8)data[2]; /* Wheel changed  (-1:CCW, 0:None, 1:CW) */
	int rv = data[6];     /* Right wheel value */
	int lv = data[7];     /* Left  wheel value */
	
	if      (lv && ch > 0) waltop_report_key(idev, BTN_Y);
	else if (lv && ch < 0) waltop_report_key(idev, BTN_Z);
	else if (rv && ch > 0) waltop_report_key(idev, BTN_BASE);
	else if (rv && ch < 0) waltop_report_key(idev, BTN_BASE2);
	
	wdata->fn_btn = WALTOP_FNBTN_ZOOM_VOL;
	wdata->sw_btn = 0;    
}

static inline void WH_0A04_SIRIUS(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	int ch = (s8)data[2]; /* Wheel changed  (-1:CCW, 0:None, 1:CW) */
	int rv = data[6];     /* Right wheel value */
	int lv = data[7];     /* Left  wheel value */
	
	if      (lv && ch > 0) waltop_report_key(idev, BTN_BASE);
	else if (lv && ch < 0) waltop_report_key(idev, BTN_BASE2);
	else if (rv && ch > 0) waltop_report_key(idev, BTN_Y);
	else if (rv && ch < 0) waltop_report_key(idev, BTN_Z);
	
	wdata->fn_btn = WALTOP_FNBTN_ZOOM_VOL;
	wdata->sw_btn = 1;
}

static inline void WH_0A0D_SIRIUS(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	int data23 = get_unaligned_le16(data + 2);
	
	input_report_key(idev, BTN_0, data23 & 0x0001); /* Left  Eraser */
	input_report_key(idev, BTN_1, data23 & 0x0002); /* Left  Shift  */
	input_report_key(idev, BTN_2, data23 & 0x0004); /* Left  Tab    */
	input_report_key(idev, BTN_3, data23 & 0x0008); /* Left  Alt    */
	input_report_key(idev, BTN_4, data23 & 0x0010); /* Left  Ctrl   */
	input_report_key(idev, BTN_5, data23 & 0x0020); /* Right Eraser */
	input_report_key(idev, BTN_6, data23 & 0x0040); /* Right Shift  */
	input_report_key(idev, BTN_7, data23 & 0x0080); /* Right Tab    */
	input_report_key(idev, BTN_8, data23 & 0x0100); /* Right Alt    */
	input_report_key(idev, BTN_9, data23 & 0x0200); /* Right Ctrl   */
	input_sync(idev);

	waltop_set_button_state(wdata, data23 & 0x0400); /* Vert/Horiz Button  */ 
	waltop_set_button_state(wdata, data23 & 0x0800); /* Zoom/Volume Button */
	waltop_set_button_state(wdata, data23 & 0x1000); /* Keyboard Button    */
	waltop_set_button_state(wdata, data23 & 0x2000); /* Func Switch Button */
}

static inline void WH_0A_SIRIUS(__u8 *data, struct waltop_data *wdata)
{   
	switch (data[1]) {        
		case 0x02: WH_0A02_SIRIUS(data, wdata); break;
		case 0x03: WH_0A03_SIRIUS(data, wdata); break;
		case 0x04: WH_0A04_SIRIUS(data, wdata); break;
		case 0x0E: WH_0A0D_SIRIUS(data, wdata); break;
	}
}

static inline void WH_0D_SIRIUS(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	int sw;
	
	switch (data[3]) {
		case 0x4F: waltop_report_key(idev, BTN_BASE3);	sw = 0; break;
		case 0x50: waltop_report_key(idev, BTN_BASE4);	sw = 0; break;
		case 0x51: waltop_report_key(idev, BTN_BASE5);	sw = 0; break;
		case 0x52: waltop_report_key(idev, BTN_BASE6);	sw = 0; break;
		case 0x4A: waltop_report_key(idev, BTN_TL);		sw = 1; break;
		case 0x4B: waltop_report_key(idev, BTN_TR);		sw = 1; break;
		case 0x4D: waltop_report_key(idev, BTN_TL2);	sw = 1; break;
		case 0x4E: waltop_report_key(idev, BTN_TR2);	sw = 1; break;       
		default:   return;
	}
	
	wdata->fn_btn = WALTOP_FNBTN_KEYBOARD;
	wdata->sw_btn = sw;
}

void WH_SIR(__u8 *data, struct waltop_data *wdata)
{  
	switch (data[0]) {
		case 0x01: WH_MOUSE(data, wdata); return;
		case 0x02: WH_STYLUS_TILT(data, wdata); return;
		case 0x05: WH_MACRO(data, wdata); return;
		case 0x0A: WH_0A_SIRIUS(data, wdata); return;
		case 0x0D: WH_0D_SIRIUS(data, wdata); return;
	}
}

/*********************************************************************************
 * Waltop Venus Tablet
 *********************************************************************************/
void WH_VNS(__u8 *data, struct waltop_data *wdata)
{  
	switch (data[0]) {
		case 0x01: WH_MOUSE(data, wdata); return;
		case 0x02: WH_STYLUS_TILT(data, wdata); return;
	}
}

/*********************************************************************************
 * Waltop Vega Tablet
 *********************************************************************************/
static inline void WH_0A02_VEGA_WHEEL(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_CURSOR].idev;
	int ch = (s8)data[2];	/* Wheel changed  (-1:CCW, 0:None, 1:CW) */
	int w  = data[3];		/* Wheel (0: Wheel1, 1: Wheel2) */
//	__u8 v = data[6];		/* Wheel value (data[6] == data[7]) */
	__u8 v = data[7];		/* Wheel value (data[6] == data[7]) */
	__u8 d = v - wdata->wheel;
	
	if		(w == 0 && ch > 0) waltop_report_rel(idev, REL_WHEEL, 	 d);
	else if (w == 0 && ch < 0) waltop_report_rel(idev, REL_WHEEL,	-d);
	else if (w == 1 && ch > 0) waltop_report_rel(idev, REL_HWHEEL,	 d);
	else if (w == 1 && ch < 0) waltop_report_rel(idev, REL_HWHEEL,	-d);
	
	wdata->wheel = v;
}

static inline void WH_0A02_VEGA(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	int ch = (s8)data[2];	/* Wheel changed  (-1:CCW, 0:None, 1:CW) */
	int w  = data[3];		/* Wheel (0: Wheel1, 1: Wheel2) */
	
	if		(w == 0 && ch > 0) waltop_report_key(idev, BTN_C);
	else if (w == 0 && ch < 0) waltop_report_key(idev, BTN_X);
	else if (w == 1 && ch > 0) waltop_report_key(idev, BTN_Y);
	else if (w == 1 && ch < 0) waltop_report_key(idev, BTN_Z);
}

static inline void WH_0A03_VEGA(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	int ch = (s8)data[2];	/* Wheel changed  (-1:CCW, 0:None, 1:CW) */
	int w  = data[3];		/* Wheel (0: Wheel1, 1: Wheel2) */
	
	if		(w == 0 && ch > 0) waltop_report_key(idev, BTN_8);
	else if (w == 0 && ch < 0) waltop_report_key(idev, BTN_9);
	else if (w == 1 && ch > 0) waltop_report_key(idev, BTN_A);
	else if (w == 1 && ch < 0) waltop_report_key(idev, BTN_B);
}

static inline void WH_0A04_VEGA(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	int ch = (s8)data[2];	/* Wheel changed  (-1:CCW, 0:None, 1:CW) */
	int w  = data[3];		/* Wheel (0: Wheel1, 1: Wheel2) */
	
	if		(w == 0 && ch > 0) waltop_report_key(idev, BTN_4);
	else if (w == 0 && ch < 0) waltop_report_key(idev, BTN_5);
	else if (w == 1 && ch > 0) waltop_report_key(idev, BTN_6);
	else if (w == 1 && ch < 0) waltop_report_key(idev, BTN_7);
}

static inline void WH_0A0E_VEGA(__u8 *data, struct waltop_data *wdata)
{
	struct input_dev *idev = wdata->widevs[WALTOP_PAD].idev;
	
	input_report_key(idev, BTN_0, data[2] & 0x0001);
	input_report_key(idev, BTN_1, data[2] & 0x0002);
	input_report_key(idev, BTN_2, data[2] & 0x0004);
	input_report_key(idev, BTN_3, data[2] & 0x0008);
	input_sync(idev);
}

static inline void WH_0A0F_VEGA(__u8 *data, struct waltop_data *wdata)
{
	switch (data[2]) {
		case 0x01: break;	/* Orange -> White  */
		case 0x02: break;	/* White  -> Green  */
		case 0x03: break;	/* Green  -> Orange */
	}
}

static inline void WH_0A_VEGA(__u8 *data, struct waltop_data *wdata)
{
	switch (data[1]) {        
		case 0x02: WH_0A02_VEGA(data, wdata); return;	/* Navigation wheel function changed (White)  */
		case 0x03: WH_0A03_VEGA(data, wdata); return;	/* Navigation wheel function changed (Green)  */
		case 0x04: WH_0A04_VEGA(data, wdata); return;	/* Navigation wheel function changed (Orange) */
		case 0x0E: WH_0A0E_VEGA(data, wdata); return;	/* Express key has been pressed or released   */
		case 0x0F: WH_0A0F_VEGA(data, wdata); return;	/* Navigation wheel button has been released  */
	}
}

void WH_VEG(__u8 *data, struct waltop_data *wdata)
{  
	switch (data[0]) {
		case 0x01: WH_MOUSE(data, wdata); return;
		case 0x02: WH_STYLUS_TILT(data, wdata); return;
		case 0x0A: WH_0A_VEGA(data, wdata); return;
	}
}