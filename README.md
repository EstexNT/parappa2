# PaRappa the Rapper 2 Decompilation
![build](https://img.shields.io/github/actions/workflow/status/parappadev/parappa2/build.yml?branch=main&label=build)
![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/total_progress.json)
![discord](https://img.shields.io/discord/302537923910303744?color=%235865F2&logo=discord&logoColor=%23FFFFFF)

<img src=".github/resources/transparent.png" style="margin:7px" align="right" width="20%" alt="PaRappa icon by pips">

A work-in-progress decompilation of [*PaRappa the Rapper 2*](https://en.wikipedia.org/wiki/PaRappa_the_Rapper_2) (パラッパラッパー2) for the PlayStation 2.<br>
We are currently targeting the July 12th NTSC-J prototype, but we aim to target the final NTSC-J/NTSC/PAL builds in the foreseeable future.<br><br>
For any questions, we have a home at the [PS1/PS2 Decompilation Discord server](https://discord.gg/VwCPdfbxgm) and the [PaRappa the Rapper Modding Community Discord server](https://discord.gg/xpvVnYd).

### Progress
*For a more detailed view of the progress, please check our [decomp(dot)dev page](https://decomp.dev/parappadev/parappa2).*

#### EE Core
| Folder | Progress | Description
|--------|----------|------------
| `dbug` | ![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/dbug_progress.json) | Debug utilities (VRAM save, debug menus, etc.)
| `os` | ![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/os_progress.json) | OS utilities (threading, pad, memory, etc.)
| `iop_mdl` | ![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/iop_mdl_progress.json) | IOP module control routines
| `main` | ![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/main_progress.json) | Gameplay code (score logic, loading screen, etc.)
| `menu` | ![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/menu_progress.json) | Menu code (UI flow, Memory Card saving, etc.)
| `prlib` | ![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/prlib_progress.json) | Game engine (rendering, models/animations, etc.)
| `src` | ![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/total_progress.json) | Total percentage

#### IOP modules
| Folder | Progress | Description
|--------|----------|------------
| `wavep2` | ![progress](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/parappadev/parappa2/main/progress/wp2cd.total_progress.json) | BGM and asset streaming
| `tapctrl` | N/A | Voice and sound effect playback

### Contributing

Please see our [build](docs/build.md) and [decompilation](docs/decompilation.md) guides.

