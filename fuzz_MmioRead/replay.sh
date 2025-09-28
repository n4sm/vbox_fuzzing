KAFL_CONFIG_FILE=kafl.yaml kafl debug \
	--resume \
	--action printk \
    	--input /dev/shm/kafl_root/corpus/timeout/payload_00003 \
	--image /home/nasm/vbox_fuzzing/gen_L1/ubuntu-server.qcow2 \
	--log-hprintf
