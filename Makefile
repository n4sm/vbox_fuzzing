LINUX_AGENT_DIR=linux/

all: linux
	gcc src/kafl_agent.c -static -o fs/setsockopt_fuzz
	gcc src/sctp_server.c -static -lsctp -o fs/server
	gcc src/sctp_client.c -static -lsctp -o fs/client

	bash gen_initrd.sh out.cpio.gz 

linuxBuild:
	cp include/nyx_api.h linux/arch/x86/include/asm/
	$(MAKE) -C $(LINUX_AGENT_DIR) x86_64_defconfig

	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable MODULE_SIG
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable DEBUG_INFO_DWARF5
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable GDB_SCRIPTS
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable KASAN
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable KASAN_INLINE
	# cd $(LINUX_AGENT_DIR) && ./scripts/config --enable IP_VS_PROTO_SCTP
	# cd $(LINUX_AGENT_DIR) && ./scripts/config --enable IP_SCTP

	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable FUZZ_KAFL

	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable USB_XHCI_HCD
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable USB_PCI

	cd $(LINUX_AGENT_DIR) &&  make olddefconfig && make -j`nproc`
	cp $(LINUX_AGENT_DIR)/arch/x86/boot/bzImage .

test:
	cp include/nyx_api.h linux/arch/x86/include/asm/
	CONF_LINUX_HEADERS=../$(LINUX_AGENT_DIR) make -j`nproc` -C module_harness/
	cp module_harness/main.ko fs_test/ 
	CONF_LINUX_HEADERS=../$(LINUX_AGENT_DIR) make -C module_harness/ clean

	bash gen_initrd.sh out.cpio.gz 

linuxClean:
	cd $(LINUX_AGENT_DIR) && make mrproper