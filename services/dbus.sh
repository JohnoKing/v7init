#!/bin/sh

echo "Starting DBus..." > /dev/console
mkdir /run/dbus
dbus-daemon --system
STATUS=$? SERVICE=DBus /etc/v7init/rc.d/rc.status
