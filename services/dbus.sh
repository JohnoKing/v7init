#!/bin/sh

echo "Starting dbus..."
mkdir /run/dbus
dbus-daemon --system
STATUS=$? SERVICE=DBus /etc/rc.d/rc.status
