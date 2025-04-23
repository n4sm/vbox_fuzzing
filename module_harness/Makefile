ifndef CONF_LINUX_HEADERS
$(error CONF_LINUX_HEADERS undefined)
endif

export ARCH=x86_64

obj-m += main.o

all:
	make -C $(CONF_LINUX_HEADERS) M=$(shell realpath ./) modules
clean:
	rm -f *~
	make -C $(CONF_LINUX_HEADERS) M=$(shell realpath ./) clean || true

