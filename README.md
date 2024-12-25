# MAC Changer

## Overview
A command-line utility to modify the MAC address of a specified network interface. Automates bringing the interface down, changing the MAC, and bringing it back up.

## Features
- Randomly generates valid MAC addresses.
- Automates interface management.
- Provides clear error messages.

## Prerequisites
- Linux OS with `libc-dev` and `linux-headers`.
- GCC or compatible compiler.
- Root privileges.

## Compilation
```bash
gcc -o macchanger macchanger.c
```

## Usage
```bash
sudo ./macchanger <network-interface>
```
Example:
```bash
sudo ./macchanger enp0s3
```

## How It Works
1. Brings the interface down.
2. Generates and sets a new MAC address.
3. Brings the interface back up.

## Limitations
- Requires root privileges.
- Tested on Linux only.

## License
Open-source under the MIT License.

