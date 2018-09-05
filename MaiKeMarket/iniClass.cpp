
#include "stdafx.h"
#include "iniClass.h"


iniClass::iniClass(){}
iniClass::~iniClass(){}


//��ini�ļ�
int iniClass::open(char * path){
	FILE *fp;
	this->errinfo = "";
	this->iniPath = path;
	if ((fp = fopen(path, "r")) == NULL){
		//�ļ���ʧ��
		this->error   = MYINI_NO;
		this->errinfo = "INI�ļ���ʧ��";
		return MYINI_NO;
	}
	char buf[MYINI_MAX] = "\0";
	char ch = 0;
	int nPos = 0;
	string secName  = ""; //С����
	string keyName  = ""; //��Ŀ����
	string noteData = ""; //��Ŀ����
	int iSec  = 0; //�����ж�С��
	int iNote = 0; //�����ж�ע��
	while ((ch = fgetc(fp)) != EOF){
		if (ch == '[' || ch == ']'){ //�ж�С��
			++iSec;
		} else if (ch == '>'){ //�ж�ע��
			++iNote;
		} else if (ch == '='){ //ȡ����Ŀ�Ľ�
			buf[nPos] = '\0';
			nPos = 0;
			keyName = buf;
		} else if (ch != '\r' && ch != '\n'){
			buf[nPos++] = ch;
		} else {
			if (iSec == 2){  //ȡ��С����
				buf[nPos] = '\0';
				secName = buf;
				iSec = 0;
				noteData = "";
			} else if (iNote == 1){  //ȡ��ע��
				buf[nPos] = '\0';
				if (noteData.length() > 1){
					noteData = noteData + "\n#" + buf;
				} else {
					noteData = noteData + "#" + buf;
				}
				
				iNote = 0;
			} else {  //ȡ����Ŀ��ֵ
				buf[nPos] = '\0';
				if (strlen(buf) > 0){ //����ǿվͲ���ֵ
					if (noteData.length() > 1){
						this->iniData[secName + "/" + keyName + "/-NOTE-"] = noteData;
					}
					this->iniData[secName + "/" + keyName] = buf;
					noteData = "";
				}
			}
			nPos = 0;
		}
	}
	fclose(fp); //�ر��ļ�����
	
	this->error = MYINI_OK;
	return MYINI_OK;
}


//��ȡ��Ŀ����������
const char * iniClass::readString(char * key){
	if (this->iniData[key].empty() == 0){ //�ж��Ƿ��������
		this->error = MYINI_OK;
		return this->iniData[key].c_str();
	}
	this->errinfo = "��Ŀ�����ڻ�ֵΪ��";
	this->error = MYINI_NO;
	return "";
}

//��ȡint����������
int iniClass::readInt(char * key){
	return atoi(this->readString(key));
}

//��ȡ��Ŀ��ע��
const char * iniClass::readNote(char * key){
	string k = key;
	k = k + "/-NOTE-";
	if (this->iniData[k].empty() == 0){ //�ж��Ƿ��������
		this->error = MYINI_OK;
		return this->iniData[k].c_str();
	}
	this->errinfo = "��Ŀ�����ڻ���ע��";
	this->error = MYINI_NO;
	return "";
}

//д����Ŀ����
int iniClass::writeData(char * key, const char * data){
	int ret = MYINI_OK;
	try{
		this->iniData[key] = data;
	}catch (...){
		this->errinfo = "��Ŀ����д�����";
		ret = MYINI_NO;
	}
	this->error = ret;
	return ret;
}


//д����Ŀ��ע��
int iniClass::writeNote(char * key, const char * data){
	//�ж��Ƿ��
	if (this->iniData[key].empty()){
		this->errinfo = "��Ŀ������";
		this->error = MYINI_NO;
		return MYINI_NO;
	}
	string k = key;
	k = k + "/-NOTE-";
	int ret = MYINI_OK;
	try{
		this->iniData[k] = data;
	}
	catch (...){
		this->errinfo = "��Ŀע��д�����";
		ret = MYINI_NO;
	}
	this->error = ret;
	return ret;
}


//����INI
int iniClass::save(void){
	string saveStr = "";
	string sec = "";
	string iSec = "";
	map<string, string>::iterator it;
	for (it = this->iniData.begin(); it != this->iniData.end(); it++){
		//�ж��Ƿ�Ϊע�ͻ�ֵΪ��ʱ��������һ��
		if (it->first.find("/-NOTE-") != it->first.npos || it->second.empty()){
			continue;
		}
		iSec = it->first.substr(0, it->first.find("/")); //ȡ��С��
		//�ж�С���Ƿ��Ѹı�
		if (sec.compare(iSec) != 0){
			sec = iSec;
			//���С�����ı��˾Ͱ�һ���µ�С����׷�ӵ��ַ�����
			saveStr.append("[");
			saveStr.append(iSec);
			saveStr.append("]\n");
		}
		//�ж��Ƿ����ע��
		if (this->iniData[it->first + "/-NOTE-"].length() > 1){
			saveStr.append(this->iniData[it->first + "/-NOTE-"]);
			saveStr.append("\n");
		}
		saveStr.append(it->first.substr(it->first.find("/") + 1));
		saveStr.append("=");
		saveStr.append(it->second);
		saveStr.append("\n");
	}
	if (saveStr.length() > 5){
		//д��INI�ļ�
		FILE *fp;
		if ((fp = fopen(this->iniPath, "w")) == NULL){
			//�ļ���ʧ��
			this->error = MYINI_NO;
			this->errinfo = "INI�ļ���ʧ��";
			return MYINI_NO;
		}
		fprintf(fp, "%s", saveStr.c_str());
		fclose(fp);
		this->error = MYINI_OK;
		return MYINI_OK;
	}
	this->error = MYINI_NO;
	return MYINI_NO;
}