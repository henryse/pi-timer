#!/usr/bin/env bash
sudo update-rc.d -f pi-timer remove
sudo cp pi-timer.sh /etc/init.d/pi-timer
sudo chmod 755 /etc/init.d/pi-timer
sudo update-rc.d pi-timer defaults