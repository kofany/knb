# Frequently Asked Questions

Common questions about Keep Nick Bot (KNB).

## General Questions

### What is KNB?

Keep Nick Bot (KNB) is an IRC bot designed to maintain control of preferred nicknames on IRC networks. It monitors configured nicknames and automatically acquires them when they become available.

### What platforms are supported?

KNB works on:
- Linux (all distributions)
- FreeBSD, OpenBSD, NetBSD
- SunOS/Solaris
- Cygwin (Windows)
- macOS/Darwin

### Do I need root/admin privileges?

No. KNB runs as a regular user. However, some features (PID file location, cron) depend on user permissions.

### Does KNB support SSL/TLS?

No, KNB does not currently support encrypted IRC connections. It connects via plain text on standard IRC ports (typically 6667).

## Installation

### How do I compile KNB?

```bash
cd src
./configure
make dynamic
```

See [Installation Guide](INSTALLATION.md) for details.

### What if I get compilation errors?

Ensure you have GCC and development libraries installed:
```bash
# Debian/Ubuntu
sudo apt-get install build-essential

# CentOS/RHEL  
sudo yum groupinstall "Development Tools"

# FreeBSD
pkg install gcc
```

### Can I use a pre-compiled binary?

You can build a release package with `make release` and distribute it, but binaries are platform-specific.

## Configuration

### Where is the configuration file?

You specify the path when starting the bot:
```bash
./knb /path/to/mybot.conf
```

### What are the minimum required settings?

- `nick` - Bot's primary nickname
- `nicks` - Nicknames to monitor
- `server` or `server6` - At least one IRC server

### How do I add myself as an authorized user?

After the bot connects, send:
```
/msg BotNick !new
```

This adds your current hostmask to the authorized users list.

### Can I edit the config while the bot is running?

Yes, but changes won't take effect until you restart the bot or use `!reload` (which reloads the userfile, not the main config).

## Operation

### How do I start the bot?

```bash
./knb mybot.conf
```

The bot forks to background by default (unless debug build).

### How do I stop the bot?

Use IRC command:
```
!die
```

Or kill the process:
```bash
kill $(cat pid.BotNick)
```

### How do I check if the bot is running?

```bash
ps aux | grep knb
# or
cat pid.BotNick  # Check PID file
```

### What if the bot crashes?

Use the watchdog script (`scripts/knbchk.sh`) with cron to automatically restart:
```bash
*/10 * * * * /home/user/knb/scripts/knbchk.sh
```

### Can the bot run on multiple servers simultaneously?

No. The bot connects to one server at a time, rotating through the configured server list on disconnect.

## Commands

### Why doesn't the bot respond to my commands?

Common reasons:
1. You're not an authorized user (add yourself with `!new`)
2. Your hostmask changed (add new mask with `!+host`)
3. Bot isn't online
4. Command syntax is incorrect

### What are the command prefixes?

Commands can start with `.`, `!`, or `@`:
```
!help
.help
@help
```

### How do I list all commands?

See [Command Reference](COMMANDS.md) for complete list.

### Can I remove myself from the user list by mistake?

Yes, be careful with `!-host`. If you remove all users including yourself, anyone can control the bot until someone adds themselves.

## Nickname Management

### How does nickname recovery work?

1. Bot periodically sends `ISON` command to check nickname availability
2. When preferred nick is available, bot changes to it
3. Bot maintains the nickname and stops checking others

### Why isn't the bot catching my nickname?

Check:
- Is catching enabled? (`!keepnick on`)
- Is the nickname configured in `nicks` option?
- Is there a queue? (`!debug` in debug mode)
- Are there penalty delays? (anti-flood protection)

### Can I add nicknames at runtime?

Yes, use `!+tempnick NickName`. These are temporary and don't persist across restarts.

### What's the difference between nick and nicks?

- `nick` - Primary nickname the bot uses
- `nicks` - List of nicknames to monitor and try to acquire

## Channels

### How do I add a channel?

Via configuration:
```ini
channel #mychannel
channel #private secretkey
```

Or via IRC command:
```
!join #mychannel
!join #private secretkey
```

### Why didn't the bot rejoin after being kicked?

The bot should auto-rejoin unless:
- Channel was removed with `!part`
- Bot is disconnected
- Join failed (banned, channel limit, etc.)

### Can the bot manage channel ops?

Yes, if the bot has operator status. Use:
- `!op <nick>` - Give ops
- `!deop <nick>` - Remove ops
- `!kick <channel> <nick>` - Kick user
- `!ban <channel> <mask>` - Set ban

## Security

### How are users authenticated?

Via IRC hostmask matching with wildcard support:
- `*` - Any sequence of characters
- `?` - Exactly one character
- `#` - Exactly one digit

Example: `*!user@*.example.com`

### Are passwords supported?

No. Authentication is hostmask-only.

### What if my IP changes?

Add a broader hostmask that covers your IP range:
```
!+host *!user@*.isp.com
```

### Is the connection encrypted?

No, KNB doesn't support SSL/TLS. All communication is plain text.

### Should I run this on a public server?

Only if you trust the network and your authorized users. There's no password protection, only hostmask-based access control.

## Advanced

### Can I update the bot remotely?

Yes, use the `!update` command (requires properly configured update server).

### Where is the userfile stored?

By default: `<nick>.uf` in the bot directory. Configure with `userfile` option.

### How do I backup the userfile?

```
!backup
```

Creates a backup file named `~<userfile>` (e.g., `~knb.uf`).

### What's the binary validator?

A security feature that validates the bot binary hasn't been tampered with using MD5 checksums. As of v0.2.6, validation mismatches produce warnings but don't stop execution.

### Can I run multiple bots on the same config?

No, they would conflict (PID file, userfile). Create separate configurations for multiple bots.

### How do I enable debug mode?

Compile with debug target:
```bash
cd src
./configure
make debug
```

Then run normally. The bot will stay in foreground and log to `debug.log`.

### What's the penalty system?

Flood protection mechanism that adds artificial delays after sending IRC commands to prevent the server from disconnecting the bot for flooding.

### Can I disable nickname catching temporarily?

Yes:
```
!keepnick off
```

Re-enable with:
```
!keepnick on
```

## Troubleshooting

### The bot won't connect

Check:
- Server hostname is correct and resolvable
- Port is correct (default 6667)
- Network connectivity
- Firewall rules

### The bot connects but doesn't join channels

Check:
- Channels are configured (`!channels` to list)
- Bot isn't banned
- Channel keys are correct (if required)

### Commands don't work

Verify:
- You're an authorized user (`!hosts` to check)
- Command syntax is correct
- Bot is connected and responsive (`!uptime`)

### The bot uses too much CPU

This is unusual. Possible causes:
- Rapid reconnection loop (check logs)
- Excessive penalty causing constant delays
- Bug (report with debug info)

### How do I report a bug?

Gather:
- Bot version (`!version`)
- Platform and compiler version
- Configuration (sanitized)
- Steps to reproduce
- Relevant logs

Then report via project issue tracker or contact author.

## Cron/Watchdog

### How does the watchdog work?

The `knbchk.sh` script checks if the bot process is running by reading the PID file and checking the process list. If the bot isn't running, it restarts it.

### How do I set up cron monitoring?

Run:
```bash
cd scripts
./cron.sh
```

Or manually add to crontab:
```bash
*/10 * * * * /home/user/knb/scripts/knbchk.sh >/dev/null 2>&1
```

### Why isn't cron restarting my bot?

Check:
- Cron job is installed (`crontab -l`)
- Script paths are correct
- Script has execute permissions
- PID file location matches script configuration

## Contributing

### How can I contribute?

- Report bugs
- Suggest features
- Submit patches
- Improve documentation
- Share configuration examples

See [Development Guide](DEVELOPMENT.md) for details.

### Where's the source code?

The KNB source code is distributed as tarballs or available via version control.

## Additional Help

For more detailed information, see:
- [Installation Guide](INSTALLATION.md)
- [Configuration Reference](CONFIGURATION.md)
- [Command Reference](COMMANDS.md)
- [Architecture Overview](ARCHITECTURE.md)
- [Troubleshooting](TROUBLESHOOTING.md)
- [Deployment Guide](DEPLOYMENT.md)

---

Still have questions? Contact the project maintainers or community.
