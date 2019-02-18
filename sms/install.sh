#!/bin/bash

set -e
#set -x

ver=$(rpm -q --queryformat '%{version}-%{release}\n' ubi-sms)

if [ ! -f /opt/sms/bin/smsd-$ver ]; then
	mv /opt/sms/bin/smsd /opt/sms/bin/smsd-$ver
fi

cp -f smsd/object/x86_64/smsd /opt/sms/bin
chown ncuser: /opt/sms/bin/smsd
chmod 750 /opt/sms/bin/smsd

(cd /opt/sms/bin/php/; patch -p0) < add-object_to_json.patch
