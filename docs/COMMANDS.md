# Command Reference

Complete reference for all IRC commands supported by Keep Nick Bot.

## Table of Contents

- [Command Basics](#command-basics)
- [User Management](#user-management)
- [Channel Management](#channel-management)
- [Channel Operations](#channel-operations)
- [Nickname Management](#nickname-management)
- [Bot Management](#bot-management)
- [Data Management](#data-management)
- [Communication](#communication)
- [Information Commands](#information-commands)
- [Debug Commands](#debug-commands)

## Command Basics

### Command Prefixes

Commands can start with any of these characters:
- `.` (period)
- `!` (exclamation)
- `@` (at sign)

**Examples** (all equivalent):
```
!help
.help
@help
```

### Sending Commands

**Via Private Message**:
```
/msg BotNick !command arguments
```

**On Channel** (where bot is present):
```
!command arguments
```

### Response Behavior

Responses depend on `replytype` and `publicreply` configuration:

- **replytype notice** (default): Responses sent as NOTICE
- **replytype msg**: Responses sent as PRIVMSG
- **publicreply off** (default): Always responds privately
- **publicreply on**: Responds in channel if command was in channel

### Access Control

- **No users added**: Anyone can use commands (and add themselves)
- **Users added**: Only authorized users (matching hostmasks) can use commands

## User Management

### !new [mask]

**Aliases**: `!new`, `!+host [mask]`, `!+mask [mask]`, `!+user [mask]`

**Description**: Add new user hostmask to authorized users list.

**Usage**:
```
!new                    # Add yourself (current hostmask)
!new *!user@*.example.com   # Add specific hostmask
!+host admin!*@*        # Add user by mask
```

**Permission**: Available to existing users, or anyone if no users exist

**Notes**:
- First user added gets full control
- Supports wildcard patterns
- Hostmask automatically expanded from partial masks

### !-host [mask|number]

**Aliases**: `!-mask [mask|number]`, `!-user [mask|number]`

**Description**: Remove user hostmask from authorized list.

**Usage**:
```
!-host                  # Remove yourself
!-host *!user@*.example.com  # Remove by mask
!-host 3                # Remove by number (from !hosts list)
```

**Permission**: Authorized users only

**Warning**: Be careful not to remove all users including yourself!

### !hosts

**Aliases**: `!masks`, `!users`

**Description**: List all authorized user hostmasks with numbers.

**Usage**:
```
!hosts
```

**Example Output**:
```
-+- [1] *!*@192.0.2.100
-+- [2] admin!*@*.example.com
-+- [3] *!user@*.isp.net
```

**Permission**: Authorized users only

## Channel Management

### !join <channel> [key]

**Aliases**: `!j <channel> [key]`

**Description**: Join an IRC channel and add to auto-join list.

**Usage**:
```
!join #bots
!join #private secretkey
!j #test
```

**Permission**: Authorized users only

**Constraints**:
- Maximum 21 channels
- Channel name max 50 characters
- Must have valid channel prefix (#, &, +, !)

### !part [channel]

**Aliases**: `!p [channel]`, `!leave [channel]`, `!l [channel]`

**Description**: Leave channel and remove from auto-join list.

**Usage**:
```
!part #test             # Part specific channel
!part                   # Part current channel (if on channel)
```

**Permission**: Authorized users only

**Notes**:
- If no channel specified and command is on a channel, parts that channel
- Cannot part special alphabet channel in some cases

### !cycle [channel]

**Aliases**: `!rejoin [channel]`

**Description**: Part and rejoin channel (useful for refreshing mode/ops).

**Usage**:
```
!cycle #bots            # Cycle specific channel
!cycle                  # Cycle current channel
```

**Permission**: Authorized users only

### !channels

**Aliases**: `!chans`

**Description**: List all channels bot is configured to join.

**Usage**:
```
!channels
```

**Example Output**:
```
-+- [1] #bots password123
-+- [2] #test
-+- [3] #ops secretkey
```

**Permission**: Authorized users only

## Channel Operations

### !op [channel] [nick]

**Description**: Give operator (@) status to user.

**Usage**:
```
!op                     # Op yourself in current channel
!op SomeUser            # Op user in current channel
!op #bots SomeUser      # Op user in specific channel
```

**Permission**: Authorized users only

**Requirements**: Bot must have operator status in the channel

### !deop [channel] [nick]

**Aliases**: `!dop [channel] [nick]`

**Description**: Remove operator status from user.

**Usage**:
```
!deop                   # Deop yourself in current channel
!deop SomeUser          # Deop user in current channel
!deop #bots SomeUser    # Deop user in specific channel
```

**Permission**: Authorized users only

### !voice [channel] [nick]

**Aliases**: `!v [channel] [nick]`

**Description**: Give voice (+) status to user.

**Usage**:
```
!voice                  # Voice yourself in current channel
!v SomeUser             # Voice user in current channel
!voice #bots SomeUser   # Voice user in specific channel
```

**Permission**: Authorized users only

### !devoice [channel] [nick]

**Aliases**: `!dv [channel] [nick]`

**Description**: Remove voice status from user.

**Usage**:
```
!devoice                # Devoice yourself
!dv SomeUser            # Devoice user in current channel
!devoice #bots SomeUser # Devoice in specific channel
```

**Permission**: Authorized users only

### !kick <channel> <nick>

**Aliases**: `!k <channel> <nick>`

**Description**: Kick user from channel.

**Usage**:
```
!kick #bots Spammer
!k #test TrollUser
```

**Permission**: Authorized users only

**Requirements**: Bot must have operator status

**Notes**: Uses default `reason` from config

### !ban <channel> <mask>

**Aliases**: `!b <channel> <mask>`

**Description**: Set ban on hostmask in channel.

**Usage**:
```
!ban #bots *!*@spammer.com
!b #test *!baduser@*
```

**Permission**: Authorized users only

**Requirements**: Bot must have operator status

### !unban <channel> <mask>

**Aliases**: `!ub <channel> <mask>`

**Description**: Remove ban on hostmask in channel.

**Usage**:
```
!unban #bots *!*@spammer.com
!ub #test *!baduser@*
```

**Permission**: Authorized users only

**Requirements**: Bot must have operator status

### !invite <channel> [nick]

**Aliases**: `!i <channel> [nick]`

**Description**: Invite user to channel.

**Usage**:
```
!invite #private SomeUser
!i #bots                # Invite yourself
```

**Permission**: Authorized users only

**Requirements**: Bot must be on channel

## Nickname Management

### !nick <nick>

**Aliases**: `!n <nick>`, `!nick -yes <nick>`

**Description**: Change to specified nickname.

**Usage**:
```
!nick DesiredNick       # Request nick change
!nick -yes ForceNick    # Force change even if caught preferred nick
```

**Permission**: Authorized users only

**Notes**:
- If bot has "caught" its preferred nick, requires `-yes` flag
- Adds nickname to high-priority queue
- Bot attempts to acquire nickname immediately

### !nicks

**Description**: List nicknames configured in config file.

**Usage**:
```
!nicks
```

**Example Output**:
```
-+- Nicks(5): a, b, c, d, e
```

**Permission**: Authorized users only

### !+tempnick <nick>

**Description**: Add temporary nickname to monitor/acquire.

**Usage**:
```
!+tempnick TestNick
!+tempnick x
```

**Permission**: Authorized users only

**Constraints**:
- Maximum 40 total nicknames (including config nicks)
- Nickname max 15 characters

**Notes**: Temporary nicknames aren't persisted across restarts

### !-tempnick <nick>

**Description**: Remove temporary nickname from list.

**Usage**:
```
!-tempnick TestNick
```

**Permission**: Authorized users only

**Notes**: Can only remove temporary nicks, not config nicks

### !tempnicks

**Description**: List temporary nicknames.

**Usage**:
```
!tempnicks
```

**Example Output**:
```
-+- Tempnicks(2): TestNick, x
```

**Permission**: Authorized users only

### !ison

**Aliases**: `!check`

**Description**: Check which configured nicknames are currently offline/available.

**Usage**:
```
!ison
!check
```

**Example Output**:
```
-+- Offline(3): a, c, x
```

**Permission**: Authorized users only

**Notes**:
- May show "outdated" if bot has caught primary nick (hasn't sent recent ISON)
- Useful to see what nicknames are available

### !keepnick [on|off]

**Aliases**: `!catch [on|off]`

**Description**: Toggle nickname catching mode.

**Usage**:
```
!keepnick               # Show current status
!keepnick on            # Enable nickname catching
!catch off              # Disable nickname catching
```

**Permission**: Authorized users only

**Notes**:
- When on: Bot actively tries to acquire configured nicknames
- When off: Bot stops trying to change nickname

## Bot Management

### !version

**Aliases**: `!ver`

**Description**: Display bot version information.

**Usage**:
```
!version
!ver
```

**Example Output**:
```
-+- [PT] Pojeby Team (Keep nick bot) v0.2.6 (tahioSyndykat-1) by Esio (tahioSyndykat Version)
```

**Permission**: Authorized users only

### !uptime

**Aliases**: `!up`

**Description**: Show bot uptime since start.

**Usage**:
```
!uptime
!up
```

**Example Output**:
```
-+- Uptime: 2 days, 5 hours, 23 mins, 15 secs
```

**Permission**: Authorized users only

### !server <server> [port]

**Aliases**: `!s <server> [port]`, `!server6 <server> [port]`, `!s6 <server> [port]`

**Description**: Change to different IRC server.

**Usage**:
```
!server irc.example.net        # Use configured server
!server irc.new.net 6667       # Use new server
!s6 irc6.example.net          # Use IPv6 server
```

**Permission**: Authorized users only

**Notes**:
- Can use configured server or specify new one
- Bot will disconnect and reconnect to new server
- Server becomes "custom" server until restart

### !reconnect

**Aliases**: `!r`

**Description**: Disconnect and reconnect to IRC.

**Usage**:
```
!reconnect
!r
```

**Permission**: Authorized users only

**Quit Message**: "Reconnecting"

### !restart

**Aliases**: `!reboot`

**Description**: Restart bot process (exec new process).

**Usage**:
```
!restart
!reboot
```

**Permission**: Authorized users only

**Notes**:
- Saves userfile before restart
- Executes new instance with same config
- PID file is removed

**Quit Message**: "Restarting: <reason>"

### !die

**Aliases**: `!quit`, `!exit`, `!kill`, `!q`, `!disco`, `!disconnect`, `!shutdown`

**Description**: Shutdown bot completely.

**Usage**:
```
!die
!quit
!shutdown
```

**Permission**: Authorized users only

**Notes**:
- Saves userfile before exit
- Removes PID file
- Process terminates

**Quit Message**: "Killed by <nick>: <reason>"

### !update [--with-restart]

**Aliases**: `!u [--with-restart]`, `!u [-r]`

**Description**: Update bot binary via HTTP.

**Usage**:
```
!update                 # Update only
!update --with-restart  # Update and restart
!u -r                   # Short form with restart
```

**Permission**: Authorized users only

**Notes**:
- Downloads new binary via HTTP
- Validates with MD5 checksum
- Optionally restarts after update
- Update URL configured in source code

### !reload

**Aliases**: `!rehash`

**Description**: Reload userfile and configuration.

**Usage**:
```
!reload
!rehash
```

**Permission**: Authorized users only

**Process**:
1. Backs up current userfile
2. Clears user list
3. Parts all channels
4. Reloads userfile
5. Rejoins channels

**Warning**: If reload fails, bot attempts to restore backup

### !save

**Description**: Save userfile to disk immediately.

**Usage**:
```
!save
```

**Permission**: Authorized users only

**Notes**: Userfile is saved automatically on changes, but this forces immediate write

### !backup

**Description**: Create backup of userfile.

**Usage**:
```
!backup
```

**Permission**: Authorized users only

**Notes**: Creates backup file named `~<userfile>` (e.g., `~knb.uf`)

## Communication

### !msg <target> <message>

**Aliases**: `!m <target> <message>`, `!privmsg <target> <message>`

**Description**: Send message to channel or user.

**Usage**:
```
!msg #channel Hello everyone
!m SomeUser Private message
```

**Permission**: Authorized users only

**Notes**: Bypasses normal reply type setting

### !notice <target> <message>

**Description**: Send notice to channel or user.

**Usage**:
```
!notice #channel Announcement
!notice SomeUser Notice message
```

**Permission**: Authorized users only

### !say <message>

**Description**: Say message in current context.

**Usage**:
```
!say Hello!
```

**Permission**: Authorized users only

**Behavior**:
- If command via PM: Sends to user
- If command on channel: Sends to channel

### !help

**Description**: Display help information (URL to documentation).

**Usage**:
```
!help
```

**Permission**: Authorized users only

**Output**: Returns URL to documentation/source

## Information Commands

### !hello

**Description**: Test command (responds with "world").

**Usage**:
```
!hello
```

**Output**: `world`

**Permission**: Authorized users only

**Purpose**: Simple test to verify bot is responsive

## Debug Commands

Available only in **debug builds** (`make debug`).

### !debug

**Description**: Display internal debug information.

**Usage**:
```
!debug
```

**Example Output**:
```
-+- me.catch: true, me.catched: false, queues: 3
-+- queue[0]: nick: a, prio: 1
-+- queue[1]: nick: b, prio: 1
-+- queue[2]: nick: c, prio: 1
```

**Permission**: Authorized users only

**Information Shown**:
- Catching status
- Caught status (has preferred nick)
- Queue count
- Queue details (nickname and priority)

## Command Syntax Notes

### Optional Arguments

Arguments in `[brackets]` are optional:
```
!part [channel]    # Channel is optional
```

### Required Arguments

Arguments in `<angle brackets>` are required:
```
!msg <target> <message>    # Both required
```

### Multiple Options

Options separated by `|` mean choose one:
```
<on|off>    # Either "on" or "off"
```

### Argument Types

- `<nick>` - IRC nickname
- `<channel>` - IRC channel (must start with #, &, +, !)
- `<mask>` - Hostmask pattern (supports wildcards)
- `<number>` - Numeric value
- `<message>` - Free-form text
- `<on|off>` - Boolean value

## Error Messages

### Common Errors

- **Usage: !command ...** - Incorrect command syntax
- **Unknown host ...** - Server not resolvable
- **Not on that channel** - Bot isn't on specified channel
- **Channel not found** - Channel not in bot's list
- **Userlist is empty** - No authorized users
- **Host allready exists** - User already added
- **Host not found** - User doesn't exist
- **Cannot join channel (Channel limit reached)** - Max 21 channels
- **Nicklist is full** - Max 40 nicknames
- **Too long** - Argument exceeds maximum length

## Response Format

All command responses follow the format:

```
-+- <message>
```

Example:
```
-+- Host 'admin!*@*.example.com' added
-+- Channel '#bots' removed
-+- Uptime: 1 day, 3 hours, 45 mins
```

## Best Practices

1. **Add yourself first**: Use `!new` as soon as bot connects
2. **Backup userfile**: Use `!backup` before major changes
3. **Test in PM**: Test commands via private message before using in channels
4. **Be careful with -host**: Don't remove all users including yourself
5. **Use -yes wisely**: `!nick -yes` bypasses safety checks
6. **Monitor after reload**: Verify bot rejoins channels after `!reload`
7. **Save before shutdown**: Bot auto-saves, but `!save` ensures data is written

## Next Steps

- Configure the bot: [Configuration Reference](CONFIGURATION.md)
- Understand architecture: [Architecture Overview](ARCHITECTURE.md)
- Deploy in production: [Deployment Guide](DEPLOYMENT.md)
- Troubleshoot issues: [Troubleshooting](TROUBLESHOOTING.md)

---

For questions, see [FAQ](FAQ.md).
