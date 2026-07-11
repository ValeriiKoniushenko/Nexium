FROM archlinux:latest

RUN pacman -Syu --noconfirm --needed \
    base-devel \
    cmake \
    ninja \
    gcc \
    clang \
    llvm \
    git \
    ccache \
    gtest \
    gdb \
    && pacman -Scc --noconfirm

ENV CCACHE_DIR=/cache/ccache
ENV PATH="/usr/lib/ccache/bin:$PATH"

WORKDIR /workspace