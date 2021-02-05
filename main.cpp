
#include "./Net/KaSocket.h"
#include "./Util/KaLog.h"
#include "./Net/TcpServer.h"
#include "./Util/KaSingleton.h"




#include <cstring>

int Test_KaSocket_Head() {

    KaNet::KaSocket ServerSock;
    //ServerSock.setNonBlock(true);       //设置为非阻塞之后就会立即返回
    ServerSock.setReuseAddr(true);
    ServerSock.setReusePort(true);
    short port = 8888;
    KaNet::KaNetAddr ServerAddr("127.0.0.1",port);
    ServerSock.bindAddr(ServerAddr);

    ServerSock.listen(10);


    KaNet::KaNetAddr Clientaddr;
    socklen_t ClientLen = sizeof(Clientaddr.GetAddr_in());
    socket_t clientfd  = ServerSock.accept(Clientaddr,&ClientLen);
    if(clientfd > 0){
        std::cout << "Get CLient sucess: " << clientfd << std::endl;
    }
    else
    {
        std::cout << "Get Client error:" << clientfd << std::endl;
    }

    char buf[] = "abcde" ;

    int ret = send(clientfd,buf,strlen(buf),0);
    std::cout << "已经发送" << ret << "个字节" << std::endl;

    return 0;
}


//测试日志系统
void  testLog(){
    LOGB("错误");
}




int main(){

    return 0;
}
