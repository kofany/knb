void add_penalty(int t);
int has_penalty(void);
int get_index_by_chan(char *ch);
int get_index_by_chan_f(char *ch);
int get_index_by_letter(char *nick);
void send_notice(char *to, char *format, ...);
void send_privmsg(char *to, char *format, ...);
void send_reply(char *to, char *format, ...);
void send_quit(char *msg, ...);
bool check_flags(char *mask);
void get_nick(char *who, char *n);
void get_nuh(char *who, char *n, char *u, char *h);
bool extendhost(char *host, unsigned int len, char *buf);
void generate_nick(char *nick);
void rand_nick(char *nick);
void irc(void);
void irc_msg(char *b);
void check_input(void);
bool istemp_letter(int index);
void selfcheck();
void send_ison();
bool add_channel(char *ch, char *key);
bool rem_channel(char *ch);
bool clear_channels();
void prepare_channels();
void channels();
void work();
void preconnect();
