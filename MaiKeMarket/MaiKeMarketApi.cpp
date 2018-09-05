// MaiKeMarketApi.cpp : 定义控制台应用程序的入口点。
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

// 链接库
#pragma comment (lib, "thostmduserapi.lib")
#pragma comment (lib, "thosttraderapi.lib")

// ---- 全局变量 ---- //
// 公共参数
TThostFtdcBrokerIDType gBrokerID = "";                   // 模拟经纪商代码
TThostFtdcInvestorIDType gInvesterID = "";             // 投资者账户名
TThostFtdcPasswordType gInvesterPassword = "";      // 投资者密码

// 行情参数
CThostFtdcMdApi *g_pMdUserApi = nullptr;                     // 行情指针
char* gMdFrontAddr = new char[81];                           // 模拟行情前置地址
char *g_pInstrumentID[] = {"cu1801","cu1805","al1801"};      // 行情合约代码列表，中、上、大、郑交易所各选一种
const int instrumentNum = 24;                                       // 行情合约订阅数量

int iRequestID = 0;// 请求编号

MySQLManager *mysql;       //数据库
iniClass mini;



//插入合约代码
void SetContratNum(string name,int place){
#pragma region 插入合约代码
	//获取当前时间
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	//截取年份后两位
	stringstream yearstream;
	yearstream << sys.wYear;
	string yearStr = yearstream.str().substr(2, 3);
	//std::cout << yearStr << endl;
	stringstream custream;
	int j=1;
	//拼接当前所需合约代码
	for (int i = 1+place; i < 13+place; i++)	//为了对应数组对应的位置
	{
		//拼接合约代码头
		string cu;
		char *aa = new char();
		cu = name + yearStr;


		custream << sys.wMonth + j;

		//如果当前月份为12月则年份加1
		if (sys.wMonth + j > 12)
		{
			yearstream.str("");
			yearstream << (sys.wYear + 1);
			yearStr = yearstream.str().substr(2, 3);
			cu = name + yearStr;
			//std::cout << yearstream.str() << endl;
		}

		//拼接合约代码
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
#pragma region 创建存储数据的本地缓存
	// 如果需要存入文件或者数据库，在这里创建表头,不同的合约单独存储
	for (int i = 0; i < 24; i++){
		char filePath[100] = {'\0'};
		sprintf(filePath, "%s_market_data.csv", g_pInstrumentID[i]);
		std::ofstream outFile;
		outFile.open(filePath, std::ios::out); // 新建文件
		outFile.close();
	}
#pragma endregion
}



//读缓存数据，存入数据库
unsigned __stdcall SetTableMesByBuffer(void* pArguments){
#pragma region 读缓存数据，存入数据库
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

			char seg[] = ","; /*分隔符这里为逗号comma，分隔符可以为你指定的，如分号，空格等*/  
			char charlist[44][44]={""};/*指定分隔后子字符串存储的位置，这里定义二维字符串数组*/  
			int v =0;  
			char *substr= strtok(str, seg);/*利用现成的分割函数,substr为分割出来的子字符串*/  
			while (substr != NULL) {    
				strcpy(charlist[v],substr);/*把新分割出来的子字符串substr拷贝到要存储的charlsit中*/  
				v++;  
				//printf("%s\n", substr);    
				substr = strtok(NULL,seg);/*在第一次调用时，strtok()必需给予参数str字符串，往后的调用则将参数str设置成NULL。每次调用成功则返回被分割出片段的指针。*/  
			};


			//往数据库指定表中插数据
			if (mysql->getConnectionStatus()&&charlist[0][0]!=0)
			{
				//cout << ">>>>>>mysql连接成功" << endl;
				char sqls[2048]={0};
				char str[50]={0};


				char nowDt[50]={0};
				//获取当前时间
				time_t rawtime;
				time(&rawtime);
				strftime(nowDt,20, "%Y-%m-%d %X", localtime(&rawtime));

				//cout << nowDt << endl;

				sprintf(str,"%s%s%s",charlist[1] ,charlist[0],charlist[20]);


#pragma region 拼接数据库插入ctp_market_data字符串
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
				//std::cout<<" 产品信息 数据库sql_in="<<sql_in<<std::endl;
				cout << charlist[1] << "插入ctp_market_data时间：" << nowDt << endl;
				int ret = mysql->inserts(sql_in);


#pragma region 拼接数据库插入字符串

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
				//std::cout<<" 产品信息 数据库sql_in="<<sql_in<<std::endl;
				cout << charlist[1] << "插入ctp_future_market时间：" << nowDt << endl;
				int num = mysql->inserts(sql_in2);

			}
		}
		
		/*int td = g_pMdUserApi->UnSubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!td)
			std::cout << ">>>>>>发送退订行情请求成功" << std::endl;
		else
			std::cerr << "--->>>发送退订行情请求失败" << std::endl;*/

		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!rt)
			std::cout << ">>>>>>发送订阅行情请求成功" << std::endl;
		else
			std::cerr << "--->>>发送订阅行情请求失败" << std::endl;
		
		//Sleep(30000);
		
	};
#pragma endregion
}



//主方法
void main(void)
{

	SetContratNum("cu",0);
	SetContratNum("al",12);


#pragma region 读取配置文件
	//数据库参数
	mini.open("./config.ini");
	string addrs(mini.readString("MySQL/addrs"));
	string usernames(mini.readString("MySQL/usernames"));
	string passwords(mini.readString("MySQL/passwords"));
	string dbnames(mini.readString("MySQL/dbnames"));
	int ports = mini.readInt("MySQL/ports");
	//cout << addrs << usernames << passwords << dbnames << ports << endl;

	//模拟行情前置地址
	string faddr(mini.readString("CTP/gMdFrontAddr"));
	strcpy(gMdFrontAddr,faddr.c_str());
	//cout << gMdFrontAddr <<endl;

	//模拟经纪商代码
	string gbId(mini.readString("CTP/gBrokerID"));
	char* gbIds = new char[11];
	strcpy(gbIds,gbId.c_str());
	strncpy(gBrokerID,gbIds,strlen(gbIds));
	//cout << gBrokerID <<endl;

	//投资者账户名
	string giId(mini.readString("CTP/gInvesterID"));
	char* giIds = new char[13];
	strcpy(giIds,giId.c_str());
	strncpy(gInvesterID,giIds,strlen(giIds));
	//cout << gInvesterID <<endl;

	//投资者账户密码
	string gip(mini.readString("CTP/gInvesterPassword"));
	char* gips = new char[41];
	strcpy(gips,gip.c_str());
	strncpy(gInvesterPassword,gips,strlen(gips));
	//cout << gInvesterPassword <<endl;
#pragma endregion

	//创建缓存文件
	setBufferFiles();

	//连接数据库
	mysql = new MySQLManager(addrs, usernames, passwords, dbnames, (unsigned int)ports);
	mysql->initConnection();

	if(mysql->getConnectionStatus()){
		std::cout << "====================================================================" << std::endl;
		std::cout << "程序名称：CTP行情接口数据抽取程序，启动成功~！" << std::endl;
		std::cout << "程序开发：普乾坤" << std::endl;
		int iniflag;
		std::cout << "====================================================================" << std::endl;
		std::cout<<"[1]是否启动程序?";
		std::cin>>iniflag;
		if(iniflag!=1){
			return;
		}
	}
	else{
		std::cout << "MySQL连接失败" << std::endl;
		return;
	}


	//创建一个线程
	HANDLE hThread;
	unsigned threadID;
	hThread = (HANDLE)_beginthreadex( NULL, 0, &SetTableMesByBuffer, NULL, 0, &threadID );


	// 初始化UserApi
	g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();   // 创建行情实例
	MaiKeMarketSpi *pMdUserSpi = new MaiKeMarketSpi;       // 创建行情回调实例

	g_pMdUserApi->RegisterSpi(pMdUserSpi);               // 注册事件类
	g_pMdUserApi->RegisterFront(gMdFrontAddr);           // 设置行情前置地址
	g_pMdUserApi->Init();
	g_pMdUserApi->Join();


	//  SetTableMesByBuffer();
	//	g_pMdUserApi->Release();
}