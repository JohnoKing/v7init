#!/bin/sh

export NAME=DBus
start()
{
    mkdir /run/dbus
    dbus-daemon --system
}
