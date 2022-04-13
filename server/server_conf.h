#ifndef SERVER_CONF_H__
#define SSERVER_CONF_H__

#define DEFAULT_MEDIADIR        "/var/media"    // Ĭ��ý���
#define DEFAULT_IF              "eth0"          // Ĭ������

enum{
    RUN_DEAMON = 1,         // ��̨���У��ػ����̣�
    RUN_FOREGROUND          // ǰ̨����
};

struct server_conf_st{              // ����������
    char *rcvport;      // ���ն˿�
    char *mgroup;       // �ಥ��
    char *media_dir;    // ý���
    char runmode;       // ǰ̨���̨����
    char *ifname;       // ����

};

extern struct server_conf_st server_conf;   // ��server_conf����Ϊ�ⲿ����

#endif SSERVER_CONF_H__