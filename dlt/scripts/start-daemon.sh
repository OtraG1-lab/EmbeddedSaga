#!/usr/bin/env bash
set -euo pipefail

package_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export LD_LIBRARY_PATH="$package_root/opt/dlt/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

rm -rf /tmp/dlt
mkfifo /tmp/dlt

config_file="$package_root/opt/dlt/etc/dlt.conf"
if [[ -f "$config_file" ]]; then
	exec "$package_root/opt/dlt/bin/dlt-daemon" -c "$config_file"
fi

exec "$package_root/opt/dlt/bin/dlt-daemon"
