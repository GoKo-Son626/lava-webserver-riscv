# **Setting up Banana Pi F3 Boot Flow for Kernel CI**
This document outlines the essential steps to integrate the Banana Pi F3 board into a 10xKernel CI (Continuous Integration) environment using LAVA (Linaro Automated Validation Architecture). 
It details the setup required on a host x86 machine to enable LAVA to deploy binaries to the device, initiate booting, and validate the Linux kernel.

## U-Boot Secondary Program Loader (SPL) Setup
The first stage bootloader for kernelCI setup is the u-boot SPL (secondary program loader) which remains in the emmc and polls for u-boot proper from the UART Y-modem. 
It is built separately. It places the u-boot proper at the certain address in the volatile memory and then jumps to it.
Once the U-Boot proper is running, you can leverage its extensive command set, as described in the **U-Boot environment for BPI-F3**, to load the Linux kernel, often via NFS.
All required images are available in [firmware_images](https://github.com/alitariq4589/k1-kernelci-setup/tree/main/images)
Firmware image is available in [opensbi_buildroot](https://github.com/alitariq4589/k1-kernelci-setup/blob/main/firmware/uboot-opensbi_buildroot.itb)

## U-Boot environment for BPI-F3
Upon entering the U-Boot state, a series of commands are executed to configure the necessary U-Boot environment variables. 
These variables are crucial for network setup, memory addressing, and boot processes.
Refer to the [u-boot-env](https://github.com/alitariq4589/k1-kernelci-setup/blob/main/u-boot-env.env) for the list of commands.

## Host Machine NFS Server Setup
To enable the Banana Pi F3 board to download kernel images and root filesystems via NFS from the host machine, follow these steps to configure your NFS server:
1. Install NFS server package:
   **sudo apt update **
   ** sudo apt install nfs-kernel-server**
2. Create the following directories to export:
   **/nfsroot/rootfs** and **/nfsroot/bootfs**
3. Configure the NFS exports
   Edit **/etc/exports** and add lines like:
   /nfsroot/bootfs    *(rw,sync,no_subtree_check,no_root_squash)
   /nfsroot/roottfs   *(rw,sync,no_subtree_check,no_root_squash)
   Note: For NFS directory setup, refer to [MOUNTING_NFS](https://github.com/alitariq4589/k1-kernelci-setup/blob/main/docs/MOUNTING_NFS.md)
4. Export the directory:
   After modifying /etc/exports, apply the changes by running: **sudo exportfs -ra**
   NOTE: Only NFS version 3 is supported by u-boot for file transfering through nfs and also udp support needs to be enabled.

5. Enable NFSv3 and UDP (Crucial for U-Boot):
   U-Boot commonly supports only NFS version 3 for file transfers, and UDP support must be enabled. To ensure this:
   Open /etc/nfs.conf file and add this change "**vers3=y**" and **"udp=y"**, save this file.

7. Start and enable the NFS server:
   **sudo systemctl restart nfs-kernel-server**
   **sudo systemctl enable nfs-kernel-server**  


## Booting the Linux kernel on BPI-F3
Once the NFS server is successfully configured and running, and the U-Boot environment variables are set (typically after transferring "uboot-opensbi_buildroot.itb" to the board using the Y-modem UART transfer protocol), 
the system is ready to boot the Linux kernel.
After the U-Boot proper is loaded, execute the custom U-Boot command **"nfs_boot"** (as defined in u-boot-env.env).
This command initiates the transfer of all necessary firmware images and the kernel via NFS to the board, subsequently starting the Linux kernel. 
A successful kernel boot will typically display a log similar to this on the serial terminal:
**Welcome to Bianbu Linux
Bianbu login: [   39.985206] ldo5: disabling**

# **Automating with LAVA Job (.yaml) File**
All the manual steps described above for "board bring-up" and "booting Linux kernel" can be fully automated using a LAVA job. This allows for continuous, remote testing and validation.
1. **Adding Banana Pi F3 as a device in LAVA:**
   Follow the instructions detailed in [Adding BPI-F3 as device](https://github.com/alitariq4589/lava-webserver-riscv/blob/main/docs/ADDING_BPI-F3.md) to register Banana Pi F3 board with LAVA instance.
2. **Setup device dictionary**:
   Configure the device-specific parameters using a LAVA device dictionary. An example configuration bpi-f3 board can be found in [**bpi-f3.jinja2**](https://github.com/alitariq4589/lava-webserver-riscv/blob/main/device_templates/bpi-f3.jinja2)
   This file will define variables and settings specific to the Banana Pi F3.
3. **Create the LAVA Job File:**
   Write a LAVA job definition (.yaml file) that outlines the desired actions. This file will orchestrate the U-Boot SPL setup and the Linux kernel booting process. 
   The boot action within the LAVA job will include the U-Boot commands (like setenv and nfs_boot) to be executed on the device's console.for intended actions (in this case setup u-boot SPL and builds Linux kernel).
4. **Submit the LAVA Job:**
   Submit the job using "_lavacli -i lava jobs submit job.yaml_" command
5. **Automated Execution:**
   The LAVA server will automatically schedule and execute the job, managing the entire boot and kernel validation process on the Banana Pi F3.

