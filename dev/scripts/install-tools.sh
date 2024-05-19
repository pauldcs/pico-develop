#!/bin/bash

set -eux

readonly SDK_FOLDER="/opt/pico-sdk"
readonly SDK_REPOSITORY="https://github.com/raspberrypi/pico-sdk.git"

sudo apt update -y
sudo apt-get install -y \
\
  build-essential         \
  curl                    \
  tree                    \
  wget                    \
  vim                     \
  cmake                   \
  lldb                    \
  clang                   \
  git                     \
  gdb-multiarch           \
  gcc-arm-none-eabi       \
  inotify-tools           \
  pkg-config              \
  libusb-1.0-0-dev        \
  libnewlib-arm-none-eabi \
  minicom                 \
  openocd 

git clone -b master "${SDK_REPOSITORY}" "${SDK_FOLDER}"
cd "${SDK_FOLDER}"
git submodule update --init

