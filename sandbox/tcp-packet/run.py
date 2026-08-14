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

IMPLEMENTATIONS = ("dirtynet", "posix")


def executable_paths(implementation: str) -> tuple[Path, Path]:
    root = Path("sandbox/tcp-packet") / implementation
    target_prefix = f"sandbox_tcp_packet_{implementation}"
    return (
        root / "server" / f"{target_prefix}_server",
        root / "client" / f"{target_prefix}_client",
    )


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
        description="Run the TCP packet sandbox server and client."
    )
    parser.add_argument(
        "implementation",
        choices=IMPLEMENTATIONS,
        help="TCP packet implementation to run.",
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
        default=5.0,
        help="Seconds to wait before checking that both processes have closed.",
    )
    parser.add_argument(
        "--startup-delay",
        type=float,
        default=0.05,
        help="Seconds to wait after starting the server before starting the client.",
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
    server_exe, client_exe = executable_paths(args.implementation)

    try:
        require_executable(build_dir, server_exe)
        require_executable(build_dir, client_exe)
    except (FileNotFoundError, PermissionError) as error:
        print(error, file=sys.stderr)
        print(
            f"Build them first with: make tcp-packet-{args.implementation}",
            file=sys.stderr,
        )
        return 1

    processes: list[tuple[str, subprocess.Popen[bytes]]] = []
    try:
        server = subprocess.Popen([f"./{server_exe}"], cwd=build_dir)
        processes.append(("server", server))

        time.sleep(args.startup_delay)

        client = subprocess.Popen([f"./{client_exe}"], cwd=build_dir)
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
