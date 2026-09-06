FROM ghcr.io/otrag1-lab/rpiqtcrossbuild:26.8

# Update the repoPath according to yours
ARG projectDir=project
ARG repoPath=/home/otra/QTonRaspberryPi/

RUN rm -rf $repoPath$projectDir

RUN mkdir -p $repoPath$projectDir

COPY $projectDir $repoPath$projectDir

RUN cd $repoPath$projectDir && \
    /build/qt6/pi/bin/qt-cmake . -DCMAKE_BUILD_TYPE=Debug && \
    cmake --build .
