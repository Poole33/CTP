
#include "stdafx.h"
#include "iniClass.h"


iniClass::iniClass(){}
iniClass::~iniClass(){}


//打开ini文件
int iniClass::open(char * path){
	FILE *fp;
	this->errinfo = "";
	this->iniPath = path;
	if ((fp = fopen(path, "r")) == NULL){
		//文件打开失败
		this->error   = MYINI_NO;
		this->errinfo = "INI文件打开失败";
		return MYINI_NO;
	}
	char buf[MYINI_MAX] = "\0";
	char ch = 0;
	int nPos = 0;
	string secName  = ""; //小节名
	string keyName  = ""; //项目健名
	string noteData = ""; //项目健名
	int iSec  = 0; //用于判断小节
	int iNote = 0; //用于判断注释
	while ((ch = fgetc(fp)) != EOF){
		if (ch == '[' || ch == ']'){ //判断小节
			++iSec;
		} else if (ch == '>'){ //判断注释
			++iNote;
		} else if (ch == '='){ //取出项目的健
			buf[nPos] = '\0';
			nPos = 0;
			keyName = buf;
		} else if (ch != '\r' && ch != '\n'){
			buf[nPos++] = ch;
		} else {
			if (iSec == 2){  //取出小节名
				buf[nPos] = '\0';
				secName = buf;
				iSec = 0;
				noteData = "";
			} else if (iNote == 1){  //取出注释
				buf[nPos] = '\0';
				if (noteData.length() > 1){
					noteData = noteData + "\n#" + buf;
				} else {
					noteData = noteData + "#" + buf;
				}
				
				iNote = 0;
			} else {  //取出项目的值
				buf[nPos] = '\0';
				if (strlen(buf) > 0){ //如果是空就不赋值
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
	fclose(fp); //关闭文件连接
	
	this->error = MYINI_OK;
	return MYINI_OK;
}


//读取项目整类型数据
const char * iniClass::readString(char * key){
	if (this->iniData[key].empty() == 0){ //判断是否存在内容
		this->error = MYINI_OK;
		return this->iniData[key].c_str();
	}
	this->errinfo = "项目不存在或值为空";
	this->error = MYINI_NO;
	return "";
}

//读取int整类型数据
int iniClass::readInt(char * key){
	return atoi(this->readString(key));
}

//读取项目的注释
const char * iniClass::readNote(char * key){
	string k = key;
	k = k + "/-NOTE-";
	if (this->iniData[k].empty() == 0){ //判断是否存在内容
		this->error = MYINI_OK;
		return this->iniData[k].c_str();
	}
	this->errinfo = "项目不存在或无注释";
	this->error = MYINI_NO;
	return "";
}

//写入项目数据
int iniClass::writeData(char * key, const char * data){
	int ret = MYINI_OK;
	try{
		this->iniData[key] = data;
	}catch (...){
		this->errinfo = "项目数据写入出错";
		ret = MYINI_NO;
	}
	this->error = ret;
	return ret;
}


//写入项目的注释
int iniClass::writeNote(char * key, const char * data){
	//判断是否存
	if (this->iniData[key].empty()){
		this->errinfo = "项目不存在";
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
		this->errinfo = "项目注释写入出错";
		ret = MYINI_NO;
	}
	this->error = ret;
	return ret;
}


//保存INI
int iniClass::save(void){
	string saveStr = "";
	string sec = "";
	string iSec = "";
	map<string, string>::iterator it;
	for (it = this->iniData.begin(); it != this->iniData.end(); it++){
		//判断是否为注释或值为空时不跳到下一个
		if (it->first.find("/-NOTE-") != it->first.npos || it->second.empty()){
			continue;
		}
		iSec = it->first.substr(0, it->first.find("/")); //取出小节
		//判断小节是否已改变
		if (sec.compare(iSec) != 0){
			sec = iSec;
			//如果小节名改变了就把一个新的小节名追加到字符串中
			saveStr.append("[");
			saveStr.append(iSec);
			saveStr.append("]\n");
		}
		//判断是否存在注释
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
		//写入INI文件
		FILE *fp;
		if ((fp = fopen(this->iniPath, "w")) == NULL){
			//文件打开失败
			this->error = MYINI_NO;
			this->errinfo = "INI文件打开失败";
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