CREATE TABLE IF NOT EXISTS  t_user  (
    f_id  bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID',
    f_user_id  bigint(20) NOT NULL COMMENT '用户ID',
    f_username  varchar(64) NOT NULL COMMENT '用户名',
    f_nickname  varchar(64) NOT NULL COMMENT '用户昵称',
    f_password  varchar(64) DEFAULT NULL COMMENT '用户密码',
    f_facetype  int(10) DEFAULT 0 COMMENT '用户头像类型',
    f_customface  varchar(32) DEFAULT NULL COMMENT '自定义头像名',
    f_customfacefmt  varchar(6) DEFAULT NULL COMMENT '自定义头像格式',
    f_gender  int(2) DEFAULT 0 COMMENT '性别',
    f_birthday  bigint(20) DEFAULT 19900101 COMMENT '生日',
    f_signature  varchar(256) DEFAULT NULL COMMENT '个性签名',
    f_address  varchar(256) DEFAULT NULL COMMENT '地址',
    f_phonenumber  varchar(64) DEFAULT NULL COMMENT '电话',
    f_mail  varchar(256) DEFAULT NULL COMMENT '邮箱',
    f_register_time int(11) unsigned NOT NULL COMMENT '注册时间',
    f_update_time  int(11) unsigned NOT NULL COMMENT '更新时间',
    f_owner_id  bigint(20) DEFAULT 0 COMMENT '群账号群主userid',
    f_remark  varchar(64) DEFAULT NULL COMMENT '备注',
    f_teaminfo blob DEFAULT NULL COMMENT '好友分组信息',
    PRIMARY KEY ( f_user_id ),
    INDEX f_user_id (f_user_id),
    KEY  f_id  ( f_id )
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8


CREATE TABLE IF NOT EXISTS  t_user_relationship  (
    f_id  bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID',
    f_user_id1  bigint(20) NOT NULL COMMENT '第一个用户id',
    f_user_id2  bigint(20) NOT NULL COMMENT '第二个用户id',
    f_user1_teamname VARCHAR(32) NOT NULL DEFAULT "我的好友" COMMENT "用户2在用户1的好友分组名称",
    f_user1_markname VARCHAR(32) COMMENT "用户2在用户1的备注名称",
    f_user2_teamname VARCHAR(32) NOT NULL DEFAULT "我的好友" COMMENT "用户1在用户2的好友分组名称",
    f_user2_markname VARCHAR(32) COMMENT "用户1在用户2的备注名称",
    f_update_time int(11) unsigned NOT NULL COMMENT '更新时间',
    f_remark  varchar(64) DEFAULT NULL COMMENT '备注',
    PRIMARY KEY ( f_id ),
    KEY  f_id  ( f_id )
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8


CREATE TABLE IF NOT EXISTS  t_chatmsg  (
    f_id  bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID',
    f_senderid  bigint(20) NOT NULL COMMENT '发送者id',
    f_targetid  bigint(20) NOT NULL COMMENT '接收者id',
    f_msgcontent  BLOB NOT NULL COMMENT '聊天内容',
    f_create_time int(11) unsigned NOT NULL COMMENT '创建时间',
    f_remark  varchar(64) DEFAULT NULL COMMENT '备注',
    PRIMARY KEY ( f_id ),
    KEY  f_id  ( f_id )
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8