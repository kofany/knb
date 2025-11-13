# Development Guide

Guidelines for contributing to Keep Nick Bot (KNB).

## Table of Contents

- [Project Structure](#project-structure)
- [Build Workflow](#build-workflow)
- [Coding Standards](#coding-standards)
- [Development Environment](#development-environment)
- [Testing](#testing)
- [Debugging](#debugging)
- [Adding Features](#adding-features)
- [Creating Releases](#creating-releases)
- [Documentation](#documentation)

## Project Structure

```
knb/
├── src/                    # Source code
│   ├── *.c / *.h           # Modules
│   ├── configure           # Configure wrapper (POSIX shell)
│   ├── configure.sh        # Configure implementation
│   └── Makefile*           # Generated build files
├── cfg-examples/          # Sample configuration files
├── scripts/               # Helper scripts
│   ├── knbchk.sh         # Watchdog script
│   └── cron.sh           # Cron helper script
├── docs/                  # Documentation (Markdown)
├── CHANGELOG              # Project history
├── README.md              # Project overview
└── README                 # Legacy text README
```

## Build Workflow

### 1. Configure

Generate Makefiles tailored to your platform:

```bash
cd src
./configure [options]
```

Options:
- `--without-ipv6` - Disable IPv6 support
- `--with-ipv6` - Enable IPv6 (useful on Cygwin)
- `--without-validator` - Disable binary validator

### 2. Build Targets

```bash
make dynamic   # Preferred build (default)
make static    # Statically linked binary
make debug     # Debug build with symbols
make release   # Build distributable package
make src       # Build source package
make clean     # Clean build artifacts
```

### 3. Output

Binaries are created in repository root:
- `knb-<version>-<platform>` - Compiled binary
- `knb` - Symlink to binary

### 4. Clean Build

```bash
cd src
./configure
make clean
```

This removes object files, binaries, generated Makefiles, and validator/key artifacts.

## Coding Standards

### Language

- C99-style code with POSIX extensions
- Avoid platform-specific APIs where possible

### Formatting

- Indentation: Tabs (consistent with existing code)
- Braces: K&R style
- Line length: Prefer <100 characters
- Naming: snake_case for functions/variables, uppercase for macros
- Comments: Use `/* ... */` for block comments, `//` not widely used

### Safety

- Use `snprintf` and `strncpy` (never `sprintf` or `strcpy`)
- Check buffer sizes before copying
- Validate all inputs (config, commands, network data)
- Clean up memory (free allocations, especially user list)

### Headers

- Include `prots.h` in source files (central include file)
- Update header prototypes when adding new functions
- Keep headers guard-protected (`#ifndef ... #define ...`)

### Error Handling

- Use `error()` for fatal errors
- Return `bool` from functions that can fail (`true`/`false`)
- Propagate error messages up the call chain

### Logging

- Use `loguj()` for logging when `logfile` is configured
- Use `printd()` for debug messages (debug build only)

## Development Environment

### Recommended Tools

- GCC / Clang
- GDB (debugging)
- Valgrind (memory checking)
- Wireshark or tcpdump (IRC traffic debugging)
- tmux/screen (managing sessions)

### Environment Variables

- `CC` - Specify compiler (e.g., `CC=clang ./configure`)
- `CFLAGS` - Additional compiler flags (set before `./configure`)
- `LDFLAGS` - Linker flags if needed

### Cross-Compilation

Not officially supported but feasible with custom toolchains. Update `CC`, `CFLAGS`, and `LFLAGS` accordingly before running `./configure`.

## Testing

### Unit Testing

No formal unit test suite currently exists. Manual testing procedures:

1. **Configuration Parsing**
   - Create multiple configuration files
   - Test invalid inputs and ensure errors reported

2. **Connection Testing**
   - Connect to test IRC server (e.g., `irc.libera.chat`)
   - Verify connection, registration, channel join

3. **Command Testing**
   - Test all commands (`!new`, `!join`, etc.)
   - Ensure permission checks work

4. **Nickname Recovery**
   - Simulate available/unavailable nicknames
   - Verify queue operation (`!ison`, `!nick`)

5. **Self-Update**
   - Mock update server or local HTTP server
   - Verify update process completes

6. **Persistence**
   - Modify user list, restart bot, ensure data persists

### Logging for Tests

- Enable `logfile` in configuration for test runs
- Review logs for errors or warnings

## Debugging

### Debug Build

Compile with debug symbols:
```bash
cd src
./configure
make debug
```

Run in foreground without daemonizing (`dontfork on` in config if desired).

### GDB

```bash
gdb --args ./knb mybot.conf
```

Use breakpoints in relevant functions:
- `got_cmd`
- `parse_line`
- `connect_bot`
- `check_input`

### Debug Logging

- `debug.log` in debug builds
- Use `printd()` for temporary debugging output (only active when `me.dontfork` is true)

### Core Dumps

Enable core dumps and run debug build to capture crash info.

```bash
ulimit -c unlimited
./knb mybot.conf
```

Analyze with GDB:
```bash
gdb ./knb core
```

## Adding Features

### New Commands

1. Modify `cmds.c`:
   - Add new `if(!strcasecmp(cmd, "newcmd"))` block in `got_cmd`
   - Parse arguments using `str2words`
   - Validate user permissions
   - Use `send_reply` for responses

2. Update documentation:
   - `docs/COMMANDS.md`
   - `README.md` command section
```

### New Configuration Options

1. Add constant in `cfg.h` (e.g., `#define CFG_NEWOPT "newopt"`)
2. Handle in `cfg.c:set_variable`
   - Parse arguments
   - Validate type/length/range
   - Store in `me` structure (`structs.h`)
3. Update defaults in `load_cfg`
4. Document in `cfg-examples/`
5. Update `docs/CONFIGURATION.md`

### New Modules

1. Create new `module.c` and `module.h`
2. Include header in `prots.h`
3. Add to build dependencies in `src/Makefile.*`
4. Document architecture changes in `docs/ARCHITECTURE.md`

## Creating Releases

1. Update `defines.h` with new version info
2. Update `CHANGELOG`
3. Update documentation (README, docs)
4. Build release package:
```bash
cd src
./configure
make release
```

Output: `knb-<version>-<platform>.tar.gz`

5. Build source package:
```bash
make src
```

Output: `knb-<version>-src.tar.gz`

6. Verify packages contain:
- Binary (for release)
- Scripts
- Example configs
- Documentation (README, CHANGELOG)
- Symlinks (`knb`, `conf`)

## Documentation

### Updating Documentation

- Primary docs in `docs/` directory
- README summarizes project
- Keep documentation synchronized with code changes

### Knowledge Base Contributions

- Add troubleshooting notes during bug fixes
- Update FAQ with common questions
- Document new command workflows

### Inline Comments

- Use comments sparingly in code
- Focus on clarifying complex logic (queue handling, update system)

## Continuous Improvement

While no automated test suite exists, consider future improvements:
- Develop unit tests for configuration parser
- Add integration tests for IRC connection simulation
- Automate build/testing with CI pipelines

## Resources

- [Architecture Overview](ARCHITECTURE.md)
- [Configuration Reference](CONFIGURATION.md)
- [Command Reference](COMMANDS.md)
- [Troubleshooting](TROUBLESHOOTING.md)
- [FAQ](FAQ.md)

---

For questions about contributing, reach out via project channels or issue tracker.
