#!/bin/sh
set -eu

export LD_LIBRARY_PATH=/opt/dlt/lib:/usr/local/lib:/usr/lib/aarch64-linux-gnu:/lib/aarch64-linux-gnu

if [ -f /etc/dbus-session.env ]; then
    . /etc/dbus-session.env
else
    echo "DBus session is not available; start the container entrypoint first" >&2
    exit 1
fi

exec /usr/local/bin/MediaClientApp.bin "$@"