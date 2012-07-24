extern int sendResponseLine(struct udpinfo *up, char *line, int len);
extern int udp_putchar(int c);
extern int udp_umoncmd(char *cmd);
extern void processChar(int c);
extern void cmdInit(void);
