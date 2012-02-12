//********************************************
//	Ini 相关函数
//********************************************
#ifdef _WIN32
//#include <stdafx.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>
#include "ini.h"
#endif

////////////////////////////////////////////////
// 通用接口
////////////////////////////////////////////////


//初始化
CIni::CIni()
{
	m_lDataLen = 0;
	m_strData = NULL;
	IndexNum = 0;
	IndexList = NULL;
}

//初始化
CIni::CIni(char *filename)
{
	m_lDataLen=0;
	m_strData=NULL;
	IndexNum=0;
	IndexList=NULL;
	Open(filename);
}

//析构释放
CIni::~CIni()
{
	if( m_lDataLen != 0 )
	{
		SAFE_DELETE( m_strData );
		m_lDataLen = 0;
	}

	if( IndexNum != 0 )
	{
		SAFE_DELETE( IndexList );
		IndexNum = 0;
	}
}

//读入文件
bool CIni::Open(char *filename)
{
	strcpy(m_strFileName, filename);

	SAFE_FREE( m_strData );

	//获取文件长度
	int fh;
	fh = _open( filename, _O_RDONLY );
	if( fh== -1 )
	{
		m_lDataLen = -1;
	}
	m_lDataLen = _filelength(fh);
	_close(fh);
	
	//文件存在
	if( m_lDataLen > 0 )
	{
		m_strData = new char[m_lDataLen];

		FILE *fp;
		fp=fopen(filename, "rb");
		assert( fp!=NULL );
		fread(m_strData, m_lDataLen, 1, fp);		//读数据
		fclose(fp);

		//初始化索引
		InitIndex();
		return true;
	}
	else	// 文件不存在
	{
		// 找不到文件
		m_lDataLen=1;
		m_strData = new char[m_lDataLen];
		memset(m_strData, 0, 1);
		InitIndex();
	}

	return false;
}

//关闭文件
void CIni::Close()
{
	if( m_lDataLen != 0 )
	{
		SAFE_DELETE( m_strData );
		m_lDataLen = 0;
	}

	if( IndexNum != 0 )
	{
		SAFE_DELETE( IndexList );
		IndexNum = 0;
	}
}

//写入文件
bool CIni::Save(char *filename)
{
	if( filename==NULL )
	{
		filename=m_strFileName;
	}

	FILE *fp;
	fp=fopen(filename, "wb");
	assert( fp!=NULL );

	fwrite(m_strData, m_lDataLen, 1, fp);
	fclose(fp);

	return true;
}

//返回文件内容
char *CIni::GetData()
{
	return m_strData;
}

//获得文件的行数
int CIni::GetLines(int cur)
{
	int n=1;
	for(int i=0; i<cur; i++)
	{
		if( m_strData[i]=='\n' )
			n++;
	}
	return n;
}

////////////////////////////////////////////////
// 内部函数
////////////////////////////////////////////////

//计算出所有的索引位置
void CIni::InitIndex()
{
	IndexNum=0;

	for(int i=0; i<m_lDataLen; i++)
	{
		//找到
		if( m_strData[i]=='[' && ( m_strData[i-1]=='\n' || i==0 ) )
		{
			IndexNum++;
		}
	}

	//申请内存
	SAFE_DELETE( IndexList );
	if( IndexNum>0 )
		IndexList=new int[IndexNum];

	int n=0;

	for(i=0; i<m_lDataLen; i++)
	{
		if( m_strData[i]=='[' && ( m_strData[i-1]=='\n' || i==0 ) )
		{
			IndexList[n]=i+1;
			n++;
		}
	}
}

//返回指定标题位置
int CIni::FindIndex(char *string)
{
	for(int i=0; i<IndexNum; i++)
	{
		char *str=ReadText( IndexList[i] );
		if( strcmp(string, str) == 0 )
		{
			SAFE_FREE( str );
			return IndexList[i];
		}
		SAFE_FREE( str );
	}
	return -1;
}

//返回指定数据的位置
int CIni::FindData(int index, char *string)
{
	int p=index;	//指针

	while(1)
	{
		p=GotoNextLine(p);
		char *name=ReadDataName(p);
		if( strcmp(string, name)==0 )
		{
			SAFE_FREE( name );
			return p;
		}

		SAFE_FREE( name );
		if( p>=m_lDataLen ) return -1;
	}
	return -1;
}

//提行
int CIni::GotoNextLine(int p)
{
	for(int i=p; i<m_lDataLen; i++)
	{
		if( m_strData[i]=='\n' )
			return i+1;
	}
	return i;
}

//在指定位置读一数据名称
char *CIni::ReadDataName(int &p)
{
	char chr;
	char *Ret;
	int m=0;

	Ret=new char[64];
	memset(Ret, 0, 64);

	for(int i=p; i<m_lDataLen; i++)
	{
		chr = m_strData[i];

		//结束
		if( chr == '\r' )
		{
			p=i+1;
			return Ret;
		}
		
		//结束
		if( chr == '=' || chr == ';' )
		{
			p=i+1;
			return Ret;
		}
		
		Ret[m]=chr;
		m++;
	}
	return Ret;
}

//在指定位置读一字符串
char *CIni::ReadText(int p)
{
	char chr;
	char *Ret;
	int n=p, m=0;

	int LineNum = GotoNextLine(p) - p + 1;
	Ret=new char[LineNum];
	memset(Ret, 0, LineNum);

	for(int i=0; i<m_lDataLen-p; i++)
	{
		chr = m_strData[n];

		//结束
		if( chr == ';' || chr == '\r' || chr == '\t' || chr == ']' )
		{
			//ShowMessage(Ret);
			return Ret;
		}
		
		Ret[m]=chr;
		m++;
		n++;
	}

	return Ret;
}

//加入一个索引
bool CIni::AddIndex(char *index)
{
	char str[256];
	memset(str, 0, 256);
	int n=FindIndex(index);

	if( n == -1 )	//新建索引
	{
		sprintf(str,"\r\n[%s]\r\n",index);
		m_strData = (char *)realloc(m_strData, m_lDataLen+strlen(str));	//重新分配内存
		sprintf(&m_strData[m_lDataLen], "%s", str);
		m_lDataLen+=strlen(str);

		InitIndex();
		return true;
	}
	
	return false;	//已经存在
}

//在当前位置加入一个数据
bool CIni::AddData(int p, char *name, char *string)
{
	char *str;
	int len=strlen(string);
	str=new char[len+256];
	memset(str, 0, len+256);
	sprintf(str,"%s=%s\r\n",name,string);
	len=strlen(str);

	p=GotoNextLine(p);	//提行
	m_strData = (char *)realloc(m_strData, m_lDataLen+len);	//重新分配内存

	char *temp=new char[m_lDataLen-p];
	memcpy(temp, &m_strData[p], m_lDataLen-p);
	memcpy(&m_strData[p+len], temp, m_lDataLen-p);	//把后面的搬到末尾
	memcpy(&m_strData[p], str, len);
	m_lDataLen+=len;

	SAFE_DELETE( temp );
	SAFE_DELETE( str );
	return true;
}

//在当前位置修改一个数据的值
bool CIni::ModityData(int p, char *name, char *string)
{
	int n=FindData(p, name);

	char *t=ReadText(n);
	p=n+strlen(t);
	if( strlen(t)>0 ) free(t);

	int newlen=strlen(string);
	int oldlen=p-n;

	m_strData = (char *)realloc(m_strData, m_lDataLen+newlen-oldlen);	//重新分配内存

	char *temp=new char[m_lDataLen-p];
	memcpy(temp, &m_strData[p], m_lDataLen-p);
	memcpy(&m_strData[n+newlen], temp, m_lDataLen-p);			//把后面的搬到末尾
	memcpy(&m_strData[n], string, newlen);
	m_lDataLen+=newlen-oldlen;

	SAFE_DELETE( temp );
	return true;
}

//把指针移动到本INDEX的最后一行
int CIni::GotoLastLine(char *index)
{
	int n=FindIndex(index);
	n=GotoNextLine(n);
	while(1)
	{
		if( m_strData[n] == '\r' || m_strData[n] == EOF || m_strData[n] == -3 || m_strData[n] == ' ' || m_strData[n] == '/' || m_strData[n] == '\t' || m_strData[n] == '\n' )
		{
			return n;
		}
		else
		{
			n=GotoNextLine(n);
			if( n >= m_lDataLen ) return n;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// 对外接口
/////////////////////////////////////////////////////////////////////

//以普通方式读一字符串数据
char *CIni::ReadText(char *index, char *name)
{
	int n=FindIndex(index);
	assert( n != -1 );

	int m=FindData(n, name);
	assert( m != -1 );

	return ReadText(m);
}
	
//在指定的行读一字符串
char *CIni::ReadText(char *index, int lines)
{
	int n=FindIndex(index);
	assert( n != -1 );

	//跳到指定行数
	n=GotoNextLine(n);
	for(int i=0; i<lines; i++)
	{
		if( n<m_lDataLen )
			n=GotoNextLine(n);
	}

	//读数据
	while( n<=m_lDataLen )
	{
		if( m_strData[n] == '=' )
		{
			n++;
			return ReadText(n);
		}
		if( m_strData[n] == '\r' )
		{
			return "";
		}
		n++;
	}

	return "";
}

//以普通方式读一整数
int CIni::ReadInt(char *index, char *name)
{
	int n=FindIndex(index);
	assert( n != -1 );

	int m=FindData(n, name);
	assert( m != -1 );

	char *str=ReadText(m);
	int ret=atoi(str);
	free(str);
	return ret;
}

//在指定的行读一整数
int CIni::ReadInt(char *index, int lines)
{
	int n=FindIndex(index);
	assert( n != -1 );

	//跳到指定行数
	n=GotoNextLine(n);
	for(int i=0; i<lines; i++)
	{
		if( n<m_lDataLen )
			n=GotoNextLine(n);
	}

	//读数据
	while( n<m_lDataLen )
	{
		if( m_strData[n] == '=' )
		{
			n++;
			char *str=ReadText(n);
			int ret=atoi(str);
			free(str);
			return ret;
		}
		if( m_strData[n] == '\r' )
		{
			return ERROR_DATA;
		}
		n++;
	}

	return ERROR_DATA;
}

//在指定的行读一数据名称
char *CIni::ReadCaption(char *index, int lines)
{
	int n=FindIndex(index);
	assert( n != -1 );

	//跳到指定行数
	n=GotoNextLine(n);
	for(int i=0; i<lines; i++)
	{
		if( n<m_lDataLen )
			n=GotoNextLine(n);
	}

	return ReadDataName(n);
}

//以普通方式写一字符串数据
bool CIni::Write(char *index, char *name, char *string)
{
	int n=FindIndex(index);
	if( n == -1 )	//新建索引
	{
		AddIndex(index);
		n=FindIndex(index);
		n=GotoLastLine(index);
		AddData(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在索引
	int m=FindData(n, name);
	if( m==-1 )		//新建数据
	{
		n=GotoLastLine(index);
		AddData(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在数据
	ModityData(n, name, string);	//修改一个数据

	return true;
}

//以普通方式写一整数
bool CIni::Write(char *index, char *name, int num)
{
	char string[32];
	sprintf(string, "%d", num);

	int n=FindIndex(index);
	if( n == -1 )	//新建索引
	{
		AddIndex(index);
		n=FindIndex(index);
		n=GotoLastLine(index);
		AddData(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在索引
	int m=FindData(n, name);
	if( m==-1 )		//新建数据
	{
		n=GotoLastLine(index);
		AddData(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在数据
	ModityData(n, name, string);	//修改一个数据

	return true;
}

//返回连续的行数
int CIni::GetContinueDataNum(char *index)
{
	int num=0;
	int n=FindIndex(index);
	n=GotoNextLine(n);
	while(1)
	{
		if( m_strData[n] == '\r' || m_strData[n] == EOF || m_strData[n] == -3 || m_strData[n] == ' ' || m_strData[n] == '/' || m_strData[n] == '\t' || m_strData[n] == '\n' )
		{
			return num;
		}
		else
		{
			num++;
			n=GotoNextLine(n);
			if( n >= m_lDataLen ) return num;
		}
	}
}
