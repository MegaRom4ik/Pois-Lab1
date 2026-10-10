"""Require strictly more than 90% line coverage for each domain implementation."""
import json
import sys

with open(sys.argv[1], encoding="utf-8") as source:
    files = json.load(source)["data"][0]["files"]
expected = {"Vector3.cpp", "TicTacToe.cpp"}
seen = set()
for item in files:
    name = item["filename"].split("/")[-1]
    if name not in expected:
        continue
    seen.add(name)
    lines = item["summary"]["lines"]
    percentage = 100 * lines["covered"] / lines["count"] if lines["count"] else 0
    print(f"{name}: {percentage:.2f}% lines")
    if percentage <= 90:
        sys.exit(f"Coverage must exceed 90%: {name}")
if seen != expected:
    sys.exit("Missing domain coverage data")
