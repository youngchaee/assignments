# 베이스 이미지
FROM ubuntu:20.04

# 패키지 설치 (한 번에)
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y default-jdk curl build-essential zip git cmake uuid-dev pkg-config && \
    rm -rf /var/lib/apt/lists/*

# ANTLR4 Java 라이브러리 다운로드
WORKDIR /usr/local/lib
RUN curl -O https://www.antlr.org/download/antlr-4.13.2-complete.jar && \
    ln -s antlr-4.13.2-complete.jar antlr-complete.jar

# 환경 변수 설정
ENV CLASSPATH=".:/usr/local/lib/antlr-complete.jar:$CLASSPATH"
RUN echo '#!/bin/bash\njava -jar /usr/local/lib/antlr-complete.jar "$@"' > /usr/local/bin/antlr4 && \
    chmod +x /usr/local/bin/antlr4

# ANTLR4 C++ 런타임 다운로드 및 빌드
WORKDIR /tmp
RUN curl -O https://www.antlr.org/download/antlr4-cpp-runtime-4.13.2-source.zip && \
    unzip antlr4-cpp-runtime-4.13.2-source.zip -d ANTLR4

WORKDIR /tmp/ANTLR4
RUN mkdir build run && \
    cd build && \
    cmake .. && \
    DESTDIR=../run make install

# 라이브러리 설치
RUN cp -r run/usr/local/include/antlr4-runtime /usr/local/include/ && \
    cp -a run/usr/local/lib/libantlr4* /usr/local/lib/ && \
    ldconfig

# 환경 변수 설정
ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

# 작업 디렉토리로 이동
WORKDIR /app

# 기본 셸 설정
CMD ["/bin/bash"]
