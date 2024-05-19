#!/bin/bash
sudo picotool load -f "${1}"
sudo rm "${1}"
sudo picotool reboot
