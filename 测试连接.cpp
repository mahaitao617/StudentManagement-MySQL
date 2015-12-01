#include <stdio.h>
#include <winsock.h>
#include <mysql.h>
#include <string.h>
#pragma comment( lib, "libmysql.lib")

int main() {
/// ����1
    puts("����1...(����)");
    //MYSQL���   (�κ�һ��mysql�������ǻ���MYSQL��������������)
    MYSQL* link_ = mysql_init(NULL);             //��ʼ��MYSQL���
	//���ó�ʱʱ��(���ӳ�ʱʱ�䣬��ѯ��ʱʱ�䣬д���ݿⳬʱʱ��)
    int  timeout =  3;                      //��ʱʱ������Ϊ3��
    if(link_ != NULL) {
        mysql_options(link_,MYSQL_OPT_CONNECT_TIMEOUT,(const char *)&timeout);
        //�������ӳ�ʱʱ��.
        mysql_options(link_,MYSQL_OPT_READ_TIMEOUT,(const char *)&timeout);
        //���ò�ѯ���ݿ�(select)��ʱʱ��
        mysql_options(link_,MYSQL_OPT_WRITE_TIMEOUT,(const char *)&timeout);
        //����д���ݿ�(update,delect,insert,replace��)�ĳ�ʱʱ�䡣
    }
	//��������mysql����
    char  host_name[1024] = "localhost";		//mysql��������IP
    char  user_name[1024] = "root";				//�û���
    char  user_password[1024] = "123456";		///����		��Ҫ��Ϊ�Լ�������
    char  server_name[1024] = "test";			///���ݿ���	��Ҫ��Ϊ�Լ������ݿ���
    unsigned short host_port = 3306;			//�������˿�

    if(!mysql_real_connect(link_,host_name,user_name,user_password,server_name,host_port,NULL,0)) {
        //ʧ�ܴ���
        int error_code  =  mysql_errno(link_);            //��ȡ������
        //��Բ�ͬ�Ĵ�����error_code����Ӧ������ͬ�Ĵ���
        mysql_close(link_);                           //�ͷž��
    	printf("���ӽ���ʧ��! :%s\n",mysql_error(link_));
    }else {
    	puts("���ӽ����ɹ�");
        //���ӽ����ɹ������Խ��о���Ĳ���(select   insert    delete   update  replace�ȣ�
    }
/// �����ǵڶ������ӣ����Ժ�����һ�����ӷֿ�����
    puts("\n����2...(���Ӳ���ѯ)");
    char SqlText[256] = "";
    MYSQL mysql;
    MYSQL_RES *res = NULL;
    MYSQL_FIELD * fd = NULL;
    MYSQL_ROW row;
    int i = 0;

    mysql_init( &mysql );

    if ( !mysql_real_connect( &mysql, "localhost", "root",
                              "123456", "test", 3306, NULL, 0) ) {
        puts("���ݿ�����ʧ��");
        printf( "Error connecting to database: %s\n",mysql_error(&mysql));
        mysql_close( &mysql );
        return FALSE;
    } else {
        puts("���ݿ����ӳɹ�");
        mysql_query(&mysql,"set names 'GBK'");//�����ַ�������ֹ�����޷�������ʾ
        sprintf( SqlText, "insert into animals(name, kg) values ('chicken',6), ('dog', 4)");
        if ( !mysql_query(&mysql, SqlText ) ) {        //insertʧ��
            printf("Can't insert data to table: ");
            printf("%s\n", mysql_error(&mysql));
            mysql_close( &mysql );
            return FALSE;
        }
        sprintf( SqlText, "select * from %s","stu");	///stu��Ҫ��Ϊ�Լ������ݿ��ж�Ӧ�ı���
        //�������ݼ���
        if ( !mysql_query(&mysql, SqlText )) {
            res = mysql_store_result( &mysql );
            i = (int)mysql_num_rows( res );
            printf("Query: %s\n%d records found:\n", SqlText, i );
            //������ֶ���
            for (; fd = mysql_fetch_field(res);)
                printf("%-*s\t",50/mysql_num_fields(res), fd->name );
            puts("");

            //��ӡ��ȡ������
            MYSQL_ROW row; //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ
            while (row = mysql_fetch_row(res)) {    //��ȡ��һ��
                for(int i=0; i<mysql_num_fields(res); i++)
                    printf("%-*s",80/mysql_num_fields(res),row[i]);
                puts("");
            }
            mysql_free_result( res );
        } else {
        	printf("��ѯʧ��: %s\n",mysql_error(&mysql));
        	puts("�����66�б���");
            mysql_close( &mysql );
            return FALSE;
        }
    }
    mysql_close(&mysql);
    getchar();
    return TRUE;
}
