# IVI - Automotive In-Vehicle Infotainment System
This project is reference of Head vision IVI and built for Raspberry pi with CommonBus api + Franca D BUs +SOME/IP

## Subsystem Specifications

### 1. 3D Cockpit Navigation and Geospatial Engine
- Rendering Pipeline: Hardware-accelerated WebGL 3D engine powered by MapLibre GL and QtWebEngineQuick.
- 3D Perspective: 56-degree forward-looking driving pitch with vector extruded 3D buildings (OpenFreeMap planet vector tiles).
- Base Cartography: High-resolution OpenStreetMap raster tiles capped at zoom level 18.0 to prevent void zoom states.
- Live GPS Coordinate Acquisition: Asynchronous startup resolution via network IP geolocation (`https://ipwho.is/` with fallback to `https://ipapi.co/json/`) for immediate, card-free global location positioning.
- Reverse Geocoding: Automated OpenStreetMap Nominatim query engine resolving live coordinates to street-level metadata (e.g. road, pedestrian way, suburb).
- Rate-Limited Geocoding Cache: Distance-delta thresholding preventing redundant Nominatim network calls during cruising.
- Navigation Reference Marker: 3D elliptical ground disc with directional blue chevron rotating 0 to 360 degrees and dynamic street name badge.
- Open-Source Map Attribution: Official OpenStreetMap branding badge with logo and company name displayed in the viewport corner.

### 2. HVAC, Climate and Thermal Comfort Suite
- Dual-Zone Temperature Control: Independent driver and passenger thermal regulation ranging from 16.0 C to 28.0 C with fine-grained 0.5 C stepping.
- 3-Level Seat Ventilation: Independent driver and passenger seat cooling with active blue level indicators and contextual flyout dialogs.
- 3-Level Seat Heating: High-efficiency PTC heating control with 3-stage visual state feedback.
- Steering Wheel Heating: Integrated driver thermal control with dedicated toggle status.
- Airflow Distribution: Configurable multi-zone vent routing (Windshield Defrost, Face Vents, Footwell Vents).
- Defrost Modes: Dedicated MAX Front Windshield Defrost and Rear Heated Glass controls.

### 3. Vehicle Telemetry and CAN Bus Simulator
- Dynamic Cruising Loop: Periodic 500 ms simulation timer modeling realistic highway driving conditions.
- Powertrain Metrics: Real-time calculation of vehicle cruising speed (64 to 72 km/h) and correlated engine/motor RPM (1900 to 2200 RPM).
- Energy Storage: Battery State of Charge (SoC) monitoring with level reporting.
- Transmission State: PRND electronic shift selector telemetry.
- Environmental Sensors: Outside ambient temperature sensing (21.5 C nominal).

### 4. Media Player and Audio Architecture
- Playback Telemetry: Track title, artist, album, elapsed track time, total track duration, and album artwork.
- Interactive Timeline: Dynamic progress bar with scrubbing and 500 ms position tracking.
- Audio Controls: Previous track, play/pause toggle, next track, and audio source selection.
- Waveform Visualizer: Animated audio spectrum visualization reflecting active media streaming states.

### 5. Telephony and Connected Cockpit Suite
- Status Chrome: Persistent top status bar displaying vehicle speed indicator, connectivity status, cellular signal strength, GPS lock, current time, and ambient temperature.
- Application Navigation: Side navigation dock enabling rapid transitions between Home, Vehicle Controls, Phone, and System Settings.
- Responsive HMI: Optimized for 1024x600, 1280x720, and 1920x720 automotive touchscreen displays.

### 6. Design System and Automotive Ergonomics
- Typography: Inter typeface family (Regular, Medium, SemiBold, Bold) bundled in application resources for deterministic text rendering across all platforms.
- Contrast Compliance: Deep slate automotive dark palette (`#0E1522`, `#162032`, `#1E293B`) adhering to ISO 15008 visual presentation standards for automotive displays.
- Visual Hierarchy: Frosted glassmorphism overlays with backdrop blur filters, high-visibility blue navigation accents (`#2563EB`, `#00D2FF`), and clear driver state affordances.
## Build and Execution

### Prerequisites

- Compiler: C++20 compliant compiler (GCC 11+, Clang 13+, MSVC 2019+)
- Build System: CMake 3.20+ and Ninja or Make
- Framework: Qt 6.5+ with the following components:
  - `Qt6::Core`
  - `Qt6::Gui`
  - `Qt6::Quick`
  - `Qt6::Qml`
  - `Qt6::QuickControls2`
  - `Qt6::Svg`
  - `Qt6::Network`
  - `Qt6::WebEngineQuick`

### Ubuntu Linux (22.04 LTS / 24.04 LTS)

```bash
# 1. Install system build dependencies
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake ninja-build \
  libgl1-mesa-dev libxkbcommon-dev libxkbcommon-x11-dev \
  libfontconfig1-dev libfreetype6-dev libasound2-dev libpulse-dev

# 2. Configure with CMake (pointing to Qt 6 installation)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# 3. Build and launch
cmake --build build -j$(nproc)
./build/Head_vision_ivi
```


docker buildx build --platform linux/amd64 --target commonapi-generator-base -f DockerfileCommonApi.base -t commonapi-generator-base:latest --load .
docker buildx build --platform linux/arm64 --target commonapi-runtime-base -f DockerfileCommonApi.base -t commonapi-runtime-base:latest --load .
docker buildx build --platform linux/arm64 -f DockerfileCommonApi -t Head-vision-ivi:latest --load .
docker buildx build --platform linux/arm64 -f DockerfileCommonApi.app -t Head-vision-ivi:latest --load .
docker run --rm --name Head-vision-ivi -p 8080:8080 -p 3490:3490 Head-vision-ivi:latest
docker run --rm --name head-vision-ivi -p 8080:8080 -p 3490:3490 -v C:\Media:/media head-vision-ivi:latest

# Split application build, after building the two base images above:
docker buildx build --platform linux/arm64 -f DockerfileCommonApi.app -t Head-vision-ivi:latest --load .

### Vehicle DBus diagnostic commands

Run these commands from another terminal while the container is running:

```bash
docker exec Head-vision-ivi sh -c '. /etc/dbus-session.env; VehicleClientApp get'
docker exec Head-vision-ivi sh -c '. /etc/dbus-session.env; VehicleClientApp speed 72'
docker exec Head-vision-ivi sh -c '. /etc/dbus-session.env; VehicleClientApp set-details 72 2100 D 21.5 88'
docker exec Head-vision-ivi sh -c '. /etc/dbus-session.env; VehicleClientApp watch 10'
```

The commands read or update speed, RPM, gear, outside temperature, and battery level through the `vehiclefunctions` CommonAPI DBus service.

### Media DBus service commands

The container starts the `org.example.MediaPlayer` service automatically. Use these commands while the container is running:

```bash
docker exec Head-vision-ivi sh -c '. /etc/dbus-session.env; MediaClientApp status'
docker exec Head-vision-ivi sh -c '. /etc/dbus-session.env; MediaClientApp play'
docker exec Head-vision-ivi sh -c '. /etc/dbus-session.env; MediaClientApp pause'
docker exec Head-vision-ivi sh -c '. /etc/dbus-session.env; MediaClientApp stop'
```

The service exposes `Play`, `Pause`, `Stop`, and the readonly `PlaybackStatus` attribute.
`Play` scans `/media` and starts the first supported `.mp4`, `.mkv`, `.mp3`, `.wav`, `.avi`, or `.webm` file.
Mount a host media directory when starting the container:

```powershell
docker run --rm --name apex-vision-ivi -p 8080:8080 -p 3490:3490 -v C:\Media:/media apex-vision-ivi:latest
```
docker exec apex-vision-ivi sh -c ". /etc/dbus-session.env; VehicleClientApp set-details 72 2100 D 21.5 88"
docker exec apex-vision-ivi sh -c ". /etc/dbus-session.env; VehicleClientApp get"

To select one specific file instead of the first discovered file, add `-e MEDIA_FILE=/media/song.mp3`.

docker buildx build --platform linux/arm64 --target application-compiler -f DockerfileCommonApi .
