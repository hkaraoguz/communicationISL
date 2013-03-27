// Client class implements communication class btw a server and its clients
// For each new client an instance of a client is created

#pragma once
#include <QtNetwork>
#include <QtCore/QByteArray>
#include <navigationISL/robotInfo.h>
#include "navigationISL/neighborInfo.h"
#define INCOMING_CLIENT 0
#define OUTGOING_CLIENT 1
class tcpComm;
class Client : public QObject
{
		Q_OBJECT
public:
	
	//Client(QTcpSocket* sock, tcpComm* comm, int clientID, int clientType);

    Client(QTcpSocket* sock, int clientType, QObject* parent = 0);
	
	//Client(tcpComm* comm, int clientID, int clientType);

    Client(int clientType, QObject* parent = 0);
	
	QTcpSocket* socket; // socket for communication
	
	tcpComm* TcpComm; // pointer to the parent tcpComm class

	void clearBuffers(); /// Clear data buffers (reduntant)

	void setClientID(int clientid); /// Sets client id

	int getClientID(); /// Returns client id

	QString getClientIP(); // Returns client IP

	void setClientIP(QString ip);

	QString getClientHostName(); // Returns client HostName

	void setClientHostName(QString Name);

	QByteArray makeDataPackage(int task, int dataSize, QByteArray data); // Prepare the Data Package

	void handleTask(int task, int dataSize);

	QString myRecData, mySentData;

	void sendHostName();

	QString receiveHostName();

    void sendRobotInfo(navigationISL::robotInfo info);

    void sendCoordinatorUpdate(navigationISL::robotInfo info);

    void receiveCoordinatorUpdate();

    void receiveRobotInfo();

	void receiveSPCounter(bool respond);

	void sendSPCounter();

	void sendImage(QImage image);

	void sendImageDataSize(QImage image);

	void receiveImageDataSize();

	void receiveAcknowledge();

	void sendAcknowledge(bool status);

	enum Task{
	
		RECV_HOST_NAME = 0x01,

		SEND_HOST_NAME = 0x02,

        SEND_IMAGE = 0x05,

		RECV_IMAGE = 0x06,

		RECV_IMAGE_DSIZE = 0x07,

		RECV_ACK_TRUE = 0x08, //I accept the request

        RECV_ACK_FALSE = 0x09, // I deny the request

        SEND_ROBOT_INFO = 0x10,

        RECV_ROBOT_INFO = 0x11,

        SEND_ROBOT_POSE = 0x12,

        RECV_COORDINATOR_UPDATE = 0x13
	
	};

	long speedCounter;

	bool speedTest;
	
public:
	
	~Client(void);

private:

	int id;

    int type; // 0: incoming client, 1: outgoing client

	char* messageDataSizeBuf;
	
	char* messageBuf;

	QString IP;

	QString hostName;
	
    //QHostAddress add;

	QList<QString> clientInfo;

	QAbstractSocket::SocketError clientSocketError;

	QByteArray myRecDataBA;

	long imageDataSize;
	
	public slots:
		
		void sendData(QByteArray data);
		void receiveData();
		void displaySocketError(QAbstractSocket::SocketError socketError);
		void getHostName(const QHostInfo &host);
		void getSocketDisconnected();
		void receiveImage();

		signals:
		//void clientIP(const QString &ip , int clientID);
        void clientDisconnected(int type);
		void sendClientInfo(const QList<QString>& list, int type);
		void imageReceived(const QImage& image);
		void acknowledgeReceived(int id);
        void neighborInfo(navigationISL::robotInfo info);
        void coordinatorUpdate(navigationISL::robotInfo info);
	
};
