#ifndef __MY_THREAD_H__
#define __MY_THREAD_H__

#include "FastDelegate.h"

#include <process.h>
#include <windows.h>


//ʹ�����ֿռ�
namespace mylib{

///////////////////////////////////////////////////////////////////////////////
//�ٽ����ļ��׷�װ
///////////////////////////////////////////////////////////////////////////////
class CCriticalSection{ 
public: 
	CCriticalSection() { 
		::InitializeCriticalSection(&m_cs); 
	} 

	~CCriticalSection() { 
		::DeleteCriticalSection(&m_cs); 
	} 

	virtual void Lock() {
		::EnterCriticalSection(&m_cs); 
	} 

	virtual void Unlock() { 
		::LeaveCriticalSection(&m_cs);
	} 

private:
	CRITICAL_SECTION m_cs; 
}; 


///////////////////////////////////////////////////////////////////////////////
//�߳�����׷�װ�����Ƽ��̳б���
///////////////////////////////////////////////////////////////////////////////
class CAutoGuard{ 
public: 
	CAutoGuard(CCriticalSection* pObject) : m_pObject(pObject) {
		if( m_pObject )
			m_pObject->Lock(); 
	}

	~CAutoGuard() {
		if( m_pObject )
			m_pObject->Unlock();
	}
	
private:

	CAutoGuard(const CAutoGuard&);
	CAutoGuard& operator =(const CAutoGuard&);

private: 

	CCriticalSection* m_pObject; 
};


///////////////////////////////////////////////////////////////////////////////
//�߳�����׷�װ�����Ƽ��̳б���
///////////////////////////////////////////////////////////////////////////////
class CThread{
public:

	typedef fastdelegate::FastDelegate<void()> _ThreadBegin;
	typedef fastdelegate::FastDelegate<void()> _ThreadEnd;
	typedef fastdelegate::FastDelegate<bool()> _ThreadExecute;

public:

	//���캯��
	CThread(){
		m_hEventSuspend = CreateEvent(0,TRUE,FALSE,NULL);//�����¼����,��Ϊ�Ǽ���״̬
		m_bExecute = true;

		//�����߳�
		m_hThread = (HANDLE)_beginthreadex(NULL,0,_ThreadProc,this,0,&m_thrdaddr);
	}

	//��������
	~CThread(){
		if( m_hEventSuspend ){
			CloseHandle(m_hEventSuspend);
			m_hEventSuspend = NULL;
		}

		if( m_hThread ){
			TerminateThread(m_hThread,0);				//ǿ���˳��߳�

			CloseHandle(m_hThread );
			m_hThread  = NULL;
		}
	}


public:

	//����
	BOOL Start(){
		return SetEvent(m_hEventSuspend);
	}

	//����
	BOOL Suspend(){
		return ResetEvent(m_hEventSuspend);
	}

	//ֹͣ
	BOOL Stop(){
		m_bExecute = false;							//���߳�״̬��Ϊ�˳�
		Start();									//���̻߳���,������������ߴ�
		WaitForSingleObject(m_hThread,300);			//�ȴ��߳��˳�

		return TRUE;
	}

public:

	//�̻߳ص��������

	_ThreadBegin	ThreadBegin;
	_ThreadEnd		ThreadEnd;

	_ThreadExecute	ThreadExecute;						//�߳�ִ�д���	

private:

	HANDLE			m_hThread;							//�߳̾��
	HANDLE			m_hEventSuspend;					//���ڱ�ʶ�߳��ǹ���,���ǹ���״̬
	bool			m_bExecute;							//�߳�״̬,FALSE��ʾ�߳��˳�״̬,TRUE��ʾδ���״̬
	unsigned		m_thrdaddr;

private:

	static unsigned __stdcall _ThreadProc( void * pObj){
		CThread* pthread = static_cast<CThread*>(pObj);

		//�߳������¼�
		if( pthread && !pthread->ThreadBegin.empty())
			pthread->ThreadBegin();

		//�����߳�ѭ��
		while( pthread && pthread->m_bExecute ){
			WaitForSingleObject(pthread->m_hEventSuspend,INFINITE);//���ƹ�������

			//����Զ��˳�
			if(pthread->ThreadExecute.empty())
				break;

			//ִ�н��
			if(!pthread->ThreadExecute())
				break;
		}

		//�߳��˳�
		pthread->m_bExecute = false;

		//�߳̽����¼�
		if( pthread && !pthread->ThreadEnd.empty())
			pthread->ThreadEnd();

		_endthreadex(0);
		return 0;
	}
};

}//namespace mylib

#endif//__MY_THREAD_H__