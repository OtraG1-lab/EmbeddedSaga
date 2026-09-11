#!/usr/bin/env bash
set -euo pipefail

package_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export LD_LIBRARY_PATH="$package_root/opt/dlt/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

exec "$package_root/bin/dlt_sample_app"
