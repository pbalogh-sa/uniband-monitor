#!/bin/bash

scan="`cat /usr/local/share/uniband-monitor/conf/uniband-monitor.conf | grep -w ignoredcard | awk '{print $2}'`"
for i in `cat /proc/net/dev | grep wlan | awk '{print $1}' | tr -d :`
do
        if [[ $i == $scan ]]
        then
                continue
        fi
        ip link set dev $i down
        iw dev $i set monitor none
        ip link set dev $i promisc on up
        echo "setup $i"
done
