#!/bin/sh
# Publish the session bus address so `docker exec` sessions can reuse it.
export LD_LIBRARY_PATH=/opt/dlt/lib:/usr/local/lib:/usr/lib/aarch64-linux-gnu:/lib/aarch64-linux-gnu
exec env LD_LIBRARY_PATH="$LD_LIBRARY_PATH" \
    dbus-run-session -- sh -c '
    printf "export DBUS_SESSION_BUS_ADDRESS='\''%s'\''\n" "$DBUS_SESSION_BUS_ADDRESS" > /etc/dbus-session.env
    export LD_LIBRARY_PATH=/opt/dlt/lib:/usr/local/lib:/usr/lib/aarch64-linux-gnu:/lib/aarch64-linux-gnu
    if [ -x /opt/dlt/bin/dlt-daemon ]; then
        if [ -f /opt/dlt/etc/dlt.conf ]; then
            /opt/dlt/bin/dlt-daemon -c /opt/dlt/etc/dlt.conf -d
        else
            /opt/dlt/bin/dlt-daemon -d
        fi
    fi
    exec "$@"
' sh "$@"
