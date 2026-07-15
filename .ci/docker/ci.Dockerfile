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
    wayland \
    wayland-protocols \
    libxkbcommon \
    libxcursor \
    libxi \
    libxinerama \
    libxrandr \
    mesa \
    vulkan-headers \
    vulkan-icd-loader \
    && pacman -Scc --noconfirm

ENV CCACHE_DIR=/cache/ccache
ENV PATH="/usr/lib/ccache/bin:$PATH"

RUN mkdir -p /cache/ccache && cat > /etc/ccache.conf <<'EOF'
hash_dir = false
sloppiness = include_file_mtime,include_file_ctime,time_macros,locale
depend_mode = true
compression = true
max_size = 20G
log_file = /tmp/ccache.log
EOF

WORKDIR /workspace