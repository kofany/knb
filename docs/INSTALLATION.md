# Installation Guide

This guide provides detailed instructions for installing Keep Nick Bot (KNB) on various platforms.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Supported Platforms](#supported-platforms)
- [Installation Steps](#installation-steps)
- [Platform-Specific Notes](#platform-specific-notes)
- [Verification](#verification)
- [Troubleshooting](#troubleshooting)

## Prerequisites

### Required

- **C Compiler**: GCC 2.9.5 or later (or compatible compiler)
- **Make**: GNU Make or compatible
- **Standard C Libraries**: POSIX-compliant system libraries
- **Shell**: Bourne shell (sh) or bash

### Optional

- **IPv6 Support**: Enabled by default on Linux/BSD/SunOS
- **GDB**: For debug builds with core dump analysis
- **Cron**: For automated watchdog monitoring

### System Requirements

- **Disk Space**: ~5 MB for source, ~500 KB for binary
- **Memory**: Minimal (~2-5 MB runtime)
- **Network**: Access to IRC servers (ports 6667, 6697, or custom)

## Supported Platforms

KNB has been tested and confirmed working on:

### Fully Supported
- **Linux**: All major distributions (Debian, Ubuntu, CentOS, Fedora, Arch, etc.)
- **FreeBSD**: All recent versions
- **OpenBSD**: All recent versions
- **NetBSD**: All recent versions
- **SunOS/Solaris**: Version 10+

### Experimental
- **Cygwin**: Windows with Cygwin environment (IPv6 disabled by default)
- **macOS/Darwin**: Recent versions with Xcode command-line tools

## Installation Steps

### 1. Obtain Source Code

Clone or download the KNB source code:

```bash
# Clone repository
git clone <repository-url>
cd knb

# Or extract from tarball
tar -xzf knb-*.tar.gz
cd knb
```

### 2. Navigate to Source Directory

```bash
cd src
```

### 3. Run Configure Script

The configure script detects your system and generates appropriate Makefiles:

```bash
./configure
```

#### Configure Options

- `--without-ipv6` - Disable IPv6 support
- `--with-ipv6` - Enable IPv6 support (for Cygwin)
- `--without-validator` - Disable binary validation

**Examples:**

```bash
# Default configuration (IPv6 enabled on most platforms)
./configure

# Disable IPv6
./configure --without-ipv6

# Disable binary validator
./configure --without-validator

# Disable both
./configure --without-ipv6 --without-validator
```

#### Configure Output

You should see output similar to:

```
-+- Running configure
-+- Operating system: linux
-+- Architecture type: x86_64
-+- Gnu C Compiler version: 9.4.0
-+- IPv6 enabled: yes (./configure --without-ipv6 to turn off)
-+- Validator enabled: yes (./configure --without-validator to turn off)
-+- Generating Makefile
-+- Generating Makefile.key
-+- Generating Makefile.dynamic
-+- Generating Makefile.static
-+- Generating Makefile.debug
-+- Generating Makefile.release
-+- Generating Makefile.src
-+- Generating Makefile.clean
-+- Configure is done
```

### 4. Compile the Bot

Choose one of the following build targets:

#### Dynamic Build (Recommended)

```bash
make dynamic
```

This creates a dynamically linked binary, which is smaller and faster for most systems.

#### Static Build

```bash
make static
```

This creates a statically linked binary, useful for:
- Systems with older or incompatible libraries
- Portable deployment across different systems
- Container/chroot environments

#### Debug Build

```bash
make debug
```

This creates a debug build with:
- Debug symbols for GDB
- Additional logging to `debug.log`
- No daemon fork (runs in foreground)
- Extra `!debug` command

### 5. Verify Build

After compilation, the binary will be in the parent directory:

```bash
cd ..
ls -lh knb*
```

You should see:
- `knb-<version>-<platform>` - The actual binary
- `knb` - Symlink to the binary

### 6. Initial Configuration

Copy an example configuration:

```bash
cp cfg-examples/conf-ipv4 mybot.conf
```

Edit the configuration:

```bash
nano mybot.conf  # or vi, emacs, etc.
```

See [Configuration Reference](CONFIGURATION.md) for details.

## Platform-Specific Notes

### Linux

No special considerations. Works out of the box.

```bash
cd src
./configure
make dynamic
```

### FreeBSD

Use `gmake` if standard `make` has issues:

```bash
cd src
./configure
gmake dynamic
```

### OpenBSD

May need to specify GCC path if not default:

```bash
cd src
CC=egcc ./configure
make dynamic
```

### SunOS/Solaris

Requires socket libraries (automatically handled):

```bash
cd src
./configure
make dynamic
```

The Makefile automatically adds `-lsocket -lnsl` flags.

### Cygwin (Windows)

IPv6 is disabled by default. To enable:

```bash
cd src
./configure --with-ipv6
make dynamic
```

**Note**: Some IRC features may have limitations on Windows.

### macOS/Darwin

Works with Xcode command-line tools:

```bash
# Install Xcode tools if not present
xcode-select --install

cd src
./configure
make dynamic
```

The configure script automatically disables `-march=native` for Clang compatibility.

## Building Packages

### Binary Release Package

Create a distributable package:

```bash
cd src
./configure
make release
```

This creates `knb-<version>-<platform>.tar.gz` containing:
- Compiled binary
- Configuration examples
- Scripts
- Documentation
- CHANGELOG and README

### Source Package

Create a source distribution:

```bash
cd src
./configure
make src
```

This creates `knb-<version>-src.tar.gz` with all source files.

## Post-Installation

### 1. Setup Script Directory

Make scripts executable:

```bash
chmod +x scripts/*.sh
```

### 2. Setup Log Directory (Optional)

Create a logs directory:

```bash
mkdir logs
```

Update your configuration to use `logs/knb.log`.

### 3. Setup Cron Monitoring (Optional)

See [Deployment Guide](DEPLOYMENT.md) for cron setup.

## Verification

### Test Compilation

```bash
./knb --help
```

Should display usage information (or just start if no help option exists).

### Check Binary Information

```bash
file knb
ldd knb  # Linux/BSD - check dynamic libraries
strings knb | grep -i "version"
```

### Test Configuration Parsing

```bash
./knb mybot.conf
```

The bot should:
1. Parse configuration successfully
2. Fork to background (unless debug mode)
3. Create PID file (if configured)
4. Connect to IRC servers

Check logs or use `ps` to verify:

```bash
ps aux | grep knb
cat pid.knb  # or your configured pidfile
```

## Troubleshooting

### Configure Fails

**Problem**: `configure` script won't run

**Solution**:
```bash
chmod +x configure
./configure
```

Or use shell directly:
```bash
sh configure
```

### Compilation Errors

**Problem**: Missing headers (e.g., `netinet/in.h`)

**Solution**: Install development packages:

```bash
# Debian/Ubuntu
sudo apt-get install build-essential

# CentOS/RHEL
sudo yum groupinstall "Development Tools"

# FreeBSD
pkg install gcc
```

**Problem**: IPv6-related errors on older systems

**Solution**: Disable IPv6:
```bash
./configure --without-ipv6
make dynamic
```

### Linking Warnings

The original README says "don't care about warnings in linking" - these are generally harmless.

### Binary Validator Warnings

If you see "This file is hacked" warnings, it's informational only and doesn't prevent execution (as of version 0.2.6).

### Permission Denied

**Problem**: Cannot execute `knb` binary

**Solution**:
```bash
chmod +x knb
```

### Missing Libraries (Dynamic Build)

**Problem**: Error about missing shared libraries

**Solution**: Use static build:
```bash
cd src
make clean
make static
```

## Clean Build

To remove all compiled files and start fresh:

```bash
cd src
./configure
make clean
```

This removes:
- Object files (`*.o`)
- Generated Makefiles
- Key files
- Binary in parent directory

## Next Steps

- Configure the bot: [Configuration Reference](CONFIGURATION.md)
- Learn commands: [Command Reference](COMMANDS.md)
- Deploy in production: [Deployment Guide](DEPLOYMENT.md)
- Understand architecture: [Architecture Overview](ARCHITECTURE.md)

---

For additional help, see [FAQ](FAQ.md) and [Troubleshooting](TROUBLESHOOTING.md).
