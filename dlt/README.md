# COVESA DLT for Raspberry Pi 5

This project builds the COVESA `dlt-daemon` source, its libraries and tools, plus a small DLT sample application for 64-bit Raspberry Pi OS on Raspberry Pi 5.

## Build the ARM64 Package

Docker Desktop must have Buildx enabled. Run:

```bash
./scripts/package-raspberry-pi.sh
```

From Windows PowerShell, run:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass -Force
.\scripts\package-raspberry-pi.ps1
```

This performs a `linux/arm64` Buildx build and writes `dlt-raspberry-pi5-arm64.tar.gz`.

## Ubuntu AMD64 Package

Build a self-contained archive for 64-bit Ubuntu x86_64:

```bash
./scripts/package-ubuntu.sh
```

From Windows PowerShell, run:

```powershell
.\scripts\package-ubuntu.ps1
```

It writes `dlt-ubuntu-amd64.tar.gz`. Extract it and use `bin/start-daemon` and `bin/run-sample` as for the Raspberry Pi package.

## Run Ubuntu with Docker

Build and start an Ubuntu AMD64 image containing the COVESA DLT daemon:

```bash
./scripts/run-ubuntu-docker.sh
```

From Windows PowerShell, run:

```powershell
.\scripts\run-ubuntu-docker.ps1
```

The script starts the daemon and sample app in a container named `dlt-ubuntu`; the daemon listens on port `3490`.

## Deploy to the Raspberry Pi 5

Copy the archive to the Pi and extract it:

```bash
tar -xzf dlt-raspberry-pi5-arm64.tar.gz
cd dlt-debug-trace
```

Install, start, and enable the daemon at every system boot:

```bash
sudo ./deploy.sh
```

The deployment script installs the required Raspberry Pi OS runtime libraries (`libdbus`, GLib, JSON-C, and zlib) before starting the daemon.

Check its status or follow its output:

```bash
sudo systemctl status dlt-daemon
sudo journalctl -u dlt-daemon -f
```

Start the sample application:

```bash
/opt/dlt-debug-trace/bin/run-sample
```

The packaged runtime is self-contained under `/opt/dlt-debug-trace`; no distribution `dlt-daemon` package is required on the Pi. Use `/opt/dlt-debug-trace/opt/dlt/bin/dlt-receive -a 127.0.0.1` from a third terminal to view the messages.

## COVESA Version

The Dockerfile builds the `v2.18.10` tag from https://github.com/COVESA/dlt-daemon. Change `DLT_DAEMON_REF` in the Dockerfile to build another compatible COVESA tag or commit.
