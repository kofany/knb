# Deployment Guide

Best practices for deploying Keep Nick Bot (KNB) in production.

## Table of Contents

- [Pre-Deployment](#pre-deployment)
- [Initial Setup](#initial-setup)
- [Production Configuration](#production-configuration)
- [Watchdog/Cron Setup](#watchdogcron-setup)
- [Monitoring](#monitoring)
- [Maintenance](#maintenance)
- [Security Best Practices](#security-best-practices)
- [Backup and Recovery](#backup-and-recovery)
- [Scaling and Performance](#scaling-and-performance)

## Pre-Deployment

### System Requirements

- Unix-like operating system (Linux, BSD, SunOS)
- User account (non-root recommended)
- Network access to IRC servers
- Disk space: ~10 MB minimum
- RAM: ~10 MB minimum

### Dependencies

- GCC (compilation)
- Cron (optional, for watchdog)
- Standard C libraries

### Planning

- Choose IRC servers (at least 2-3 for redundancy)
- Select channels to monitor
- Determine nicknames to manage
- Plan file locations (logs, userfile, PID)

## Initial Setup

### 1. Create Directory Structure

```bash
mkdir -p ~/knb/{logs,data,scripts,backup}
cd ~/knb
```

### 2. Compile Bot

```bash
cd src
./configure
make dynamic
cd ..
```

### 3. Create Configuration

```bash
cp cfg-examples/conf-ipv4 production.conf
nano production.conf
```

**Essential configuration**:
```ini
# Identity
nick ProductionBot
realname Production IRC Bot
ident prodbot

# Nicknames to manage
nicks prod backup temp1 temp2

# Servers (multiple for redundancy)
server irc1.network.net 6667
server irc2.network.net 6667
server irc3.network.net 6667

# Channels
channel #operations
channel #monitoring

# Files
logfile /home/user/knb/logs/knb.log
pidfile /home/user/knb/data/knb.pid
userfile /home/user/knb/data/knb.uf

# Anti-flood
delay 1

# CTCP
ctcptype 1
ctcpreply Production Bot v1.0

# Reply settings
replytype notice
publicreply off
reason Maintenance
```

### 4. Initial Test Run

```bash
./knb production.conf
```

Check:
- Bot connects successfully
- Joins configured channels
- PID file created

### 5. Add Yourself as Admin

```
/msg ProductionBot !new
```

Verify:
```
!hosts
```

### 6. Test Commands

```
!version
!uptime
!channels
!nicks
```

## Production Configuration

### File Locations

Use absolute paths for production:

```ini
logfile /var/log/knb/production.log
pidfile /var/run/knb/production.pid
userfile /var/lib/knb/production.uf
```

Ensure directories exist and are writable:
```bash
mkdir -p /var/log/knb /var/run/knb /var/lib/knb
chown user:user /var/{log,run,lib}/knb
```

### Multiple Servers

Configure at least 2-3 servers for redundancy:

```ini
server irc1.network.net 6667
server irc2.network.net 6667
server irc3.network.net 6667
```

Bot will rotate through servers on disconnect.

### Logging

Enable logging for production:

```ini
logfile /var/log/knb/production.log
```

### Anti-Flood

For busy networks, set delay:

```ini
delay 2
```

### Channel Management

List all channels to auto-join:

```ini
channel #operations password123
channel #monitoring
channel #alerts
```

## Watchdog/Cron Setup

Ensure bot automatically restarts if it crashes.

### 1. Configure Watchdog Script

Edit `scripts/knbchk.sh`:

```bash
KNBDIR="/home/user/knb"
NICK="ProductionBot"
PIDFILE="data/production.pid"
CONF="production.conf"
KNB="knb"
```

### 2. Test Script

```bash
chmod +x scripts/knbchk.sh
./scripts/knbchk.sh
```

### 3. Add to Crontab

**Manual method**:
```bash
crontab -e
```

Add line:
```
*/10 * * * * /home/user/knb/scripts/knbchk.sh >/dev/null 2>&1
```

**Automated method**:
```bash
cd scripts
./cron.sh
```

### 4. Verify Cron

```bash
crontab -l
```

Should show your entry.

### 5. Test Watchdog

Kill bot process:
```bash
kill $(cat data/production.pid)
```

Wait up to 10 minutes. Bot should restart automatically.

## Monitoring

### Log Monitoring

Monitor logs for issues:
```bash
tail -f /var/log/knb/production.log
```

### Process Monitoring

Check bot is running:
```bash
ps aux | grep knb
cat /var/run/knb/production.pid
```

### IRC Monitoring

Set up monitoring channel:
```ini
channel #botmon
```

Use commands:
- `!uptime` - Check uptime
- `!version` - Verify version
- `!ison` - Check nickname status
- `!channels` - Verify channels

### External Monitoring

Use external tools:
- Nagios/Icinga plugins for IRC bot monitoring
- Custom scripts that check PID file
- Log analysis tools (logwatch, etc.)

## Maintenance

### Regular Backups

Automated backup script:
```bash
#!/bin/bash
# backup-knb.sh
DATE=$(date +%Y%m%d-%H%M%S)
cp /var/lib/knb/production.uf /var/backups/knb/production-${DATE}.uf
find /var/backups/knb -mtime +30 -delete
```

Add to cron:
```
0 2 * * * /home/user/knb/backup-knb.sh
```

Via IRC:
```
!backup
```

### Log Rotation

Configure logrotate:
```bash
# /etc/logrotate.d/knb
/var/log/knb/*.log {
    weekly
    rotate 12
    compress
    delaycompress
    missingok
    notifempty
    create 0640 user user
}
```

### Updates

To update bot:
```bash
# Stop bot
kill $(cat /var/run/knb/production.pid)

# Backup current binary
cp knb knb.backup

# Recompile
cd src
./configure
make dynamic
cd ..

# Restart bot
./knb production.conf

# Verify
ps aux | grep knb
```

Or use built-in update:
```
!update --with-restart
```

### Configuration Changes

1. Stop bot
2. Edit config file
3. Test configuration (start bot, check logs)
4. Verify changes
5. Add to version control (recommended)

### Userfile Management

Backup before changes:
```
!backup
```

Reload after manual editing:
```
!reload
```

## Security Best Practices

### 1. Run as Non-Root User

Never run as root. Create dedicated user:
```bash
sudo useradd -r -s /bin/false knb
sudo -u knb ./knb production.conf
```

### 2. Limit Access

Restrict file permissions:
```bash
chmod 600 production.conf  # Config may contain channel keys
chmod 600 data/production.uf
chmod 755 knb
```

### 3. Hostmask Security

Use specific hostmasks:
```
admin!*@trusted.example.com
*!user@192.0.2.100
```

Avoid:
```
*!*@*  # Too permissive
```

### 4. Regular User Audits

Periodically review authorized users:
```
!hosts
```

Remove unused users:
```
!-host <mask>
```

### 5. Separate Environments

Use different configs for development/production:
- `dev.conf` - Development
- `staging.conf` - Testing
- `production.conf` - Production

### 6. Network Security

- Use firewall to restrict outbound connections
- Consider VPN for sensitive networks
- Monitor unusual traffic patterns

### 7. Incident Response

If compromised:
1. Immediately stop bot
2. Delete userfile
3. Review logs for unauthorized actions
4. Restart with fresh userfile
5. Add only trusted users

## Backup and Recovery

### Backup Strategy

**What to backup**:
- Userfile (`production.uf`)
- Configuration (`production.conf`)
- Binary (optional, can recompile)

**Frequency**:
- Userfile: Daily
- Config: On change
- Binary: On update

### Automated Backup

```bash
#!/bin/bash
# Daily backup script
BACKUP_DIR="/var/backups/knb"
DATE=$(date +%Y%m%d)

mkdir -p ${BACKUP_DIR}
cp /var/lib/knb/production.uf ${BACKUP_DIR}/knb-${DATE}.uf
cp /home/user/knb/production.conf ${BACKUP_DIR}/knb-${DATE}.conf
find ${BACKUP_DIR} -mtime +30 -delete
```

### Recovery Procedure

1. Stop bot if running
2. Restore userfile:
   ```bash
   cp /var/backups/knb/knb-20240101.uf /var/lib/knb/production.uf
   ```
3. Restore config if needed
4. Restart bot
5. Verify with `!hosts` and `!channels`

### Disaster Recovery

If all backups lost:
1. Recompile bot
2. Create new configuration
3. Start bot
4. Add yourself: `!new`
5. Reconfigure channels: `!join`
6. Add other authorized users

## Scaling and Performance

### Single Bot Limitations

- One server connection at a time
- Up to 21 channels
- Up to 40 nicknames

### Multiple Bot Strategy

For managing more resources:
1. Run separate bot instances
2. Use different configs
3. Separate PID files
4. Different userfiles

Example:
- `bot1.conf` - Manages channels 1-20
- `bot2.conf` - Manages channels 21-40

### Resource Optimization

- Dynamic build (smaller binary)
- Minimal logging (only errors)
- Reduce penalty delays if network allows
- Disable features not needed

### High Availability

- Multiple IRC servers
- Watchdog script
- Redundant bot instances (different users/nicks)
- External monitoring

## Deployment Checklist

- [ ] Compile bot successfully
- [ ] Create production configuration
- [ ] Set up directory structure
- [ ] Configure logging
- [ ] Set up PID file
- [ ] Test initial connection
- [ ] Add administrator users
- [ ] Configure channels
- [ ] Set up watchdog script
- [ ] Add cron job
- [ ] Test automatic restart
- [ ] Configure log rotation
- [ ] Set up backups
- [ ] Document deployment
- [ ] Establish monitoring
- [ ] Test disaster recovery

## References

- [Installation Guide](INSTALLATION.md)
- [Configuration Reference](CONFIGURATION.md)
- [Command Reference](COMMANDS.md)
- [Troubleshooting](TROUBLESHOOTING.md)
- [FAQ](FAQ.md)

---

Proper deployment ensures reliability and minimizes downtime. Regular maintenance and monitoring are key to successful operation.
