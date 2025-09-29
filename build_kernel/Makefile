LINUX_AGENT_DIR=kafl.linux/

kvm:
	cd $(LINUX_AGENT_DIR) && make clean && make mrproper
	cp config  ./$(LINUX_AGENT_DIR)/.config
	# disable kernel signature
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable SYSTEM_TRUSTED_KEYS
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable SYSTEM_REVOCATION_KEYS
	# enable KVM
	cd $(LINUX_AGENT_DIR) && ./scripts/config --module KVM
	# disable KVM_WERROR (otherwise compilation issues in modified KVM)
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable KVM_WERROR
	# enable Nyx
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable KVM_NYX
	# disable KVM_HYPERV due to compilation issue
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable KVM_HYPERV
	# tweak locaversion
	cd $(LINUX_AGENT_DIR) && ./scripts/config --set-str LOCALVERSION -nyx
	# save space
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable DEBUG_INFO
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable MODULE_COMPRESS
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable MODULE_COMPRESS_ZSTD
	cd $(LINUX_AGENT_DIR) && make olddefconfig

	cd $(LINUX_AGENT_DIR) && ./compile_kvm_nyx_standalone.sh && sudo ./load_kvm_nyx.sh

fullBuild:
	git clone -b kvm-nyx-6.8 git@github.com:IntelLabs/kafl.linux.git

	cd $(LINUX_AGENT_DIR) && make clean && make mrproper
	cp config  ./$(LINUX_AGENT_DIR)/.config
	# disable kernel signature
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable SYSTEM_TRUSTED_KEYS
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable SYSTEM_REVOCATION_KEYS
	# enable KVM
	cd $(LINUX_AGENT_DIR) && ./scripts/config --module KVM
	# disable KVM_WERROR (otherwise compilation issues in modified KVM)
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable KVM_WERROR
	# enable Nyx
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable KVM_NYX
	# disable KVM_HYPERV due to compilation issue
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable KVM_HYPERV
	# tweak locaversion
	cd $(LINUX_AGENT_DIR) && ./scripts/config --set-str LOCALVERSION -nyx
	# save space
	cd $(LINUX_AGENT_DIR) && ./scripts/config --disable DEBUG_INFO
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable MODULE_COMPRESS
	cd $(LINUX_AGENT_DIR) && ./scripts/config --enable MODULE_COMPRESS_ZSTD
	cd $(LINUX_AGENT_DIR) && make olddefconfig
	cd $(LINUX_AGENT_DIR) &&  make -j12 bindeb-pkg
	sudo dpkg -i *.deb



get_deb:
	wget https://github.com/IntelLabs/kafl.linux/releases/download/kvm-nyx-6.8/linux-headers-6.8.0-nyx+_6.8.0-g0509b850d2bd-1_amd64.deb
	wget https://github.com/IntelLabs/kafl.linux/releases/download/kvm-nyx-6.8/linux-image-6.8.0-nyx+-dbg_6.8.0-g0509b850d2bd-1_amd64.deb
	wget https://github.com/IntelLabs/kafl.linux/releases/download/kvm-nyx-6.8/linux-image-6.8.0-nyx+_6.8.0-g0509b850d2bd-1_amd64.deb
	wget https://github.com/IntelLabs/kafl.linux/releases/download/kvm-nyx-6.8/linux-libc-dev_6.8.0-g0509b850d2bd-1_amd64.deb

