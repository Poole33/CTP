// MaiKeMarketApi.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <time.h>
#include <process.h>
#include "ThostFtdcTraderApi.h"
#include "MaiKeMarketSpi.h"
#include "MySQLManager.h"
#include "iniClass.h"

using namespace std;

// ���ӿ�
#pragma comment (lib, "thostmduserapi.lib")
#pragma comment (lib, "thosttraderapi.lib")

// ---- ȫ�ֱ��� ---- //
// ��������
TThostFtdcBrokerIDType gBrokerID = "";                   // ģ�⾭���̴���
TThostFtdcInvestorIDType gInvesterID = "";             // Ͷ�����˻���
TThostFtdcPasswordType gInvesterPassword = "";      // Ͷ��������

// �������
CThostFtdcMdApi *g_pMdUserApi = nullptr;                     // ����ָ��
char* gMdFrontAddr = new char[81];                           // ģ������ǰ�õ�ַ
char *g_pInstrumentID[] = {"cu1801","cu1805","al1801"};      // �����Լ�����б��С��ϡ���֣��������ѡһ��
const int instrumentNum = 24;                                       // �����Լ��������

int iRequestID = 0;// ������

MySQLManager *mysql;       //���ݿ�
iniClass mini;



//�����Լ����
void SetContratNum(string name,int place){
#pragma region �����Լ����
	//��ȡ��ǰʱ��
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	//��ȡ��ݺ���λ
	stringstream yearstream;
	yearstream << sys.wYear;
	string yearStr = yearstream.str().substr(2, 3);
	//std::cout << yearStr << endl;
	stringstream custream;
	int j=1;
	//ƴ�ӵ�ǰ�����Լ����
	for (int i = 1+place; i < 13+place; i++)	//Ϊ�˶�Ӧ�����Ӧ��λ��
	{
		//ƴ�Ӻ�Լ����ͷ
		string cu;
		char *aa = new char();
		cu = name + yearStr;


		custream << sys.wMonth + j;

		//�����ǰ�·�Ϊ12������ݼ�1
		if (sys.wMonth + j > 12)
		{
			yearstream.str("");
			yearstream << (sys.wYear + 1);
			yearStr = yearstream.str().substr(2, 3);
			cu = name + yearStr;
			//std::cout << yearstream.str() << endl;
		}

		//ƴ�Ӻ�Լ����
		if (sys.wMonth + j < 10)
		{

			cu += "0" + custream.str();
			custream.str("");

		}
		else if (sys.wMonth + j > 12)
		{
			custream.str("");
			custream << sys.wMonth + j - 12;
			if (sys.wMonth + j - 12 <10)
			{
				cu += "0" + custream.str();
				custream.str("");
			}
			else
			{
				cu += custream.str();
				custream.str("");
			}
		}
		else
		{
			cu += custream.str();
			custream.str("");
			//std::cout << cu << endl;
		}

		//std::cout << (char*)cu.data() << endl;
		strcpy(aa,cu.c_str());
		g_pInstrumentID[i - 1] = aa;
		j++;
		//std::cout << g_pInstrumentID[1] << endl;
	}
	//instrumentNum +=12;
#pragma endregion
}



void setBufferFiles(){
#pragma region �����洢���ݵı��ػ���
	// �����Ҫ�����ļ��������ݿ⣬�����ﴴ����ͷ,��ͬ�ĺ�Լ�����洢
	for (int i = 0; i < 24; i++){
		char filePath[100] = {'\0'};
		sprintf(filePath, "%s_market_data.csv", g_pInstrumentID[i]);
		std::ofstream outFile;
		outFile.open(filePath, std::ios::out); // �½��ļ�
		outFile.close();
	}
#pragma endregion
}



//���������ݣ��������ݿ�
unsigned __stdcall SetTableMesByBuffer(void* pArguments){
#pragma region ���������ݣ��������ݿ�
	while (1)
	{
		Sleep(300000);
		SetContratNum("cu",0);
		SetContratNum("al",12);
		for (int i = 0; i < 24; i++)
		{
			char filePath[100] = {'\0'};
			sprintf(filePath, "%s_market_data.csv", g_pInstrumentID[i]);
			ifstream inFile;
			inFile.open(filePath,ios::in);
			char str[1024];
			inFile >> str;
			//cout << str << endl;
			inFile.close();

			char seg[] = ","; /*�ָ�������Ϊ����comma���ָ�������Ϊ��ָ���ģ���ֺţ��ո��*/  
			char charlist[44][44]={""};/*ָ���ָ������ַ����洢��λ�ã����ﶨ���ά�ַ�������*/  
			int v =0;  
			char *substr= strtok(str, seg);/*�����ֳɵķָ��,substrΪ�ָ���������ַ���*/  
			while (substr != NULL) {    
				strcpy(charlist[v],substr);/*���·ָ���������ַ���substr������Ҫ�洢��charlsit��*/  
				v++;  
				//printf("%s\n", substr);    
				substr = strtok(NULL,seg);/*�ڵ�һ�ε���ʱ��strtok()����������str�ַ���������ĵ����򽫲���str���ó�NULL��ÿ�ε��óɹ��򷵻ر��ָ��Ƭ�ε�ָ�롣*/  
			};


			//�����ݿ�ָ�����в�����
			if (mysql->getConnectionStatus()&&charlist[0][0]!=0)
			{
				//cout << ">>>>>>mysql���ӳɹ�" << endl;
				char sqls[2048]={0};
				char str[50]={0};


				char nowDt[50]={0};
				//��ȡ��ǰʱ��
				time_t rawtime;
				time(&rawtime);
				strftime(nowDt,20, "%Y-%m-%d %X", localtime(&rawtime));

				//cout << nowDt << endl;

				sprintf(str,"%s%s%s",charlist[1] ,charlist[0],charlist[20]);


#pragma region ƴ�����ݿ����ctp_market_data�ַ���
				sprintf(sqls,"INSERT IGNORE INTO ctp_market_data VALUES \
							 ('%s','%s','%s','%s','%s',\
							 '%s','%s','%s','%s','%s','%s','%s',\
							 '%s',\
							 '%s','%s','%s','%s','%s','%s','%s','%s',\
							 '%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s')",
							 str,
							 charlist[0],
							 charlist[1],
							 charlist[2],
							 charlist[3],
							 charlist[4],
							 charlist[5],
							 charlist[6],
							 charlist[7],
							 charlist[8],
							 charlist[9],
							 charlist[10],
							 charlist[11],
							 charlist[12],
							 charlist[13],
							 charlist[14],
							 charlist[15],
							 charlist[16],
							 charlist[17],
							 charlist[18],
							 charlist[19],
							 charlist[20],
							 charlist[21],
							 charlist[22],
							 charlist[23],
							 charlist[24],
							 charlist[25],
							 charlist[26],
							 charlist[27],
							 charlist[28],
							 charlist[29],
							 charlist[30],
							 charlist[31],
							 charlist[32],
							 charlist[33],
							 charlist[34],
							 charlist[35],
							 charlist[36],
							 charlist[37],
							 charlist[38],
							 charlist[39],
							 charlist[40],
							 charlist[41],
							 charlist[42],
							 charlist[43],
							 nowDt);
#pragma endregion


				std::string sql_in(sqls);
				//cout << sqls << endl;
				//std::cout<<" ��Ʒ��Ϣ ���ݿ�sql_in="<<sql_in<<std::endl;
				cout << charlist[1] << "����ctp_market_dataʱ�䣺" << nowDt << endl;
				int ret = mysql->inserts(sql_in);


#pragma region ƴ�����ݿ�����ַ���

				char sql2[2048]={0};
				sprintf(sql2,"REPLACE INTO ctp_future_market VALUES \
							 ('%s',left('%s',2),'%s','%s','%s','%s',\
							 '%s','%s','%s','%s','%s','%s','%s',\
							 '%s',\
							 '%s','%s','%s','%s','%s','%s','%s','%s',\
							 '%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s','%s',\
							 '%s','%s','%s')",
							 str,
							 charlist[1],
							 charlist[0],
							 charlist[1],
							 charlist[2],
							 charlist[3],
							 charlist[4],
							 charlist[5],
							 charlist[6],
							 charlist[7],
							 charlist[8],
							 charlist[9],
							 charlist[10],
							 charlist[11],
							 charlist[12],
							 charlist[13],
							 charlist[14],
							 charlist[15],
							 charlist[16],
							 charlist[17],
							 charlist[18],
							 charlist[19],
							 charlist[20],
							 charlist[21],
							 charlist[22],
							 charlist[23],
							 charlist[24],
							 charlist[25],
							 charlist[26],
							 charlist[27],
							 charlist[28],
							 charlist[29],
							 charlist[30],
							 charlist[31],
							 charlist[32],
							 charlist[33],
							 charlist[34],
							 charlist[35],
							 charlist[36],
							 charlist[37],
							 charlist[38],
							 charlist[39],
							 charlist[40],
							 charlist[41],
							 charlist[42],
							 charlist[43],
							 nowDt);
#pragma endregion
				std::string sql_in2(sql2);
				//cout << sqls << endl;
				//std::cout<<" ��Ʒ��Ϣ ���ݿ�sql_in="<<sql_in<<std::endl;
				cout << charlist[1] << "����ctp_future_marketʱ�䣺" << nowDt << endl;
				int num = mysql->inserts(sql_in2);

			}
		}
		
		/*int td = g_pMdUserApi->UnSubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!td)
			std::cout << ">>>>>>�����˶���������ɹ�" << std::endl;
		else
			std::cerr << "--->>>�����˶���������ʧ��" << std::endl;*/

		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!rt)
			std::cout << ">>>>>>���Ͷ�����������ɹ�" << std::endl;
		else
			std::cerr << "--->>>���Ͷ�����������ʧ��" << std::endl;
		
		//Sleep(30000);
		
	};
#pragma endregion
}



//������
void main(void)
{

	SetContratNum("cu",0);
	SetContratNum("al",12);


#pragma region ��ȡ�����ļ�
	//���ݿ����
	mini.open("./config.ini");
	string addrs(mini.readString("MySQL/addrs"));
	string usernames(mini.readString("MySQL/usernames"));
	string passwords(mini.readString("MySQL/passwords"));
	string dbnames(mini.readString("MySQL/dbnames"));
	int ports = mini.readInt("MySQL/ports");
	//cout << addrs << usernames << passwords << dbnames << ports << endl;

	//ģ������ǰ�õ�ַ
	string faddr(mini.readString("CTP/gMdFrontAddr"));
	strcpy(gMdFrontAddr,faddr.c_str());
	//cout << gMdFrontAddr <<endl;

	//ģ�⾭���̴���
	string gbId(mini.readString("CTP/gBrokerID"));
	char* gbIds = new char[11];
	strcpy(gbIds,gbId.c_str());
	strncpy(gBrokerID,gbIds,strlen(gbIds));
	//cout << gBrokerID <<endl;

	//Ͷ�����˻���
	string giId(mini.readString("CTP/gInvesterID"));
	char* giIds = new char[13];
	strcpy(giIds,giId.c_str());
	strncpy(gInvesterID,giIds,strlen(giIds));
	//cout << gInvesterID <<endl;

	//Ͷ�����˻�����
	string gip(mini.readString("CTP/gInvesterPassword"));
	char* gips = new char[41];
	strcpy(gips,gip.c_str());
	strncpy(gInvesterPassword,gips,strlen(gips));
	//cout << gInvesterPassword <<endl;
#pragma endregion

	//���������ļ�
	setBufferFiles();

	//�������ݿ�
	mysql = new MySQLManager(addrs, usernames, passwords, dbnames, (unsigned int)ports);
	mysql->initConnection();

	if(mysql->getConnectionStatus()){
		std::cout << "====================================================================" << std::endl;
		std::cout << "�������ƣ�CTP����ӿ����ݳ�ȡ���������ɹ�~��" << std::endl;
		std::cout << "���򿪷�����Ǭ��" << std::endl;
		int iniflag;
		std::cout << "====================================================================" << std::endl;
		std::cout<<"[1]�Ƿ���������?";
		std::cin>>iniflag;
		if(iniflag!=1){
			return;
		}
	}
	else{
		std::cout << "MySQL����ʧ��" << std::endl;
		return;
	}


	//����һ���߳�
	HANDLE hThread;
	unsigned threadID;
	hThread = (HANDLE)_beginthreadex( NULL, 0, &SetTableMesByBuffer, NULL, 0, &threadID );


	// ��ʼ��UserApi
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();   // ��������ʵ��
	MaiKeMarketSpi *pMdUserSpi = new MaiKeMarketSpi;       // ��������ص�ʵ��

	g_pMdUserApi->RegisterSpi(pMdUserSpi);               // ע���¼���
	g_pMdUserApi->RegisterFront(gMdFrontAddr);           // ��������ǰ�õ�ַ
	g_pMdUserApi->Init();
	g_pMdUserApi->Join();


	//  SetTableMesByBuffer();
	//	g_pMdUserApi->Release();
}