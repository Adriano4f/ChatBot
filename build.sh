set -euo pipefail

BUILD_DIR="build"
BIN_DIR="bin"

mkdir -p "$BUILD_DIR"
cmake -S . -B "$BUILD_DIR"

mkdir -p "$BIN_DIR"
cmake --build "$BUILD_DIR"
