#!/bin/sh

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

# Build and install libstty
cd libstty
$CC -c -O2 -ffast-math stty.c
ar cr libstty.a stty.o
rm stty.o
sudo mv libstty.a /lib
cd ..

# Build and install getty, init (the program itself), and the rc script
$CC -c -O2 -ffast-math getty.c init.c
$CC -o getty-v7 getty.o -lstty
$CC -o init-v7 init.o
rm *.o
sudo mv getty-v7 init-v7 /bin
sudo cp rc-v7 /etc

# Allow the user to edit the rc script
editrc()
{
    echo "Would you like to edit the init script (/etc/rc-v7)? Press 1 for yes, 2 for no:"
    read -n1 editinit && echo
    if [ $editinit = 1 ]
    then
        sudo vi /etc/rc-v7
    elif [ $editinit = 2 ]
    then
        command
    else
        echo "Please press 1 or 2."
        echo
        editrc
    fi
}
editrc

# Install the example script, make the scripts executable
sudo mkdir /etc/rc-v7.d
sudo cp example.sh /etc/rc-v7.d
sudo chmod +x /etc/rc-v7.d/example.sh
sudo chmod +x /etc/rc-v7

# Install a /etc/ttys file if none exists
if [ ! -e /etc/ttys ]
then
    sudo cp ttys /etc
fi

# Exit
exit $?
