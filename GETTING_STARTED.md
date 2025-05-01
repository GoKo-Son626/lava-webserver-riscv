# Getting Started with Linaro's LAVA webserver

This document includes complete guide from installing LAVA webserver and adding a new user to running first job on the added QEMU device on the LAVA webserver. 

Adding a new device in the LAVA webserver will be covered in other docs.

## Overview of LAVA webserver (Master, Worker, Device Types and Devices)

LAVA webserver is used to manage and run tests for the low form factor computers (Raspberry Pi boards, RISC-V VisionFive, QEMU emulator etc.)

The devices are managed in the LAVA webserver in the following manner.

1. Each device is added to a device type
2. Each device type is part of a worker (which is, basically, the machine which is connected to the device)

An example of this can be RISC-V QEMU System running a ubuntu on an x86 host. In this example, QEMU System is a device and x86 is the worker with which the device is connected.

The worker can also be refered to as the slave in LAVA webserver. 

The LAVA webserver itself is known as the master (the machine on which the LAVA UI is running with all the databases).

The worker and master can be on the same machine without any problem because they run as a separate `systemd` service on the computer.

For running the jobs from the worker, there has to be an authenticated connection between master and the worker. This is managed by adding identity in the worker. The LAVA master can generate an authentication token which can be used in the worker to authenticate with the master for running the jobs.

## Installing the LAVA webserver

For installing the LAVA webserver, you either need to have the docker installed (if your machine is not running a Debian OS) or a Debian OS installed (Debian 12 bookworm for this guide).

I prefer setting it up on the system running Debian 12 on it (without any virtualization layers i.e. containers or VMs) as that simplifies the networking setup later on. So for this document, it is assumed that you have Debian 12 bookworm already installed.

Use the following command on the Debian to install LAVA webserver.

```
sudo apt update
sudo apt install curl gpg     # just in case you don't have them installed yet
curl -fsSL "https://apt.lavasoftware.org/lavasoftware.key.asc" | gpg --dearmor > lavasoftware.gpg
sudo mv lavasoftware.gpg /usr/share/keyrings
sudo chmod u=rw,g=r,o=r /usr/share/keyrings/lavasoftware.gpg
echo 'deb [signed-by=/usr/share/keyrings/lavasoftware.gpg] https://apt.lavasoftware.org/release bookworm main' | sudo tee /etc/apt/sources.list.d/lavasoftware.list
sudo apt install postgresql lava-server lavacli -y
sudo a2dissite 000-default
sudo a2enmod proxy
sudo a2enmod proxy_http
sudo a2ensite lava-server.conf
sudo service apache2 restart
sudo systemctl enable lava-server-gunicorn.service
sudo systemctl start lava-server-gunicorn.service
```

After this, LAVA webserver should be accessible on `localhost`.

## Creating a user in LAVA webserver

For using LAVA webserver, it is better to create a user which is not superuser and then use it to log in to the LAVA webserver.

Create a user in the LAVA webserver using following commands.

```
sudo lava-server manage users add USERNAME
```

This will give you a preset password. If you want to change the password, you can use the following commands 

```
# Enter the LAVA python shell
sudo lava-server manage shell
```

Use the following script inside the LAVA python shell to change the password.

```
from django.contrib.auth.models import User
u = User.objects.get(username='USERNAME')
u.set_password('new_secure_password')
u.save()
```

After this, you can go to `localhost` and log in with the username and password created above.

## Adding a LAVA worker

For adding a LAVA worker, you need to create a LAVA worker on the webserver. After that you will have to create a worker on the machine. The two workers are synchronized via an authentication token to make it a single LAVA worker. The main worker is the machine with which your device is going to be attached. You have to create a worker on the LAVA webserver just to add the details of the LAVA worker which will be connected to the device.

Just to mention it again, LAVA worker and LAVA webserver can be on the same physical machine as they run with separate programs with systemd.

On the machine where the LAVA webserver is running, execute the following command to create a LAVA worker.

```
sudo lava-server manage workers add WORKER_NAME # This will give you a token which you should note (will also be able to access it `<LAVA_WEBSERVER_UI>/worker/WORKER_NAME/change/` which is the LAVA server web interface administration settings for LAVA worker which you added)
```

This will give you an authentication token which you should note.

On the LAVA worker machine, change the file `/etc/lava-dispatcher/lava-worker` to add the url, the token you just created above and name of the lava worker (which has to be the same as the LAVA worker you created in the LAVA webserver with the above commands).

In short, the file `/etc/lava-dispatcher/lava-worker` can look as follows.

```
# Configuration for lava-worker daemon

# worker name
# Should be set for host that have random hostname (containers, ...)
# The name can be any unique string.
WORKER_NAME="--name WORKER_NAME"
URL="LAVA_SERVER_UI_URI"
# Logging level should be uppercase (DEBUG, INFO, WARN, ERROR)
 LOGLEVEL="DEBUG"

# Server connection
# URL="http://localhost/"
TOKEN="--token TOKEN"
# WS_URL="--ws-url http://localhost/ws/"
# HTTP_TIMEOUT="--http-timeout 600"
# JOB_LOG_INTERVAL="--job-log-interval 5"

# Sentry Data Source Name.
# SENTRY_DSN="--sentry-dsn <sentry-dsn>"
```

After the file is added, use the following commands to enable the worker.

```
sudo systemctl enable lava-worker
sudo systemctl start lava-worker
sudo systemctl restart lava-server-gunicorn.service
```

The status of the LAVA worker will be seen in the LAVA webserver UI.

## Add an identity in the LAVA worker

Go to the LAVA webserver UI and log in with the username and password created above.

In the top bar, expand `API` and select `Authentication Tokens`. Create a new token and copy it.

Now from the LAVA worker, add a new identity and authenticate it with LAVA webserver using the following command.

```
lavacli identities add --uri LAVA_SERVER_URI/RPC2/ --username USERNAME --token TOKEN IDENTITY_NAME
```

This will add a new identity with the name `IDENTITY_NAME` in the LAVA worker which can then run jobs on the LAVA webserver.

## Adding a device in LAVA

For this document, QEMU Linux will be added as the device in the LAVA worker.

Add a device type in the worker with following command.

```
sudo lava-server manage device-types list # List the device-types already present
sudo lava-server manage device-types add qemu # Add QEMU device
```

List the worker with following command.

```
lava-server manage workers list # Usually the first worker is example.com if the slave and master are same
```

Add a device to the device type for qemu with following command.

```
lava-server manage devices add \
    --device-type qemu \
    --worker WORKER_NAME \
    qemu01
sudo lava-server manage devices update --health UNKNOWN qemu01
```

Now you need to create a device dictionary with file name same as the device name which you just added as `/etc/lava-server/dispatcher-config/devices/qemu01.jinja2` as follows.

```
{% extends "qemu.jinja2" %}

{% set netdevice = "user" %}
{% set memory = 1024 %}
```

To get information about a device use the following command:

```
sudo lava-server manage devices details <device>
```

## Submitting the first job

Submitting the job will also run it once a device is ready for running the job.

For job to be submitted and run, the LAVA scheduler should be running.

```
sudo systemctl status lava-scheduler.service
sudo systemctl enable lava-scheduler
sudo systemctl start lava-scheduler
```

You can submit your first job using the command below.

A simple QEMU x86 job is as follows. You can create a file anywhere as `first_job.yaml`.

```
device_type: qemu
job_name: simple qemu job
timeouts:
  job:
    minutes: 20
priority: medium
visibility: public
context:
  arch: amd64
actions:
- deploy:
    to: tmpfs
    timeout:
      minutes: 20
    images:
      rootfs:
        url: http://images.validation.linaro.org/kvm/debian-sid-2014_08_21-amd64.qcow2.xz
        image_arg: -drive format=qcow2,file={rootfs}
        format: qcow2
        compression: xz
    os: debian
- boot:
    method: qemu
    media: tmpfs
    timeout:
      minutes: 5
    prompts:
    - 'root@debian:~#'
    auto_login:
      login_prompt: "login:"
      username: root
- test:
    timeout:
      minutes: 5
    definitions:
    - repository: https://github.com/Linaro/test-definitions
      from: git
      path: automated/linux/smoke/smoke.yaml
      name: smoke-tests
    - repository: https://github.com/Linaro/test-definitions
      from: git
      path: automated/linux/meminfo/meminfo.yaml
      name: meminfo
```

For running this job, use the command below.

```
sudo lavacli -i IDENTITY_NAME jobs submit first_job.yaml
```

