# Adding the First Device

For this document, StarFive VisionFive 2 will be added to the setup.

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