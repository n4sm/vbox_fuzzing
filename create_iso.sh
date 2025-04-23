mkdir -p iso/boot/grub
cp bzImage out.cpio.gz iso/boot/
cat > iso/boot/grub/grub.cfg <<EOF
menuentry "Linux" {
    linux /boot/bzImage root=/dev/ram0 console=ttyS0
    initrd /boot/out.cpio.gz
}
EOF
grub-mkrescue -o boot.iso iso/