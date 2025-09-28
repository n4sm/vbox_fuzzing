#!/usr/bin/bash
#/home/nasm/kAFL/kafl/qemu/x86_64-softmmu/qemu-system-x86_64 \
sudo qemu-system-x86_64 \
    -hda ubuntu-server.qcow2 \
    -m 20G \
    -smp 5 \
    -accel kvm -cpu host \
    -fsdev local,id=shared_folder,path=../vbox/,security_model=none \
    -device virtio-9p-pci,fsdev=shared_folder,mount_tag=shared \
    -nic user,hostfwd=tcp:127.0.0.1:2222-:22 \
    -d guest_errors \
    -D hypercall.log \
    -nographic \

