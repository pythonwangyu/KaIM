//
// Created by ka on 2021/1/15.
//

#ifndef KANET_DAEMONRUN_H
#define KANET_DAEMONRUN_H

/*
 * @ 程序守护进程运行
 *
 * %    使用fork创建一个新的进程，然后将父进程杀死，之后新的进程就会成为孤儿进程，自动被INIT进程托管。
 *      此外会将进程的输入制定到无底洞中/dev/null。
 *      这样进程就会被后台自动接管，这样就不会在shell中输出奇怪的符号。
 * */

void daemon_run();


#endif //KANET_DAEMONRUN_H
