#!/bin/sh
# Waltop Tablet Drivers Ubuntu Uninstaller 0.1
# By Fabio Dell'Aria - fabio.dellaria@gmail.com - July 2015

# Check if the current user is "root" otherwise restart the script with "sudo"...
[ "$(whoami)" != "root" ] && exec sudo -- "$0" "$@"

echo "Waltop Tablet Drivers Ubuntu Uninstaller 0.1"
echo "------------------------------------------"
echo

echo "Clear all compiled files..."
make clean
echo "Done"
echo

echo "Driver Uninstallation..."
# Remove configuration file...
rm -f /usr/share/X11/xorg.conf.d/60-waltop.conf

# Remove module and python related file...
rm -f /lib/modules/waltop.ko
rm -f /lib/modules/unbind.py

# Remove Driver-Loading file...
rm -f /lib/modules/waltop-load.sh

# Remove /etc/rc.local Loading line...
sed -i '/waltop-load.sh/d' /etc/rc.local
echo "Done"
echo
echo "Driver Unloading..."
rmmod waltop.ko
echo "Done"
