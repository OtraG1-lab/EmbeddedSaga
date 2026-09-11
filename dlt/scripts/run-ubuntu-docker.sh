#!/usr/bin/env bash
set -euo pipefail

image_name="dlt-ubuntu:local"
container_name="dlt-ubuntu"

docker build --target ubuntu-runtime --tag "$image_name" .
docker rm --force "$container_name" 2>/dev/null || true
docker run --detach --name "$container_name" --init --publish 3490:3490 "$image_name"
docker exec --detach "$container_name" dlt_sample_app
echo "DLT daemon and sample app are running in $container_name."
echo "View logs with: docker logs -f $container_name"
echo "Stop with: docker rm -f $container_name"