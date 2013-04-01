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
	
    // Constructor for incoming Client
    Client(QTcpSocket* sock, int clientType, QObject* parent = 0);
	
    // Constructor for outgoing Client
    Client(int clientType, QObject* parent = 0);

    ~Client(void);
	

    // Socket for communication
    QTcpSocket* socket;

    // Pointer to the parent tcpComm class (redundant)
    tcpComm* TcpComm;

    // Return client IP
    QString getIP();

    void setIP(QString ip);

    // Return client hostname
    QString getHostName();

    void setHostName(QString Name);

    // Prepare package for sending data
	QByteArray makeDataPackage(int task, int dataSize, QByteArray data); // Prepare the Data Package

    // Handle the incoming task
	void handleTask(int task, int dataSize);

    // Send the hostname to the outgoing client
	void sendHostName();

    // Receive the incoming client's hostname
    void receiveHostName();

    // Send the current robot information to the outgoing client
    void sendRobotInfotoNeighbor(navigationISL::robotInfo info);

    // Send the coordinator update to the outgoing client
    void sendCoordinatorUpdatetoCoordinator(navigationISL::neighborInfo info);

    // Receive the coordinator update from the incoming client
    void receiveCoordinatorUpdateFromClient();

    // Receive the robot info from the incoming client
    void receiveRobotInfoFromNeighbor();

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
	
private:



    // 0: incoming client, 1: outgoing client
    int type;

    QString IP;

	QString hostName;
	
	QAbstractSocket::SocketError clientSocketError;

    // Received and Sent data
    QString myRecData, mySentData;

	QByteArray myRecDataBA;

	long imageDataSize;
	
	public slots:
		
		void sendData(QByteArray data);
		void receiveData();
		void displaySocketError(QAbstractSocket::SocketError socketError);
        //void getHostName(const QHostInfo &host);
		void getSocketDisconnected();
		void receiveImage();

		signals:

        void clientDisconnected(int type);
		void sendClientInfo(const QList<QString>& list, int type);
		void imageReceived(const QImage& image);
		void acknowledgeReceived(int id);
        void neighborInfo(navigationISL::robotInfo info);
        void coordinatorUpdate(navigationISL::neighborInfo info);
	
};
