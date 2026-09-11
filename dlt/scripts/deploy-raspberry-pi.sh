#!/usr/bin/env bash
set -euo pipefail

package_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
install_root="/opt/dlt-debug-trace"
service_file="/etc/systemd/system/dlt-daemon.service"

if [[ $EUID -ne 0 ]]; then
    echo "Run this script with sudo." >&2
    exit 1
fi

runtime_packages=(libdbus-1-3 libglib2.0-0 libjson-c5 zlib1g)
missing_packages=()

for package in "${runtime_packages[@]}"; do
    if ! dpkg-query -W -f='${Status}' "$package" 2>/dev/null | grep -q 'install ok installed'; then
        missing_packages+=("$package")
    fi
done

if ((${#missing_packages[@]} > 0)); then
    apt-get update
    apt-get install -y --no-install-recommends "${missing_packages[@]}"
fi

rm -rf "$install_root"
mkdir -p "$install_root"
cp -a "$package_root/." "$install_root/"
install -m 0644 "$install_root/systemd/dlt-daemon.service" "$service_file"

systemctl daemon-reload
systemctl enable --now dlt-daemon.service
systemctl --no-pager --full status dlt-daemon.service