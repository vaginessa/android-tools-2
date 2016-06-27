
governor=$1
cur_freq=$2
i=0;
while [ $i -lt 8 ];do
    echo "CPU$i:"
    cat /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor
    cat /sys/devices/system/cpu/cpu$i/cpufreq/scaling_cur_freq
    if [ "$governor" != "" ];then
        echo "$governor" > /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor
    fi
    if [ "$cur_freq" != "" ];then
        echo "$cur_freq" > /sys/devices/system/cpu/cpu$i/cpufreq/scaling_setspeed
    fi
    let i=$i+1
done
