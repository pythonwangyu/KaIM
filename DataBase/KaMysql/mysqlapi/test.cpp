/* Simple C program that connects to MySQL Database server*/   
#include <mysql/mysql.h>
#include <stdio.h>   
  
int main_test() {   
    MYSQL *conn;   
    MYSQL_RES *res;   
    MYSQL_ROW row;   
  
   char *server = "localhost";   
   char *user = "kanon";   
   char *password = "python2018"; /* 此处改成你的密码 */   
   char *database = "KaIM";   
  
    conn = mysql_init(NULL);   
  
   /* Connect to database */   
   if (!mysql_real_connect(conn, server,   
          user, password, database, 0, NULL, 0)) {   
      fprintf(stderr, "%s\n", mysql_error(conn));   
      exit(1);   
   }   
  
   /* send SQL query */   
   if (mysql_query(conn, "show tables")) {   
      fprintf(stderr, "%s\n", mysql_error(conn));   
      exit(1);   
   }   
  
    res = mysql_use_result(conn);   
  
   /* output table name */   
   printf("MySQL Tables in mysql database:\n");   
   while ((row = mysql_fetch_row(res)) != NULL)   
      printf("%s \n", row[0]);   

   if(mysql_query(conn,"desc t_user")){
       fprintf(stderr,"%s\n",mysql_error(conn));
       exit(1);
   }
  res = mysql_use_result(conn);
  while((row = mysql_fetch_row(res)) != NULL)
      printf("%s \n",row[0]);
   /* close connection */   
    mysql_free_result(res);   
    mysql_close(conn);   
    return 0;
} 


#include "KaDataBaseMysql.h"
#include <iostream>

using namespace std;

int main(){
    DataBaseMysql base; 
    base.initialize("127.0.0.1","kanon","python2018","KaIM");
    QueryResult *res = NULL;
    res = base.query("desc t_user");

    cout << "获取行号" << endl;
    cout << res->getRowCount() << endl;
    cout << "获取属性的数量" << endl;
    cout << res->getFieldCount() << endl;


    return 0;
}



















