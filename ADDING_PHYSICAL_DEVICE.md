# Adding the First Device

For this document, StarFive VisionFive 2 will be added to the setup. But first off we will try it out with device which is already available in LAVA, just to check that all the parts in infrastructure are working.

## Adding Raspberry Pi 4 Model B

I have a raspberry pi 4b so I will integrate that for this tutorial.

I do not have a PDU at the time of making this documentation. Luckily, the RUN pin on the raspberry pi 4b can be used to safely restart the raspberry pi 4b and thats all I need.

For raspberry pi 5, I have following equipment.

1. A USB-to-serial adapter
2. Some jumper cables
3. An arduino nano

This is a cheap infrastructure as compared to a PDU, rack etc. for testing.

### Configuring the arduino

Since LAVA expects a command that can be run in the Linux in which it is running for reboot/power on/power off, so we will have to use arduino through command line. For that, you must install `arduino-cli` (I have added arduino-cli compiled for x86 in [arduino](/arduino/) folder ot this repository)

Once that is done, you need to add a shell script that can compile and flash the arduino code on the arduino nano.

The RUN pin of raspberry pi 4 model b is active low. Keeping it low for 200ms is enough to safely reboot the raspberry pi 4b. 

Here pin A0 of arduino is used to pull it low. But make sure to keep it high after the board is reset otherwise the reboot will not complete.

All the scripts are present in [arduino](/arduino/) directory. For this setup, it is better to copy the contents of [arduino](/arduino/) to a direcotry inside the root directory and then give it full permission for every user so that the setup does not fail due to lack of permissions. My directory tree looks as follows.

```
/lava-shared
├── /lava-shared/arduino-cli
└── /lava-shared/rpi-4-reset
    ├── /lava-shared/rpi-4-reset/rpi-4-reset.ino
    └── /lava-shared/rpi-4-reset/rpi-4-reset.sh

2 directories, 3 files
```



## Adding VisionFive 2

I have

1. A USB-to-Serial Adapter
2. An ethernet cable (optional for now)
3. A StarFive VisionFive 2 Board
4. A power adapter with USB type-C to power on the board

For existing devices, there are already some dictionary templates which can be extended in the device diciotnary. These templates are located in `/usr/share/lava-server/device-types` in default installation of LAVA webserver.

Just as usual, first, you have to create a device type.

```
sudo lava-server manage device-types add visionfive2
```

Once added, you can add the StarFive VisionFive 2 device with following command.

```
lava-server manage devices add \
    --device-type visionfive2 \
    --worker debian-12-bookworm \
    visionfive2-1
```
  
_The worker name here is `debian-12-bookworm`. This is because it is hostname of my machine at the time of writing this documentation. If you dont change the worker name, its usually the hostname of your machine (which is mostly located in `/etc/hostname`)._

Now you can list the device information with following command.

```
sudo lava-server manage devices details visionfive2-1
```

It will give you output something like following.

```
hostname   : visionfive2-1
device-type: visionfive2
state      : Idle
health     : Good
health job : False
description: None
physical   : 
device-dict: False
worker     : debian-12-bookworm
current_job: None
```

Now you have to add a dictionary for this device. The dictionary can extend the already available device-types in the `/usr/share/lava-server/device-types` as explained before. This is very convinient. Then you can add device-specific overrides in your device dictionary.

Add a file `/etc/lava-server/dispatcher-config/devices/visionfive2-1.jinja2` with following content.

```
{% extends 'jh7100-visionfive.jinja2' %}

{% set console_backend = 'serial' %}
{% set serial_port = '/dev/ttyUSB0' %}
{% set serial_baud_rate = 115200 %}

```

After this device dictionary, run the following command again.

```
sudo lava-server manage devices details visionfive2-1
```

This time you will see the output as follows.

```
hostname   : visionfive2-1
device-type: visionfive2
state      : Idle
health     : Good
health job : False
description: None
physical   : 
device-dict: True
worker     : debian-12-bookworm
current_job: None
```

The difference is that the `device-dict` is now set to `True` as you have added a dictionary