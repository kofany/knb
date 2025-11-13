# Architecture Overview

Technical architecture documentation for Keep Nick Bot (KNB).

## Table of Contents

- [System Overview](#system-overview)
- [Core Components](#core-components)
- [Data Flow](#data-flow)
- [Module Details](#module-details)
- [Key Algorithms](#key-algorithms)
- [Data Structures](#data-structures)
- [Build System](#build-system)
- [Security Considerations](#security-considerations)

## System Overview

KNB is a monolithic C application designed for minimal resource usage and maximum reliability. The architecture follows a modular design with clear separation of concerns while maintaining a single-process model.

### Design Principles

1. **Simplicity**: Single-threaded event loop, no complex threading
2. **Reliability**: Robust error handling, automatic recovery
3. **Efficiency**: Minimal memory footprint, low CPU usage
4. **Portability**: POSIX-compliant, works across Unix-like systems
5. **Maintainability**: Modular code organization, clear interfaces

### Process Model

```
┌─────────────────────────────────────────┐
│           Main Process (knb)             │
├─────────────────────────────────────────┤
│  ┌───────────────────────────────────┐  │
│  │        Configuration Layer        │  │
│  │  (cfg.c - Parse & Validate)       │  │
│  └───────────────────────────────────┘  │
│  ┌───────────────────────────────────┐  │
│  │         IRC Protocol Layer        │  │
│  │  (irc.c - Socket I/O & RFC)       │  │
│  └───────────────────────────────────┘  │
│  ┌───────────────────────────────────┐  │
│  │         Event Processing          │  │
│  │  (events.c - IRC Events)          │  │
│  └───────────────────────────────────┘  │
│  ┌───────────────────────────────────┐  │
│  │        Command Processing         │  │
│  │  (cmds.c - User Commands)         │  │
│  └───────────────────────────────────┘  │
│  ┌───────────────────────────────────┐  │
│  │      Nickname Queue Manager       │  │
│  │  (queue.c - Priority Queue)       │  │
│  └───────────────────────────────────┘  │
│  ┌───────────────────────────────────┐  │
│  │       Persistence Layer           │  │
│  │  (uf.c - User/Channel Storage)    │  │
│  └───────────────────────────────────┘  │
│  ┌───────────────────────────────────┐  │
│  │         Update Mechanism          │  │
│  │  (update.c - HTTP Self-Update)    │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘
         ↓ Fork (for updates)
┌─────────────────────────────────────────┐
│         Child Process (updater)          │
│  - Download new binary via HTTP          │
│  - Validate MD5 checksum                 │
│  - Replace old binary                    │
│  - Signal parent or exit                 │
└─────────────────────────────────────────┘
```

## Core Components

### 1. Configuration System (cfg.c/cfg.h)

**Purpose**: Parse and validate configuration files

**Key Functions**:
- `load_cfg(char *file)` - Main configuration loader
- `set_variable(char *var, char *args, char *error)` - Set individual option
- `err_return()` - Generate error messages

**Features**:
- Line-by-line parsing
- Type validation
- Range checking
- Duplicate detection
- Default value assignment

### 2. IRC Protocol (irc.c/irc.h)

**Purpose**: Handle low-level IRC socket operations

**Key Functions**:
- `connect_bot()` - Establish IRC connection
- `check_input()` - Read and parse IRC messages
- `parse_line(char *line)` - Parse IRC protocol lines
- `send_nick()`, `send_user()` - IRC registration
- `send_pong()` - Respond to PING

**Protocol Implementation**:
- RFC 1459/2812 compliant
- Non-blocking I/O
- Timeout handling
- Multi-server rotation
- IPv4/IPv6 support

### 3. Event Processing (events.c/events.h)

**Purpose**: Handle IRC events (numeric and text)

**Key Events**:
- `got_numeric()` - Handle numeric responses (001, 433, etc.)
- `got_privmsg()` - Process PRIVMSG
- `got_nick()` - Handle NICK changes
- `got_join()` - Handle JOIN
- `got_kick()` - Handle KICK
- `got_invite()` - Handle INVITE

**Event Flow**:
```
IRC Line → parse_line() → Event Handler → Action
```

### 4. Command Processing (cmds.c/cmds.h)

**Purpose**: Execute user commands from IRC

**Key Functions**:
- `got_cmd()` - Main command dispatcher
- Command authentication (hostmask matching)
- Parameter parsing
- Response generation

**Command Categories**:
- User management (`!new`, `!-host`, etc.)
- Channel operations (`!join`, `!part`, etc.)
- Bot control (`!restart`, `!die`, etc.)
- Nickname management (`!nick`, `!ison`, etc.)

### 5. Nickname Queue (queue.c/queue.h)

**Purpose**: Manage nickname acquisition priority

**Key Functions**:
- `add_queue(char *nick, int prio)` - Add nickname to queue
- `rem_queue(char *nick)` - Remove from queue
- `check_queue()` - Process queue

**Priority Levels**:
- `NO_PRIO (0)` - No priority
- `LOW_PRIO (1)` - Low priority (standard)
- `MID_PRIO (2)` - Medium priority
- `HIGH_PRIO (3)` - High priority (user requested)

**Queue Algorithm**:
1. Sort by priority (high to low)
2. Within same priority, FIFO order
3. Send NICK command with penalty
4. Remove on success or failure

### 6. User File System (uf.c/uf.h)

**Purpose**: Persist user and channel data

**Key Functions**:
- `load_uf(char *file)` - Load userfile
- `save_uf(char *file)` - Save userfile

**Storage Format**:
- Binary format (not human-readable)
- Stores hostmasks (nick!ident@host)
- Linked list in memory
- Channel data stored separately

**Persistence**:
- Auto-save on changes
- Backup support (`!backup`)
- Recovery on load failure

### 7. Self-Update System (update.c/update.h, http.c/http.h)

**Purpose**: HTTP-based binary updates with validation

**Update Process**:
1. Fork child process
2. Download binary via HTTP
3. Validate MD5 checksum
4. Replace old binary
5. Optionally restart

**Key Functions**:
- `fork_and_go()` - Fork update process
- `http_get()` - Simple HTTP client
- MD5 validation (md5.c/md5.h)

**Security**:
- MD5 checksum verification
- Binary validator (validate.c/validate.h)
- Signal-based parent notification

### 8. Pattern Matching (match.c/match.h)

**Purpose**: Wildcard pattern matching for hostmasks

**Wildcards**:
- `*` - Match zero or more characters
- `?` - Match exactly one character
- `#` - Match exactly one digit

**Key Functions**:
- `match(char *mask, char *string)` - Pattern match
- `extendhost()` - Expand partial hostmask
- Case-insensitive comparison

### 9. Signal Handling (signals.c/signals.h)

**Purpose**: Handle Unix signals

**Signals**:
- `SIGCHLD` - Child process (update) completion
- `SIGTERM`, `SIGINT` - Graceful shutdown
- `SIGSEGV`, `SIGBUS` - Crash handling (debug mode)

## Data Flow

### Connection Flow

```
Start
  ↓
Parse Config → Load Userfile
  ↓
Fork to Background (unless debug)
  ↓
Create PID File
  ↓
┌──────────────────┐
│ Connection Loop  │
│ Try next server  │
└────────┬─────────┘
         ↓
  Connect Socket → Register (USER/NICK)
         ↓
  Wait for 001 (Welcome)
         ↓
  Join Channels
         ↓
┌──────────────────┐
│   Main Loop      │
│ - Read IRC data  │
│ - Process events │
│ - Execute work() │
│ - Check timeouts │
└────────┬─────────┘
         ↓
  Disconnect
         ↓
  Reconnect or Exit
```

### Command Flow

```
IRC: PRIVMSG bot :!command args
         ↓
parse_line() extracts command
         ↓
Check if starts with !, ., or @
         ↓
Extract command and arguments
         ↓
got_cmd(who, to, cmd, args)
         ↓
Check user authorization (hostmask)
         ↓
Parse command-specific arguments
         ↓
Execute command logic
         ↓
Generate response
         ↓
send_reply() or send_privmsg()
```

### Nickname Queue Flow

```
User: !nick DesiredNick
         ↓
add_queue("DesiredNick", HIGH_PRIO)
         ↓
work() calls check_queue()
         ↓
Send NICK command to server
         ↓
Add penalty (flood protection)
         ↓
Mark as sent in queue
         ↓
Wait for response...
         ↓
got_nick() or got_numeric(433)
         ↓
Update queue: success → remove
              failure → retry later
```

## Key Algorithms

### Penalty System (Flood Protection)

**Purpose**: Prevent IRC server from disconnecting for flooding

**Implementation**:
```c
struct knb_user {
    unsigned int penalty;  // Current penalty
    time_t next_penalty_reset;
};

void add_penalty(int seconds) {
    me.penalty += seconds;
}

void work() {
    if (me.penalty > 0) {
        me.penalty--;
        return;  // Skip this iteration
    }
    // Process queue, send commands
}

void penalty_reset() {
    // Every 24 hours
    me.penalty = 0;
}
```

**Penalty Values**:
- `NICK`/`ISON`: 1 second + configured delay
- `JOIN`: 3 seconds
- `PART`: 5 seconds
- `PRIVMSG`: 2 seconds
- `MODE`: 4 seconds
- `KICK`: 4 seconds

### Nickname Recovery Algorithm

**Phase 1: Check**
- Every ~30 minutes (randomized): Send `ISON nick1 nick2 ...`
- Parse response to determine available nicknames
- Update status in `me.letter[]` array

**Phase 2: Catch**
- If preferred nick is available and `me.catch == true`
- Add to queue with appropriate priority
- Send `NICK` command via queue system

**Phase 3: Maintain**
- Once preferred nick is acquired, set `me.catched = true`
- Stop checking other nicknames (optimization)
- Protect nickname from accidental changes

**Phase 4: Collision Handling**
- On nick collision (433): Try alternative
- If `altnick on`: Append characters from `nickappend`
- If `noaltnick on`: Don't use UID as fallback
- Mark nickname as unavailable for 30 minutes

### Pattern Matching Algorithm

**Wildcard Expansion**:
```c
// * matches zero or more characters
// ? matches exactly one character
// # matches exactly one digit

bool match(char *mask, char *string) {
    while (*mask && *string) {
        if (*mask == '*') {
            // Try matching rest at each position
            ...
        } else if (*mask == '?') {
            // Match any single character
            mask++; string++;
        } else if (*mask == '#') {
            // Match single digit
            if (!isdigit(*string)) return false;
            mask++; string++;
        } else {
            // Exact match (case insensitive)
            if (tolower(*mask) != tolower(*string))
                return false;
            mask++; string++;
        }
    }
    ...
}
```

## Data Structures

### Main Bot Structure

```c
struct knb_user {
    // Identity
    char nick[NICK_LEN+1];
    char orignick[NICK_LEN+1];  // Preferred nick
    char uid[UID_LEN+1];
    char ident[IDENT_LEN+1];
    char realname[REALNAME_LEN+1];
    
    // Connection
    int sock;                    // Socket FD
    unsigned int conn;           // Connection status
    time_t timeout;
    time_t connected;
    
    // Servers
    struct knb_server server[MAX_SERVERS];
    unsigned int servers;
    int last_server;
    
    // Channels
    struct knb_channel channel[MAX_CHANNELS];
    int channels;
    
    // Nicknames
    struct knb_nicklist letter[MAX_NICKS];
    unsigned int letters;
    
    // State
    bool catch;                  // Catching enabled
    bool catched;                // Has preferred nick
    unsigned int penalty;        // Flood penalty
    
    // Config
    int ctcp;
    int reply_type;
    int public_reply;
    int delay;
    int alphabet;
    
    // Files
    char userfile[CHAN_LEN+1];
    char logfile[CHAN_LEN+1];
    char pidfile[CHAN_LEN+1];
};
```

### Nickname Queue

```c
struct knb_queue {
    char nick[NICK_LEN + 1];
    bool used;
    unsigned int type;
    unsigned int prio;           // Priority level
    bool send;                   // Already sent NICK
};

// Global array
struct knb_queue queue[MAX_QUEUE];  // Max 50
int queues;  // Current count
```

### User List (Linked List)

```c
struct knb_userlist {
    char nick[NICK_LEN + 1];
    char ident[IDENT_LEN + 1];
    char host[MASK_LEN + 1];
    struct knb_userlist *next;
};

// Global pointers
struct knb_userlist *first_user;
struct knb_userlist *user;
struct knb_userlist *user2;
```

## Build System

### Configure Script

**Purpose**: Detect platform and generate Makefiles

**Process**:
1. Detect OS (`uname`)
2. Detect architecture
3. Detect GCC version
4. Process command-line options
5. Generate Makefile and Makefile.* for each target

**Platform Detection**:
- Linux: `-march=native -pipe -O2`
- SunOS: `-lsocket -lnsl` libraries
- Cygwin: Windows-specific flags
- macOS: Disable `-march=native` for Clang

### Build Targets

**Dynamic** (default):
- Dynamically linked binary
- Validator enabled
- Stripped debug symbols
- Patched with MD5

**Static**:
- Statically linked
- Portable across systems
- Larger binary size

**Debug**:
- Debug symbols included
- `DEBUG` macro defined
- No fork to background
- Extra logging
- No stripping

**Release**:
- Creates distributable package
- Includes docs, scripts, examples
- Tar/gzip archive

### Binary Validation

**Purpose**: Detect binary tampering

**Process**:
1. Compile bot binary
2. Run `validator` tool
3. Compute MD5 of binary
4. Patch magic bytes + MD5 into binary
5. At runtime: `validate()` checks MD5

**Note**: As of v0.2.6, validation mismatch is warning-only

## Security Considerations

### Input Validation

- All configuration values validated (length, type, range)
- IRC input sanitized before parsing
- Command arguments checked for length
- Hostmask patterns validated

### Buffer Management

- All buffers have fixed sizes with checks
- `strncpy()` used instead of `strcpy()`
- `snprintf()` used instead of `sprintf()` (v0.2.6+)
- Proper null termination

### Access Control

- Hostmask-based authentication
- Wildcard pattern matching
- No passwords (hostmask-only)
- First user has full control

### Network Security

- No SSL/TLS support (plain text)
- Basic flood protection
- Timeout handling
- No automatic DCC or file transfers

### Process Security

- Fork to background (daemon)
- PID file for watchdog
- Signal handlers for cleanup
- No shell execution (except `!restart`)

## Performance Characteristics

### Memory Usage

- **Static**: ~500 KB binary
- **Runtime**: ~2-5 MB (depends on users, channels, queue)
- **Scaling**: O(n) with number of users/channels

### CPU Usage

- **Idle**: <1% on modern systems
- **Active**: Spikes during command processing
- **Penalty system**: Intentional delays for flood control

### Network

- **Bandwidth**: Minimal (~1-10 KB/s typical)
- **Latency**: Sub-second command response
- **Connections**: One persistent IRC connection

## Extensibility

### Adding New Commands

1. Edit `cmds.c`
2. Add command handler in `got_cmd()`
3. Recompile

### Adding Configuration Options

1. Define option in `cfg.h`
2. Add parsing in `cfg.c:set_variable()`
3. Add to structure in `structs.h`
4. Update examples in `cfg-examples/`

### Adding IRC Events

1. Add handler function in `events.c`
2. Call from `parse_line()` in `irc.c`
3. Recompile

## Future Improvements

Potential enhancements (not currently implemented):

- SSL/TLS support
- SASL authentication
- Multi-threading for updates
- SQLite for userfile
- Plugin system
- Web interface
- Statistics tracking

## Code Metrics

- **Total Source Lines**: ~10,000 lines
- **Core Files**: 15 C files + headers
- **Compilation Time**: <5 seconds on modern hardware
- **Binary Size**: ~500 KB (stripped, dynamic)

## Debugging

### Debug Build

```bash
make debug
./knb conf
```

Features:
- Runs in foreground
- `debug.log` file
- `printd()` debug output
- Core dumps on crash (with GDB)

### Troubleshooting Tips

1. Check `debug.log` for detailed events
2. Use `!debug` command for queue state
3. Increase logging verbosity in code
4. Use GDB for crash analysis
5. Check IRC server logs

## Related Documentation

- [Installation Guide](INSTALLATION.md)
- [Configuration Reference](CONFIGURATION.md)
- [Development Guide](DEVELOPMENT.md)
- [Troubleshooting](TROUBLESHOOTING.md)

---

For questions or contributions, see [DEVELOPMENT.md](DEVELOPMENT.md).
