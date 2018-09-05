#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "MaiKeMarketSpi.h"
#include "MySQLManager.h"

using namespace std;

// ---- ȫ�ֲ������� ---- //
extern CThostFtdcMdApi *g_pMdUserApi;            // ����ָ��
extern char gMdFrontAddr[];                      // ģ������ǰ�õ�ַ
extern TThostFtdcBrokerIDType gBrokerID;         // ģ�⾭���̴���
extern TThostFtdcInvestorIDType gInvesterID;     // Ͷ�����˻���
extern TThostFtdcPasswordType gInvesterPassword; // Ͷ��������
extern char *g_pInstrumentID[];                  // �����Լ�����б��С��ϡ���֣��������ѡһ��
extern const int instrumentNum=24;                        // �����Լ��������
//extern int requestID;

//MySQLManager *mysql;       //���ݿ�


//��ȡ�ļ�����
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

// ---- ctp_api�ص����� ---- //
// ���ӳɹ�Ӧ��
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
		cout << ">>>>>>mysql���ӳɹ�" << endl;

	}*/

	// ��ʼ��¼
	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy(loginReq.BrokerID, gBrokerID);
	strcpy(loginReq.UserID, gInvesterID);
	strcpy(loginReq.Password, gInvesterPassword);
	static int requestID = 0; // ������
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		std::cout << ">>>>>>���͵�¼����ɹ�" << std::endl;
	else
		std::cerr << "--->>>���͵�¼����ʧ��" << std::endl;
}

// �Ͽ�����֪ͨ
void MaiKeMarketSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "=====�������ӶϿ�=====" << std::endl;
	std::cerr << "�����룺 " << nReason << std::endl;
}

// ������ʱ����
void MaiKeMarketSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "=====����������ʱ=====" << std::endl;
	std::cerr << "���ϴ�����ʱ�䣺 " << nTimeLapse << std::endl;
}

// ��¼Ӧ��
void MaiKeMarketSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====�˻���¼�ɹ�=====" << std::endl;
		std::cout << "�����գ� " << pRspUserLogin->TradingDay << std::endl;
		std::cout << "��¼ʱ�䣺 " << pRspUserLogin->LoginTime << std::endl;
		std::cout << "�����̣� " << pRspUserLogin->BrokerID << std::endl;
		std::cout << "�ʻ����� " << pRspUserLogin->UserID << std::endl;
		// ��ʼ��������
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);
		if (!rt)
			std::cout << ">>>>>>���Ͷ�����������ɹ�" << std::endl;
		else
			std::cerr << "--->>>���Ͷ�����������ʧ��" << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// �ǳ�Ӧ��
void MaiKeMarketSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====�˻��ǳ��ɹ�=====" << std::endl;
		std::cout << "�����̣� " << pUserLogout->BrokerID << std::endl;
		std::cout << "�ʻ����� " << pUserLogout->UserID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ����֪ͨ
void MaiKeMarketSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (bResult)
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ��������Ӧ��
void MaiKeMarketSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====��������ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
		// �����Ҫ�����ļ��������ݿ⣬�����ﴴ����ͷ,��ͬ�ĺ�Լ�����洢
		//char filePath[100] = {'\0'};
		//sprintf(filePath, "%s_market_data.csv", pSpecificInstrument->InstrumentID);
		//std::ofstream outFile;
		//outFile.open(filePath, std::ios::out); // �¿��ļ�
		///*outFile << "��Լ����" << ","
		//	<< "����ʱ��" << ","
		//	<< "���¼�" << ","
		//	<< "�ɽ���" << ","
		//	<< "���һ" << ","
		//	<< "����һ" << ","
		//	<< "����һ" << ","
		//	<< "����һ" << ","
		//	<< "�ֲ���" << ","
		//	<< "������"
		//	<< std::endl;*/
		////outFile << "" << std::endl;
		//outFile.close();

		//���ļ�
		/*std::ifstream infile;
		infile.open(filePath,std::ios::in);*/

		//��ȡ����
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
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ȡ����������Ӧ��
void MaiKeMarketSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, 
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====ȡ����������ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ����ѯ��Ӧ��
void MaiKeMarketSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====����ѯ�۳ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ȡ������ѯ��Ӧ��
void MaiKeMarketSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		std::cout << "=====ȡ������ѯ�۳ɹ�=====" << std::endl;
		std::cout << "��Լ���룺 " << pSpecificInstrument->InstrumentID << std::endl;
	}
	else
		std::cerr << "���ش���--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

// ��������֪ͨ
void MaiKeMarketSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	// ��ӡ���飬�ֶν϶࣬��ȡ����
	/*std::cout << "=====����������=====" << std::endl;
	std::cout << "�����գ� " << pDepthMarketData->TradingDay << std::endl;
	std::cout << "���������룺 " << pDepthMarketData->ExchangeID << std::endl;
	std::cout << "��Լ���룺 " << pDepthMarketData->InstrumentID << std::endl;
	std::cout << "��Լ�ڽ������Ĵ��룺 " << pDepthMarketData->ExchangeInstID << std::endl;
	std::cout << "���¼ۣ� " << pDepthMarketData->LastPrice << std::endl;
	std::cout << "������ " << pDepthMarketData->Volume << std::endl;
	std::cout << "ʱ�䣺 " << pDepthMarketData->UpdateTime << std::endl;
	std::cout << "����ۣ� " << pDepthMarketData->AskPrice1 << std::endl;*/
	//std::cout << "���������룺 " << pDepthMarketData->ExchangeID << std::endl;
	//std::cout << "��Լ�ڽ������Ĵ��룺 " << pDepthMarketData->ExchangeInstID << std::endl;
	//std::cout << "��Լ�ڽ������Ĵ��룺 " << pDepthMarketData->AskVolume5 << std::endl;
	
	/*if (pDepthMarketData->ExchangeInstID[0]==0)
	{
		cout << "bingo"<<endl;
	}*/

	//�������������
	// ���ֻ��ȡĳһ����Լ���飬������tick�ش����ļ������ݿ�
	char filePath[100] = {'\0'};
	sprintf(filePath, "%s_market_data.csv", pDepthMarketData->InstrumentID);
	std::ofstream outFile;
	outFile.open(filePath, std::ios::trunc); // �ļ�����д�� 
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
	cout << ">>>>>>mysql���ӳɹ�" << endl;
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
	std::cout<<" ��Ʒ��Ϣ ���ݿ�sql_in="<<sql_in<<std::endl;
	int ret = mysql->inserts(sql_in);
	}*/
}

// ѯ������֪ͨ
void MaiKeMarketSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	// ����ѯ�۽��
	std::cout << "=====���ѯ�۽��=====" << std::endl;
	std::cout << "�����գ� " << pForQuoteRsp->TradingDay << std::endl;
	std::cout << "���������룺 " << pForQuoteRsp->ExchangeID << std::endl;
	std::cout << "��Լ���룺 " << pForQuoteRsp->InstrumentID << std::endl;
	std::cout << "ѯ�۱�ţ� " << pForQuoteRsp->ForQuoteSysID << std::endl;
}



