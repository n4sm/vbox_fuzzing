KAFL_CONFIG_FILE=kafl.yaml kafl fuzz \
	--image /home/nasm/vbox_fuzzing/gen_L1/ubuntu-server.qcow2 \
	--purge \
	--redqueen \
	--grimoire \
	--workdir $PWD/workdir
