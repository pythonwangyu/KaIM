#include <iostream>
#include "KaBaseDefine.pb.h"

int main()
{
    IMBase::UserInfo a;
    a.set_user_id(1);
    a.set_user_gender(2);
    a.set_user_nick_name("kaonon");
    a.set_passwd("python2018");
    a.set_status(3);
    return 0;
}

