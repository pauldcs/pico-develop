#!/bin/bash

set -eux

sudo cp monitor.service /etc/systemd/system/monitor.service
sudo ln -s monitor.sh /home/pico/monitor.sh
sudo ln -s on_change.sh /home/pico/on_change.sh

sudo systemctl daemon-reload
sudo systemctl enable monitor.service
sudo systemctl start monitor.service
sudo systemctl status -l monitor.service