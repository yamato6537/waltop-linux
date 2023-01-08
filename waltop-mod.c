/*
 * waltop-mod.c
 *
 *  Waltop tablet driver for linux - module's entry point
 * 
 *  This driver supports "proprietary mode" of Waltop tablet
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/kernel.h>       /* pr_xxx() */
#include <linux/slab.h>         /* kxalloc(), kfree() */
#include <linux/module.h>       /* MODULE_XXX(), THIS_MODULE */
#include <linux/usb/input.h>
#include "waltop.h"

#define DRIVER_NAME     "waltop"
#define DRIVER_VERSION  "0.0.7"
#define DRIVER_AUTHOR   "lambda2111@gmail.com"
#define DRIVER_DESC     "WALTOP Proprietary Mode Tablet Driver"
#define DRIVER_LICENSE  "GPL"

#define WALTOP_INFO(fmt, ...) pr_info("%s: " fmt, DriverName, ##__VA_ARGS__)
#define WALTOP_ERR(fmt, ...) pr_err("%s: in %s(), " fmt, DriverName, __func__, ##__VA_ARGS__)

static char DriverName[] = DRIVER_NAME;

extern struct waltop_icaps WC_STD[];
extern struct waltop_icaps WC_MYN[];
extern struct waltop_icaps WC_SIR[];
extern struct waltop_icaps WC_MAR[];
extern struct waltop_icaps WC_VNS[];
extern struct waltop_icaps WC_VEG[];
extern void WH_STD(__u8 *data, struct waltop_data *wdata);
extern void WH_MY1(__u8 *data, struct waltop_data *wdata);
extern void WH_MY2(__u8 *data, struct waltop_data *wdata);
extern void WH_SIR(__u8 *data, struct waltop_data *wdata);
extern void WH_MAR(__u8 *data, struct waltop_data *wdata);
extern void WH_VNS(__u8 *data, struct waltop_data *wdata);
extern void WH_VEG(__u8 *data, struct waltop_data *wdata);
extern int waltop_set_default_mode(struct usb_interface *intf);
extern int waltop_set_tablet_mode (struct usb_interface *intf);
extern int waltop_set_ink_mode    (struct usb_interface *intf);
extern int waltop_set_macro_keys  (struct usb_interface *intf, __u8 x);
extern int waltop_set_autogain_on (struct usb_interface *intf);
extern int waltop_set_filter_on   (struct usb_interface *intf);
extern int waltop_set_resolution  (struct usb_interface *intf);
extern int waltop_get_max_x       (struct usb_interface *intf);
extern int waltop_get_max_y       (struct usb_interface *intf);
extern int waltop_get_model_code  (struct usb_interface *intf);
extern int waltop_get_odm_code    (struct usb_interface *intf);
extern int waltop_get_firmware_ver(struct usb_interface *intf);
extern int waltop_get_max_pressure(struct usb_interface *intf);

/*********************************************************************************
 * Driver Info
 *********************************************************************************/
struct waltop_driver_info
{
    char *name;
    int min_tx;
    int max_tx;
    int min_ty;
    int max_ty;
    struct waltop_icaps *wicaps;
    waltop_report_handler handler;
};

static struct waltop_driver_info WDI_0x0022 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0024 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0025 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0026 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0027 = {"Waltop My Note T01S",            0,  0,   0,  0,    WC_MYN, WH_MY1};
static struct waltop_driver_info WDI_0x0028 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0030 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0031 = {"Waltop Slim Tablet 12.1",        0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0032 = {"Waltop Slim Tablet 5.8",         0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0033 = {"Waltop My Note R01",             0,  0,   0,  0,    WC_MYN, WH_MY2};
static struct waltop_driver_info WDI_0x0034 = {"Waltop Slim Tablet 12.1",        0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0035 = {"Waltop Whiteboard",              0,  0,   0,  0,    WC_MYN, WH_MY2};
static struct waltop_driver_info WDI_0x0036 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0037 = {"Waltop Q Pad",                   0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0038 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0039 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0046 = {"Waltop Mars BR Tablet",          0,  0,   0,  0,    WC_MAR, WH_MAR};
static struct waltop_driver_info WDI_0x0047 = {"Waltop Mars A Tablet",           0,  0,   0,  0,    WC_MAR, WH_MAR};
static struct waltop_driver_info WDI_0x0051 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0052 = {"Waltop Tablet (Yiynova MSP19)",  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0053 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0054 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0055 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0056 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0058 = {"Waltop Tablet",                  0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0500 = {"Waltop Media Tablet 14.1",       0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0501 = {"Waltop Media Tablet 10.6",       0,  0,   0,  0,    WC_STD, WH_STD};
static struct waltop_driver_info WDI_0x0502 = {"Waltop Sirius Tablet",         -70, 70, -70, 70,    WC_SIR, WH_SIR};
static struct waltop_driver_info WDI_0x0503 = {"Waltop Venus S Tablet",        -70, 70, -70, 70,    WC_VNS, WH_VNS};
static struct waltop_driver_info WDI_0x0509 = {"Waltop Vega Tablet",           -60, 60, -60, 60,    WC_VEG, WH_VEG};

/*********************************************************************************
 * USB Device Table
 *********************************************************************************/
#define WALTOP_DEVICE(pid) \
    {USB_DEVICE(0x172F, pid), .driver_info = (kernel_ulong_t)&WDI_##pid}

static const struct usb_device_id waltop_devices[] =
{
    WALTOP_DEVICE(0x0022),
    WALTOP_DEVICE(0x0024),
    WALTOP_DEVICE(0x0025),
    WALTOP_DEVICE(0x0026),
    WALTOP_DEVICE(0x0027),
    WALTOP_DEVICE(0x0028), 
    WALTOP_DEVICE(0x0030),
    WALTOP_DEVICE(0x0031),
    WALTOP_DEVICE(0x0032),
    WALTOP_DEVICE(0x0033),
    WALTOP_DEVICE(0x0034),
    WALTOP_DEVICE(0x0035),
    WALTOP_DEVICE(0x0036),
    WALTOP_DEVICE(0x0037),
    WALTOP_DEVICE(0x0038),
    WALTOP_DEVICE(0x0039),
    WALTOP_DEVICE(0x0046),
    WALTOP_DEVICE(0x0047),
    WALTOP_DEVICE(0x0051),
    WALTOP_DEVICE(0x0052),
    WALTOP_DEVICE(0x0053),
    WALTOP_DEVICE(0x0054),
    WALTOP_DEVICE(0x0055),
    WALTOP_DEVICE(0x0056),
    WALTOP_DEVICE(0x0058),
    WALTOP_DEVICE(0x0500),
    WALTOP_DEVICE(0x0501),
    WALTOP_DEVICE(0x0502),
    WALTOP_DEVICE(0x0503),
    WALTOP_DEVICE(0x0509),
    { }
};

/*********************************************************************************
 * complete() for urb
 *********************************************************************************/
static void waltop_irq(struct urb *urb)
{
    switch (urb->status)
    {
        case 0:
        {
            struct waltop_data *wdata = urb->context;
            wdata->handler(urb->transfer_buffer, wdata);
            goto submit;
        }
        case -ECONNRESET:
        case -ENOENT:
        case -ESHUTDOWN:
            WALTOP_INFO("urb shutting down with %d.\n", urb->status);
            return;
        default:
            WALTOP_INFO("urb status(%d) received.\n", urb->status);
            goto submit;
    }

submit:
    {
        int ret = usb_submit_urb(urb, GFP_ATOMIC);
        if (ret)
            WALTOP_INFO("usb_submit_urb() failed with %d.\n", ret);
    }
}

/*********************************************************************************
 * open() and close() for input device
 *********************************************************************************/
static int waltop_input_open(struct input_dev *idev)
{
    struct waltop_data *wdata = input_get_drvdata(idev); 
    int ret;

    if (idev == wdata->widevs[0].idev) {
        ret = usb_submit_urb(wdata->urb, GFP_KERNEL);
        if (ret) {
            WALTOP_ERR("usb_submit_urb() failed with %d.\n", ret);
            return -EIO;
        }
    }

    return 0;
}

static void waltop_input_close(struct input_dev *idev)
{
    struct waltop_data *wdata = input_get_drvdata(idev); 

    if (idev == wdata->widevs[0].idev)    
        usb_kill_urb(wdata->urb);
}

/*********************************************************************************
 * Input Device Setting Funcitons
 *********************************************************************************/
void waltop_set_idev_abs(struct input_dev *idev, int abs, int min, int max)
{
    set_bit(abs, idev->absbit);
    input_set_abs_params(idev, abs, min, max, 0, 0);
}

void waltop_set_idev_bit(int *icaps, unsigned long *input_bit)
{
    int i;

    for (i = 0; icaps[i] != -1; i++)
        set_bit(icaps[i], input_bit);
}

static int waltop_setup_icaps(  size_t index,
                                struct input_dev *idev,
                                struct waltop_info *winfo,
                                struct waltop_icaps *wicaps)
{
    if (wicaps->evts == NULL)
        return 0;
    else
        waltop_set_idev_bit(wicaps->evts, idev->evbit);

    if (wicaps->btns != NULL)
        waltop_set_idev_bit(wicaps->btns, idev->keybit);

    if (wicaps->rels != NULL)
        waltop_set_idev_bit(wicaps->rels, idev->relbit);

    if (wicaps->mscs != NULL)
        waltop_set_idev_bit(wicaps->mscs, idev->mscbit);     
        
    if (wicaps->abss != NULL) {
        int i, abs, min, max;
        for (i = 0; (abs = wicaps->abss[i]) != -1; i++) {
            if      (abs == ABS_X && index == WALTOP_STYLUS) min = 0, max = winfo->max_x;
            else if (abs == ABS_Y && index == WALTOP_STYLUS) min = 0, max = winfo->max_y;
            else if (abs == ABS_X && index == WALTOP_PAD) min = 0, max = 1;
            else if (abs == ABS_Y && index == WALTOP_PAD) min = 0, max = 1;
            else if (abs == ABS_PRESSURE && index == WALTOP_STYLUS) min = 0, max = winfo->max_pressure;
            else if (abs == ABS_TILT_X && index == WALTOP_STYLUS) min = winfo->min_tx, max = winfo->max_tx;
            else if (abs == ABS_TILT_Y && index == WALTOP_STYLUS) min = winfo->min_ty, max = winfo->max_ty;
            else min = 0, max = 0;
            waltop_set_idev_abs(idev, abs, min, max);
        }
    }

    return 0;
}

void waltop_clean_idev(struct waltop_idev *widevs, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (widevs[i].idev != NULL) {
            if (widevs[i].res)
                input_free_device(widevs[i].idev);
            else
                input_unregister_device(widevs[i].idev);
        }
    }
}

static int waltop_setup_idev(   struct waltop_data *wdata, 
                                struct waltop_info *winfo,
                                struct waltop_icaps *wicaps,
                                struct usb_interface *intf,
                                void *drvdata)
{
    struct usb_device *udev = interface_to_usbdev(intf);
    size_t len_wid = ARRAY_SIZE(wdata->widevs);
    size_t len_phy = sizeof(wdata->phys); 
    size_t i;
    int res;
    char *types[WALTOP_IDEV_LEN] = {"Cursor", "Pen", "Pad", "Macro"};

    usb_make_path(udev, wdata->phys, len_phy);
    snprintf(wdata->phys, len_phy, "%s/input0", wdata->phys);

    for (i = 0; i < len_wid; i++)
        wdata->widevs[i].idev = NULL;

    for (i = 0; i < len_wid; i++) {
        struct waltop_idev *wid = &wdata->widevs[i];
        struct waltop_icaps *wic = &wicaps[i];
        
        if (wic->evts == NULL)
            continue;
        
        wid->idev = input_allocate_device();
        if (wid->idev == NULL) {
            WALTOP_ERR("input_allocate_device() failed.\n");
            res = -ENOMEM;
            goto failed;
        }
        else {
            snprintf(wid->name, sizeof(wid->name), "%s %s", winfo->name, types[i]);
            wid->idev->name = wid->name;
            wid->idev->phys = wdata->phys;
            wid->idev->open = waltop_input_open;
            wid->idev->close = waltop_input_close;
            wid->idev->dev.parent = &intf->dev;
            usb_to_input_id(udev, &wid->idev->id); 
            input_set_drvdata(wid->idev, drvdata);
            waltop_setup_icaps(i, wid->idev, winfo, wic);
            res = wid->res = input_register_device(wid->idev);    
            if(wid->res) {
                WALTOP_ERR("input_register_device() failed.\n");
                goto failed;
            }
        }
    }

    return 0;

failed:
    waltop_clean_idev(wdata->widevs, len_wid);
    return res;
}

/*********************************************************************************
 *  Getting device information
 *********************************************************************************/
static int waltop_get_device_info(struct waltop_info *winfo, struct usb_interface *intf)
{
    winfo->max_x = waltop_get_max_x(intf);
    if (winfo->max_x < 0) {
        WALTOP_ERR("waltop_get_max_x() failed.\n");
        return -EIO;
    }

    winfo->max_y = waltop_get_max_y(intf);
    if (winfo->max_y < 0) {
        WALTOP_ERR("waltop_get_max_y() failed.\n");
        return -EIO;
    }

    winfo->model_code = waltop_get_model_code(intf);
    if (winfo->model_code < 0) {
        WALTOP_ERR("waltop_get_model_code() failed.\n");
        return -EIO;
    }

    winfo->odm_code = waltop_get_odm_code(intf);
    if (winfo->odm_code < 0) {
        WALTOP_ERR("waltop_get_odm_code() failed.\n");
        return -EIO;
    }

    winfo->firmware_ver = waltop_get_firmware_ver(intf);
    if (winfo->firmware_ver < 0) {
        WALTOP_ERR("waltop_get_firmware_ver() failed.\n");
        return -EIO;
    }

    winfo->max_pressure = waltop_get_max_pressure(intf);
    if (winfo->max_pressure < 0) {
        WALTOP_ERR("waltop_get_pressure_levels() failed.\n");
        return -EIO;
    }
        
    return 0;
}

/*********************************************************************************
 *  probe() and disconnect() for USB driver
 *********************************************************************************/
static int waltop_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(intf);
    struct usb_endpoint_descriptor *epd = &intf->cur_altsetting->endpoint[0].desc;
    unsigned int pipe = usb_rcvintpipe(udev, epd->bEndpointAddress);    
    struct waltop_driver_info *drvinfo = (struct waltop_driver_info *)id->driver_info;
    struct waltop_info winfo;
    struct waltop_data *wdata;
    unsigned char *data;
    dma_addr_t data_dma;
    int ret;

    WALTOP_INFO("probe\n");
    WALTOP_INFO("TabletName=\"%s\"\n", drvinfo->name);

    /******************** device init ********************/
    ret = waltop_set_tablet_mode(intf);
    if (ret) {
        WALTOP_ERR("waltop_set_tablet_mode() failed.\n");
        goto failed0;
    } 
    ret = waltop_set_macro_keys(intf, 0x0F);
    if (ret) {
        WALTOP_ERR("waltop_set_macro_keys() failed.\n");
        goto failed0;
    }

    /******************** device info ********************/
    winfo.name = drvinfo->name;
    ret = waltop_get_device_info(&winfo, intf);
    if (ret) {
        WALTOP_ERR("waltop_get_device_info() failed.\n");
        goto failed0;
    }
    WALTOP_INFO("ModleCode=%d, ODMCode=%d, FirmwareVer=%d\n",
                winfo.model_code, winfo.odm_code, winfo.firmware_ver);
    WALTOP_INFO("MaxX=%d, MaxY=%d, MaxPressure=%d\n",
                winfo.max_x, winfo.max_y, winfo.max_pressure);
    winfo.min_tx = drvinfo->min_tx;
    winfo.max_tx = drvinfo->max_tx;
    winfo.min_ty = drvinfo->min_ty;
    winfo.max_ty = drvinfo->max_ty;
    WALTOP_INFO("TiltX=%d, %d, TiltY=%d, %d",
                winfo.min_tx, winfo.max_tx, winfo.min_ty, winfo.max_ty);

    /******************** waltop data ********************/
    ret = -ENOMEM;
    wdata = kmalloc(sizeof(struct waltop_data), GFP_KERNEL);
    if (wdata == NULL) {
        WALTOP_ERR("kmalloc() failed.\n");
        goto failed0;
    }
    wdata->in_range = 0;
    wdata->fn_btn = WALTOP_FNBTN_SCROLL;
    wdata->sw_btn = 0;
    wdata->wheel = 0;
    wdata->handler = drvinfo->handler;

    /******************** interrupt urb ********************/
    data = usb_alloc_coherent(udev, epd->wMaxPacketSize, GFP_KERNEL, &data_dma);
    if (data == NULL) {
        WALTOP_ERR("usb_alloc_coherent() failed.\n");
        goto failed1;
    }
    wdata->urb = usb_alloc_urb(0, GFP_KERNEL);
    if (wdata->urb == NULL) {
        WALTOP_ERR("usb_alloc_urb() failed.\n");
        goto failed2;
    }
    usb_fill_int_urb(wdata->urb, udev, pipe, data,
                        epd->wMaxPacketSize, waltop_irq, wdata, epd->bInterval);
    wdata->urb->transfer_dma = data_dma;
    wdata->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    /******************** input device ********************/
    ret = waltop_setup_idev(wdata, &winfo, drvinfo->wicaps, intf, wdata);
    if (ret) {
        WALTOP_ERR("drvinfo->setupper() failed.\n");
        goto failed3;
    }

    usb_set_intfdata(intf, wdata);  

    return 0;

failed3: usb_free_urb(wdata->urb);
failed2: usb_free_coherent(udev, epd->wMaxPacketSize, data, data_dma);
failed1: kfree(wdata);
failed0: usb_set_intfdata(intf, NULL);

    return ret;
}

static void waltop_disconnect(struct usb_interface *intf)
{
    struct waltop_data *wdata = usb_get_intfdata(intf);

    if (wdata != NULL)
    {
        usb_kill_urb(wdata->urb);
        waltop_clean_idev(wdata->widevs, ARRAY_SIZE(wdata->widevs));
        {
            struct urb *urb = wdata->urb;
            struct usb_device *udev = urb->dev;
            __u8 *data = urb->transfer_buffer;
            u32 data_len = urb->transfer_buffer_length;
            dma_addr_t data_dma = urb->transfer_dma;

            usb_free_urb(urb);
            usb_free_coherent(udev, data_len, data, data_dma);
        }
        kfree(wdata);
        usb_set_intfdata(intf, NULL);
    }

    WALTOP_INFO("disconnect\n");
}

/*********************************************************************************
 * init() and exit() for module
 *********************************************************************************/
static struct usb_driver waltop_driver =
{
    .name = DriverName,
    .id_table = waltop_devices,
    .probe = waltop_probe,
    .disconnect = waltop_disconnect,
};

static int __init waltop_init(void)
{
    int ret;

    WALTOP_INFO("init\n");
    WALTOP_INFO("Version=%s\n", DRIVER_VERSION);

    ret = usb_register(&waltop_driver);
    if (ret) {
        WALTOP_ERR("usb_register() failed with %d.\n", ret);
        return ret;
    }

    return 0;
}

static void __exit waltop_exit(void)
{
    usb_deregister(&waltop_driver);

    WALTOP_INFO("exit\n");    
}

/*********************************************************************************
 * module declarations
 *********************************************************************************/
module_init(waltop_init);
module_exit(waltop_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE(DRIVER_LICENSE);
MODULE_DEVICE_TABLE(usb, waltop_devices);
