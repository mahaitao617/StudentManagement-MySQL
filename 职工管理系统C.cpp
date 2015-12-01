#include <windows.h>
#include <iostream>
#include <mysql.h>
#include <iomanip>
#include <cstring>
#include <conio.h>
#include <cstdio>
#include <ctime>
using namespace std;
char column[20][30];	//�洢��������
FILE *fplog;
char info[10][50];		//��ʱ��ŵ�ǰ���������ֵ
MYSQL mysql, *sock;		//mysql����
MYSQL_RES *res;			//����ṹ�������е�һ����ѯ�����
int num_fields;			//������е�����
char query[1024];		//��ѯ���
char tableName[20];		//����

bool QueryDatabase(const char*s=0,int show=1,const char* todo="select * ");
bool ConnectDatabase();
void printDateTime(FILE * fp);
void DB(const char* tb = "worker");
void close();
void Choose();
void Add();
void Delete();
void Search();
void Modify();
void Sort();
void ShowAll();

int main() {
	DB("workers");
    ShowAll();
    Choose();
    close();
    return 0;
}

void Choose() {
    char c;
    while(c!='0') {
        cout<<setfill('*')<<setw(80)<<"*"<<endl;
        cout<<"1.����\n2.ɾ��\n3.����\n4.�޸�\n5.����\n6.��ʾ����\n0.�˳�"<<endl;
        cout<<setfill('*')<<setw(80)<<"*"<<endl<<"��ѡ��: ";
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
                cout<<"\r�����������������:  \b\b";
            }
        } while(c<'0' || c>'6');
        cout<<endl;
        fclose(fplog);
        fplog = fopen("log.txt","a+");
    }
}

void Add() {
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
    } else {
        puts("��ӳɹ�");
        fprintf(fplog,"%s\n",query);		//������־
    }
}
void Delete() {
    char temp[128];
    cout<<"��������Ҫɾ����"<<column[0]<<": ";
    cin>>info[0];
    sprintf(temp,"where %s='%s'",column[0],info[0]);
    if(QueryDatabase(temp)) {      //ִ��SQL���
        cout<<"ȷ��ɾ����Ԫ�飿(1��yȷ��):";
        char c=getch();
        if(c=='1' || c=='y' || c=='Y') {
            printDateTime(fplog);
            if(QueryDatabase(temp,1,"delete")) {
                puts("ɾ���ɹ�");
                fprintf(fplog,"%s\n",query);		//������־
            } else {
                printf("ɾ��ʧ�� (%s)\n",mysql_error(sock));
                fprintf(fplog,"Error: %s (%s)\n",query,mysql_error(sock));		//������־
            }
        } else
            cout<<"��ȡ��ɾ��"<<endl;
    } else {
        printDateTime(fplog);
        puts("ɾ��ʧ�� (��Ԫ�鲻����)");
        fprintf(fplog,"Error: %s : ��Ԫ�鲻����\n",query);		//������־
    }
}
void Search() {
    cout<<"��ѡ����ҷ�ʽ:\n";
    for(int i=0; i<num_fields; i++)
        cout<<i+1<<"."<<column[i]<<"  ";
    char c=getch();
    if(c>48 && c<49+num_fields) {
        cout<<"\n��������Ҫ���ҵ�"<<column[c-49]<<": ";
        cin>>info[0];
        char temp[128];
        sprintf(temp,"where %s='",column[c-49]);
        strcat(temp,info[0]);
        strcat(temp,"'");
        QueryDatabase(temp);
    }
}
void Modify() {
    cout<<"��ѡ����Ҫ�޸ĵ���������:\n";
    for(int i=0; i<num_fields; i++)
        cout<<i+1<<"."<<column[i]<<"  ";
    char c=getch();
    char temp[128];
    if(c>48 && c<49+num_fields) {
        cout<<"\n�������������Ե�"<<column[c-49]<<": ";
        cin>>info[0];
        sprintf(temp,"where %s='%s'",column[c-49],info[0]);
        if(QueryDatabase(temp)) {
            cout<<"��ѡ����Ҫ�޸ĵ�����:\n";
            for(int i=0; i<num_fields; i++)
                cout<<i+1<<"."<<column[i]<<"    ";
            char c=getch();
            if(c>48 && c<49+num_fields) {
                cout<<"\n�������µ�"<<column[c-49]<<": ";
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
        } else
            cout<<"δ���������޸����������޸���������"<<endl;
    }
}
void Sort() {
    cout<<"��ѡ������ʽ:\n";
    for(int i=0; i<num_fields; i++)
        cout<<i+1<<"."<<column[i]<<"  ";
    char c=getch();
    if(c>48 && c<49+num_fields) {
        char temp[128], way[5]="asc";
        cout<<"\r��"<<column[c-49]<<"���������밴1��y,��qȡ������,������������������: ";
        char c2=getch();
        cout<<"\r\t\t\t\t\t\t\t\r��"<<column[c-49];
        if(c2=='q' || c2=='Q') {
            cout<<"\r\t\t\t\t\t\t\t\r��ȡ������"<<endl;
            return;
        } else if(c2=='1' || c2=='y' || c2=='Y') {
            strcpy(way,"desc");
            cout<<"��������"<<endl;
        } else
            cout<<"��������"<<endl;
        sprintf(temp,"order by %s %s",column[c-49],way);
        if(QueryDatabase(temp)) {
            printDateTime(fplog);
            fprintf(fplog,"%s\n",query);
        }
    }
}
void ShowAll() {
    QueryDatabase();
}
//�������ݿ�
bool ConnectDatabase() {
    int result = 1;
    mysql_init(&mysql);  //��ʼ��mysql���������ݿ�
    if (!(sock = mysql_real_connect(&mysql,"localhost", "root", 0, "test",0,NULL,0))) {
        printf( "Error connecting to database:%s\n",mysql_error(&mysql));
        result = 0;
    } else {
        printf("Connected successful\n");
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

//��ѯ����
bool QueryDatabase(const char*s,int show,const char* todo) {
    int result = 1;
    sprintf(query, "%s from %s ",todo,tableName);
    if(s)
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
        printf(">>>����ѯ�� %d ����<<<\n",affect);
        //��ȡ�ֶε���Ϣ
        if(affect>0) {
            for(int i=0; i<num_fields; i++)
                printf("%-*s",80/num_fields,column[i]);
            puts("");
            //��ӡ��ȡ������
            MYSQL_ROW row; //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ
            while (row = mysql_fetch_row(res)) {    //��ȡ��һ��
                for(int i=0; i<num_fields; i++)
                    printf("%-*s",80/num_fields,row[i]);
                puts("");
            }
        }
    }
    puts("����������������������������������������\n");
    return affect;
}

void printDateTime(FILE * fp) {
    time_t now_time=time(NULL);
    struct tm *newtime=localtime(&now_time);
    char tmpbuf[128];
    static int first = 1;
    if(first) {
        strftime(tmpbuf,128,"\n******%Y/%m/%d******\n",newtime);
        fprintf(fp,tmpbuf);
        first = 0;
    }
    strftime(tmpbuf,128,"%H:%M:%S    ",newtime);
    fprintf(fp,tmpbuf);
}

void DB(const char*tb) {
    fplog = fopen("log.txt","a+");
    strcpy(tableName,tb);
    if(!ConnectDatabase()) {
        cout<<"���ݿ�����ʧ��,�������Ӻ����ԣ�"<<endl;
        system("pause");
        exit(-1);
    }
}
//�ͷ���Դ
void close() {
    mysql_free_result(res);
    mysql_close(sock);
    fclose(fplog);
}
