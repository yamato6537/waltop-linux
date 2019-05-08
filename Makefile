KVERSION = $(shell uname -r)
obj-m := waltop.o
waltop-objs := waltop-cmd.o waltop-dev.o waltop-mod.o
CFLAGS_waltop-cmd.o := -O2
CFLAGS_waltop-dev.o := -O2
CFLAGS_waltop-mod.o := -O2
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
