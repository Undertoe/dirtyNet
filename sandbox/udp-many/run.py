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

HOST_EXE = Path("sandbox/udp-many/host/sandbox_udp_many_host")
UNICAST_CLIENT_EXE = Path("sandbox/udp-many/unicast-client/unicast-client")
MULTICAST_CLIENT_EXE = Path("sandbox/udp-many/multicast-client/multicast-client")
DEFAULT_BASE_PORT = 9000
DEFAULT_MULTICAST_PORT = 9100
DEFAULT_TIMEOUT_SECONDS = 30.0
MAX_PORT = 65535


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
        description="Run the UDP many sandbox host and clients."
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=default_build_dir(),
        help="CMake build directory. Defaults to BUILD_DIR, then _build, then build.",
    )
    parser.add_argument(
        "--clients",
        type=int,
        default=4,
        help="Number of clients to start.",
    )
    parser.add_argument(
        "--base-port",
        type=int,
        default=DEFAULT_BASE_PORT,
        help=f"First unicast client port. Defaults to {DEFAULT_BASE_PORT}.",
    )
    parser.add_argument(
        "--multicast-port",
        type=int,
        default=DEFAULT_MULTICAST_PORT,
        help=f"Shared multicast client port. Defaults to {DEFAULT_MULTICAST_PORT}.",
    )
    parser.add_argument(
        "--startup-delay",
        type=float,
        default=0.2,
        help="Seconds to wait after starting the host before starting clients.",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=DEFAULT_TIMEOUT_SECONDS,
        help="Seconds to wait for all processes to finish before stopping them.",
    )
    return parser.parse_args()


def validate_port(port: int, name: str) -> int:
    if port < 1 or port > MAX_PORT:
        raise ValueError(f"{name} must be between 1 and {MAX_PORT}")

    return port


def client_ports(base_port: int, clients: int) -> list[int]:
    if clients < 0:
        raise ValueError("--clients must be greater than or equal to 0")

    validate_port(base_port, "--base-port")

    if clients == 0:
        return []

    last_port = base_port + clients - 1
    if last_port > MAX_PORT:
        raise ValueError(
            f"client port range {base_port}-{last_port} exceeds {MAX_PORT}"
        )

    return list(range(base_port, last_port + 1))


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
        unicast_ports = client_ports(args.base_port, args.clients)
        multicast_port = validate_port(args.multicast_port, "--multicast-port")
    except ValueError as error:
        print(error, file=sys.stderr)
        return 2

    try:
        require_executable(build_dir, HOST_EXE)
        require_executable(build_dir, UNICAST_CLIENT_EXE)
        require_executable(build_dir, MULTICAST_CLIENT_EXE)
    except (FileNotFoundError, PermissionError) as error:
        print(error, file=sys.stderr)
        print("Build them first with: make udp-many", file=sys.stderr)
        return 1

    processes: list[tuple[str, subprocess.Popen[bytes]]] = []
    try:
        host = subprocess.Popen([f"./{HOST_EXE}"], cwd=build_dir)
        processes.append(("host", host))

        time.sleep(args.startup_delay)

        for client_id, port in enumerate(unicast_ports):
            client = subprocess.Popen(
                [f"./{UNICAST_CLIENT_EXE}", str(port)],
                cwd=build_dir,
            )
            processes.append((f"unicast-client-{client_id}:{port}", client))

        for client_id in range(args.clients):
            client = subprocess.Popen(
                [f"./{MULTICAST_CLIENT_EXE}", str(multicast_port)],
                cwd=build_dir,
            )
            processes.append(
                (f"multicast-client-{client_id}:{multicast_port}", client)
            )

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
