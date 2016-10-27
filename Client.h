#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

#include <string>
#include "Common.h"

using namespace std;

// �ͻ����࣬�������ӷ��������ͺͽ�����Ϣ
class Client {

public:
    // �޲������캯��
    Client();

    // ���ӷ�����
    void Connect();

    // �Ͽ�����
    void Close();

    // �����ͻ���
    void Start();

private:

    // ��ǰ���ӷ������˴�����socket
    int sock;

    // ��ǰ����ID
    int pid;
    
    // epoll_create������ķ���ֵ
    int epfd;

    // �����ܵ�������fd[0]���ڸ����̶���fd[1]�����ӽ���д
    int pipe_fd[2];

    // ��ʾ�ͻ����Ƿ���������
    bool isClientwork;

    // ������Ϣ������
    char message[BUF_SIZE];

    //�û����ӵķ����� IP + port
    struct sockaddr_in serverAddr;
};



#endif //CHATROOM_CLIENT_H