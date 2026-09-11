# Build with Docker Buildx for the target architecture.
FROM ubuntu:24.04 AS build

ARG DLT_DAEMON_REF=v2.18.10
ENV DEBIAN_FRONTEND=noninteractive
ENV DLT_PREFIX=/opt/dlt
ENV PKG_CONFIG_PATH=/opt/dlt/lib/pkgconfig
ENV LD_LIBRARY_PATH=/opt/dlt/lib

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        git \
        libdbus-1-dev \
        libglib2.0-dev \
        libjson-c-dev \
        pkg-config \
        zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 --branch "$DLT_DAEMON_REF" https://github.com/COVESA/dlt-daemon.git /tmp/dlt-daemon \
    && cmake -S /tmp/dlt-daemon -B /tmp/dlt-daemon/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$DLT_PREFIX" \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DCMAKE_INSTALL_SYSCONFDIR=etc \
    && cmake --build /tmp/dlt-daemon/build --parallel \
    && cmake --install /tmp/dlt-daemon/build \
    && rm -rf /tmp/dlt-daemon

WORKDIR /workspace
COPY CMakeLists.txt ./
COPY src ./src

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel

FROM scratch AS package
COPY --from=build /opt/dlt /dlt-debug-trace/opt/dlt
COPY --from=build /workspace/build/dlt_sample_app /dlt-debug-trace/bin/dlt_sample_app
COPY --chmod=755 scripts/start-daemon.sh /dlt-debug-trace/bin/start-daemon
COPY --chmod=755 scripts/run-sample.sh /dlt-debug-trace/bin/run-sample
COPY --chmod=755 scripts/deploy-raspberry-pi.sh /dlt-debug-trace/deploy.sh
COPY systemd/dlt-daemon.service /dlt-debug-trace/systemd/dlt-daemon.service

FROM build AS package-archive
COPY --from=package /dlt-debug-trace /dlt-debug-trace
RUN tar -C / -czf /dlt-package.tar.gz dlt-debug-trace

FROM scratch AS package-export
COPY --from=package-archive /dlt-package.tar.gz /dlt-package.tar.gz

FROM ubuntu:24.04 AS ubuntu-runtime

ENV DEBIAN_FRONTEND=noninteractive
ENV LD_LIBRARY_PATH=/opt/dlt/lib

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libdbus-1-3 \
        libglib2.0-0 \
        libjson-c5 \
        zlib1g \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/dlt /opt/dlt
COPY --from=build /workspace/build/dlt_sample_app /usr/local/bin/dlt_sample_app
COPY --chmod=755 scripts/start-docker-daemon.sh /usr/local/bin/start-docker-daemon

EXPOSE 3490
ENTRYPOINT ["/usr/local/bin/start-docker-daemon"]
