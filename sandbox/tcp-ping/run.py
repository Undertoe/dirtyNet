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

SERVER_EXE = Path("sandbox/tcp-ping/server/sandbox_tcp_ping_server")
CLIENT_EXE = Path("sandbox/tcp-ping/client/sandbox_tcp_ping_client")


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
        description="Run the TCP ping sandbox server and client."
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=default_build_dir(),
        help="CMake build directory. Defaults to BUILD_DIR, then _build, then build.",
    )
    parser.add_argument(
        "--check-delay",
        type=float,
        default=1.0,
        help="Seconds to wait before checking that both processes have closed.",
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


def main() -> int:
    args = parse_args()
    build_dir = args.build_dir.resolve()

    try:
        require_executable(build_dir, SERVER_EXE)
        require_executable(build_dir, CLIENT_EXE)
    except (FileNotFoundError, PermissionError) as error:
        print(error, file=sys.stderr)
        print("Build them first with: make tcp-ping", file=sys.stderr)
        return 1

    processes: list[tuple[str, subprocess.Popen[bytes]]] = []
    try:
        server = subprocess.Popen([f"./{SERVER_EXE}"], cwd=build_dir)
        processes.append(("server", server))

        client = subprocess.Popen([f"./{CLIENT_EXE}"], cwd=build_dir)
        processes.append(("client", client))

        time.sleep(args.check_delay)

        still_running = [
            (name, process)
            for name, process in processes
            if process.poll() is None
        ]
        if still_running:
            for name, process in still_running:
                print(
                    f"{name} still running after {args.check_delay:g}s; stopping.",
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
        process.wait()
        if process.returncode != 0:
            return process.returncode
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
