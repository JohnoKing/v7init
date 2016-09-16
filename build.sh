#!/bin/sh

# Check if the compiled failed
check()
{
    if [ $? != 0 ]
    then
        echo "The compile failed!"
        exit 1
    fi
}

# Check for a compiler to use
command -v gcc
if [ $? = 1 ]
then
    command -v clang
    if [ $? = 1]
    then
        echo "You need to install gcc or clang!"
        exit 1
    else
        export CC=clang
    fi
else
    export CC=gcc
fi

# Build getty and init
$CC -c -O2 getty.c init.c
check
$CC -o getty-v7 getty.o
check
$CC -o init-v7 init.o
check
rm *.o

# Installation
#
# Allow the user to edit the rc script
editrc()
{
    read -n1 editinit && echo
    if [ $editinit = 1 ]
    then
        sudo vi /etc/rc-v7
    elif [ $editinit = 2 ]
    then
        command
    else
        echo "Please press 1 or 2."
        editrc
    fi
}

installv7()
{
    # Install getty and init
    sudo mv getty-v7 init-v7 /sbin

    # Install the scripts
    sudo cp rc-v7 /etc
    echo "Would you like to edit the init script (/etc/rc-v7)? Press 1 for yes, 2 for no:"
    editrc
    if [ ! -e /etc/rc-v7.d ]
    then
        sudo mkdir -p /etc/rc-v7.d
    fi
    sudo cp example.sh /etc/rc-v7.d
    sudo chmod +x /etc/rc-v7.d/example.sh
    sudo chmod +x /etc/rc-v7

    # Install /etc/ttys if it doesn't exist
    if [ ! -e /etc/ttys ]
    then
        sudo cp ttys /etc
    fi
}

# Run installv7()
if [ "$1" = "-i" ]
then
    installv7
elif [ "$1" = "--install" ]
then
    installv7
fi

exit 0
