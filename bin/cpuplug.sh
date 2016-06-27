index=$1
onoff=$2
echo "${onoff}" > /sys/devices/system/cpu/cpu${index}/online
