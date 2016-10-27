#include <iostream>

#include "Client.h"

using namespace std;

// �ͻ������Ա����

// �ͻ����๹�캯��
Client::Client(){
    
    // ��ʼ��Ҫ���ӵķ�������ַ�Ͷ˿�
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // ��ʼ��socket
    sock = 0;
    
    // ��ʼ�����̺�
    pid = 0;
    
    // �ͻ���״̬
    isClientwork = true;
    
    // epool fd
    epfd = 0;
}

// ���ӷ�����
void Client::Connect() {
    cout << "Connect Server: " << SERVER_IP << " : " << SERVER_PORT << endl;
    
    // ����socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("sock error");
        exit(-1); 
    }

    // ���ӷ����
    if(connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect error");
        exit(-1);
    }

    // �����ܵ�������fd[0]���ڸ����̶���fd[1]�����ӽ���д
    if(pipe(pipe_fd) < 0) {
        perror("pipe error");
        exit(-1);
    }

    // ����epoll
    epfd = epoll_create(EPOLL_SIZE);
    
    if(epfd < 0) {
        perror("epfd error");
        exit(-1); 
    }

    //��sock�͹ܵ���������������ӵ��ں��¼�����
    addfd(epfd, sock, true);
    addfd(epfd, pipe_fd[0], true);

}

// �Ͽ����ӣ������ر��ļ�������
void Client::Close() {

    if(pid){
       //�رո����̵Ĺܵ���sock
        close(pipe_fd[0]);
        close(sock);
    }else{
        //�ر��ӽ��̵Ĺܵ�
        close(pipe_fd[1]);
    }
}

// �����ͻ���
void Client::Start() {

    // epoll �¼�����
    static struct epoll_event events[2];
    
    // ���ӷ�����
    Connect();
    
    // �����ӽ���
    pid = fork();
    
    // ��������ӽ���ʧ�����˳�
    if(pid < 0) {
        perror("fork error");
        close(sock);
        exit(-1);
    } else if(pid == 0) {
        // �����ӽ���ִ������
        //�ӽ��̸���д��ܵ�������ȹرն���
        close(pipe_fd[0]); 

        // ����exit�����˳�������
        cout << "Please input 'exit' to exit the chat room" << endl;

        // ����ͻ������������򲻶϶�ȡ���뷢�͸������
        while(isClientwork){
            bzero(&message, BUF_SIZE);
            fgets(message, BUF_SIZE, stdin);

            // �ͻ����exit,�˳�
            if(strncasecmp(message, EXIT, strlen(EXIT)) == 0){
                isClientwork = 0;
            }
            // �ӽ��̽���Ϣд��ܵ�
            else {
                if( write(pipe_fd[1], message, strlen(message) - 1 ) < 0 ) { 
                    perror("fork error");
                    exit(-1);
                }
            }
        }
    } else { 
        //pid > 0 ������
        //�����̸�����ܵ����ݣ�����ȹر�д��
        close(pipe_fd[1]); 

        // ��ѭ��(epoll_wait)
        while(isClientwork) {
            int epoll_events_count = epoll_wait( epfd, events, 2, -1 );

            //��������¼�
            for(int i = 0; i < epoll_events_count ; ++i)
            {
                bzero(&message, BUF_SIZE);

                //����˷�����Ϣ
                if(events[i].data.fd == sock)
                {
                    //���ܷ������Ϣ
                    int ret = recv(sock, message, BUF_SIZE, 0);

                    // ret= 0 ����˹ر�
                    if(ret == 0) {
                        cout << "Server closed connection: " << sock << endl;
                        close(sock);
                        isClientwork = 0;
                    } else {
                        cout << message << endl;
                    }
                }
                //�ӽ���д���¼������������̴������ͷ����
                else { 
                    //�����̴ӹܵ��ж�ȡ����
                    int ret = read(events[i].data.fd, message, BUF_SIZE);

                    // ret = 0
                    if(ret == 0)
                        isClientwork = 0;
                    else {
                        // ����Ϣ���͸������
                        send(sock, message, BUF_SIZE, 0);
                    }
                }
            }//for
        }//while
    }
    
    // �˳�����
    Close();
}