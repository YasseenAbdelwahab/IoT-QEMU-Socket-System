SUMMARY = "QEMU image for Project 3 (IoT temperature client)"
LICENSE = "MIT"

inherit core-image

IMAGE_INSTALL:append = " iotclient iot-prompt"
