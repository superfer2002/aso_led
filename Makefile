TARGET_MODULE:=fase1
ARCH=arm

BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

obj-m += $(TARGET_MODULE).o

all:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean
install:
	insmod ./$(TARGET_MODULE).ko
uninstall:
	rmmod ./$(TARGET_MODULE).ko
