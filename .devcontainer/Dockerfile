FROM ubuntu:latest

ARG USERNAME=vscode

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        bubblewrap \
        ca-certificates \
        catch2 \
        clang \
        clangd-21 \
        clang-format-21 \
        clang-tools \
        clang-tidy-21 \
        cmake \
        curl \
        g++ \
        gcc \
        gdb \
        git \
        hyperfine \
        lld \
        lldb \
        linux-tools-common \
        linux-tools-generic \
        make \
        ninja-build \
        nodejs \
        npm \
        pkg-config \
        sudo \
        time \
        valgrind \
        libbenchmark-dev \
    && rm -rf /var/lib/apt/lists/*

RUN npm install -g @openai/codex

RUN git config --system --add safe.directory /home/vscode/.ai-memories \
    && git config --system --add safe.directory /workspace/dirtyNet

COPY devcontainer-entrypoint /usr/local/bin/devcontainer-entrypoint
RUN chmod +x /usr/local/bin/devcontainer-entrypoint

RUN useradd --create-home --shell /bin/bash "${USERNAME}" \
    && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > "/etc/sudoers.d/${USERNAME}" \
    && chmod 0440 "/etc/sudoers.d/${USERNAME}"

WORKDIR /workspace/dirtyNet
