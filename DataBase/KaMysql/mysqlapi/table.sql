CREATE TABLE IF NOT EXISTS  t_user  (
         f_id  bigint(20) NOT NULL AUTO_INCREMENT COMMENT '自增ID',
		 f_user_id  bigint(20) NOT NULL COMMENT '用户ID',
		 f_username  varchar(64) NOT NULL COMMENT '用户名',
         f_nickname  varchar(64) NOT NULL COMMENT '用户昵称',
         f_password  varchar(64) DEFAULT NULL COMMENT '用户密码',
		 f_gender  int(2) DEFAULT 0 COMMENT '性别',
         PRIMARY KEY ( f_user_id ),
		 INDEX f_user_id (f_user_id),
         KEY  f_id  ( f_id )
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8
