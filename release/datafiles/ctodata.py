#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2009 Blender Authors
#
# SPDX-License-Identifier: GPL-2.0-or-later

__all__ = (
    "__all__",
)

import sys


def main() -> int:
    argv = sys.argv[:]

    strip_byte = False
    if "--strip-byte" in argv:
        argv.remove("--strip-byte")
        strip_byte = True

    if len(argv) < 2:
        sys.stdout.write("Usage: ctodata <c_file> [--strip-byte]\n")
        return 1

    filename = argv[1]

    try:
        fpin = open(filename, "r")
    except:
        sys.stdout.write("Unable to open input {:s}\n".format(argv[1]))
        return 1

    data_as_str = fpin.read().rsplit("{")[-1].split("}")[0]
    data_as_str = data_as_str.replace(",", " ")
    data_as_list = [int(v) for v in data_as_str.split()]
    del data_as_str

    if strip_byte:
        # String data gets trailing byte.
        last = data_as_list.pop()
        assert last == 0

    data = bytes(data_as_list)
    del data_as_list

    dname = filename + ".ctodata"

    sys.stdout.write("Making DATA file <{:s}>\n".format(dname))

    try:
        fpout = open(dname, "wb")
    except:
        sys.stdout.write("Unable to open output {:s}\n".format(dname))
        sys.exit(1)

    size = fpout.write(data)

    sys.stdout.write("{:d}\n".format(size))
    return 0


if __name__ == "__main__":
    sys.exit(main())
