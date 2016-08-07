#!/usr/bin/env bash
sudo update-rc.d -f pitimer remove
sudo cp pitimer.sh /etc/init.d/pitimer
sudo chmod 755 /etc/init.d/pitimer
sudo update-rc.d pitimer defaults