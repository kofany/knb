# Keep Nick Bot (KNB)

A robust POSIX C IRC bot designed to maintain control of preferred nicknames on IRC networks. KNB features a modular architecture with intelligent nickname recovery, multi-server support, and automated management capabilities.

## 🚀 Features

- **Intelligent Nickname Management**: Queue-based priority system for recovering preferred nicknames
- **Multi-Server Support**: Connect to multiple IRC servers with IPv4/IPv6 support
- **Channel Management**: Auto-join, part, cycle with key support
- **User Authentication**: Host mask-based access control with wildcard pattern matching
- **CTCP Support**: Configurable CTCP responses (VERSION, TIME, PING, etc.)
- **Flood Protection**: Built-in penalty system to prevent IRC flooding
- **HTTP Self-Update**: Automated bot updates via HTTP with MD5 validation
- **Persistent State**: User and channel configuration persistence
- **Watchdog Scripts**: Cron-based monitoring and automatic restart capabilities
- **Flexible Configuration**: Comprehensive configuration file with multiple examples
- **Virtual Host Support**: Configure custom vhost for IRC connections

## 📋 Table of Contents

- [Quick Start](#quick-start)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [Commands](#commands)
- [Documentation](#documentation)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

## ⚡ Quick Start

```bash
# Clone the repository (replace <repository-url> with your fork)
git clone <repository-url>
cd knb

# Configure and compile
cd src
./configure
make dynamic

# Copy example configuration
cp ../cfg-examples/conf-ipv4 ../mybot.conf

# Edit configuration (set nick, servers, channels)
nano ../mybot.conf

# Run the bot
cd ..
./knb mybot.conf
```

## 🔧 Installation

### Prerequisites

- **Compiler**: GCC 2.9.5 or later
- **Operating System**: Linux, BSD, SunOS, or Cygwin
- **Optional**: IPv6 support (enabled by default)
- **Optional**: GDB for debug builds

### Compilation

1. **Navigate to source directory**:
   ```bash
   cd knb/src
   ```

2. **Run configure script**:
   ```bash
   ./configure
   ```
   
   Configure options:
   - `--without-validator`: Disable binary validation
   - `--without-ipv6`: Disable IPv6 support (enabled by default on Linux/BSD/SunOS)
   - `--with-ipv6`: Enable IPv6 support (for Cygwin)

3. **Choose build type**:
   ```bash
   make dynamic    # Preferred for most systems
   make static     # Static linking
   make debug      # Debug build with symbols
   ```

4. **Build release package** (optional):
   ```bash
   make release    # Creates binary package
   make src        # Creates source package
   ```

5. **Clean build artifacts**:
   ```bash
   make clean      # Remove object files and binaries
   ```

### Post-Installation

The compiled binary will be placed in the parent directory as `knb-<version>-<platform>` with a symlink `knb` pointing to it.

## ⚙️ Configuration

### Basic Configuration File

Create a configuration file (e.g., `mybot.conf`):

```ini
# Required settings
nick Knb
nicks a b c d e f g h i j k l m n o p q r s t u v w x y z

# Server configuration
server irc.example.net 6667
server irc2.example.net 6667

# Optional settings
realname Keep Nick Bot - http://knb.example.com/
ident knb
logfile knb.log
userfile knb.uf
pidfile knb.pid

# Channel auto-join
channel #mybots
channel #test secretkey

# CTCP configuration
ctcptype 1                    # 0=disabled, 1=friends only, 2=all
ctcpreply Custom CTCP Reply

# Reply settings
replytype notice              # msg or notice
publicreply off               # Reply on channel or private only
reason Bot maintenance

# Advanced options
delay 0                       # Additional delay for NICK/ISON (0-10 seconds)
alphabet off                  # Auto-join alphabet channel
altnick on                    # Use alternative nick on collision
nickappend _                  # Chars to append for alt nick
```

### Configuration Examples

See `cfg-examples/` directory for various setups:
- `conf-ipv4`: Basic IPv4 configuration
- `conf-ipv6`: IPv6 only configuration
- `conf-ipv4-and-ipv6`: Dual-stack configuration
- `conf-ipv4-with-vhost`: IPv4 with virtual host
- `conf-ipv6-with-vhost`: IPv6 with virtual host

## 🎯 Usage

### Running the Bot

```bash
# Start with configuration file
./knb mybot.conf

# Update bot binary
./knb -u
```

### Initial Setup

1. **Start the bot** with your configuration file
2. **Add your first user** (via IRC):
   ```
   /msg Knb !new
   ```
   or on a channel where the bot is present:
   ```
   !new
   ```
   This adds your current hostmask as an administrator.

3. **Test commands**:
   ```
   !help       # Get help information
   !version    # Check bot version
   !uptime     # View bot uptime
   ```

### Hostmask Wildcards

When adding users, you can use wildcards in hostmasks:
- `*` - Matches any sequence of characters
- `?` - Matches exactly one character
- `#` - Matches exactly one digit

Example: `*!*user@*.example.com`

## 📝 Commands

Commands can be prefixed with `.`, `!`, or `@`.

### User Management
- `!new [mask]` - Add new user (yourself if no mask given)
- `!+host <mask>` / `!+user <mask>` - Add user hostmask
- `!-host [mask|number]` - Remove user hostmask
- `!hosts` / `!users` / `!masks` - List all users

### Channel Management
- `!join <channel> [key]` / `!j <channel> [key]` - Join channel
- `!part [channel]` / `!p [channel]` - Part channel (current if omitted)
- `!cycle [channel]` / `!rejoin [channel]` - Rejoin channel
- `!channels` / `!chans` - List joined channels

### Channel Operations
- `!op [channel] [nick]` - Give operator status
- `!deop [channel] [nick]` / `!dop [channel] [nick]` - Remove operator
- `!voice [channel] [nick]` / `!v [channel] [nick]` - Give voice
- `!devoice [channel] [nick]` / `!dv [channel] [nick]` - Remove voice
- `!kick <channel> <nick>` / `!k <channel> <nick>` - Kick user
- `!ban <channel> <mask>` / `!b <channel> <mask>` - Ban user
- `!unban <channel> <mask>` / `!ub <channel> <mask>` - Unban user
- `!invite <channel> [nick]` / `!i <channel> [nick]` - Invite user

### Nickname Management
- `!nick <nick>` / `!n <nick>` - Change to specified nick
- `!nick -yes <nick>` - Force nick change (even if caught)
- `!nicks` - List configured nicknames
- `!+tempnick <nick>` - Add temporary nickname to watch
- `!-tempnick <nick>` - Remove temporary nickname
- `!tempnicks` - List temporary nicknames
- `!ison` / `!check` - Check nickname availability
- `!keepnick [on|off]` / `!catch [on|off]` - Toggle nickname catching

### Bot Management
- `!version` / `!ver` - Show bot version
- `!uptime` / `!up` - Show bot uptime
- `!server <server> [port]` / `!s <server> [port]` - Change server
- `!server6 <server> [port]` / `!s6 <server> [port]` - Change to IPv6 server
- `!reconnect` / `!r` - Reconnect to IRC
- `!restart` / `!reboot` - Restart bot process
- `!die` / `!quit` / `!exit` / `!kill` - Shutdown bot
- `!update [--with-restart]` / `!u [--with-restart]` - Update bot binary
- `!reload` / `!rehash` - Reload configuration and userfile

### Data Management
- `!save` - Save userfile
- `!backup` - Create userfile backup

### Communication
- `!msg <target> <message>` / `!m <target> <message>` - Send message
- `!notice <target> <message>` - Send notice
- `!say <message>` - Say message in current context
- `!help` - Display help information

## 📚 Documentation

Comprehensive documentation is available in the `docs/` directory:

- **[Installation Guide](docs/INSTALLATION.md)** - Detailed installation instructions
- **[Configuration Reference](docs/CONFIGURATION.md)** - Complete configuration options
- **[Command Reference](docs/COMMANDS.md)** - All available commands
- **[Architecture Overview](docs/ARCHITECTURE.md)** - Technical architecture
- **[Development Guide](docs/DEVELOPMENT.md)** - Contributing guidelines
- **[FAQ](docs/FAQ.md)** - Frequently asked questions
- **[Troubleshooting](docs/TROUBLESHOOTING.md)** - Common issues and solutions
- **[Deployment Guide](docs/DEPLOYMENT.md)** - Production deployment

## 🏗️ Project Structure

```
knb/
├── src/                    # Source code
│   ├── main.c             # Main entry point
│   ├── cfg.c/cfg.h        # Configuration parsing
│   ├── irc.c/irc.h        # IRC protocol implementation
│   ├── events.c/events.h  # IRC event handlers
│   ├── cmds.c/cmds.h      # Command processing
│   ├── queue.c/queue.h    # Nickname queue management
│   ├── functions.c        # Utility functions
│   ├── update.c/update.h  # Self-update mechanism
│   ├── http.c/http.h      # HTTP client
│   ├── md5.c/md5.h        # MD5 hashing
│   ├── validate.c/validate.h  # Binary validation
│   ├── uf.c/uf.h          # User file handling
│   ├── match.c/match.h    # Pattern matching
│   ├── signals.c/signals.h # Signal handlers
│   ├── structs.h          # Data structures
│   ├── defines.h          # Constants and macros
│   ├── configure          # Build configuration script
│   └── Makefile*          # Build system
├── cfg-examples/          # Example configurations
├── scripts/               # Helper scripts
│   ├── knbchk.sh         # Cron watchdog script
│   └── cron.sh           # Cron setup script
├── docs/                  # Documentation
├── CHANGELOG              # Version history
└── README.md             # This file
```

## 🔌 Watchdog/Cron Monitoring

KNB includes watchdog scripts for automated monitoring and restart:

### Setup Cron Monitoring

1. **Edit the watchdog script** (`scripts/knbchk.sh`):
   ```bash
   KNBDIR="$HOME/knb"     # Bot directory
   NICK="knb"             # Bot nickname
   PIDFILE="pid.$NICK"    # PID file name
   CONF="conf"            # Config file name
   KNB="knb"              # Binary name
   ```

2. **Install to crontab**:
   ```bash
   # Check every 10 minutes
   */10 * * * * /home/user/knb/scripts/knbchk.sh >/dev/null 2>&1
   ```
   
   Or use the automated setup script:
   ```bash
   cd scripts
   ./cron.sh
   ```

The watchdog script checks if the bot is running via PID file and restarts it if necessary.

## 🛠️ Development

### Debug Build

Compile with debug symbols:
```bash
cd src
./configure
make debug
```

Debug features:
- No daemon fork (runs in foreground)
- Debug logging to `debug.log`
- Core dump generation on crash (requires GDB)
- Additional `!debug` command

### Architecture Components

- **Configuration System**: Robust parsing with validation
- **IRC Protocol**: Full RFC-compliant IRC implementation
- **Event Loop**: Non-blocking I/O with timeout handling
- **Queue System**: Priority-based nickname recovery
- **Penalty System**: Flood protection with automatic reset
- **Update System**: Fork-based HTTP update with validation
- **Persistence**: Binary userfile format with backup

## 🤝 Contributing

Contributions are welcome! Please see [DEVELOPMENT.md](docs/DEVELOPMENT.md) for guidelines.

### Reporting Issues

When reporting bugs, include:
- Bot version (`!version`)
- Operating system and compiler version
- Configuration file (sanitized)
- Relevant log output
- Steps to reproduce

## 📄 License

Keep Nick Bot (KNB) is developed by Esio (tahioSyndykat Version).

## 🔗 Links

- **Documentation**: See `docs/` directory
- **Configuration Examples**: See `cfg-examples/` directory
- **Scripts**: See `scripts/` directory

## ✨ Credits

- **Author**: Esio ([PT] Pojeby Team)
- **Version**: 0.2.6 (tahioSyndykat-1)
- **Contributors**: See CHANGELOG for complete history

---

For detailed information, please refer to the documentation in the `docs/` directory.
