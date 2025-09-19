# Adding Banana Pi F3 as a device in LAVA

## Setting up the PDU

If you havent, read the [ADDING_RPI_4B.md](/docs/ADDING_RPI_4B.md) for adding the first device in the LAVA server. It includes basic information needed to set up the first device in the LAVA.

For this documentation, I will be adding Banana Pi F3 as a device in the LAVA webserver.

## Setting up the PDU

For this setup, I have created a cheaper but a more sophisticated reset infrastructure than described in the previous documentation.


I have

1. A USB-to-Serial Adapter
2. An ethernet cable (optional for now)
3. A Banana Pi F3 (16GB memory) Board
4. A power adapter with USB type-C to power on the board
5. An arduino uno
6. A 16-channel 5V input signal relay
7. Some wires and power sockets

The setup is pretty simple. Each input of the relay module is active low and is turned on when a LOW signal is transmitted to it. I have programmed the arduino uno for this. Pins 2-5 are used for powering on and off the relays 1-4. The algorithm checks for the string from the serial UART from the USB serial. 

IMPORTANT: Make sure to remove the trace of RESET_EN physically on arduino else the arduino will reset everytime you try to transmit a string through USB serial. This is the expected behavior of Arduino. But once you remove the trace/line connecting the RESET and DTR (RESET_EN), for every program flashed in arduino, you will have to manually press the reset button on arduino.

Following is the image showing how the setup looks like with arduino and relays.

![arduino_relays](/assets/IMG_20250514_222155.jpg)

For existing devices, there are already some dictionary templates which can be extended in the device diciotnary. These templates are located in `/usr/share/lava-server/device-types` in default installation of LAVA webserver.

Just as usual, first, you have to create a device type.

```
sudo lava-server manage device-types add banana-pi-f3
```

Once added, you can add the Banana Pi F3 device with following command.

```
lava-server manage devices add \
    --device-type banana-pi-f3 \
    --worker worker-2 \
    bpi-f3
```
  

Now you can list the device information with following command.

```
sudo lava-server manage devices details bpi-f3
```

It will give you output something like following.

```
hostname   : bpi-f3
device-type: banana-pi-f3
state      : Idle
health     : Unknown
health job : False
description: None
physical   : 
device-dict: False
worker     : worker-2
current_job: None
```

Now you have to add a dictionary for this device. The dictionary can extend the already available device-types in the `/usr/share/lava-server/device-types` as explained before. This is very convinient. Then you can add device-specific overrides in your device dictionary.

Here we do not have any template for banana Pi F3 but banana pi f3 uses U-boot bootloader so I have extended it instead of using some other device.

For using the arduino relay, you can use the arduino code available in [arduino-control.ino](/arduino/relay-algorithm/arduino-control/arduino-control.ino). This has to be running on the Arduino before passing the commands below.

Add a file `/etc/lava-server/dispatcher-config/devices/bpi-f3.jinja2` with following content.

```
{% extends 'base-uboot.jinja2' %}
{% set connection_list = ['uart0'] %}
{% set connection_commands = {'uart0': 'telnet localhost 5000'} %}
{% set connection_tags = {'uart0': ['primary','telnet']} %}

{% set power_off_command = 'bash -c "echo SET 2 1 > /dev/ttyACM0"' %}
{% set power_on_command  = 'bash -c "echo SET 2 0 > /dev/ttyACM0"' %}
{% set hard_reset_command = 'bash -c "echo SET 2 1 > /dev/ttyACM0; sleep 3; echo SET 2 0 > /dev/ttyACM0"' %}

```

After this device dictionary, run the following command again.

```
sudo lava-server manage devices details visionfive2-1
```

This time you will see the output as follows.

```
hostname   : bpi-f3
device-type: banana-pi-f3
state      : Idle
health     : Unknown
health job : False
description: None
physical   : 
device-dict: True
worker     : worker-2
current_job: None
```

The difference is that the `device-dict` is now set to `True` as you have added a dictionary


Add ser2net.yaml configuration for connecting to the board through UART with telnet.

```
connection: &bpi-f3
    accepter: tcp,localhost,5000 
    enable: on                 
    options:
      banner: *banner
      kickolduser: true
      telnet-brk-on-sync: true
    connector: serialdev,
               /dev/ttyUSB0,
               115200n81,local
```

IMPORTANT: Make sure the board is connected to the /dev/ttyUSB0

After this you can check the board's connection by running a simple job. If the health is bad, you can set it to unknown as described previously.