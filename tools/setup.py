#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright 2025 polyproxy
# SPDX-License-Identifier: MIT

import os
import requests
import io
import zipfile
import shutil
import subprocess
import sys

CCC_VER = "v2.1"
CCC_NAME = f"ccc_{CCC_VER}_linux-glibc2.35-0ubuntu3.8"
CCC_URL = f"https://github.com/chaoticgd/ccc/releases/download/{CCC_VER}/{CCC_NAME}.zip"
CCC_PATH = "tools/ccc"

P3_ELF_PATH = "iso/SCPS_150.17"
P3_GP_VALUE = 0x3a0ef0

BASE_DUMP_DIR = "dump"
DISASM_DIR = "disasm"
SYMDUMP_DIR = "sym"

def setup_ccc():
    if os.path.exists(CCC_PATH):
        return

    print(f"Downloading CCC ({CCC_VER})...")

    #os.makedirs(CCC_PATH, exist_ok=True)

    r = requests.get(CCC_URL)
    with zipfile.ZipFile(io.BytesIO(r.content)) as z:
        z.extractall("tools/")
    shutil.move(os.path.join("tools", CCC_NAME), CCC_PATH)
    #os.rename(CCC_NAME, "ccc")

def dump_symbols():
    print("Dumping symbols...")

    stdump_p = f"{CCC_PATH}/stdump"
    if not os.path.exists(stdump_p):
        print(f"(error): stdump executable not found at \"{stdump_p}\"")
        return
    
    symdump_p = f"{BASE_DUMP_DIR}/{SYMDUMP_DIR}"
    os.chmod(stdump_p, 0o755)
    os.makedirs(symdump_p, exist_ok=True)

    # todo: do the same for the IRX files?
    p3_p = "iso/SCPS_150.17"
    commands = [
        [stdump_p, "functions", "--demangle-parameters", "-o", f"{symdump_p}/p3_functions.cpp", p3_p],
        [stdump_p, "globals", "-o", f"{symdump_p}/p3_globals.cpp", p3_p],
        [stdump_p, "types", "-o", f"{symdump_p}/p3_types.cpp", p3_p],
        [stdump_p, "files", "-o", f"{symdump_p}/p3_files.cpp", p3_p],
        [stdump_p, "includes", "-o", f"{symdump_p}/p3_includes.cpp", p3_p],
        [stdump_p, "json", "-o", f"{symdump_p}/p3.json", p3_p],
    ]

    for cmd in commands:
        subprocess.run(cmd, check=True)

def dump_asm():
    print("Dumping disassembly...")
    import spimdisasm

    # argparse reads from sys.argv
    sys.argv = ["spimdisasm.elfObjDisasm",
                "--instr-category", "r5900",
                "--compiler", "EEGCC",
                "--endian", "little",
                "--gp", f"{P3_GP_VALUE}",
                f"{P3_ELF_PATH}",
                f"{BASE_DUMP_DIR}/{DISASM_DIR}"]
    spimdisasm.elfObjDisasm.elfObjDisasmMain()

def main():
    print("--- parappa2 setup ---")
    setup_ccc()
    dump_symbols()
    dump_asm()
    print("------- done! --------")

if __name__ == "__main__":
    main()
