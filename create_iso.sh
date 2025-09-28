mkdir -p iso/boot/grub
cp bzImage out.cpio.gz iso/boot/
cat > iso/boot/grub/grub.cfg <<EOF
set default=0
set timeout=0

menuentry "Linux" {
    insmod gzio
    insmod part_msdos
    linux /boot/bzImage console=ttyS0,115200n8 earlyprintk=serial,ttyS0,115200n8 root=/dev/ram0 \

    initrd /boot/out.cpio.gz
}
EOF
grub-mkrescue -o boot.iso iso/
