#!/usr/bin/env bash

# Setup on startup.
sudo update-rc.d -f pitimer remove
sudo cp pitimer.sh /etc/init.d/pitimer
sudo chmod 755 /etc/init.d/pitimer
sudo update-rc.d pitimer defaults

# setup we don't want to run as sudo all of the time.
sudo chown root /usr/local/bin/pi-timer
sudo chmod 4755 /usr/local/bin/pi-timer