KAFL_CONFIG_FILE=kafl.yaml kafl fuzz \
	--image /home/nasm/fuzz/vbox_fuzzing/gen_L1/ubuntu-server.qcow2 \
	--purge \
	--redqueen \
	--grimoire \
	--log-crashes \
	--t-soft 4 \
	--workdir $PWD/workdir \
	--seed-dir $PWD/corpus/regular/ \
	--log-hprintf
