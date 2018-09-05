/*    
 * MySQLManager.cpp    
 */   
#include "stdafx.h"
#include "MySQLManager.h"    

MySQLManager::MySQLManager(string hosts, string userName, string password, string dbName, unsigned int port){
	IsConnected = false;    
    this ->setHosts(hosts);            //    ��������IP��ַ    
    this ->setUserName(userName);      //    ���õ�¼�û���    
    this ->setPassword(password);      //    ���õ�¼����    
    this ->setDBName(dbName);          //    �������ݿ���    
    this ->setPort(port);              //    ���ö˿ں�    
}    

MySQLManager::~MySQLManager(){   
	this ->destroyConnection();    
}    

void MySQLManager::setDBName(string dbName){  
	if ( dbName.empty() ){
		//�û�û��ָ�����ݿ���    
        std::cout << "DBName is null! Used default value: mysql" << std::endl;    
        this ->DBNAME = new char[5];    
        strcpy(this ->DBNAME, "mysql");    
     }else{    
        this ->DBNAME = new char[dbName.length()];    
        strcpy(this ->DBNAME, dbName.c_str());    
     }    
}    

void MySQLManager::setHosts(string hosts){ 
	if ( hosts.empty() ){
		//�û�û��ָ�����ݿ�IP��ַ    
        std::cout << "Hosts is null! Used default value: localhost" << std::endl;    
        this ->HOSTS = new char[9];    
        strcpy(this ->HOSTS, "localhost");    
    }else{  
		 this ->HOSTS = new char[hosts.length()];    
         strcpy(this ->HOSTS, hosts.c_str());    
    }    
}    

void MySQLManager::setPassword(string password){
	//�û�û��ָ������    
    if ( password.empty() ){ 
		std::cout << "Password is null! Used default value: " << std::endl;    
        this ->PASSWORD = new char[1];    
        strcpy(this ->PASSWORD, "");    
	}else{    
        this ->PASSWORD = new char[password.length()];    
        strcpy(this ->PASSWORD, password.c_str());
	}    
}    

void MySQLManager::setPort(unsigned int port){
	//�û�û��ָ���˿ںţ�ʹ��Ĭ�϶˿ں�    
    if ( port <= 0 ){
		std::cout << "Port number is null! Used default value: 0" << std::endl;    
        this->DEFAULTPORT = 0;
	}else{              
		this ->DEFAULTPORT = port;
	}    
}    

void MySQLManager::setUserName(string userName){  
	//�û�û��ָ����¼�û���     
	if ( userName.empty() ){            
		std::cout << "UserName is null! Used default value: root" << std::endl;             
		this ->USERNAME = new char[4];              
		strcpy(this ->USERNAME, "root");         
	}else{                   
		this ->USERNAME = new char[userName.length()];                   
		strcpy(this ->USERNAME, userName.c_str());    
	}    
}    

void MySQLManager::initConnection(){     
	if ( IsConnected ){		   
		//�Ѿ����ӵ�������    
		std::cout << "Is connected to server!" <<std::endl;             
		return;       
	}    
	mysql_init(&mySQLClient);//    ��ʼ����ض���        
	if ( !mysql_real_connect( &mySQLClient, HOSTS, USERNAME, PASSWORD, DBNAME, DEFAULTPORT, NULL, 0) ){		    
		//���ӵ�������           
		cout << "---------"<<endl;           
		std::cout << "Error connection to database: \n" << mysql_error(&mySQLClient) << std::endl;          
	}      
	IsConnected = true;//�޸����ӱ�ʶ    
}    

bool MySQLManager::runSQLCommand(string sql){    
	if ( !IsConnected ){	    
		//û�����ӵ�������          
		std::cout << "û�����ӵ����ݿ�~!" << std::endl;         
		return false;        
	}         
	if ( sql.empty() ){			    
		//SQL���Ϊ��                 
		std::cout << "SQL���Ϊ��~��" << std::endl;             
		return false;       
	}          
	MYSQL_RES *res;        
	MYSQL_ROW row;       
	unsigned int i,j = 0;        
	i = mysql_real_query(&mySQLClient,sql.c_str(),(unsigned int)strlen(sql.c_str()));//    ִ�в�ѯ       
	if ( i < 0 ){             
		std::cout << "Error query from database: \n" << mysql_error(&mySQLClient) << std::endl;              
		return false;     
	}        
	res = mysql_store_result(&mySQLClient);   
	vector<string> objectValue;
	while((row = mysql_fetch_row(res))){    
		//���������
		objectValue.clear();            
		for ( j = 0 ; j < mysql_num_fields(res) ; j++ ){                      
			objectValue.push_back(row[j]);                 
		}                   
		this ->resultList.push_back(objectValue);           
	}           
	mysql_free_result(res);         //free result after you get the result          
	return true;    
}

unsigned int MySQLManager::inserts(std::string sqlin){
    if(!IsConnected) {
        cout << "" << endl;
        return -1;
    }
    if(sqlin.empty()){
        cout << "sqlin���Ϊ��~" << endl;
        return -1;
    }
    int rows = -1;
	mysql_query(&mySQLClient, "set names gbk"); 
	int res = mysql_query(&mySQLClient, sqlin.c_str());
    if(res >= 0){
        // ������Ӱ�������
        rows = mysql_affected_rows(&mySQLClient);
        cout << "�Ѳ��� "<< rows << " ��\n";
        return rows;
    } else {
        cout << "Insert error " << mysql_errno(&mySQLClient) << "," << mysql_error(&mySQLClient) << endl;
        return -1;
    }
}

unsigned int MySQLManager::deletes(std::string sql){
	if(!IsConnected) {
        cout << "" << endl;
        return -1;
    }
	if(sql.empty()){
		cout<<"[delete] cmd error"<<endl;
		return -1;
	}
	int ret = mysql_query(&mySQLClient,sql.c_str());
	if(ret !=0){
		cout << "Database Delete Info: not exist." <<endl;
		return -1;
	}
	return 0;
}

unsigned int MySQLManager::updates(std::string sql){
	if(!IsConnected) {
        cout << "" << endl;
        return -1;
    }
	if(sql.empty()){
		cout<<"[update] cmd error"<<endl;
		return -1;
	}
	mysql_query(&mySQLClient, "set names gbk");
	int ret = mysql_query(&mySQLClient,sql.c_str());
	if(ret !=0){
		cout << "Database Update Info: not exist, I am insert." <<endl<<endl;
		return -1;
	}
	return 0;
}

unsigned int MySQLManager::querys(std::string sql){
	if(!IsConnected) {
        cout << "" << endl;
        return -1;
    }
	if(sql.empty()){
		cout<<"[query] cmd error"<<endl;
		return -1;
	}
	mysql_real_query(&mySQLClient,sql.c_str(),(unsigned int)strlen(sql.c_str()));
	MYSQL_RES *result = mysql_store_result(&mySQLClient);
	MYSQL_ROW row; 
	unsigned int resultsize,fieldsize;
	if(result != NULL){
		resultsize = mysql_num_rows(result);
		cout<<"[query] mysql_num_rows=="<<resultsize<<endl;
		fieldsize = mysql_num_fields(result);
		cout<<"[query] mysql_num_fields=="<<fieldsize<<endl;
	}else{
		cout<<"[query] mysql_store_result error !"<<endl;
		return -1;
	}
	MYSQL_FIELD *fields = mysql_fetch_fields(result);
	for(unsigned int i = 0; i < fieldsize; i++){
		this->fieldList.push_back(fields[i].name);
		//printf("Field %u is %s\n", i, fields[i].name);
	}
	/*
	while((row = mysql_fetch_row(result))){
		printf("%s,  %s,  %s,  %s\n",row[0],row[1],row[2],row[3]);
	}
	*/
	vector<string> objectValue;
	while((row = mysql_fetch_row(result))){    
		objectValue.clear();            
		for (int j = 0; j < mysql_num_fields(result); j++){                      
			objectValue.push_back(row[j]);                 
		}                   
		this ->resultList.push_back(objectValue);           
	}
    mysql_free_result(result);
	return 0;
}

unsigned int MySQLManager::getrows(std::string sql){
	if(!IsConnected) {
        cout << "" << endl;
        return -1;
    }
	if(sql.empty()){
		//cout<<"[query] cmd error"<<endl;
		return -1;
	}
	mysql_real_query(&mySQLClient,sql.c_str(),(unsigned int)strlen(sql.c_str()));
	unsigned int resultsize,fieldsize;
	//fieldsize = mysql_field_count(&mySQLClient);   //���Բ�ִ��mysql_store_result()��ȡ�����������
	//resultsize = mysql_affected_rows(&mySQLClient);//��ִ��update\insert\delete ʱ������Ӱ�������
	MYSQL_RES *result = mysql_store_result(&mySQLClient);
	MYSQL_ROW row; 
	
	if(result != NULL){
		resultsize = mysql_num_rows(result);
		//cout<<"[query] mysql_num_rows=="<<resultsize<<endl;
	}else{
		resultsize = 0;
	}
    mysql_free_result(result);
	return resultsize;
}

vector< vector<string> > MySQLManager::getResult(){    
    return resultList;    
}  

vector<string> MySQLManager::getFields(){
	return fieldList;
}

void MySQLManager::destroyConnection(){    
    mysql_close(&mySQLClient);    
    this ->IsConnected = false;    
}    

bool MySQLManager::getConnectionStatus(){    
    return IsConnected;    
} 