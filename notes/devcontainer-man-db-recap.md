# Devcontainer man-db Recap

Date: 2026-06-18

## Context

Terry wanted `man-db` available in the devcontainer for Linux networking work, especially pages such as `socket(2)`, `bind(2)`, `recvfrom(2)`, and related C library/system-call references.

The active branch at the time was:

```sh
feature/udp-pingpong-sandbox
```

## What We Tried In The Live Container

- Checked the current Ubuntu version:

```sh
lsb_release -a
cat /etc/os-release
```

- The container reported Ubuntu 26.04 LTS, codename `resolute`.
- Installed `man-db` with apt.
- `dpkg-query -W man-db` showed `man-db 2.13.1-1build1`.
- `man --version` initially still printed Ubuntu's minimized-image message instead of the real `man` version.

## What We Found

The live container had Ubuntu minimized-image behavior in two layers:

1. `/usr/bin/man` was diverted to a small minimized-image stub.
2. `/etc/dpkg/dpkg.cfg.d/excludes` contained:

```text
path-exclude=/usr/share/man/*
```

That exclusion causes dpkg to discard manpage files during package installation. So even after installing `man-db`, commands such as this still failed:

```sh
man 1 man
man 2 socket
```

## Live Container Repair Attempt

We removed the `/usr/bin/man` diversion and restored the real binary enough for:

```sh
man --version
```

to report:

```text
man 2.13.1
```

However, manpage content was still missing because the dpkg path exclusion remained. Removing that exclusion in the live container was not completed, because the approval request for editing `/etc/dpkg/dpkg.cfg.d/excludes` was rejected.

## Dockerfile Direction

Terry pinned the devcontainer base image to:

```dockerfile
FROM ubuntu:26.04
```

We then updated `.devcontainer/Dockerfile` so fresh devcontainer builds should install manpages correctly from the start:

- If `/etc/dpkg/dpkg.cfg.d/excludes` exists, remove only the `/usr/share/man/*` exclusion.
- Install:
  - `man-db`
  - `manpages`
  - `manpages-dev`
  - `less`

The important Dockerfile behavior is:

```dockerfile
RUN if [ -f /etc/dpkg/dpkg.cfg.d/excludes ]; then \
        sed -i '\#^path-exclude=/usr/share/man/#d' /etc/dpkg/dpkg.cfg.d/excludes; \
    fi \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        man-db \
        manpages \
        manpages-dev \
        less
```

The actual package list remains part of the larger devcontainer dependency install layer.

## Follow-up Finding

After the rebuild, `man` still printed the minimized-image message. The package install had succeeded and the manpage path exclusion was gone, but the image still had a local dpkg diversion:

```text
local diversion of /usr/bin/man to /usr/bin/man.REAL
```

That left Ubuntu's minimized-image stub at `/usr/bin/man`, even though the real `man-db` binary existed at `/usr/bin/man.REAL`.

The first attempt to remove the diversion with only `dpkg-divert --rename --remove /usr/bin/man` failed because the minimized stub still occupied `/usr/bin/man`.

The Dockerfile now removes that stub first, then removes the diversion after installing `man-db`:

```dockerfile
&& if dpkg-divert --list /usr/bin/man | grep -q '/usr/bin/man'; then \
    if [ -e /usr/bin/man ] && [ -e /usr/bin/man.REAL ]; then \
        rm -f /usr/bin/man; \
    fi; \
    dpkg-divert --rename --remove /usr/bin/man; \
fi \
&& mandb -c \
```

The final `mandb -c` forces a fresh manual-page database after restoring the real binary.

## After Rebuild, Verify

After rebuilding the devcontainer, run:

```sh
man --version
man 1 man
man 2 socket
man 2 bind
man 3 getaddrinfo
```

Expected result:

- `man --version` prints a real `man-db` version.
- `man 1 man` opens the `man` manual.
- `man 2 socket`, `man 2 bind`, and `man 3 getaddrinfo` find actual manual pages.

## Related Work In Progress

This happened while starting the UDP ping-pong sandbox feature. The branch also includes scaffold work for:

```text
sandbox/udp_pingpong/server
sandbox/udp_pingpong/client
make udp-ping
```
