#ifndef ECON_NETWORKING_MESSAGES_H
#define ECON_NETWORKING_MESSAGES_H
#ifdef _WIN32
#pragma once
#endif


#include "tier1/mempool.h"
#include "vstdlib/coroutine.h"

class CNetPacket;
class INetPacket;

//-----------------------------------------------------------------------------
// Purpose: Interface for processing network packets
//-----------------------------------------------------------------------------
abstract_class IMessageHandler
{
public:
	virtual ~IMessageHandler() {}
	virtual bool ProcessMessage( INetPacket *pPacket ) = 0;
	virtual bool BWaitForCompletion( void ) = 0;
};

class CBaseMsgHandler : public IMessageHandler
{
public:
	CBaseMsgHandler();

	virtual bool ProcessMessage( INetPacket *pPacket )	{ return false; }
	bool BWaitForCompletion( void ) OVERRIDE;
	void QueueWork( INetPacket *pPacket );
private:
	INetPacket *m_pWorkPacket;
	HCoroutine m_hCoroutine;
};


void RegisterEconNetworkMessageHandler( MsgType_t eMsg, IMessageHandler *pHandler );
#define REG_ECON_MSG_HANDLER( classType, msgType, msgName ) \
		static classType s_##msgName##Handler; \
		static class CReg##classType \
		{ \
		public: \
			CReg##classType() { RegisterEconNetworkMessageHandler( msgType, &s_##msgName##Handler ); } \
		} g_Reg##classType



template< class TProtoMsg >
class CProtobufMsg
{
	static CUtlMemoryPool *sm_MsgPool;
	static bool sm_bRegisteredPool;
public:
	CProtobufMsg()
		: m_pPacket( NULL )
	{
		m_pBody = AllocMsg();
	}
	CProtobufMsg( INetPacket *pPacket )
		: m_pPacket( NULL )
	{
		InitMsgFromNetwork( this, pPacket );
	}
	virtual ~CProtobufMsg()
	{
		if ( m_pPacket )
		{
			m_pPacket->Release();
		}

		if ( m_pBody )
		{
			FreeMsg( m_pBody );
		}
	}

	static TProtoMsg *AllocMsg( void )
	{
		if ( !sm_bRegisteredPool )
		{
			Assert( sm_MsgPool == NULL );
			sm_MsgPool = new CUtlMemoryPool( sizeof( TProtoMsg ), 1 );
			sm_bRegisteredPool = true;
		}

		TProtoMsg *pMsg = (TProtoMsg *)sm_MsgPool->Alloc();
		Construct<TProtoMsg>( pMsg );
		return pMsg;
	}

	static void FreeMsg( TProtoMsg *pObj )
	{
		Destruct<TProtoMsg>( pObj );
		sm_MsgPool->Free( (void *)pObj );
	}

	static bool InitMsgFromNetwork( CProtobufMsg<TProtoMsg> *pMsg, INetPacket *pPacket )
	{
		pMsg->m_pPacket = pPacket;
		pMsg->NetPacket()->AddRef();

		pMsg->m_pBody = AllocMsg();
		Assert( pMsg->m_pBody );

		return pMsg->Body().ParseFromArray( (void *)pPacket->Data(), (int)pPacket->Size() );
	}

	INetPacket *NetPacket( void ) const { return m_pPacket; }
	TProtoMsg &Body( void ) { return *m_pBody; }
	TProtoMsg const &Body( void ) const { return *m_pBody; }

private:
	INetPacket *m_pPacket;
	TProtoMsg *m_pBody;

	// Copying is illegal
	CProtobufMsg( const CProtobufMsg& );
	CProtobufMsg& operator=( const CProtobufMsg& );
};

template< typename TProtoMsg >
CUtlMemoryPool *CProtobufMsg<TProtoMsg>::sm_MsgPool;
template< typename TProtoMsg >
bool CProtobufMsg<TProtoMsg>::sm_bRegisteredPool;

#endif
