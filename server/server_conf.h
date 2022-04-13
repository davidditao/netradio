#ifndef SERVER_CONF_H__
#define SSERVER_CONF_H__

#define DEFAULT_MEDIADIR        "/var/media"    // 默认媒体库
#define DEFAULT_IF              "eth0"          // 默认网卡

enum{
    RUN_DEAMON = 1,         // 后台运行（守护进程）
    RUN_FOREGROUND          // 前台运行
};

struct server_conf_st{              // 服务器配置
    char *rcvport;      // 接收端口
    char *mgroup;       // 多播组
    char *media_dir;    // 媒体库
    char runmode;       // 前台或后台运行
    char *ifname;       // 网卡

};

extern struct server_conf_st server_conf;   // 将server_conf设置为外部变量

#endif SSERVER_CONF_H__