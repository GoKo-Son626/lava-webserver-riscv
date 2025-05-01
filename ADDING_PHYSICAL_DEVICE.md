# Adding the First Device

For this document, StarFive VisionFive 2 will be added to the setup.

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