#!/bin/sh

echo "Handing off init to OpenRC..." > /dev/console
/sbin/openrc sysinit
/sbin/openrc boot
/sbin/openrc default
STATUS=$? SERVICE=DBus /etc/v7init/rc.status
