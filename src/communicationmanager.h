#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H


#include "tcpComm.h"
#include "robot.h"
#include <QObject>
#include <navigationISL/robotInfo.h>
#include <QXmlStreamReader>

class RosThread;

class CommunicationManager : public QObject
{
    Q_OBJECT
public:
    explicit CommunicationManager(QObject *parent = 0);

    // Connection request to the given address and port
    void connectToHost(QString hostAddress, quint16 port);

    // Handle messages from Navigation ISL
    void handleNavigationISLInfo(navigationISL::robotInfo msg);

    void handleCoordinatorUpdate(navigationISL::robotInfo info);

    bool readConfigFile(QString filename);

    bool initializeNetwork();



    RosThread* rosthread;

private:

    tcpComm* TcpComm;
    QTcpSocket tempSocket;
    Client* tempClient;

    QVector<Robot*> robots;

    QVector<QString> neighbors;

    Robot* myrobot;



   // RosThread* rosthread;

signals:
    
public slots:

private slots:

    void connecttt();

    void handleSocketConnect();

    void handleSocketError(QAbstractSocket::SocketError error);
    
    void handleNewCommRequest(QTcpSocket* socket);

    void getClientDisconnected(int type);

    void connectToRobots();
};

#endif // COMMUNICATIONMANAGER_H
