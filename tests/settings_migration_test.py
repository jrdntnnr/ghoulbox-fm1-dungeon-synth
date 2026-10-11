#!/usr/bin/env python3
"""The two conflicting PER4 layouts must stay distinct during archive conversion."""
import base64
import copy
import importlib.util
import struct
import unittest
import zlib
from pathlib import Path

spec = importlib.util.spec_from_file_location("migration", Path(__file__).parents[1] / "tools/migrate_settings.py")
migration = importlib.util.module_from_spec(spec)
spec.loader.exec_module(migration)


class SettingsMigration(unittest.TestCase):
    def test_source_fields_and_archive_integrity(self):
        data = bytearray(572)
        struct.pack_into("<I", data, 0, 0x50455234)
        row14, row15 = len(data) - 520 + 448, len(data) - 520 + 480
        data[row14:row14 + 32] = bytes(range(32))
        data[row15 + 27] = 3
        data[row15 + 28] = 1
        data[row15 + 30] = 0
        obj = lambda i, b: dict(id=i, size=len(b), crc=zlib.crc32(b), data=base64.b64encode(b).decode())
        archive = dict(format="felucca-backup", version=1, objects=[obj(0,b"music"),obj(1,data)])
        for source in ("felucca", "ghoulbox"):
            out = migration.migrate(copy.deepcopy(archive), source)
            self.assertEqual(out["objects"][0], archive["objects"][0])
            o = out["objects"][1]
            b = base64.b64decode(o["data"])
            self.assertEqual((len(b), zlib.crc32(b)), (o["size"],o["crc"]))
            self.assertEqual(struct.unpack_from("<I",b)[0],0x47425331)
            if source == "felucca":
                self.assertEqual(b[-32:], bytes(range(32)))
                self.assertEqual(b[row14:row14 + 32], bytes(32))
                self.assertEqual((b[row15+12],b[row15+13],b[row15+27],b[row15+28]),(0,1,3,1))
            else:
                self.assertEqual(b[-32:], bytes(32))
                self.assertEqual(b[row14:row14 + 32], bytes(range(32)))
                self.assertEqual((b[row15+12],b[row15+13],b[row15+27],b[row15+28]),(3,1,0,0))
            self.assertEqual(b[row15+30],0)
        broken = copy.deepcopy(archive)
        broken["objects"][0]["crc"] ^= 1
        with self.assertRaises(ValueError): migration.migrate(broken,"felucca")
        with self.assertRaises(ValueError): migration.convert(bytes(572),"felucca")
        with self.assertRaises(ValueError): migration.convert(data,"guess")
        with self.assertRaises(ValueError): migration.convert(data + bytes(4),"ghoulbox")


if __name__ == "__main__": unittest.main()
