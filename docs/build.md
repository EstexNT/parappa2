# Build guide

## Table of contents

- [Build guide](#build-guide)
  - [Table of contents](#table-of-contents)
  - [Initial setup](#initial-setup)
  - [Build](#build)
  - [Generate/update the progress report](#generateupdate-the-progress-report)
  - [asm-differ setup](#asm-differ-setup)

## Initial setup

1. `sudo apt install gcc-mipsel-linux-gnu binutils-mips-linux-gnu ninja-build`
2. `sudo apt install python3-pip`
3. `python3 -m pip install -U -r requirements.txt`

*Note: For compiling, you need to add the `i386` architecture. For instructions on that please check out [this guide](https://askubuntu.com/questions/454253/how-to-run-32-bit-app-in-ubuntu-64-bit) if you're on Ubuntu.*<br>
*Note: It's recommended that you set up a Python virtual environment.*

4. Create a folder named `iso` in the root of the repo.
5. Extract the following files from the July 12th prototype and place it in said `iso` folder:
    - `SCPS_150.17` (main executable - found in the root)
    - `TAPCTRL.IRX` (found in the `IRX` folder)
    - `WAVE2PS2.IRX` (found in the `IRX` folder)
    - Every file (*.OLM) in the `MDL` folder.

6. `./tools/setup.py`

## Build
1. `./configure.py`
2. `ninja`

After running `ninja`, you should see something like the following:
```
[238/248] sha1sum config/irx.wave2ps2.jul12.checksum.sha1
build/WAVE2PS2.IRX.rom: OK
[248/248] sha1sum config/p3.jul12.checksum.sha1
build/SCPS_150.17.rom: OK
```

## Generate/update the progress report
1. After a successful build, run `./configure.py --objdiff`
2. `./tools/objdiff/objdiff-cli report generate -p . -o progress/report.json -f json-pretty`

*Note: You will need to make a clean build by running the configure script again before attempting to run ninja.*

## asm-differ setup
1. After a successful build, create a folder named `expected`.
2. Copy the `build` folder inside the `expected` folder.
3. `./tools/diff/diff.py -mwo FUNCTION_NAME`
    - Use the `-t` argument to diff functions in IRX modules.
    - Possible targets are: `ee`/`wp2cd`/`tapctrl`. Default is `ee`. The `tapctrl` target will not work as of writing.
    - Example: `./tools/diff/diff.py -mwo -t wp2cd FUNCTION_NAME`.

*Note: Every time you add a new symbol, you must do the asm-differ setup again so they appear in the diff view.*
