#!/bin/bash

sudo mknod /dev/hostmem c 60 0
sudo chmod 666 /dev/hostmem
sudo insmod pci_hostmem.ko
#sudo rmmod pci_hostmem.ko

