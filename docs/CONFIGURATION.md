# Configuration Reference

Complete reference for all KNB configuration options.

## Table of Contents

- [Configuration File Format](#configuration-file-format)
- [Required Options](#required-options)
- [Optional Options](#optional-options)
- [Server Configuration](#server-configuration)
- [Channel Configuration](#channel-configuration)
- [User Authentication](#user-authentication)
- [CTCP Configuration](#ctcp-configuration)
- [Advanced Options](#advanced-options)
- [Complete Examples](#complete-examples)

## Configuration File Format

### Basic Syntax

```ini
# Comments start with # or ;
option_name value
option_name value1 value2  # Multiple values on same line
```

### Rules

1. **Comments**: Lines starting with `#` or `;` are ignored
2. **Whitespace**: Leading/trailing whitespace is ignored
3. **Case-Sensitive**: Option names are case-sensitive
4. **One Option Per Line**: Each configuration option on separate line
5. **No Quotes**: Values don't need quotes (they're part of the value if used)

### File Location

The configuration file path is specified as a command-line argument:

```bash
./knb /path/to/config.conf
```

## Required Options

These options **must** be present in your configuration:

### nick

**Syntax**: `nick <nickname>`

**Description**: The primary nickname the bot will use and try to maintain.

**Example**:
```ini
nick MyBot
```

**Constraints**:
- Maximum 15 characters (IRC standard)
- Should be a valid IRC nickname
- Cannot be duplicated in config

### nicks

**Syntax**: `nicks <nick1> <nick2> <nick3> ...`

**Description**: List of nicknames the bot will monitor and try to acquire.

**Example**:
```ini
nicks a b c d e f g h i j k l m n o p q r s t u v w x y z
nicks SingleLetters OneTwoThree TestNick
```

**Constraints**:
- Maximum 40 nicknames
- Each nickname max 15 characters
- Space-separated
- No duplicates allowed
- Case-insensitive comparison

### server / server6

**Syntax**: 
- `server <hostname> [port]` - IPv4 or hostname
- `server6 <hostname> [port]` - IPv6 (requires IPv6 support)

**Description**: IRC servers to connect to. Bot will try servers in order until successful.

**Example**:
```ini
server irc.example.net 6667
server irc2.example.net
server6 irc6.example.net 6667
```

**Constraints**:
- Maximum 20 servers total
- Default port: 6667
- Port range: 1-65535
- Hostname max 255 characters
- Hostname must be resolvable
- IPv6 requires `--with-ipv6` compile option

## Optional Options

### altnick

**Syntax**: `altnick <on|off>`

**Description**: Enable/disable alternative nickname on collision.

**Default**: `off`

**Example**:
```ini
altnick on
```

When enabled and primary nick is taken, bot will append characters from `nickappend` to create alternative.

### nickappend

**Syntax**: `nickappend <characters>`

**Description**: Characters to append when generating alternative nickname.

**Default**: `abcdefghijklmnopqrstuvwxyz` (if altnick is on)

**Example**:
```ini
altnick on
nickappend _-0123456789
```

Bot will try: `MyBot_`, `MyBot-`, `MyBot0`, etc.

**Constraints**:
- Maximum 40 characters
- Only used when `altnick on`

### noaltnick

**Syntax**: `noaltnick <on|off>`

**Description**: Prevent bot from using UID as nickname on registration failure.

**Default**: `off`

**Example**:
```ini
noaltnick on
```

Used in specific situations where UID nicknames are undesirable.

### ident

**Syntax**: `ident <username>`

**Description**: IRC username/ident field.

**Default**: System username or nick

**Example**:
```ini
ident mybot
```

**Constraints**:
- Maximum 15 characters
- Appears in hostmask as `nick!ident@host`

### realname

**Syntax**: `realname <real name text>`

**Description**: IRC real name (GECOS) field.

**Default**: None

**Example**:
```ini
realname Keep Nick Bot - http://example.com/knb
```

**Constraints**:
- Maximum 160 characters
- Can contain spaces

### logfile

**Syntax**: `logfile <filename>`

**Description**: Path to log file for IRC events.

**Default**: None (no logging)

**Example**:
```ini
logfile /home/user/knb/logs/knb.log
logfile knb.log
```

**Logged Events**:
- Nickname changes
- JOIN/PART/QUIT/KICK
- MODE changes
- Some other significant events

### userfile

**Syntax**: `userfile <filename>`

**Description**: Path to user database file (stores authorized users and channels).

**Default**: `<nick>.uf`

**Example**:
```ini
userfile /home/user/knb/data/knb.uf
userfile mybot.uf
```

**Note**: Bot creates this file automatically if it doesn't exist.

### pidfile

**Syntax**: `pidfile <filename>`

**Description**: Path to PID file (process ID).

**Default**: `pid.<nick>`

**Example**:
```ini
pidfile /var/run/knb.pid
pidfile knb.pid
```

Used by watchdog scripts to monitor bot process.

### vhost

**Syntax**: `vhost <ip_address>`

**Description**: IP address to bind for outgoing connections (virtual host).

**Default**: System default interface

**Example**:
```ini
vhost 192.0.2.100
vhost 2001:db8::1
```

**Use Cases**:
- Multiple IPs on server
- IPv6 configuration
- Custom hostmask display

**Note**: Cannot combine IPv4 and IPv6 servers when vhost is set.

## Server Configuration

### Multiple Servers

The bot tries servers in order and rotates on disconnection:

```ini
server irc1.example.net 6667
server irc2.example.net 6667
server irc3.example.net 6697
```

### IPv4 and IPv6

Mix IPv4 and IPv6 servers (without vhost):

```ini
server irc.example.net 6667     # IPv4 or hostname
server6 irc6.example.net 6667   # IPv6
server another.example.net       # Uses default port 6667
```

### Custom Ports

```ini
server irc.example.net 6697     # SSL port
server irc.example.net 7000     # Custom port
```

## Channel Configuration

### channel

**Syntax**: `channel <#channel> [key]`

**Description**: Channels to auto-join on connect.

**Example**:
```ini
channel #bots
channel #private secretkey
channel #test
```

**Constraints**:
- Maximum 21 channels
- Channel name max 50 characters
- Key max 50 characters
- Must start with valid channel prefix (#, &, +, !)

### Channel Behavior

- Bot auto-joins on connect
- Rejoins after kick
- Persisted in userfile
- Can add/remove via IRC commands

## User Authentication

User authentication uses hostmask matching. The userfile stores authorized users.

### Initial User

Add your first user via IRC after bot connects:

```
/msg BotNick !new
```

or on a channel:

```
!new
```

This adds your current hostmask.

### Hostmask Format

```
nick!ident@host
```

### Wildcards

- `*` - Matches any sequence of characters
- `?` - Matches exactly one character
- `#` - Matches exactly one digit

### Examples

```
*!*@192.0.2.100           # Specific IP
*!user@*.example.com      # Domain match
*!~user@*.isp.net        # ISP match
admin!*@*                 # Specific nickname only
*!*@2001:db8::/32        # IPv6 network
```

### Security Notes

- No users = anyone can control bot (adds themselves)
- First user added = full control
- User database persists in userfile
- Backup userfile regularly with `!backup` command

## CTCP Configuration

### ctcptype

**Syntax**: `ctcptype <0|1|2>`

**Description**: CTCP response behavior.

**Values**:
- `0` - Disabled (no CTCP responses)
- `1` - Friends only (authorized users only) - **default**
- `2` - Everyone (respond to all CTCP requests)

**Example**:
```ini
ctcptype 2
```

### ctcpreply

**Syntax**: `ctcpreply <reply text>`

**Description**: Custom CTCP VERSION reply.

**Default**: `[PT] Pojeby Team (Keep nick bot) v<version> by Esio (tahioSyndykat Version)`

**Example**:
```ini
ctcpreply My Custom Bot v1.0
```

**Constraints**:
- Maximum 255 characters
- Don't include `\001` delimiters (added automatically)

### CTCP Commands

Bot responds to:
- **VERSION**: Returns version string
- **TIME**: Returns current time
- **PING**: Returns ping argument
- (Others may be supported)

## Advanced Options

### replytype

**Syntax**: `replytype <msg|notice>`

**Description**: How bot sends command responses.

**Values**:
- `notice` - Send as NOTICE (default, less intrusive)
- `msg` - Send as PRIVMSG (more visible)

**Example**:
```ini
replytype notice
```

### publicreply

**Syntax**: `publicreply <on|off>`

**Description**: Allow bot to respond to commands on channels.

**Default**: `off` (private responses only)

**Example**:
```ini
publicreply on
```

**Behavior**:
- `off`: Always responds via private message to user
- `on`: Responds in channel if command was in channel, private otherwise

### reason

**Syntax**: `reason <text>`

**Description**: Default reason for PART/KICK/QUIT messages.

**Default**: `[PT] Pojeby Team`

**Example**:
```ini
reason Bot maintenance in progress
```

**Constraints**:
- Maximum 64 characters

### delay

**Syntax**: `delay <seconds>`

**Description**: Additional delay/penalty for NICK/ISON commands.

**Default**: `0`

**Range**: `0-10` seconds

**Example**:
```ini
delay 2
```

**Purpose**: Avoid triggering anti-flood systems on some networks.

### alphabet

**Syntax**: `alphabet <on|off>`

**Description**: Auto-join special alphabet channel when catching single-letter nick.

**Default**: `on`

**Example**:
```ini
alphabet off
```

**Note**: Internal feature from special builds, mostly for historical compatibility.

### dontfork (Debug Only)

**Syntax**: `dontfork <on|off>`

**Description**: Don't fork to background (run in foreground).

**Default**: `off`

**Example**:
```ini
dontfork on
```

**Only available in debug builds** (`make debug`).

## Complete Examples

### Basic IPv4 Setup

```ini
# Basic configuration
nick MyBot
nicks a b c d e f g h i j k l m n o p q r s t u v w x y z

# Servers
server irc.example.net 6667
server irc2.example.net 6667

# Channels
channel #mybots
channel #test

# Optional
realname Keep Nick Bot
ident mybot
```

### IPv4 with Virtual Host

```ini
nick KnbBot
nicks letters singlechar test1 test2
vhost 192.0.2.100
realname Knb with vhost

server irc.example.net 6667
server irc2.example.net 6667

channel #bots password123
channel #test

logfile /home/user/knb/knb.log
pidfile /home/user/knb/knb.pid
userfile /home/user/knb/knb.uf
```

### IPv6 Setup

```ini
nick Knb6
nicks a b c d e
realname IPv6 Bot

server6 irc6.example.net 6667
server6 irc6.alternate.net 6667

channel #ipv6
channel #bots

ctcptype 2
replytype notice
```

### Dual Stack (IPv4 + IPv6)

```ini
nick DualBot
nicks test1 test2 test3

# Mix IPv4 and IPv6 (no vhost)
server irc.example.net 6667
server6 irc6.example.net 6667
server backup.example.net

channel #network

realname Dual Stack Bot
ctcptype 1
publicreply on
reason Maintenance
```

### Production Setup

```ini
# Bot identity
nick ProductionBot
realname Production IRC Bot - https://example.com/bot
ident prodbot

# Nicknames to manage
nicks prod backup temp1 temp2

# IRC servers
server irc1.prod.net 6667
server irc2.prod.net 6667
server irc3.prod.net 6667

# Channels
channel #operations opspassword
channel #monitoring
channel #alerts

# Files
logfile /var/log/knb/knb.log
pidfile /var/run/knb/knb.pid
userfile /var/lib/knb/knb.uf

# CTCP
ctcptype 1
ctcpreply Production Bot v1.0 - Operations Use Only

# Reply behavior
replytype notice
publicreply off
reason Scheduled maintenance

# Anti-flood
delay 1

# Features
alphabet off
altnick on
nickappend _-
```

## Configuration Validation

### Testing Configuration

Test your configuration file:

```bash
./knb mybot.conf
```

Bot will:
1. Parse configuration
2. Print any errors with line numbers
3. Exit if critical errors found
4. Connect if configuration is valid

### Common Errors

**Error**: `error: mandatory option 'nick' not set`
- **Fix**: Add `nick <nickname>` line

**Error**: `error: mandatory option 'nicks' not set`
- **Fix**: Add `nicks <nick1> <nick2> ...` line

**Error**: `error: mandatory option 'server' or 'server6' not set`
- **Fix**: Add at least one `server` or `server6` line

**Error**: `argument too long for 'realname'`
- **Fix**: Shorten realname to 160 characters or less

**Error**: `unknown host for 'server'`
- **Fix**: Check hostname is correct and resolvable

**Error**: `duplicate entries for 'nick'`
- **Fix**: Remove duplicate lines

**Error**: `overrange in 2nd argument for 'server'`
- **Fix**: Port must be 1-65535

## Next Steps

- Learn IRC commands: [Command Reference](COMMANDS.md)
- Deploy in production: [Deployment Guide](DEPLOYMENT.md)
- Troubleshoot issues: [Troubleshooting](TROUBLESHOOTING.md)

---

For questions, see [FAQ](FAQ.md).
