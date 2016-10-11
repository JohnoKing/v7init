#!/bin/sh

echo "Starting dbus..." > /dev/console
mkdir /run/dbus
dbus-daemon --system
STATUS=$? SERVICE=DBus /etc/rc.d/rc.status
