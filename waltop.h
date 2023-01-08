/*
 * waltop.h
 *
 *  Waltop tablet driver for linux
 * 
 *  This driver supports "proprietary mode" of Waltop tablet
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#pragma once

/*
 *  input_dev index
 */
#define WALTOP_CURSOR   0
#define WALTOP_STYLUS   1
#define WALTOP_PAD      2
#define WALTOP_MACRO    3
#define WALTOP_IDEV_LEN 4

/*
 *  Function button state for Sirius tablet
 */
#define WALTOP_FNBTN_SCROLL   1
#define WALTOP_FNBTN_ZOOM_VOL 2
#define WALTOP_FNBTN_KEYBOARD 3

struct waltop_data;
struct waltop_info;
struct waltop_idev;

typedef void (*waltop_report_handler)(__u8 *data, struct waltop_data *wdata);

struct waltop_icaps
{
    int *evts;
    int *btns;
    int *rels;
    int *abss;
    int *mscs;
};

struct waltop_info
{
    char *name;
    int max_x;
    int max_y;
    int min_tx;
    int max_tx;
    int min_ty;
    int max_ty;
    int model_code;
    int odm_code;
    int firmware_ver;
    int max_pressure;
};

struct waltop_idev
{
    char name[64];
    struct input_dev *idev;
    int res;
};

struct waltop_data
{
    char phys[256];
    struct urb *urb;
    struct waltop_idev widevs[WALTOP_IDEV_LEN];
    waltop_report_handler handler;
    int in_range;
    /* function button status (for Sirius) */
    int fn_btn;
    /* dial function switch button status (0: off, 1: on) (for Sirius) */
    int sw_btn;
    /* preveious wheel value (for Vega) */
    __u8 wheel;
};

