/******************************************************************************\
 * Copyright (c) 2004-2011
 *
 * Author(s):
 *  Volker Fischer
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later 
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#if !defined ( PROTOCOL_H__3B123453_4344_BB2392354455IUHF1912__INCLUDED_ )
#define PROTOCOL_H__3B123453_4344_BB2392354455IUHF1912__INCLUDED_

#include <qglobal.h>
#include <qmutex.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <list>
#include "global.h"
#include "util.h"


/* Definitions ****************************************************************/
// protocol message IDs
#define PROTMESSID_ILLEGAL                     0 // illegal ID
#define PROTMESSID_ACKN                        1 // acknowledge
#define PROTMESSID_JITT_BUF_SIZE              10 // jitter buffer size
#define PROTMESSID_REQ_JITT_BUF_SIZE          11 // request jitter buffer size
#define PROTMESSID_NET_BLSI_FACTOR            12 // OLD (not used anymore)
#define PROTMESSID_CHANNEL_GAIN               13 // set channel gain for mix
#define PROTMESSID_CONN_CLIENTS_LIST          14 // connected client list
#define PROTMESSID_SERVER_FULL                15 // OLD (not used anymore)
#define PROTMESSID_REQ_CONN_CLIENTS_LIST      16 // request connected client list
#define PROTMESSID_CHANNEL_NAME               17 // set channel name for fader tag
#define PROTMESSID_CHAT_TEXT                  18 // contains a chat text
#define PROTMESSID_PING_MS                    19 // for measuring ping time
#define PROTMESSID_NETW_TRANSPORT_PROPS       20 // properties for network transport
#define PROTMESSID_REQ_NETW_TRANSPORT_PROPS   21 // request properties for network transport
#define PROTMESSID_DISCONNECTION              22 // disconnection
#define PROTMESSID_REQ_CHANNEL_NAME           23 // request channel name for fader tag

// message IDs of connection less messages (CLM)
// DEFINITION -> start at 1000, end at 1999, see IsConnectionLessMessageID
#define PROTMESSID_CLM_PING_MS                1001 // for measuring ping time
#define PROTMESSID_CLM_SERVER_FULL            1002 // server full message
#define PROTMESSID_CLM_REGISTER_SERVER        1003 // register server
#define PROTMESSID_CLM_UNREGISTER_SERVER      1004 // unregister server -> TODO
#define PROTMESSID_CLM_SERVER_LIST            1005 // server list
#define PROTMESSID_CLM_REQ_SERVER_LIST        1006 // request server list
#define PROTMESSID_CLM_SEND_EMPTY_MESSAGE     1007 // an empty message shall be send
#define PROTMESSID_CLM_EMPTY_MESSAGE          1008 // empty message


// lengths of message as defined in protocol.cpp file
#define MESS_HEADER_LENGTH_BYTE         7 // TAG (2), ID (2), cnt (1), length (2)
#define MESS_LEN_WITHOUT_DATA_BYTE      ( MESS_HEADER_LENGTH_BYTE + 2 /* CRC (2) */ )

// time out for message re-send if no acknowledgement was received
#define SEND_MESS_TIMEOUT_MS            400 // ms


/* Classes ********************************************************************/
class CProtocol : public QObject
{
    Q_OBJECT

public:
    CProtocol();

    void Reset();

    void CreateJitBufMes ( const int iJitBufSize );
    void CreateReqJitBufMes();
    void CreateChanGainMes ( const int iChanID, const double dGain );
    void CreateConClientListMes ( const CVector<CChannelShortInfo>& vecChanInfo );
    void CreateReqConnClientsList();
    void CreateChanNameMes ( const QString strName );
    void CreateReqChanNameMes();
    void CreateChatTextMes ( const QString strChatText );
    void CreatePingMes ( const int iMs );
    void CreateNetwTranspPropsMes ( const CNetworkTransportProps& NetTrProps );
    void CreateReqNetwTranspPropsMes();

    void CreateCLPingMes ( const CHostAddress& InetAddr, const int iMs );
    void CreateCLServerFullMes ( const CHostAddress& InetAddr );
    void CreateCLRegisterServerMes ( const CHostAddress&    InetAddr,
                                     const CServerCoreInfo& ServerInfo );
    void CreateCLServerListMes ( const CHostAddress&        InetAddr,
                                 const CVector<CServerInfo> vecServerInfo );
    void CreateCLReqServerListMes ( const CHostAddress& InetAddr );
    void CreateCLSendEmptyMesMes ( const CHostAddress& InetAddr,
                                   const CHostAddress& TargetInetAddr );
    void CreateCLEmptyMes ( const CHostAddress& InetAddr );

    void CreateAndImmSendDisconnectionMes();
    void CreateAndImmSendAcknMess ( const int& iID,
                                    const int& iCnt );

    bool ParseMessage ( const CVector<uint8_t>& vecbyData,
                        const int               iNumBytes );

    bool ParseConnectionLessMessage ( const CVector<uint8_t>& vecbyData,
                                      const int               iNumBytes,
                                      const CHostAddress&     InetAddr );

    bool IsProtocolMessage ( const CVector<uint8_t>& vecbyData,
                             const int               iNumBytes );

protected:
    class CSendMessage
    {
    public:
        CSendMessage() : vecMessage ( 0 ), iID ( PROTMESSID_ILLEGAL ),
            iCnt ( 0 ) {}
        CSendMessage ( const CVector<uint8_t>& nMess, const int iNCnt,
            const int iNID ) : vecMessage ( nMess ), iID ( iNID ),
            iCnt ( iNCnt ) {}

        CSendMessage& operator= ( const CSendMessage& NewSendMess )
        {
            vecMessage.Init ( NewSendMess.vecMessage.Size() );
            vecMessage = NewSendMess.vecMessage;

            iID  = NewSendMess.iID;
            iCnt = NewSendMess.iCnt;
            return *this; 
        }

        CVector<uint8_t> vecMessage;
        int              iID, iCnt;
    };

    void EnqueueMessage ( CVector<uint8_t>& vecMessage,
                          const int         iCnt,
                          const int         iID );

    bool ParseMessageFrame ( const CVector<uint8_t>& vecIn,
                             const int               iNumBytesIn,
                             int&                    iCnt,
                             int&                    iID,
                             CVector<uint8_t>&       vecData );

    void GenMessageFrame ( CVector<uint8_t>&       vecOut,
                           const int               iCnt,
                           const int               iID,
                           const CVector<uint8_t>& vecData );

    void PutValOnStream ( CVector<uint8_t>&  vecIn,
                          int&               iPos,
                          const uint32_t     iVal,
                          const int          iNumOfBytes );

    void PutStringOnStream ( CVector<uint8_t>&  vecIn,
                             int&               iPos,
                             const QString&     sString );

    uint32_t GetValFromStream ( const CVector<uint8_t>& vecIn,
                                int&                    iPos,
                                const int               iNumOfBytes );

    bool GetStringFromStream ( const CVector<uint8_t>& vecIn,
                               int&                    iPos,
                               const int               iMaxStringLen,
                               QString&                strOut );

    bool IsConnectionLessMessageID ( const int iID ) const
        { return (iID >= 1000) & (iID < 2000); }

    void SendMessage();

    void CreateAndSendMessage ( const int               iID,
                                const CVector<uint8_t>& vecData );

    void CreateAndImmSendConLessMessage ( const int               iID,
                                          const CVector<uint8_t>& vecData,
                                          const CHostAddress&     InetAddr );

    bool EvaluateJitBufMes             ( const CVector<uint8_t>& vecData );
    bool EvaluateReqJitBufMes();
    bool EvaluateChanGainMes           ( const CVector<uint8_t>& vecData );
    bool EvaluateConClientListMes      ( const CVector<uint8_t>& vecData );
    bool EvaluateReqConnClientsList();
    bool EvaluateChanNameMes           ( const CVector<uint8_t>& vecData );
    bool EvaluateReqChanNameMes();
    bool EvaluateChatTextMes           ( const CVector<uint8_t>& vecData );
    bool EvaluatePingMes               ( const CVector<uint8_t>& vecData );
    bool EvaluateNetwTranspPropsMes    ( const CVector<uint8_t>& vecData );
    bool EvaluateReqNetwTranspPropsMes();
    bool EvaluateDisconnectionMes();

    bool EvaluateCLPingMes             ( const CHostAddress&     InetAddr,
                                         const CVector<uint8_t>& vecData );
    bool EvaluateCLServerFullMes();
    bool EvaluateCLRegisterServerMes   ( const CHostAddress&     InetAddr,
                                         const CVector<uint8_t>& vecData );
    bool EvaluateCLServerListMes       ( const CHostAddress&     InetAddr,
                                         const CVector<uint8_t>& vecData );
    bool EvaluateCLReqServerListMes    ( const CHostAddress& InetAddr );
    bool EvaluateCLSendEmptyMesMes     ( const CVector<uint8_t>& vecData );

    int                     iOldRecID;
    int                     iOldRecCnt;

    // these two objects must be sequred by a mutex
    uint8_t                 iCounter;
    std::list<CSendMessage> SendMessQueue;

    QTimer                  TimerSendMess;
    QMutex                  Mutex;

public slots:
    void OnTimerSendMess() { SendMessage(); }

signals:
    // transmitting
    void MessReadyForSending ( CVector<uint8_t> vecMessage );
    void CLMessReadyForSending ( CHostAddress     InetAddr,
                                 CVector<uint8_t> vecMessage );

    // receiving
    void ChangeJittBufSize ( int iNewJitBufSize );
    void ReqJittBufSize();
    void ChangeNetwBlSiFact ( int iNewNetwBlSiFact );
    void ChangeChanGain ( int iChanID, double dNewGain );
    void ConClientListMesReceived ( CVector<CChannelShortInfo> vecChanInfo );
    void ServerFullMesReceived();
    void ReqConnClientsList();
    void ChangeChanName ( QString strName );
    void ReqChanName();
    void ChatTextReceived ( QString strChatText );
    void PingReceived ( int iMs );
    void NetTranspPropsReceived ( CNetworkTransportProps NetworkTransportProps );
    void ReqNetTranspProps();
    void Disconnection();

    void CLPingReceived ( CHostAddress InetAddr, int iMs );
    void CLRegisterServerReceived ( CHostAddress    InetAddr,
                                    CServerCoreInfo ServerInfo );
    void CLServerListReceived ( CHostAddress         InetAddr,
                                CVector<CServerInfo> vecServerInfo );
    void CLReqServerList ( CHostAddress InetAddr );
    void CLSendEmptyMes ( CHostAddress TargetInetAddr );
};

#endif /* !defined ( PROTOCOL_H__3B123453_4344_BB2392354455IUHF1912__INCLUDED_ ) */
