#!/usr/bin/env bash
set -euo pipefail

package_name="dlt-ubuntu-amd64"
staging_dir="build/$package_name"

rm -rf "$staging_dir"
mkdir -p "$staging_dir"

docker buildx build \
    --platform linux/amd64 \
    --target package-export \
    --output "type=local,dest=$staging_dir" \
    .

mv "$staging_dir/dlt-package.tar.gz" "$package_name.tar.gz"
printf 'Created %s\n' "$package_name.tar.gz"