#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import signal
import subprocess
import sys
import time
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parents[1]

SERVER_EXE = Path("sandbox/udp-ping/server/sandbox_udp_ping_server")
CLIENT_EXE = Path("sandbox/udp-ping/client/sandbox_udp_ping_client")


def default_build_dir() -> Path:
    env_build_dir = os.environ.get("BUILD_DIR")
    if env_build_dir:
        return Path(env_build_dir)

    underscore_build = REPO_ROOT / "_build"
    if underscore_build.exists():
        return underscore_build

    return REPO_ROOT / "build"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run the UDP ping sandbox server and client."
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=default_build_dir(),
        help="CMake build directory. Defaults to BUILD_DIR, then _build, then build.",
    )
    parser.add_argument(
        "--startup-delay",
        type=float,
        default=0.2,
        help="Seconds to wait after starting the server before running the client.",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=10.0,
        help="Seconds to wait for both processes to finish before stopping them.",
    )
    return parser.parse_args()


def require_executable(build_dir: Path, exe: Path) -> Path:
    exe_path = build_dir / exe
    if not exe_path.exists():
        raise FileNotFoundError(f"Missing executable: {exe_path}")
    if not os.access(exe_path, os.X_OK):
        raise PermissionError(f"Not executable: {exe_path}")
    return exe_path


def stop_process(process: subprocess.Popen[bytes]) -> int:
    if process.poll() is not None:
        return process.returncode

    process.send_signal(signal.SIGTERM)
    try:
        return process.wait(timeout=2)
    except subprocess.TimeoutExpired:
        process.kill()
        return process.wait()


def wait_for_processes(
    processes: list[tuple[str, subprocess.Popen[bytes]]],
    timeout: float,
) -> bool:
    deadline = time.monotonic() + timeout

    while time.monotonic() < deadline:
        if all(process.poll() is not None for _, process in processes):
            return True
        time.sleep(0.05)

    return all(process.poll() is not None for _, process in processes)


def main() -> int:
    args = parse_args()
    build_dir = args.build_dir.resolve()

    try:
        require_executable(build_dir, SERVER_EXE)
        require_executable(build_dir, CLIENT_EXE)
    except (FileNotFoundError, PermissionError) as error:
        print(error, file=sys.stderr)
        print("Build them first with: make udp-ping", file=sys.stderr)
        return 1

    processes: list[tuple[str, subprocess.Popen[bytes]]] = []
    try:
        server = subprocess.Popen([f"./{SERVER_EXE}"], cwd=build_dir)
        processes.append(("server", server))
        time.sleep(args.startup_delay)
        client = subprocess.Popen([f"./{CLIENT_EXE}"], cwd=build_dir)
        processes.append(("client", client))

        finished = wait_for_processes(processes, args.timeout)
        if not finished:
            for name, process in processes:
                if process.poll() is None:
                    print(
                        f"{name} still running after {args.timeout:g}s; stopping.",
                        file=sys.stderr,
                    )
                    stop_process(process)
            return 124
    except KeyboardInterrupt:
        print("Interrupted; stopping child processes.", file=sys.stderr)
        for _, process in processes:
            stop_process(process)
        return 130

    for _, process in processes:
        if process.returncode != 0:
            return process.returncode
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
