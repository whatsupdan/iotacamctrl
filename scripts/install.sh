


 configure_usbfs_memory_mb() {
#  2a. If raspberry pi add `sudo sh -c 'echo 32 > /sys/module/usbcore/parameters/usbfs_memory_mb’ before `exit 0` in /etc/rc.local
  os=$(cat /proc/cpuinfo | grep 'Raspberry Pi OS' )
    if [ -z "$os" ]; then
      echo "Updating Raspberry Pi OS usbfs_memory_mb to 32mb"
      updated_cmd="echo\s32\s>\s/sys/module/usbcore/parameters/usbfs_memory_mb$(printf '\n')exit0"
      sed -e s/exit\s0/"$updated_cmd"/g -i /etc/rc.local
    else
      echo "Updating Ubuntu OS usbfs_memory_mb to 32mb"

#  2b. If ubuntu,  Change “APPEND ${cbootargs} quiet” to "APPEND ${cbootargs} usbcore.usbfs_memory_mb=32 usbcore.autosuspend=-1" the command below in the file “/boot/extlinux/extlinux.conf”
      sed -e s/APPEND \$\{cbootargs\} quiet/APPEND\s\$\{cbootargs\}\susbcore.usbfs_memory_mb=32\susbcore.autosuspend=-1/g -i /boot/extlinux/extlinux.conf
    fi
 }

install_deps() {
  apt install libssl-dev autoconf libtool libudev-dev gcc g++ make cmake unzip libxml2-dev python3-dev libffi-dev
}


 setup() {
   echo "Setting up iota dependencies"
   install_deps
   configure_usbfs_memory_mb

   echo "Done!"
 }


