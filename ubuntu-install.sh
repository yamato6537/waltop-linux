#!/bin/sh
# Waltop Tablet Drivers Ubuntu Installer 0.1
# By Fabio Dell'Aria - fabio.dellaria@gmail.com - July 2015

# Check if the current user is "root" otherwise restart the script with "sudo"...
[ "$(whoami)" != "root" ] && exec sudo -- "$0" "$@"

echo "Waltop Tablet Drivers Ubuntu Installer 0.1"
echo "------------------------------------------"
echo

# Define Global Variables...
LOADING_FILE="/lib/modules/waltop-load.sh"
BOOT_FILE="/etc/rc.local"

echo "Installation of the required packages..."
apt install build-essential linux-headers-$(uname -r)
echo "Done"
echo

echo "Driver Building..."
make
echo "Done"
echo

echo "Driver Installation..."
# Copy configuration file...
cp 60-waltop.conf /usr/share/X11/xorg.conf.d/

# Copy module and python related files...
cp waltop.ko /lib/modules/
cp unbind.py /lib/modules/

# Create Driver-Loading file...
rm -f $LOADING_FILE
echo "#!/bin/sh" >> $LOADING_FILE
echo "cd /lib/modules/" >> $LOADING_FILE
echo "python3 unbind.py" >> $LOADING_FILE
echo "insmod waltop.ko" >> $LOADING_FILE
chmod +x $LOADING_FILE

# Add /etc/rc.local Loading line...
grep -q -F $LOADING_FILE $BOOT_FILE || sed -i -e '$i \'$LOADING_FILE $BOOT_FILE
echo "Done"
echo
echo "Driver Loading..."
$LOADING_FILE
echo "Done"
