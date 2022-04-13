#include <stdio.h>
#include <stdlib.h>

#include <proto.h>

#inlcude "server_conf.h"

// -M       指定多播组
// -P       指定接收端口
// -F       前台运行
// -D       指定媒体库位置
// -I       指定网络设备/网卡
// -H       显示帮助

struct server_conf_st server_conf = {.rcvport = DEFAULT_RCVPORT,\
                                    .mgroup = DEFAULT_MGROUP,\
                                    .media_dir = DEFAULT_MEDIADIR,\
                                    .runmode = RUN_DAEMON,\
                                    .ifname = DEFAULT_IF};  // 初始化默认值

void printfhelp(void){
    printf("-M       指定多播组\n");
    printf("-P       指定接收端口\n");
    printf("-F       前台运行\n");
    printf("-D       指定媒体库位置\n");
    printf("-I       指定网络设备/网卡\n");
    printf("-H       显示帮助\n");
}

int main(int argc, char *argv){
    // 命令行分析
    while(1){
        c = getopt(argc, argv, "M:P:FD:I:H");
        if(c < 0){
            break;
        }
        switch(c){
            case 'M':
                server_conf.mgroup = optarg;
                break;
            case 'P':
                server_conf.rcvport = optarg;
                break;
            case 'F':
                server_conf.runmode = RUN_FORGROUND;
                break;
            case 'D':
                server_conf.media_dir = optarg;
                break;
            case 'I':
                server_conf.ifname = optarg;
                break;
            case 'H':
                printfhelp();
                exit(0);
                break;
            default:
                abort();
                break;
        }
    }

    // 守护进程的实现

    // socket初始化

    // 获取频道信息

    // 创建节目单线程

    // 创建频道线程

    while(1){
        pause();
    }

    exit(0);
}

