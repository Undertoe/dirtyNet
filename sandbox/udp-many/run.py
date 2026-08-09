#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import signal
import subprocess
import sys
import threading
import time
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parents[1]

IMPLEMENTATIONS = ("dirtynet", "posix")
DEFAULT_BASE_PORT = 9000
DEFAULT_MULTICAST_PORT = 10000
DEFAULT_TIMEOUT_SECONDS = 5.0
SHUTDOWN_GRACE_SECONDS = 2.0
MAX_PORT = 65535

ProcessEntry = tuple[str, subprocess.Popen[str], threading.Thread]
OUTPUT_LOCK = threading.Lock()


def executable_paths(implementation: str) -> tuple[Path, Path, Path]:
    root = Path("sandbox/udp-many") / implementation
    target_prefix = f"sandbox_udp_many_{implementation}"
    return (
        root / "host" / f"{target_prefix}_host",
        root / "unicast-client" / f"{target_prefix}_unicast_client",
        root / "multicast-client" / f"{target_prefix}_multicast_client",
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
        description="Run the UDP many sandbox host and clients."
    )
    parser.add_argument(
        "implementation",
        choices=IMPLEMENTATIONS,
        help="UDP many implementation to run.",
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
        default=0.0,
        help="Optional seconds to wait after starting the host before starting clients.",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=DEFAULT_TIMEOUT_SECONDS,
        help=f"Seconds to wait for all processes to finish before stopping them. Defaults to {DEFAULT_TIMEOUT_SECONDS:g}.",
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


def wait_for_processes(
    processes: list[ProcessEntry],
    timeout: float,
) -> bool:
    deadline = time.monotonic() + timeout

    while time.monotonic() < deadline:
        if all(process.poll() is not None for _, process, _ in processes):
            return True
        time.sleep(0.05)

    return all(process.poll() is not None for _, process, _ in processes)


def reap_processes(processes: list[ProcessEntry]) -> None:
    for _, process, _ in processes:
        process.wait()
    for _, _, output_thread in processes:
        output_thread.join(timeout=1)


def print_process_statuses(processes: list[ProcessEntry]) -> None:
    for name, process, _ in processes:
        status = process.poll()
        if status is None:
            print(f"{name} is still running.", file=sys.stderr)
        else:
            print(f"{name} exited with status {status}.", file=sys.stderr)


def stop_processes(
    processes: list[ProcessEntry],
    reason: str,
) -> None:
    running_processes = [
        (name, process) for name, process, _ in processes if process.poll() is None
    ]
    if not running_processes:
        reap_processes(processes)
        print_process_statuses(processes)
        return

    print(reason, file=sys.stderr)
    print_process_statuses(processes)

    for name, process in running_processes:
        print(f"{name} still running; sending SIGTERM.", file=sys.stderr)
        process.send_signal(signal.SIGTERM)

    deadline = time.monotonic() + SHUTDOWN_GRACE_SECONDS
    while time.monotonic() < deadline:
        if all(process.poll() is not None for _, process, _ in processes):
            reap_processes(processes)
            return
        time.sleep(0.05)

    for name, process, _ in processes:
        if process.poll() is None:
            print(f"{name} ignored SIGTERM; sending SIGKILL.", file=sys.stderr)
            process.kill()

    reap_processes(processes)
    print_process_statuses(processes)


def forward_output(name: str, process: subprocess.Popen[str]) -> None:
    if process.stdout is None:
        return

    for line in process.stdout:
        with OUTPUT_LOCK:
            print(f"[{name}] {line}", end="", flush=True)


def launch_process(
    processes: list[ProcessEntry],
    name: str,
    command: list[str],
    cwd: Path,
) -> None:
    with OUTPUT_LOCK:
        print(f"[runner] starting {name}", flush=True)

    process = subprocess.Popen(
        command,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
    )
    output_thread = threading.Thread(
        target=forward_output,
        args=(name, process),
        daemon=True,
    )
    output_thread.start()
    processes.append((name, process, output_thread))


def main() -> int:
    args = parse_args()
    build_dir = args.build_dir.resolve()
    host_exe, unicast_client_exe, multicast_client_exe = executable_paths(
        args.implementation
    )

    if args.timeout < 0:
        print("--timeout must be greater than or equal to 0", file=sys.stderr)
        return 2

    try:
        unicast_ports = client_ports(args.base_port, args.clients)
        multicast_port = validate_port(args.multicast_port, "--multicast-port")
    except ValueError as error:
        print(error, file=sys.stderr)
        return 2

    try:
        require_executable(build_dir, host_exe)
        require_executable(build_dir, unicast_client_exe)
        require_executable(build_dir, multicast_client_exe)
    except (FileNotFoundError, PermissionError) as error:
        print(error, file=sys.stderr)
        print(
            f"Build them first with: make udp-many-{args.implementation}",
            file=sys.stderr,
        )
        return 1

    processes: list[ProcessEntry] = []
    try:
        launch_process(processes, "host", [f"./{host_exe}"], build_dir)

        if args.startup_delay > 0:
            time.sleep(args.startup_delay)

        for client_id, port in enumerate(unicast_ports):
            launch_process(
                processes,
                f"unicast-client-{client_id}:{port}",
                [f"./{unicast_client_exe}", str(port)],
                build_dir,
            )

        for client_id in range(args.clients):
            launch_process(
                processes,
                f"multicast-client-{client_id}:{multicast_port}",
                [f"./{multicast_client_exe}", str(multicast_port)],
                build_dir,
            )

        finished = wait_for_processes(processes, args.timeout)
        if not finished:
            stop_processes(
                processes,
                f"Timeout reached after {args.timeout:g}s; stopping child processes.",
            )
            return 124
    except KeyboardInterrupt:
        stop_processes(processes, "Interrupted; stopping child processes.")
        return 130

    reap_processes(processes)

    for _, process, _ in processes:
        if process.returncode != 0:
            return process.returncode
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
