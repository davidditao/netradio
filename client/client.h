#ifndef CLIENT_H_
#define CLIENT_H_

#define DEFAULT_PLAYERCMD  "/usr/bin/mpg123 - > /dev/null" // 默认播放器 
// mpg123 命令行中加 - 表示只接收标准输入来的内容
// 将播放器的输出重定向到 /dev/null 中

struct client_conf_st{		// 可以由用户指定的参数
	char *rcvport;			// 接收端口号
	char *mgroup;			// 多播组地址
	char *player_cmd;		// 命令行参数
};

extern struct client_conf_st client_conf;  // 默认的用户参数


#endif
