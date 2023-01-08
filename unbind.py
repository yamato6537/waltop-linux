import os

USBHID_DIR = '/sys/bus/usb/drivers/usbhid/'

WALTOP_IDS = [
    [0x172F, 0x0022],
    [0x172F, 0x0024],
    [0x172F, 0x0025],
    [0x172F, 0x0026],
    [0x172F, 0x0027],
    [0x172F, 0x0028], 
    [0x172F, 0x0030],
    [0x172F, 0x0031],
    [0x172F, 0x0032],
    [0x172F, 0x0033],
    [0x172F, 0x0034],
    [0x172F, 0x0035],
    [0x172F, 0x0036],
    [0x172F, 0x0037],
    [0x172F, 0x0038],
    [0x172F, 0x0039],
    [0x172F, 0x0046],
    [0x172F, 0x0047],
    [0x172F, 0x0051],
    [0x172F, 0x0052],
    [0x172F, 0x0053],
    [0x172F, 0x0054],
    [0x172F, 0x0055],
    [0x172F, 0x0056],
    [0x172F, 0x0058],
    [0x172F, 0x0500],
    [0x172F, 0x0501],
    [0x172F, 0x0502],
    [0x172F, 0x0503],
    [0x172F, 0x0509],
]

def main():
    hids = list_dirs(USBHID_DIR)
    hids.remove('module')
    uevs = [get_uevent(os.path.join(USBHID_DIR, hid, 'uevent')) for hid in hids]
    dids = [get_device_id(uev['PRODUCT']) for uev in uevs]
    for hid, did in zip(hids, dids):
        if did in WALTOP_IDS:
            print('{0} ({1:04X}:{2:04X}) : unbound from usbhid'.format(hid, did[0], did[1]))
            unbind(hid)

def unbind(hid):
    path = os.path.join(USBHID_DIR, 'unbind')
    with open(path, mode='w', encoding='ascii') as fout:
        fout.write(hid)

def get_device_id(product):
    return [int(x, base=16) for x in product.split('/')[0:2]]

def get_uevent(uevent):
    with open(uevent, mode='r', encoding='ascii') as fin:
        doc = fin.read()
    return dict([line.split('=') for line in doc.splitlines()])

def list_dirs(path):
    return [x for x in os.listdir(path) if os.path.isdir(os.path.join(path, x))]

if __name__ == '__main__':
    main()
