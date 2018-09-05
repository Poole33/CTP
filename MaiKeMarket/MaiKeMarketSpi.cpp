#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "MaiKeMarketSpi.h"
#include "MySQLManager.h"

using namespace std;

// ---- 全局参数声明 ---- //
extern CThostFtdcMdApi *g_pMdUserApi;            // 行情指针
extern char gMdFrontAddr[];                      // 模拟行情前置地址
extern TThostFtdcBrokerIDType gBrokerID;         // 模拟经纪商代码
extern TThostFtdcInvestorIDType gInvesterID;     // 投资者账户名
extern TThostFtdcPasswordType gInvesterPassword; // 投资者密码
extern char *g_pInstrumentID[];                  // 行情合约代码列表，中、上、大、郑交易所各选一种
extern const int instrumentNum=24;                        // 行情合约订阅数量
//extern int requestID;

//MySQLManager *mysql;       //数据库


//获取文件列数
int getFileColumns(const char * fileName){  
	ifstream fileStream ;  
	fileStream.open(fileName,std::ios::_Nocreate);  
	double tmp;  
	char c;  
	int count=0;  
	for(int i=0;i<10000;i++){  
		fileStream>>tmp;  
		++count;  
		c=fileStream.peek();  
		if('\n'==c)  
		{  
			break;  
		}  
	}  
	fileStream.close();  
	return count;  
} 

// ---- ctp_api回调函数 ---- //
// 连接成功应答
void MaiKeMarketSpi::OnFrontConnected()
{
	std::cerr << "=====OnFrontConnected=====" << std::endl;

	/*string addrs("192.168.18.18");
	string usernames("delight");
	string passwords("delight");
	string dbnames("market");
	int ports = 3306;

	mysql = new MySQLManager(addrs, usernames, passwords, dbnames, (unsigned int)ports);
	mysql->initConnection();
	if (mysql->getConnectionStatus())
	{
		cout << ">>>>>>mysql连接成功" << endl;

	}*/

	// 开始登录
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvesterID);
	strcpy(loginReq.Password, gInvesterPassword);
	static int requestID = 0; // 请求编号
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		std::cout << ">>>>>>发送登录请求成功" << std::endl;
	else
		std::cerr << "--->>>发送登录请求失败" << std::endl;
}

// 断开连接通知
void MaiKeMarketSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====网络连接断开=====" << std::endl;
	std::cerr << "错误码： " << nReason << std::endl;
}

// 心跳超时警告
void MaiKeMarketSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====网络心跳超时=====" << std::endl;
	std::cerr << "距上次连接时间： " << nTimeLapse << std::endl;
}

// 登录应答
void MaiKeMarketSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====账户登录成功=====" << std::endl;
		std::cout << "交易日： " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "登录时间： " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "经纪商： " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "帐户名： " << pRspUserLogin->UserID << std::endl;
		// 开始订阅行情
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!rt)
			std::cout << ">>>>>>发送订阅行情请求成功" << std::endl;
		else
			std::cerr << "--->>>发送订阅行情请求失败" << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 登出应答
void MaiKeMarketSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====账户登出成功=====" << std::endl;
		std::cout << "经纪商： " << pUserLogout->BrokerID << std::endl;
		std::cout << "帐户名： " << pUserLogout->UserID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 错误通知
void MaiKeMarketSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 订阅行情应答
void MaiKeMarketSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====订阅行情成功=====" << std::endl;
		std::cout << "合约代码： " << pSpecificInstrument->InstrumentID << std::endl;
		// 如果需要存入文件或者数据库，在这里创建表头,不同的合约单独存储
		//char filePath[100] = {'\0'};
		//sprintf(filePath, "%s_market_data.csv", pSpecificInstrument->InstrumentID);
		//std::ofstream outFile;
		//outFile.open(filePath, std::ios::out); // 新开文件
		///*outFile << "合约代码" << ","
		//	<< "更新时间" << ","
		//	<< "最新价" << ","
		//	<< "成交量" << ","
		//	<< "买价一" << ","
		//	<< "买量一" << ","
		//	<< "卖价一" << ","
		//	<< "卖量一" << ","
		//	<< "持仓量" << ","
		//	<< "换手率"
		//	<< std::endl;*/
		////outFile << "" << std::endl;
		//outFile.close();

		//读文件
		/*std::ifstream infile;
		infile.open(filePath,std::ios::in);*/

		//获取行数
		/*int line = 0;
		string tmp;
		while (getline(infile,tmp))
		{
		line++;
		}
		cout << line << endl;*/

		/*char str[1024];
		infile >> str;
		infile.close();*/
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 取消订阅行情应答
void MaiKeMarketSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====取消订阅行情成功=====" << std::endl;
		std::cout << "合约代码： " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 订阅询价应答
void MaiKeMarketSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====订阅询价成功=====" << std::endl;
		std::cout << "合约代码： " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 取消订阅询价应答
void MaiKeMarketSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====取消订阅询价成功=====" << std::endl;
		std::cout << "合约代码： " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// 行情详情通知
void MaiKeMarketSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	// 打印行情，字段较多，截取部分
	/*std::cout << "=====获得深度行情=====" << std::endl;
	std::cout << "交易日： " << pDepthMarketData->TradingDay << std::endl;
	std::cout << "交易所代码： " << pDepthMarketData->ExchangeID << std::endl;
	std::cout << "合约代码： " << pDepthMarketData->InstrumentID << std::endl;
	std::cout << "合约在交易所的代码： " << pDepthMarketData->ExchangeInstID << std::endl;
	std::cout << "最新价： " << pDepthMarketData->LastPrice << std::endl;
	std::cout << "数量： " << pDepthMarketData->Volume << std::endl;
	std::cout << "时间： " << pDepthMarketData->UpdateTime << std::endl;
	std::cout << "申买价： " << pDepthMarketData->AskPrice1 << std::endl;*/
	//std::cout << "交易所代码： " << pDepthMarketData->ExchangeID << std::endl;
	//std::cout << "合约在交易所的代码： " << pDepthMarketData->ExchangeInstID << std::endl;
	//std::cout << "合约在交易所的代码： " << pDepthMarketData->AskVolume5 << std::endl;
	
	/*if (pDepthMarketData->ExchangeInstID[0]==0)
	{
		cout << "bingo"<<endl;
	}*/

	//往表里面存数据
	// 如果只获取某一个合约行情，可以逐tick地存入文件或数据库
	char filePath[100] = {'\0'};
	sprintf(filePath, "%s_market_data.csv", pDepthMarketData->InstrumentID);
	std::ofstream outFile;
	outFile.open(filePath, std::ios::trunc); // 文件覆盖写入 
	outFile << pDepthMarketData->TradingDay << "," 
	    <<pDepthMarketData->InstrumentID << "," 
		<< (pDepthMarketData->ExchangeID[0]==0?"0":pDepthMarketData->ExchangeID) << "," 
		<< (pDepthMarketData->ExchangeInstID[0]==0?"0":pDepthMarketData->ExchangeInstID) << "," 
		<< pDepthMarketData->LastPrice << "," 
		<< pDepthMarketData->PreSettlementPrice << "," 
		<< pDepthMarketData->PreClosePrice << "," 
		<< pDepthMarketData->PreOpenInterest << "," 
		<< pDepthMarketData->OpenPrice << "," 
		<< pDepthMarketData->HighestPrice << "," 
		<< pDepthMarketData->LowestPrice << "," 
		<< pDepthMarketData->Volume << "," 
		<< pDepthMarketData->Turnover << "," 
		<< pDepthMarketData->OpenInterest << "," 
		<< pDepthMarketData->ClosePrice << "," 
		<< pDepthMarketData->SettlementPrice << "," 
		<< pDepthMarketData->UpperLimitPrice << "," 
		<< pDepthMarketData->LowerLimitPrice << "," 
		<< pDepthMarketData->PreDelta << "," 
		<< pDepthMarketData->CurrDelta << "," 
		<< pDepthMarketData->UpdateTime << "," 
		<< pDepthMarketData->UpdateMillisec << "," 
		<< pDepthMarketData->BidPrice1 << "," 
		<< pDepthMarketData->BidVolume1 << "," 
		<< pDepthMarketData->AskPrice1 << "," 
		<< pDepthMarketData->AskVolume1 << "," 
		<< pDepthMarketData->BidPrice2 << "," 
		<< pDepthMarketData->BidVolume2 << "," 
		<< pDepthMarketData->AskPrice2 << "," 
		<< pDepthMarketData->AskVolume2 << "," 
		<< pDepthMarketData->BidPrice3 << "," 
		<< pDepthMarketData->BidVolume3 << "," 
		<< pDepthMarketData->AskPrice3 << "," 
		<< pDepthMarketData->AskVolume3 << "," 
		<< pDepthMarketData->BidPrice4 << "," 
		<< pDepthMarketData->BidVolume4 << "," 
		<< pDepthMarketData->AskPrice4 << "," 
		<< pDepthMarketData->AskVolume4 << "," 
		<< pDepthMarketData->BidPrice5 << "," 
		<< pDepthMarketData->BidVolume5 << "," 
		<< pDepthMarketData->AskPrice5 << "," 
		<< pDepthMarketData->AskVolume5 << "," 
		<< pDepthMarketData->AveragePrice << "," 
		<< pDepthMarketData->ActionDay << std::endl;
	outFile.close();

	/*if (mysql->getConnectionStatus())
	{
	cout << ">>>>>>mysql连接成功" << endl;
	char sqls[2048]={0};
	sprintf(sqls,"INSERT INTO test VALUES ('%s','%s','%f','%d','%f','%d','%f','%d','%f','%f')",
	pDepthMarketData->InstrumentID,
	pDepthMarketData->UpdateTime,
	pDepthMarketData->LastPrice,
	pDepthMarketData->Volume,
	pDepthMarketData->BidPrice1,
	pDepthMarketData->BidVolume1,
	pDepthMarketData->AskPrice1,
	pDepthMarketData->AskVolume1,
	pDepthMarketData->OpenInterest,
	pDepthMarketData->Turnover);

	std::string sql_in(sqls);
	cout << sqls << endl;
	std::cout<<" 产品信息 数据库sql_in="<<sql_in<<std::endl;
	int ret = mysql->inserts(sql_in);
	}*/
}

// 询价详情通知
void MaiKeMarketSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	// 部分询价结果
	std::cout << "=====获得询价结果=====" << std::endl;
	std::cout << "交易日： " << pForQuoteRsp->TradingDay << std::endl;
	std::cout << "交易所代码： " << pForQuoteRsp->ExchangeID << std::endl;
	std::cout << "合约代码： " << pForQuoteRsp->InstrumentID << std::endl;
	std::cout << "询价编号： " << pForQuoteRsp->ForQuoteSysID << std::endl;
}



