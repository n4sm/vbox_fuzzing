KAFL_CONFIG_FILE=kafl.yaml kafl debug \
	--resume \
	--action single \
    	--input $1 \
	--debug \
	--workdir workdir/ \
	--image /home/nasm/fuzz/vbox_fuzzing/gen_L1/ubuntu-server.qcow2 \
	--log-hprintf
