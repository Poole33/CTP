
#pragma once
#define MYINI_OK  0    //操作成功
#define MYINI_NO -1    //操作不成功
#define MYINI_MAX 1024 //最大读写数

#include <string>
#include <map>

using namespace std;
class iniClass{
public:
	int error; //错误值
	char * errinfo; //错误信息
	char * iniPath; //ini文件路径
	map<string, string> iniData; //ini数据表
	iniClass();
	~iniClass();

	/* 打开INI文件
	** 参数1: (char *)文件路径
	***********************************************
	** 返回: 成功 0 MYINI_OK; 失败 -1 MYINI_NO
	** 失败: error 设置为 -1 MYINI_NO
	*/
	int open(char * path);

	/* 读取项目的字符串数据
	** 参数1: (char *)要读取键名 (如: Section/Keys)
	***********************************************
	** 返回: 返回相应键的数据
	** 失败: error 设置为 -1 MYINI_NO
	*/
	const char * readString(char * key);


	/* 读取项目整类型数据
	** 参数1: (char *)要读取键名 (如: Section/Keys)
	***********************************************
	** 返回: 返回相应键的数据;
	** 失败: error 设置为 -1 MYINI_NO
	*/
	int readInt(char * key);

	/* 读取项目的注释
	** 参数1: (char *)要读取键名 (如: Section/Keys)
	***********************************************
	** 返回: 返回相应键的注释
	** 失败: error 设置为 -1 MYINI_NO
	*/
	const char * readNote(char * key);

	/* 写入项目的数据
	** 参数1: (char *)要写入键名 (如: Section/Keys)
	** 参数1: (const char *)要写入的数据
	***********************************************
	** 返回: 成功 0 MYINI_OK; 失败 -1 MYINI_NO
	** 失败: error 设置为 -1 MYINI_NO
	*/
	int writeData(char * key, const char * data);

	/* 写入项目的注释
	** 参数1: (char *)要写入键名 (如: Section/Keys)
	** 参数1: (const char *)要写入的注释内容
	***********************************************
	** 返回: 成功 0 MYINI_OK; 失败 -1 MYINI_NO
	** 失败: error 设置为 -1 MYINI_NO
	*/
	int writeNote(char * key, const char * data);

	/* 保存INI文件
	** 无参数
	***********************************************
	** 返回: 成功 0 MYINI_OK; 失败 -1 MYINI_NO
	** 失败: error 设置为 -1 MYINI_NO
	*/
	int save(void);
};