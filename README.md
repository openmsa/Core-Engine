UBIqube OpenMSA Open Core Engine
================================


Overview
--------

The Ubiqube OpenMSA is an all in one network and security management
system.  This repository contains the OpenMSA core-engine component.

Deploying the core-engine requires an OpenMSA .ova image which must
be downloaded separately and run in an appropriate VM environment.


Pre-requisite
-------------

- download the OpenMSA .ova image
- setup and run the OpenMSA VM


Installing
----------

- ssh to the OpenMSA as root
- git clone this repository
- run: `make && make install`


Checking it works
-----------------

- ssh to the OpenMSA as root
- run: `tstsms status`

