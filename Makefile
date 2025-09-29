LINUX_AGENT_DIR=linux-6.14.4/
ASSETS_DIR=assets/

all:
	bash gen_initrd.sh out.cpio.gz 

unpack_assets:
	tar xvf $(ASSETS_DIR)/linux-6.14.4.tar.xz

linuxBuild: unpack_assets
	cp src/nyx_api.h $(LINUX_AGENT_DIR)/arch/x86/include/asm/
	$(MAKE) -C $(LINUX_AGENT_DIR) x86_64_defconfig

	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable MODULE_SIG
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable DEBUG_INFO_DWARF5
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable GDB_SCRIPTS
	#cd $(LINUX_AGENT_DIR) && ./scripts/config --enable KASAN
	#cd $(LINUX_AGENT_DIR) && ./scripts/config --enable KASAN_INLINE
	# cd $(LINUX_AGENT_DIR) && ./scripts/config --enable IP_VS_PROTO_SCTP
	# cd $(LINUX_AGENT_DIR) && ./scripts/config --enable IP_SCTP

	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable FUZZ_KAFL

	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable USB_XHCI_HCD
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable USB_PCI
	
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable VIRTIO_CONSOLE
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable SERIAL_8250
	
	cd $(LINUX_AGENT_DIR) &&  make olddefconfig && make -j`nproc`
	cp $(LINUX_AGENT_DIR)/arch/x86/boot/bzImage .

run_harness: linuxBuild
	cp src/nyx_api.h $(LINUX_AGENT_DIR)/arch/x86/include/asm/
	CONF_LINUX_HEADERS=../../$(LINUX_AGENT_DIR) make -j`nproc` -C src/module_harness/
	cp src/module_harness/main.ko fs/ 
	CONF_LINUX_HEADERS=../../$(LINUX_AGENT_DIR) make -C src/module_harness/ clean

	sudo mkdir -p /tmp/qcow_mount
	sudo guestmount -a /home/nasm/fuzz/vbox_fuzzing/gen_L1/ubuntu-server.qcow2 -i /tmp/qcow_mount/

	bash tools/gen_initrd.sh out.cpio.gz
	bash tools/create_iso.sh

	sudo cp boot.iso /tmp/qcow_mount/home/nasm
	
	sudo guestunmount /tmp/qcow_mount
	sudo rmdir /tmp/qcow_mount

	cd build_kernel && sudo make kvm

clean:
	rm boot.iso bzImage out.cpio.gz 2>/dev/null

linuxClean:
	cd $(LINUX_AGENT_DIR) && make mrproper
