$ErrorActionPreference = 'Stop'

$imageName = 'dlt-ubuntu:local'
$containerName = 'dlt-ubuntu'

docker build --target ubuntu-runtime --tag $imageName .
docker rm --force $containerName 2>$null
docker run --detach --name $containerName --init --publish 3490:3490 $imageName
docker exec --detach $containerName dlt_sample_app

Write-Host "DLT daemon and sample app are running in $containerName."
Write-Host "View logs with: docker logs -f $containerName"
Write-Host "Stop with: docker rm -f $containerName"