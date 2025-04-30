# Getting Started with Linaro's LAVA webserver

This document includes complete guide from installing LAVA webserver and adding a new user to running first job on the added QEMU device on the LAVA webserver. 

Adding a new device in the LAVA webserver will be covered in other docs.

## Installing the LAVA webserver

For installing the LAVA webserver, you either need to have the docker installed (if your machine is not running a Debian OS) or a Debian OS installed (Debian 12 bookworm for this guide).

I prefer setting it up on the system running Debian 12 on it (without any virtualization layers i.e. containers or VMs) as that simplifies the networking setup later on. So for this document, it is assumed that you have Debian 12 bookworm already installed.