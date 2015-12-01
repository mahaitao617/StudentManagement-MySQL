#include <windows.h>
#include <iostream>
#include <mysql.h>
#include <iomanip>
#include <cstring>
#include <conio.h>
#include <cstdio>
#include <ctime>
using namespace std;
class DB_worker {
private:
    char column[20][30];	//�洢��������
    FILE *fplog;
    char info[10][50];		//��ʱ��ŵ�ǰ���������ֵ
    MYSQL mysql, *sock;		//mysql����
    MYSQL_RES *res;			//����ṹ�������е�һ����ѯ�����
    int num_fields;			//������е�����
    char query[1024];		//��ѯ���
    char tableName[20];		//����

    bool QueryDatabase(const char*s=0,int show=1,const char* todo="select *",int saveToFile=0);
    bool ConnectDatabase();
    void printDateTime(FILE * fp);
public:
    DB_worker(const char* tb = "stu");
    ~DB_worker();
    void Choose();
    void Add();
    void Delete();
    void Search();
    void Modify();
    void Sort();
    void ShowAll(int saveToFile=0);
};

int main() {
	system("color fd");
    DB_worker workers("worker");
    workers.ShowAll();
    workers.Choose();
    return 0;
}
//���캯�����������ݿ�
DB_worker::DB_worker(const char*tb) {
    fplog = fopen("log.txt","a+");
    strcpy(tableName,tb);
    if(!ConnectDatabase()) {
        cout<<"���ݿ�����ʧ��,�������Ӻ����ԣ�"<<endl;
        system("pause");
        exit(-1);
    }
}
//�ͷ����Ӻ�������Դ
DB_worker::~DB_worker() {
    mysql_free_result(res);
    mysql_close(sock);
    fclose(fplog);
}
void DB_worker::Choose() {
    char c;
    while(c!='0') {
    	ShowAll(1);
        for(int j=0;j<65;j++)
			cout<<"��";
		cout<<endl;
        cout<<"\t\t\t\t\t\t\t1.����һ��ְ��\n\t\t\t\t\t\t\t2.ɾ��һ��ְ��\n\t\t\t\t\t\t\t3.����"
			<<"\n\t\t\t\t\t\t\t4.�޸�\n\t\t\t\t\t\t\t5.����\n\t\t\t\t\t\t\t6.��ʾ����\n\t\t\t\t\t\t\t0.�˳�"<<endl;
        for(int j=0;j<65;j++)
			cout<<"��";
		cout<<endl;
        cout<<endl<<"��ѡ��: ";
        do {
            c=getche();
            cout<<"\r\t\t\t\r"<<c<<":";
            switch(c) {
            case '1':
                Add();
                break;
            case '2':
                Delete();
                break;
            case '3':
                Search();
                break;
            case '4':
                Modify();
                break;
            case '5':
                Sort();
                break;
            case '6':
                ShowAll();
                break;
            case '0':
                break;
            default:
                cout<<"\r�����������������:";
            }
        } while(c<'0' || c>'6');
        cout<<endl;
        fclose(fplog);
        fplog = fopen("log.txt","a+");
    }
}
void DB_worker::Add() {
    sprintf(query, "insert into %s values ('",tableName);
    cout<<"������ "<<column[0]<<": ";
    cin>>info[0];
    strcat(query,info[0]);
    for(int i=1; i<num_fields; i++) {
        cout<<"������ "<<column[i]<<": ";
        cin>>info[i];
        strcat(query,"','");
        strcat(query,info[i]);
    }
    strcat(query,"')");
	printDateTime(fplog);
    if(mysql_query(sock, query)) {      //ִ��SQL���
        printf("���ʧ�� (%s)\n",mysql_error(sock));
        fprintf(fplog,"Error: %s (%s)\n",query,mysql_error(sock));		//������־
    } else{
        puts("��ӳɹ�");
        fprintf(fplog,"%s\n",query);		//������־
    }
}
void DB_worker::Delete() {
    char temp[128];
    cout<<"��������Ҫɾ����"<<column[0]<<": ";
    cin>>info[0];
    sprintf(temp,"where %s='%s'",column[0],info[0]);
    if(QueryDatabase(temp)) {      //ִ��SQL���
        cout<<"ȷ��ɾ����Ԫ�飿(1��yȷ��):";
        char c=getch();
        if(c=='1' || c=='y' || c=='Y'){
			printDateTime(fplog);
            if(QueryDatabase(temp,1,"delete")) {
                puts("ɾ���ɹ�");
                fprintf(fplog,"%s\n",query);		//������־
            } else {
                printf("ɾ��ʧ�� (%s)\n",mysql_error(sock));
                fprintf(fplog,"Error: %s (%s)\n",query,mysql_error(sock));		//������־
            }
        }else
			cout<<"��ȡ��ɾ��"<<endl;
    } else {
		printDateTime(fplog);
        puts("ɾ��ʧ�� (��Ԫ�鲻����)");
        fprintf(fplog,"Error: %s : ��Ԫ�鲻����\n",query);		//������־
    }
}
void DB_worker::Search() {
    cout<<"��ѡ����ҷ�ʽ:\n";
    for(int i=0; i<num_fields; i++)
        cout<<i+1<<"."<<column[i]<<"  ";
    char c=getch();
    if(c>48 && c<49+num_fields) {
        cout<<"\n��������Ҫ���ҵ�"<<column[c-49]<<": ";
        cin>>info[0];
        char temp[128];
        sprintf(temp,"where %s='%s'",column[c-49],info[0]);
        QueryDatabase(temp);
    }
}
void DB_worker::Modify() {
    cout<<"��ѡ����Ҫ�޸�Ԫ�����������:\n";
    for(int i=0; i<num_fields; i++)
        cout<<i+1<<"."<<column[i]<<"  ";
    char c=getch();
    char temp[128];
    if(c>48 && c<49+num_fields) {
        cout<<"\r\t\t\t\t\t\t\t\t\t\t\r����������Ԫ���"<<column[c-49]<<": ";
        cin>>info[0];
        sprintf(temp,"where %s='%s'",column[c-49],info[0]);
        if(QueryDatabase(temp)) {
            cout<<"��ѡ����Ҫ�޸ĵ�����:\n";
            for(int i=0; i<num_fields; i++)
                cout<<i+1<<"."<<column[i]<<"  ";
            char c=getch();
            if(c>48 && c<49+num_fields) {
                cout<<"\r\t\t\t\t\t\t\t\t\t\t\r�������µ�"<<column[c-49]<<": ";
                cin>>info[0];
                sprintf(query, "update %s set %s='%s' %s",tableName,column[c-49],info[0],temp);
				printDateTime(fplog);
                if(mysql_query(sock, query)) {      //ִ��SQL���
                    printf("�޸�ʧ�� (%s)\n",mysql_error(sock));
                    fprintf(fplog,"Error: %s (%s)\n",query,mysql_error(sock));	//������־
                } else {
                    puts("�޸ĳɹ�");
                    fprintf(fplog,"%s\n",query);	//������־
                }
            }
        }else
			cout<<"δ���������޸����������޸���������"<<endl;
    }
}
void DB_worker::Sort() {
    cout<<"��ѡ������ʽ:\n";
    for(int i=0; i<num_fields; i++)
        cout<<i+1<<"."<<column[i]<<"  ";
    char c=getch();
    if(c>48 && c<49+num_fields) {
        char temp[128], way[5]="asc";
        cout<<"\r\t\t\t\t\t\t\t\t\t\t\t\r��"<<column[c-49]
			<<"���������밴1��y,��qȡ������,������������������: ";
        char c2=getch();
        cout<<"\r\t\t\t\t\t\t\t\r��"<<column[c-49];
        if(c2=='q' || c2=='Q'){
        	cout<<"\r\t\t\t\t\t\t\t\r��ȡ������"<<endl;
			return;
        }else if(c2=='1' || c2=='y' || c2=='Y'){
            strcpy(way,"desc");
            cout<<"��������"<<endl;
        }else
			cout<<"��������"<<endl;
        sprintf(temp,"order by %s %s",column[c-49],way);
        if(QueryDatabase(temp)){
			printDateTime(fplog);
            fprintf(fplog,"%s\n",query);
        }
    }
}
void DB_worker::ShowAll(int saveToFile) {
    QueryDatabase(0,1,"select *",saveToFile);
}
//�������ݿ�
bool DB_worker::ConnectDatabase() {
    int result = 1;
    mysql_init(&mysql);  //��ʼ��mysql���������ݿ�
    if (!(sock = mysql_real_connect(&mysql,"localhost", "root", "", "test",0,NULL,0))) {
        printf( "Error connecting to database:%s\n",mysql_error(&mysql));
        result = 0;
    } else {
        printf("Connected successful\n");
		int timeout =  2;                      //��ʱʱ������Ϊ3��
		if(sock != NULL) {
			//�������ӳ�ʱʱ��.
			mysql_options(sock,MYSQL_OPT_CONNECT_TIMEOUT,(const char *)&timeout);
			//���ò�ѯ���ݿ�(select)��ʱʱ��
			mysql_options(sock,MYSQL_OPT_READ_TIMEOUT,(const char *)&timeout);
			//����д���ݿ�(update,delect,insert,replace��)�ĳ�ʱʱ�䡣
			mysql_options(sock,MYSQL_OPT_WRITE_TIMEOUT,(const char *)&timeout);
		}
        mysql_query(sock,"set names 'GBK'");//�����ַ�������ֹ�����޷�������ʾ
        sprintf(query, "select * from %s ",tableName);
        mysql_query(sock, query);
        res = mysql_store_result(sock);
        num_fields = mysql_num_fields(res);
        //��ȡ�ֶε���Ϣ
        for(int i=0; i<num_fields; i++)
            strcpy(column[i], mysql_fetch_field(res)->name);
        puts("");
    }
    puts("����������������������������������������\n");
    return result;
}
//������ѯ
bool DB_worker::QueryDatabase(const char*s,int show,const char* todo,int saveToFile) {
    sprintf(query, "%s from %s ",todo,tableName);
    if(s)	// �������������
        strcat(query,s);
    if(mysql_query(sock, query)) {      //ִ��SQL���
        printf("Query failed (%s)\n",mysql_error(sock));
        return 0;
    } else if(strcmp(todo,"delete")==0)
        return 1;
    //��ȡ�����
    if (!(res=mysql_store_result(sock))) {  //���sql�������󷵻صĽ����
        printf("Couldn't get result from %s\n", mysql_error(sock));
        return 0;
    }
    //��ӡ�����
    int affect = mysql_affected_rows(sock);
    if(show) {
		if(!saveToFile)
			printf(">>>ְ�����С�����������ѯ�� %d ����<<<\n",affect);
        //��ȡ�ֶε���Ϣ
        if(affect>0) {
        	if(saveToFile)
				freopen("data.txt","w",stdout);
            for(int i=0; i<num_fields; i++)
                printf("%-*s",130/num_fields,column[i]);
            puts("");
            //��ӡ��ȡ������
            MYSQL_ROW row; //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ
            while (row = mysql_fetch_row(res)) {    //��ȡ��һ��
                for(int i=0; i<num_fields; i++)
                    printf("%-*s",130/num_fields,row[i]);
                puts("");
            }
        	if(saveToFile){
				freopen("CON","w",stdout);
				return affect;
        	}
        }
    }
    puts("����������������������������������������\n");
    return affect;
}
//����־�ļ��д�ӡ��ǰ���ں�ʱ��
void DB_worker::printDateTime(FILE * fp) {
    time_t now_time=time(NULL);
    struct tm *newtime=localtime(&now_time);
    char tmpbuf[128];
    static int first = 1;
    if(first){
		strftime(tmpbuf,128,"\n******%Y/%m/%d******\n",newtime);
		fprintf(fp,tmpbuf);
		first = 0;
    }
    strftime(tmpbuf,128,"%H:%M:%S    ",newtime);
    fprintf(fp,tmpbuf);
}
