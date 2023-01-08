# Waltop tablet driver


## About

This is a Linux kernel driver for USB tablets based on Waltop chipsets. This driver supports the proprietary mode.

## Supported tablets

| PID:VID |Model name              |Supported features                 |
|:-------:|:-----------------------|:----------------------------------|
|172F:0022|?                       |                                   |
|172F:0024|?                       |                                   |
|172F:0025|?                       |                                   |
|172F:0026|?                       |                                   |
|172F:0027|Waltop My Note T01S     |                                   |
|172F:0028|?                       |                                   |
|172F:0030|?                       |                                   |
|172F:0031|Waltop Slim Tablet 12.1 |                                   |
|172F:0032|Waltop Slim Tablet 5.8  |                                   |
|172F:0033|Waltop My Note R01      |                                   |
|172F:0034|Waltop Slim Tablet 12.1 |                                   |
|172F:0035|Waltop Whiteboard       |                                   |
|172F:0036|?                       |                                   |
|172F:0037|Waltop Q Pad            |                                   |
|172F:0038|?                       |                                   |
|172F:0039|?                       |                                   |
|172F:0046|Waltop Mars BR Tablet   |4000LPI, 2048PL                    |
|172F:0047|Waltop Mars A Tablet    |4000LPI, 2048PL                    |
|172F:0051|?                       |                                   |
|172F:0052|Yiynova MSP19           |                                   |
|172F:0053|?                       |                                   |
|172F:0054|?                       |                                   |
|172F:0055|?                       |                                   |
|172F:0056|?                       |                                   |
|172F:0058|?                       |                                   |
|172F:0500|Waltop Media Tablet 14.1|4000LPI (untested)                 |
|172F:0501|Waltop Media Tablet 10.6|4000LPI (untested)                 |
|172F:0502|Waltop Sirius Tablet    |4000LPI, 1024PL, TS, EK, touch pads|
|172F:0503|Waltop Venus S Tablet   |4000LPI, 2048PL, TS  (untested)    |
|172F:0509|Waltop Vega Tablet      |5080LPI, 2048PL, TS, EK, wheels    |

|ABBR|Meaning         |
|:---|:---------------|
|LPI |lines per inch  |
|PL  |pressure level  |
|TS  |tilt sensitivity|
|EK  |express keys    |

## Build and Install

### Dependencies

- gcc
- GNU make
- Linux kernel headers
- xf86-input-wacom
- python3

### Build

    $ git clone https://github.com/lambda2111/waltop-linux.git
    $ cd waltop-linux
    $ make
    $ sudo cp 60-waltop.conf /usr/share/X11/xorg.conf.d/

After that, reboot your PC.

### Install

    $ cd waltop-linux
    $ sudo python3 unbind.py
    $ sudo insmod waltop.ko

You need to type these commands every boot time.

### Note

You need to rebuild the driver after a Linux kernel update.

### Rebuild:

    $ cd waltop-linux
    $ make clean
    $ make

## Contributors

- Fabio Dell'Aria (installer/uninstaller for Ubuntu)
- floric (Waltop Mars A Tablet)
- gengen (Waltop Mars BR Tablet)
- inamura prototype D (Waltop Vega Tablet)

Thank you for your contribution!

## Special Thanks

- [DIGImend](https://digimend.github.io/)

## License

GPL (or later)
