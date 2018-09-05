
#pragma once
#define MYINI_OK  0    //�����ɹ�
#define MYINI_NO -1    //�������ɹ�
#define MYINI_MAX 1024 //����д��

#include <string>
#include <map>

using namespace std;
class iniClass{
public:
	int error; //����ֵ
	char * errinfo; //������Ϣ
	char * iniPath; //ini�ļ�·��
	map<string, string> iniData; //ini���ݱ�
	iniClass();
	~iniClass();

	/* ��INI�ļ�
	** ����1: (char *)�ļ�·��
	***********************************************
	** ����: �ɹ� 0 MYINI_OK; ʧ�� -1 MYINI_NO
	** ʧ��: error ����Ϊ -1 MYINI_NO
	*/
	int open(char * path);

	/* ��ȡ��Ŀ���ַ�������
	** ����1: (char *)Ҫ��ȡ���� (��: Section/Keys)
	***********************************************
	** ����: ������Ӧ��������
	** ʧ��: error ����Ϊ -1 MYINI_NO
	*/
	const char * readString(char * key);


	/* ��ȡ��Ŀ����������
	** ����1: (char *)Ҫ��ȡ���� (��: Section/Keys)
	***********************************************
	** ����: ������Ӧ��������;
	** ʧ��: error ����Ϊ -1 MYINI_NO
	*/
	int readInt(char * key);

	/* ��ȡ��Ŀ��ע��
	** ����1: (char *)Ҫ��ȡ���� (��: Section/Keys)
	***********************************************
	** ����: ������Ӧ����ע��
	** ʧ��: error ����Ϊ -1 MYINI_NO
	*/
	const char * readNote(char * key);

	/* д����Ŀ������
	** ����1: (char *)Ҫд����� (��: Section/Keys)
	** ����1: (const char *)Ҫд�������
	***********************************************
	** ����: �ɹ� 0 MYINI_OK; ʧ�� -1 MYINI_NO
	** ʧ��: error ����Ϊ -1 MYINI_NO
	*/
	int writeData(char * key, const char * data);

	/* д����Ŀ��ע��
	** ����1: (char *)Ҫд����� (��: Section/Keys)
	** ����1: (const char *)Ҫд���ע������
	***********************************************
	** ����: �ɹ� 0 MYINI_OK; ʧ�� -1 MYINI_NO
	** ʧ��: error ����Ϊ -1 MYINI_NO
	*/
	int writeNote(char * key, const char * data);

	/* ����INI�ļ�
	** �޲���
	***********************************************
	** ����: �ɹ� 0 MYINI_OK; ʧ�� -1 MYINI_NO
	** ʧ��: error ����Ϊ -1 MYINI_NO
	*/
	int save(void);
};