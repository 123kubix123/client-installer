#!/bin/bash
mkdir /home;
mount /dev/"" /pracownia
btrfs sub del /pracownia/home2
btrfs sub snap /pracownia/home /pracownia/home2
subvolid=`btrfs sub list /pracownia | grep home2 | awk '{ print $2 }'`
umount /pracownia
mount -t btrfs -o subvolid=$subvolid /dev/sda3 /home
restorecon -R /home
