#!/bin/sh

# Create mount point if needed
mkdir -p mountpoint/

# Running QEMU
qemu-system-x86_64 \
    -enable-kvm \
    -kernel ./bzImage \
    -initrd ./out.cpio.gz  \
    -append 'console=ttyS0 loglevel=3 oops=panic panic=1 nokaslr' \
    -device qemu-xhci,id=xhci -trace "usb_xhci_*" -D /tmp/qemu-trace.log \
    -m 8G \
    -no-reboot \
    -nographic
