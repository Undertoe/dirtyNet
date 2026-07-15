#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parents[1]

SERVER_EXE = Path("sandbox/tcp-multi/server/sandbox_tcp_multi_server")
CLIENT_EXE = Path("sandbox/tcp-multi/client/sandbox_tcp_multi_client")


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
        description="Run the TCP multi-client sandbox scaffolds."
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=default_build_dir(),
        help="CMake build directory. Defaults to BUILD_DIR, then _build, then build.",
    )
    parser.add_argument(
        "--port",
        type=int,
        default=8090,
        help="Port passed to the server and client. Defaults to 8090.",
    )
    return parser.parse_args()


def require_executable(build_dir: Path, exe: Path) -> Path:
    exe_path = build_dir / exe
    if not exe_path.exists():
        raise FileNotFoundError(f"Missing executable: {exe_path}")
    if not os.access(exe_path, os.X_OK):
        raise PermissionError(f"Not executable: {exe_path}")
    return exe_path


def main() -> int:
    args = parse_args()
    if not 1 <= args.port <= 65_535:
        print(f"Invalid port: {args.port}", file=sys.stderr)
        return 2

    build_dir = args.build_dir.resolve()

    try:
        server = require_executable(build_dir, SERVER_EXE)
        client = require_executable(build_dir, CLIENT_EXE)
    except (FileNotFoundError, PermissionError) as error:
        print(error, file=sys.stderr)
        print("Build them first with: make tcp-multi", file=sys.stderr)
        return 1

    port = str(args.port)
    server_result = subprocess.run([server, port], cwd=build_dir, check=False)
    if server_result.returncode != 0:
        return server_result.returncode

    client_result = subprocess.run([client, port], cwd=build_dir, check=False)
    return client_result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
