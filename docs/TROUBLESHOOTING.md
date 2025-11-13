# Troubleshooting Guide

Common issues and solutions for Keep Nick Bot (KNB).

## Table of Contents

- [Installation Issues](#installation-issues)
- [Configuration Problems](#configuration-problems)
- [Connection Issues](#connection-issues)
- [Runtime Problems](#runtime-problems)
- [Command Issues](#command-issues)
- [Nickname Recovery Issues](#nickname-recovery-issues)
- [Update System Issues](#update-system-issues)
- [Logging and Monitoring](#logging-and-monitoring)
- [Emergency Procedures](#emergency-procedures)

## Installation Issues

### `./configure: Permission denied`

**Cause**: Configure script not executable.

**Solution**:
```bash
cd src
chmod +x configure
./configure
```

### `configure: command not found`

**Cause**: Running `configure` from wrong directory or not extracted.

**Solution**: Run from `src/` directory:
```bash
cd src
./configure
```

### Missing header files (`netinet/in.h`, etc.)

**Cause**: Build-essential packages not installed.

**Solution**:
```bash
# Debian/Ubuntu
sudo apt-get install build-essential

# CentOS/RHEL
sudo yum groupinstall "Development Tools"

# FreeBSD
pkg install gcc
```

### Undefined reference to functions during linking

**Cause**: Using static build without required libraries.

**Solution**: Use dynamic build (`make dynamic`) or install missing libraries.

### `strip: command not found`

**Cause**: `binutils` missing.

**Solution**: Install `binutils` package.

## Configuration Problems

### `error: mandatory option 'nick' not set`

**Cause**: `nick` option missing in config.

**Solution**: Add `nick <nickname>` line.

### `error: mandatory option 'nicks' not set`

**Cause**: `nicks` option missing.

**Solution**: Add `nicks <nick1> <nick2> ...` line.

### `error: mandatory option 'server' not set`

**Cause**: No IRC server defined.

**Solution**: Add `server <hostname> [port]` or `server6` lines.

### `argument too long` errors

**Cause**: Option value exceeds allowed length.

**Solution**: Shorten the value to comply with limits (see [Configuration Reference](CONFIGURATION.md)).

### `duplicate entries` errors

**Cause**: Option specified multiple times (e.g., duplicate servers or nicknames).

**Solution**: Remove redundant entries.

### `unknown variable` errors

**Cause**: Typo in configuration option.

**Solution**: Check spelling and see [Configuration Reference](CONFIGURATION.md).

## Connection Issues

### Bot fails to connect (no output after `Connecting to...`)

**Causes**:
- Network unreachable
- DNS resolution failure
- Firewall blocking
- Wrong port

**Solutions**:
- Check network connectivity
- Verify hostnames resolve (`nslookup irc.example.net`)
- Confirm port is correct (default 6667)
- Check firewall rules

### Immediate disconnect after connect

**Causes**:
- Nickname in use
- K-lined/banned
- Flood/penalty too high

**Solutions**:
- Configure alternative nick (`altnick on`)
- Check `me.conn` status in debug logs
- Contact IRC network staff

### `Cannot resolve host` errors

**Cause**: DNS resolution failed.

**Solution**:
- Check DNS servers
- Use IP address directly (not recommended)
- Verify server name

### `PING? PONG!` but no other activity

**Cause**: Stuck after registration.

**Solution**:
- Ensure configuration includes channels and nicks
- Check server logs
- Verify no pending command in queue

## Runtime Problems

### Bot exits immediately

**Possible causes**:
- Invalid configuration
- Permission issues (userfile/logfile paths)
- Missing directories

**Solution**:
- Run in foreground (debug build) to see output
- Check file permissions and paths
- Review logs

### `Write error` messages

**Cause**: Socket write failed (network issue).

**Solution**:
- Check network connectivity
- Ensure server is up
- Bot will reconnect automatically

### Bot loops reconnecting rapidly

**Causes**:
- Invalid ident/realname
- Server rejects connection
- Flood protection triggered

**Solutions**:
- Increase `delay` (anti-flood)
- Check server logs for reason
- Add more servers in config

### High CPU usage

**Causes**:
- Tight reconnection loop
- Debug build with verbose logging

**Solutions**:
- Check logs for repeated errors
- Introduce backoff or fix underlying issue

### `This file is hacked` warning at startup

**Cause**: Binary validator detected mismatch.

**Solution**:
- Recompile the bot
- Ignore if custom build (warning only in v0.2.6)

### Bot does not daemonize (stays in foreground)

**Cause**: Debug build or `dontfork on` in config.

**Solution**:
- Use dynamic/static build
- Ensure `dontfork` not enabled

## Command Issues

### Bot doesn't respond to commands

**Causes**:
- You're not an authorized user
- Bot doesn't see your hostmask
- Command prefix missing (`!`, `.`, `@`)
- `publicreply off` and expecting channel response

**Solutions**:
- Add yourself with `!new`
- Check `!hosts` via private message
- Use private message to bot
- Verify command syntax

### `Host not found` when removing user

**Cause**: Hostmask not in list.

**Solution**: Use `!hosts` to find exact mask or number and remove by number.

### `Channel limit reached`

**Cause**: Over 21 channels configured.

**Solution**: Remove unused channels (`!part`) or increase limit in source code (not recommended).

### `Nicklist is full`

**Cause**: More than 40 nicknames configured.

**Solution**: Remove unnecessary nicknames or increase limit in source (not recommended).

### `Cannot add to queue (It's full)`

**Cause**: Nick queue (50 entries) full.

**Solution**:
- Wait for queue to process
- Remove entries from queue manually (not currently exposed)

## Nickname Recovery Issues

### Bot fails to catch nick even when available

**Check**:
- `!ison` output
- `!keepnick` status
- `delay` setting (if too high)
- Penalty queue (maybe saturated)
- `me.catched` state (if it thinks it already has nick)

**Solutions**:
- Force catch: `!nick -yes DesiredNick`
- Toggle catching: `!keepnick off` then `!keepnick on`
- Restart bot to reset state

### `Offline(...)(outdated)` message from `!ison`

**Cause**: Bot already has preferred nick (no need to check).

**Solution**: Ignore (informational). To force update, disable catch temporarily.

### Unable to set alt nick

**Cause**: `altnick on` but `nickappend` missing.

**Solution**: Add `nickappend` option or accept default alphabet (auto-set when needed).

## Update System Issues

### `!update` command fails silently

**Check**:
- Bot compiled with validator (`HAVE_VALIDATOR`)
- Update server reachable
- `update` structure values (requires code inspection)

**Solution**:
- Verify update URL in source code (`update.c`)
- Check HTTP response
- Run bot in debug mode for more output

### `validator: command not found`

**Cause**: Validator binary not built.

**Solution**: Ensure `md5.o` exists and Makefile runs `validator` step (part of default build).

### Update causes bot to exit

**Cause**: Update process failed; child process exit triggers error.

**Solution**:
- Check logs for update errors
- Run update manually with debug logging
- Ensure update server provides valid binary

## Logging and Monitoring

### Log file not created

**Possible causes**:
- `logfile` not set in config
- Bot lacks permissions for log path
- Directory missing

**Solutions**:
- Set `logfile` path in configuration
- Create directories and set permissions
- Check for errors when bot tries to log

### Empty log file

**Cause**: Bot only logs certain events by default.

**Solution**: Test with actions that trigger logging (JOIN/PART/NICK/MODE).

### PID file not created

**Causes**:
- `pidfile` not configured
- Bot lacking permission to write path
- Bot exited before creating file

**Solution**:
- Set `pidfile` in config
- Ensure directory writable
- Check if bot started successfully

## Emergency Procedures

### Bot is unresponsive

1. Check process status: `ps aux | grep knb`
2. Check logs (`logfile`, `debug.log`)
3. If hung, kill and restart

### Bot stuck in reconnect loop

1. Stop bot
2. Increase `delay`
3. Add more servers
4. Check server-side bans

### Lost access to bot (no authorized users)

1. Edit userfile manually (complex, binary format)
2. Delete userfile to reset (anyone can re-add themselves)
3. Restart bot and immediately use `!new`

### Corrupted userfile

1. Restore from backup (`~userfile`)
2. If no backup, delete userfile (start fresh)
3. Use `!save` regularly to ensure persistence

### Crash debugging

1. Build debug version: `make debug`
2. Run with `ulimit -c unlimited`
3. On crash, analyze core dump: `gdb ./knb core`
4. Report stack trace with issue report

## Diagnostic Checklist

1. **Is the bot running?**
   - `ps aux | grep knb`
   - Check PID file

2. **Is configuration valid?**
   - Run bot and check for config errors

3. **Can the bot connect?**
   - Test network connectivity to server

4. **Are users authorized?**
   - `!hosts`

5. **Is the queue working?**
   - `!debug` (debug build)

6. **Are logs being written?**
   - Check `logfile` and `debug.log`

## Reporting Issues

When reporting, include:
- Bot version (`!version`)
- Operating system and compiler version
- Configuration file (sanitized)
- Logs (`logfile`, `debug.log`)
- Steps to reproduce
- Any core dumps (debug builds)

## Useful Commands

- `!version` - Check version
- `!uptime` - Check runtime
- `!hosts` - List authorized users
- `!channels` - List channels
- `!ison` - Check nickname availability
- `!keepnick` - Check catch mode
- `!debug` - Show queue state (debug build)

## References

- [Installation Guide](INSTALLATION.md)
- [Configuration Reference](CONFIGURATION.md)
- [Command Reference](COMMANDS.md)
- [Architecture Overview](ARCHITECTURE.md)
- [Deployment Guide](DEPLOYMENT.md)
- [FAQ](FAQ.md)

---

Most issues can be resolved by carefully reviewing configuration, logs, and network state. Use the debug build for deeper inspection.
