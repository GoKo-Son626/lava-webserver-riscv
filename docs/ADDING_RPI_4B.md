# Adding the First Physical Device

For this document, Raspberry Pi 4 model B will be added to the setup since it is easily available and there is necessary documentation available for it.

## Adding Raspberry Pi 4 Model B

I do not have a PDU at the time of making this documentation. Luckily, the RUN pin on the raspberry pi 4b can be used to safely restart the raspberry pi 4b and thats all I need.

For raspberry pi 5, I have following equipment.

1. A USB-to-serial adapter
2. Some jumper cables
3. An arduino nano

This is a cheap infrastructure as compared to a PDU, rack etc. for testing.

Connect the RUN pin of the Raspberry pi 5 with the pin A0 of the arduino nano. Connect the ground of the raspberry pi 5 with the ground of Arduino nano.

Connect the serial adapter with raspberry pi according to suitable connections of jumper wire (i.e. TX of raspberry pi 5 with RX of serial adapter and RX of serial adapter with TX of raspberry pi 5 and GND to GND).



### Configuring the arduino for PDU commands

Since LAVA expects a command that can be run in the Linux in which it is running for reboot/power on/power off, so we will have to use arduino through command line. For that, you must install `arduino-cli` (I have added arduino-cli compiled for x86 in [arduino](/arduino/) folder ot this repository)

Once that is done, you need to add a shell script that can compile and flash the arduino code on the arduino nano.

The RUN pin of raspberry pi 4 model b is active low. Keeping it low for 200ms is enough to safely reboot the raspberry pi 4b. 

Here pin A0 of arduino is used to pull it low. But make sure to keep it high after the board is reset otherwise the reboot will not complete.

```
/lava-shared
├── /lava-shared/arduino-cli
└── /lava-shared/rpi-4-reset
    ├── /lava-shared/rpi-4-reset/rpi-4-reset.ino
    └── /lava-shared/rpi-4-reset/rpi-4-reset.sh

2 directories, 3 files
```
### Adding Raspberry Pi 4 Model B in LAVA worker

The process of adding the device is same as was for QEMU. 

First create a device type with following command in the LAVA server.

```
lava-server manage devices add --device-type raspberry-pi     --worker debian-12-bookworm  rpi-4-1
```

Then create add a device to the device-type using following command.

```
sudo lava-server manage devices add --device-type raspberry-pi --worker debian-12-bookworm rpi-4-1
sudo lava-server manage devices update --health UNKNOWN rpi-4-1
```
Add the raspberry pi device dictionary in `/etc/lava-server/dispatcher-config/devices/` directory with following content.

```
{% extends 'bcm2711-rpi-4-b.jinja2' %}
{% set connection_list = ['uart0']%}
{% set connection_commands = {'uart0': 'telnet localhost 5000'}%}
{% set connection_tags = {'uart0': ['primary','telnet']}%}
{% set power_off_command = '/lava-shared/rpi-4-reset/rpi-4-reset.sh'  %}
{% set hard_reset_command = '/lava-shared/rpi-4-reset/rpi-4-reset.sh' %}
{% set power_on_command = '/lava-shared/rpi-4-reset/rpi-4-reset.sh' %}
```

Before testing this setup there is another important step that you need to take care of. The LAVA worker cannot access the device directly. It needs `ser2net` which is NECESSARY for it to detect connections and connect to them. `ser2net` should be automatically installed with lava-worker.

Add the following block in ser2net for raspberry pi device which you just added.

```
connection: &raspberrypi_usb0
    accepter: tcp,localhost,5000 
    enable: on                 
    options:
      banner: *banner
      kickolduser: true
      telnet-brk-on-sync: true
    connector: serialdev,
               /dev/ttyUSB1,
               115200n81,local
```

Restart the `ser2net.service`

```
sudo systemctl restart ser2net.service
```

You must change the `/dev/ttyUSB1` to the port where your device is connected. Usually, Linux adds the device in numerically ascending order so if there is nothing plugged in your laptop, connecting a raspberry pi 4 model b will add it to `/dev/ttyUSB0`, if there is one device already  connected and you plug it, it will become `/dev/ttyUSB1` and so on.

### Connecting the infrastructure

Now that everything is added, you need to make the cable connections. Connect the raspberry pi 4b with laptop through USB port. Connect the A0 pin of arduino with the RUN pin of raspberry pi 4 model b, the GND pin of arduino to GND of raspberry pi 4b. Connect the Serial-to-USB adapter with raspberry pi 4b and connect it with your laptop/machine in USB port.

The whole set up looks something as follows for me.

![Arduino Connection](/assets/IMG_20250512_203639.jpg)

### Submitting the job

You can submit the following Raspberry Pi 4 model b job for general connection check.


```
device_type: raspberry-pi
job_name: simple-rpi-login-test-v3 # Iterated name for clarity
metadata:
  build_id: simple-test-build-v3
  job: simple-rpi-login-test-v3
  retries: 3

context:
  kernel_start_message: "" # This was a key fix, keep it.

priority: medium
timeouts:
  job:
    minutes: 5
  action:
    minutes: 2 # Default for actions unless overridden
  actions:
    power-off:
      seconds: 30
visibility: public

actions:
  - boot:
      method: minimal
      connection: serial
      auto_login:
        login_prompt: 'login:'
        username: raspy
        password_prompt: 'Password:'
        password: ILikeraspberry
      prompts:
        # This regular expression will match your shell prompt even with
        # leading ANSI escape codes like [?2004h
        - '.*raspy@rasp4-1:~\$'
      timeout:
        minutes: 2 # Or 3 if boot/login is a bit slow
```

This job will pass if everything is working fine. This is just a simple job to check if the infrastructure is working.

