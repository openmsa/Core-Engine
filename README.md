UBIqube OpenMSA Core Engine
===========================


Overview
--------

The Ubiqube OpenMSA is an all in one network and security management
system.  This repository contains the OpenMSA Core Engine component.

The OpenMSA Core Engine is a daemon that pushes configuration to devices
using Devices Adaptors.  In addition, the daemon handles firmware updates
and configuration archiving for network elements.  VNF management is also
supported.

Deploying the core-engine requires an OpenMSA .ova image which must
be downloaded separately and run in an appropriate VM environment.


Pre-requisite
-------------

- download the OpenMSA .ova image
- setup and run the OpenMSA VM

Please refer to the download and install instructions at
https://openmsa.co/documentation/getting-started-with-the-openmsa-freeware/


Installing
----------

- ssh to the OpenMSA as root
- wget `http://repository.ubiqube.com/share/OpenMSA/unixODBC-devel-16.2.2-1.el6.x86_64.rpm`
- rpm -i `unixODBC-devel-16.2.2-1.el6.x86_64.rpm`
- git clone this repository
- run: `cd sms && make && make install`
- run: `make restart`


Checking it works
-----------------

- ssh to the OpenMSA as root
- run: `tstsms ISALIVE`
