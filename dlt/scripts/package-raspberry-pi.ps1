$ErrorActionPreference = 'Stop'

$packageName = 'dlt-raspberry-pi5-arm64'
$stagingDir = Join-Path 'build' $packageName

Remove-Item -Recurse -Force $stagingDir -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $stagingDir | Out-Null

docker buildx build `
    --platform linux/arm64 `
    --target package-export `
    --output "type=local,dest=$stagingDir" `
    .

Move-Item -Force (Join-Path $stagingDir 'dlt-package.tar.gz') "$packageName.tar.gz"
Write-Host "Created $packageName.tar.gz"