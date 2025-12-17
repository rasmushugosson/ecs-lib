#!/usr/bin/env bash
set -euo pipefail

USER_CONFIG="${1:-Debug}"

MAKE_CONFIG="$(printf '%s' "$USER_CONFIG" | tr '[:upper:]' '[:lower:]')"

BUILD_DIR="build/linux-clang"

echo "==> Using make configuration: $MAKE_CONFIG"

echo "==> Running premake5 gmake-clang"
premake5 gmake-clang

echo "==> Entering ${BUILD_DIR}"
pushd "${BUILD_DIR}" > /dev/null

echo "==> Running bear with make (config=${MAKE_CONFIG})"
bear -- make config="${MAKE_CONFIG}" -j"$(nproc)"

popd > /dev/null

if [[ -f "${BUILD_DIR}/compile_commands.json" ]]; then
    echo "==> Linking compile_commands.json to project root"
    ln -sf "${BUILD_DIR}/compile_commands.json" compile_commands.json
    echo "Done. clangd should now see all include paths."
else
    echo "ERROR: compile_commands.json not found in ${BUILD_DIR}"
    exit 1
fi
