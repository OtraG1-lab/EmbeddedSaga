#!/usr/bin/env bash
set -euo pipefail

package_name="dlt-raspberry-pi5-arm64"
staging_dir="build/$package_name"

rm -rf "$staging_dir"
mkdir -p "$staging_dir"

docker buildx build \
    --platform linux/arm64 \
    --target package-export \
    --output "type=local,dest=$staging_dir" \
    .

mv "$staging_dir/dlt-package.tar.gz" "$package_name.tar.gz"
printf 'Created %s\n' "$package_name.tar.gz"
