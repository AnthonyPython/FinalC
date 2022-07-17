#ifndef ECON_NETWORKING_MESSAGES_H
#define ECON_NETWORKING_MESSAGES_H
#ifdef _WIN32
#pragma once
#endif


#include "tier1/mempool.h"
#include "tier1/smartptr.h"

// Currently only using protobuf
enum EProtocolType
{
	k_EProtocolStruct		= 1,
	k_EProtocolProtobuf		= 2
};

struct MsgHdr_t
{
	MsgType_t m_eMsgType;	// Message type
	uint32 m_unMsgSize;		// Size of message without header
	uint8 m_ubProtoVer;
	EProtocolType m_eProtoType;
	uint64 m_ulSourceID;
	uint64 m_ulTargetID;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CNetPacket : public IRefCounted
{
	DECLARE_FIXEDSIZE_ALLOCATOR_MT( CNetPacket );
public:
	CNetPacket()
	{
		m_pMsg = NULL;
		m_Hdr.m_eMsgType = k_EInvalidMsg;
	}

	void *Data( void ) const { return (byte *)m_pMsg; }
	byte *MutableData( void ) { return (byte *)m_pMsg + sizeof(MsgHdr_t); }
	uint32 Size( void ) const { return m_Hdr.m_unMsgSize + sizeof(MsgHdr_t); }
	MsgHdr_t const &Hdr( void ) const { return m_Hdr; }

protected:
	virtual ~CNetPacket()
	{
		Assert( m_cRefCount == 0 );
		Assert( m_pMsg == NULL );
	}

	friend class CEconNetworking;
	void Init( uint32 size, MsgType_t eMsg );
	void InitFromMemory( void const *pMemory, uint32 size );

private:
	void *m_pMsg;
	MsgHdr_t m_Hdr;

	friend class CRefCountAccessor;
	virtual int AddRef( void )
	{
		return ThreadInterlockedIncrement( &m_cRefCount );
	}
	virtual int Release( void )
	{
		Assert( m_cRefCount > 0 );
		int nRefCounts = ThreadInterlockedDecrement( &m_cRefCount );
		if ( nRefCounts == 0 )
		{
			if ( m_pMsg )
				free( m_pMsg );

			delete this;
		}

		return nRefCounts;
	}
	volatile uint m_cRefCount;
};

//-----------------------------------------------------------------------------
// Purpose: Interface for processing network packets
//-----------------------------------------------------------------------------
abstract_class IMessageHandler
{
public:
	virtual ~IMessageHandler() {}
	virtual bool ProcessMessage( CNetPacket *pPacket ) = 0;
};


void RegisterEconNetworkMessageHandler( MsgType_t eMsg, IMessageHandler *pHandler );
#define REG_ECON_MSG_HANDLER( classType, msgType, msgName ) \
		static classType s_##msgName##Handler; \
		static class CReg##classType \
		{ \
		public: \
			CReg##classType() { RegisterEconNetworkMessageHandler( msgType, &s_##msgName##Handler ); } \
		} g_Reg##classType

bool QueueEconNetworkMessageWork( IMessageHandler *pHandler, CSmartPtr<CNetPacket> const &pPacket );


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
	CProtobufMsg( CNetPacket *pPacket )
		: m_pPacket( pPacket )
	{
		m_pBody = AllocMsg();
		Assert( m_pBody );

		CRefCountAccessor::AddRef( m_pPacket );
		m_pBody->ParseFromArray( m_pPacket->MutableData(), m_pPacket->Size() - sizeof(MsgHdr_t) );
	}
	virtual ~CProtobufMsg()
	{
		if ( m_pBody )
		{
			FreeMsg( m_pBody );
		}

		if ( m_pPacket )
		{
			CRefCountAccessor::Release( m_pPacket );
		}
	}

	TProtoMsg &Body( void ) { return *m_pBody; }
	TProtoMsg const &Body( void ) const { return *m_pBody; }
	TProtoMsg *operator->() { return m_pBody; }

protected:
	TProtoMsg *AllocMsg( void )
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

	void FreeMsg( TProtoMsg *pObj )
	{
		Destruct<TProtoMsg>( pObj );
		sm_MsgPool->Free( (void *)pObj );
	}

private:
	CNetPacket *m_pPacket;
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