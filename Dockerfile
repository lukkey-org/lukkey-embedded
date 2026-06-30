FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    python3 \
    autoconf \
    automake \
    libtool \
    pkg-config \
    xutils-dev \
    python3-venv \
    python3-pip \
    bsdiff \
    openssl \
    zip \
    unzip \
    && rm -rf /var/lib/apt/lists/*

ENV PATH="/opt/toolchain/bin:${PATH}"

WORKDIR /workspace
