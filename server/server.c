#include <stdio.h>
#include <stdlib.h>

#include <proto.h>

#inlcude "server_conf.h"

// -M       ָ���ಥ��
// -P       ָ�����ն˿�
// -F       ǰ̨����
// -D       ָ��ý���λ��
// -I       ָ�������豸/����
// -H       ��ʾ����

struct server_conf_st server_conf = {.rcvport = DEFAULT_RCVPORT,\
                                    .mgroup = DEFAULT_MGROUP,\
                                    .media_dir = DEFAULT_MEDIADIR,\
                                    .runmode = RUN_DAEMON,\
                                    .ifname = DEFAULT_IF};  // ��ʼ��Ĭ��ֵ

void printfhelp(void){
    printf("-M       ָ���ಥ��\n");
    printf("-P       ָ�����ն˿�\n");
    printf("-F       ǰ̨����\n");
    printf("-D       ָ��ý���λ��\n");
    printf("-I       ָ�������豸/����\n");
    printf("-H       ��ʾ����\n");
}

int main(int argc, char *argv){
    // �����з���
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

    // �ػ����̵�ʵ��

    // socket��ʼ��

    // ��ȡƵ����Ϣ

    // ������Ŀ���߳�

    // ����Ƶ���߳�

    while(1){
        pause();
    }

    exit(0);
}

