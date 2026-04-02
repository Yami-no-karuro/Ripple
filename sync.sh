#!/usr/bin/env bash

set -euo pipefail

MAX_PROC=1
declare -a DIR_LIST=(
    "/path/to/dir_a"    "/path/to/backup/dir_a"
    "/path/to/dir_b"    "/path/to/backup/dir_b"
    # ...
)

while getopts "c:" opt; do
    case $opt in
        c) MAX_PROC="$OPTARG" ;;
        \?) echo "Usage: $0 [..args]" >&2; exit 1 ;;
    esac
done

shift $((OPTIND-1))

if ! command -v ripple &> /dev/null; then
    echo "[!] Error, 'ripple' not found in PATH" >&2
    exit 1
fi

declare -A job_map
active_jobs=0
exit_code=0

for (( i=0; i<${#DIR_LIST[@]}; i+=2 )); do
    src="${DIR_LIST[$i]}"
    dst="${DIR_LIST[$i+1]}"

    mkdir -p "$dst"
    ripple "$src" "$dst" &
    pid=$!
    job_map[$pid]="$src -> $dst"
    (( active_jobs++ )) || true

    if (( active_jobs >= MAX_PROC )); then
        if ! wait -n -p finished_pid 2>/dev/null; then
            echo "[*] Warning, job failed: ${job_map[$finished_pid]}" >&2
            exit_code=1
        fi

        unset "job_map[$finished_pid]"
        (( active_jobs-- )) || true
    fi
done

for pid in "${!job_map[@]}"; do
    if ! wait "$pid"; then
        echo "[*] Warning, job failed: ${job_map[$pid]}" >&2
        exit_code=1
    fi
done

exit "$exit_code"
