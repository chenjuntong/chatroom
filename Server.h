#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H

#include <string>

#include "Common.h"

using namespace std;

// ������࣬��������ͻ�������
class Server {

public:
    // �޲������캯��
    Server();

    // ��ʼ��������������
    void Init();

    // �رշ���
    void Close();

    // ���������
    void Start();

private:
    // �㲥��Ϣ�����пͻ���
    int SendBroadcastMessage(int clientfd);

    // ��������serverAddr��Ϣ
    struct sockaddr_in serverAddr;
    
    //����������socket
    int listener;

    // epoll_create������ķ���ֵ
    int epfd;
    
    // �ͻ����б�
    list<int> clients_list;
};



#endif //CHATROOM_SERVER_H