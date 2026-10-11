#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Copy a .fm1backup archive with explicitly sourced PER4 settings converted to GBS1.

Felucca and GHOULBOX assigned different meanings to the same PER4 bytes.
Use --source felucca for a backup from upstream firmware, or --source ghoulbox
for the dungeon fork. The input archive is never overwritten.
"""
import argparse
import base64
import json
import struct
import zlib
from pathlib import Path


def convert(data, source):
    if source not in ("felucca", "ghoulbox"):
        raise ValueError("Choose felucca or ghoulbox as the settings source")
    if len(data) != 572 or struct.unpack_from("<I", data)[0] != 0x50455234:
        raise ValueError("Expected legacy PER4 settings; current GBS1 needs no conversion")
    out = bytearray(data) + bytearray(32)
    factory = len(data) - 520
    row14, row15 = factory + 14 * 32, factory + 15 * 32
    if source == "felucca":
        out[-32:] = out[row14:row14 + 32]
        out[row14:row14 + 32] = bytes(32)
        out[row15 + 12] = 0  # dungeon scene; screen timeout and click stay where they were
        out[row15 + 13] = 1  # preserve the owner's USB serial choice
    else:
        out[row15 + 12] = out[row15 + 27]
        out[row15 + 13] = out[row15 + 28] & 1
        out[row15 + 27] = out[row15 + 28] = 0
    struct.pack_into("<I", out, 0, 0x47425331)
    return bytes(out)


def migrate(archive, source):
    if archive.get("format") != "felucca-backup" or archive.get("version") != 1:
        raise ValueError("Expected a Felucca/GHOULBOX backup archive")
    objects = archive.get("objects", [])
    settings = [obj for obj in objects if obj.get("id") == 1]
    if len(settings) != 1:
        raise ValueError("Archive must contain exactly one settings object")
    for obj in objects:
        data = base64.b64decode(obj["data"], validate=True)
        if len(data) != obj["size"] or zlib.crc32(data) != obj["crc"]:
            raise ValueError(f"Invalid checksum or size in object {obj['id']}")
    obj = settings[0]
    data = convert(base64.b64decode(obj["data"]), source)
    obj.update(data=base64.b64encode(data).decode(), size=len(data), crc=zlib.crc32(data))
    return archive


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", required=True, choices=("felucca", "ghoulbox"))
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    result = migrate(json.loads(args.input.read_text()), args.source)
    with args.output.open("x") as f:
        json.dump(result, f, indent=2)
        f.write("\n")
    print(f"Converted {args.source} settings to GBS1: {args.output}")
