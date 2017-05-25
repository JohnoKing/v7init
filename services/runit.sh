#!/bin/sh

echo "Handing off init to runit..." > /dev/console
/etc/runit/1
/etc/runit/2
STATUS=$? SERVICE=Runit /etc/v7init/rc.status
