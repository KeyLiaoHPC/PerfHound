#!/usr/bin/env python3
"""Summarize PerfHound CSV intervals (start/end tag pairs) per rank/cpu file."""

from __future__ import annotations

import argparse
import csv
import glob
import os
import statistics
import sys
from collections import defaultdict


def load_ctag(ctag_path: str) -> dict[tuple[int, int], str]:
    tags: dict[tuple[int, int], str] = {}
    with open(ctag_path, newline="") as fp:
        reader = csv.DictReader(fp)
        for row in reader:
            gid = int(row["gid"])
            pid = int(row["pid"])
            tags[(gid, pid)] = row["description"]
    return tags


def load_etag(etag_path: str) -> list[str]:
    names: list[str] = []
    with open(etag_path, newline="") as fp:
        reader = csv.DictReader(fp)
        for row in reader:
            names.append(row["name"])
    return names


def parse_rec_row(row: list[str]) -> tuple[int, int, int, int, list[int]]:
    gid = int(row[0])
    pid = int(row[1])
    cycle = int(float(row[2]))
    nsec = int(float(row[3]))
    evs = [int(float(v)) for v in row[5:]] if len(row) > 5 else []
    return gid, pid, cycle, nsec, evs


def summarize_file(
    csv_path: str,
    pairs: list[tuple[int, int, int]],
    ev_names: list[str],
) -> list[dict]:
    rows_by_key: dict[tuple[int, int], list[tuple[int, int, list[int]]]] = defaultdict(list)
    with open(csv_path, newline="") as fp:
        reader = csv.reader(fp)
        header = next(reader, None)
        if header is None:
            return []
        for row in reader:
            if not row:
                continue
            gid, pid, cycle, nsec, evs = parse_rec_row(row)
            rows_by_key[(gid, pid)].append((cycle, nsec, evs))

    out: list[dict] = []
    base = os.path.basename(csv_path)
    for gid, pid_start, pid_end in pairs:
        starts = rows_by_key.get((gid, pid_start), [])
        ends = rows_by_key.get((gid, pid_end), [])
        n = min(len(starts), len(ends))
        if n == 0:
            continue
        dcycles = [ends[i][0] - starts[i][0] for i in range(n)]
        dns = [ends[i][1] - starts[i][1] for i in range(n)]
        item = {
            "file": base,
            "gid": gid,
            "pid_start": pid_start,
            "pid_end": pid_end,
            "samples": n,
            "dcycle_min": min(dcycles),
            "dcycle_med": int(statistics.median(dcycles)),
            "dcycle_max": max(dcycles),
            "dns_min": min(dns),
            "dns_med": int(statistics.median(dns)),
            "dns_max": max(dns),
        }
        if ev_names:
            for ev_idx, ev_name in enumerate(ev_names):
                dev = []
                for i in range(n):
                    se = starts[i][2]
                    ee = ends[i][2]
                    if ev_idx < len(se) and ev_idx < len(ee):
                        dev.append(ee[ev_idx] - se[ev_idx])
                if dev:
                    item[f"dev_{ev_name}_med"] = int(statistics.median(dev))
        out.append(item)
    return out


def find_latest_run(data_root: str) -> str:
    runs = []
    for path in glob.glob(os.path.join(data_root, "run_*")):
        base = os.path.basename(path)
        if os.path.isdir(path) and base[4:].isdigit():
            runs.append(path)
    runs = sorted(runs)
    if not runs:
        raise SystemExit(f"No run_<N> directory under {data_root}")
    return runs[-1]


def default_pairs(tags: dict[tuple[int, int], str]) -> list[tuple[int, int, int]]:
    pairs: list[tuple[int, int, int]] = []
    by_gid: dict[int, list[int]] = defaultdict(list)
    for gid, pid in tags:
        if gid == 0:
            continue
        if "Start" in tags[(gid, pid)] or "End" in tags[(gid, pid)]:
            by_gid[gid].append(pid)
    for gid, pids in by_gid.items():
        pids = sorted(set(pids))
        starts = [p for p in pids if "Start" in tags[(gid, p)]]
        ends = [p for p in pids if "End" in tags[(gid, p)]]
        for ps, pe in zip(starts, ends):
            pairs.append((gid, ps, pe))
    if not pairs:
        pairs.append((1, 1, 2))
    return pairs


def main() -> int:
    parser = argparse.ArgumentParser(description="Summarize PerfHound run CSV intervals")
    parser.add_argument("data_root", help="ph_init root, e.g. ./ph_data/test3_asmmpi_x86")
    parser.add_argument("--run", help="run directory (default: latest run_*)")
    args = parser.parse_args()

    run_dir = args.run or find_latest_run(args.data_root)
    ctag_path = os.path.join(run_dir, "ctag.csv")
    etag_path = os.path.join(run_dir, "etag.csv")
    tags = load_ctag(ctag_path)
    ev_names = load_etag(etag_path) if os.path.exists(etag_path) else []
    pairs = default_pairs(tags)

    rec_files = glob.glob(os.path.join(run_dir, "*", "r*c*.csv"))
    if not rec_files:
        raise SystemExit(f"No r*c*.csv under {run_dir}")

    print(f"run={run_dir} files={len(rec_files)} pairs={pairs}")
    for rec in sorted(rec_files):
        for item in summarize_file(rec, pairs, ev_names):
            print(
                f"{item['file']} gid={item['gid']} "
                f"{item['pid_start']}->{item['pid_end']} "
                f"n={item['samples']} "
                f"dcycle_med={item['dcycle_med']} dns_med={item['dns_med']}",
                end="",
            )
            for key, val in item.items():
                if key.startswith("dev_") and key.endswith("_med"):
                    print(f" {key}={val}", end="")
            print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
