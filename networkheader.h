#ifndef __INCLUDE_FUNC_MY__
#define __INCLUDE_FUNC_MY__

typedef struct {
	unsigned char destMAC[6];					// 宛先MACアドレス
	unsigned char srcMAC[6];					// 送信元MACアドレス
	unsigned char type[2];						// タイプ
} ETHERHEADER;


typedef struct {
	unsigned short hardwareType;				// ハードウェア識別
	unsigned short protocolType;				// プロトコル
	unsigned char hardwareSize;					// HLEN
	unsigned char protocolSize;					// PLEN
	unsigned short opcode;						// 動作
	unsigned char senderMAC[6];					// 送信元MACアドレス
	unsigned char senderIP[4];					// 送信元IPアドレス
	unsigned char targetMAC[6];					// 宛先MACアドレス
	unsigned char targetIP[4];					// 宛先IPアドレス
} ARPDATA;


typedef struct {
	unsigned char headerLength : 4;				// バージョン
	unsigned char version : 4;					// ヘッダー長
	unsigned char service;						// サービス・タイプ
	unsigned short totalLength;					// データグラム長
	unsigned short id;							// ID
	unsigned char offset1 : 6;					// オフセット
	unsigned char dfFlag : 1;					// Don't Fragment フラフ
	unsigned char mfFlag : 1;					// More Fragment フラフ
	unsigned char offset2;						// オフセット
	unsigned char ttl;							// TTL
	unsigned char protocol;						// プロトコル番号
	unsigned short checksum;					// ヘッダ・チェックサム
	unsigned char srcIP[4];						// 送信元IPアドレス
	unsigned char destIP[4];					// 宛先IPアドレス
} IPHEADER;


typedef struct {
	unsigned short srcPort;						// 送信元ポート
	unsigned short destPort;					// 宛先ポート
	unsigned short length;						// 長さ（UDPヘッダを含む）
	unsigned short checksum;					// チェックサム
} UDPHEADER;


typedef struct {
	unsigned char type;							// タイプ
	unsigned char code;							// コード
	unsigned short checksum;					// チェックサム
	unsigned short id;							// ID
	unsigned short seq;							// シーケンス番号
} ICMPDATA;


typedef struct {
	unsigned char msgType;						// メッセージタイプ
	unsigned char hardwareType;					// ハードウェアタイプ
	unsigned char hardwareAddressLength;		// ハードウェアアドレスサイズ
	unsigned char hops;							// DHCPリレーエージェントHOP数
	unsigned int id;							// トランザクションID
	unsigned short seconds;						// 秒数
	unsigned short flag;						// フラグ
	unsigned char clientIP[4];					// 要求元クライアントIPアドレス
	unsigned char yourIP[4];					// 割り当てIPアドレス
	unsigned char serverIP[4];					// サーバIPアドレス
	unsigned char relayIP[4];					// DHCPリレーエージェントIPアドレス
	unsigned char hardwareAddress[16];			// ハードウェアアドレス
	unsigned char serverName[64];				// サーバ名
	unsigned char bootFileName[128];			// ブートファイル名
	unsigned char magicCookie[4];				// MagicCookie
} DHCPDATA;


typedef struct {
	unsigned short srcPort;						// 送信元ポート番号
	unsigned short destPort;					// 宛先ポート番号
	unsigned int seq;							// シーケンス番号
	unsigned int ack;							// Ack番号
	unsigned char offset1 : 4;					// オフセット
	unsigned char headerLength : 4;				// データ・オフセット
	unsigned short signal;
	unsigned char finFlag : 1;						// フラグ
	unsigned char synFlag : 1;
	unsigned char rstFlag : 1;
	unsigned char pshFlag : 1;
	unsigned char ackFlag : 1;
	unsigned char urgFlag : 1;
	unsigned char offset2 : 2;					// オフセット
	unsigned short winSize;						// ウインドウ・サイズ
	unsigned short checksum;					// チェックサム
	unsigned short urgent;						// 緊急ポインタ
} TCPHEADER;


#endif