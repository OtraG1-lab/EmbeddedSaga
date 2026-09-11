#!/usr/bin/env bash
set -euo pipefail

rm -rf /tmp/dlt
mkfifo /tmp/dlt

if [[ -f /opt/dlt/etc/dlt.conf ]]; then
	exec /opt/dlt/bin/dlt-daemon -c /opt/dlt/etc/dlt.conf
fi

exec /opt/dlt/bin/dlt-daemon