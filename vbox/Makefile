LINUX_AGENT_DIR=linux-source-6.8.0/

linuxBuild:
	#$(MAKE) -C $(LINUX_AGENT_DIR) defconfig
	#cp /boot/config-6.8.0-71-generic $(LINUX_AGENT_DIR)/.config

	#cd $(LINUX_AGENT_DIR) && ./scripts/config --disable MODULE_SIG
	#cd $(LINUX_AGENT_DIR) && ./scripts/config --enable DEBUG_INFO_DWARF5
	#cd $(LINUX_AGENT_DIR) && ./scripts/config --enable GDB_SCRIPTS
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable KASAN
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable KASAN_INLINE
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable SYSTEM_TRUSTED_KEYS
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable SYSTEM_REVOCATION_KEYS 
	# cd $(LINUX_AGENT_DIR) && ./scripts/config --enable IP_VS_PROTO_SCTP
	# cd $(LINUX_AGENT_DIR) && ./scripts/config --enable IP_SCTP

	#cd $(LINUX_AGENT_DIR) && ./scripts/config --enable FUZZ_KAFL

	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable USB_XHCI_HCD
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable USB_PCI
	
	#cd $(LINUX_AGENT_DIR) && ./scripts/config --enable VIRTIO_CONSOLE
	#cd $(LINUX_AGENT_DIR) && ./scripts/config --enable SERIAL_8250
	
	cd $(LINUX_AGENT_DIR) &&  make olddefconfig && make -j`nproc`
	cp $(LINUX_AGENT_DIR)/arch/x86/boot/bzImage .

linuxClean:
	cd $(LINUX_AGENT_DIR) && make mrproper
