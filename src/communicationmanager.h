#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H


#include "tcpComm.h"
#include "robot.h"
#include <QObject>
#include <navigationISL/robotInfo.h>
#include <navigationISL/networkInfo.h>
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

    // Handle coordinator updates from Navigation ISL
    void handleCoordinatorUpdate(navigationISL::robotInfo info);

    void handleNetworkUpdateFromCoordinator(navigationISL::networkInfo info);

    bool readConfigFile(QString filename);

    bool initializeNetwork();

    RosThread* rosthread;

private:

    tcpComm* TcpComm;

    QTcpSocket tempSocket;

    Client* tempClient;

    QVector<Robot*> robots;

    QStringList neighbors;

    Robot* myrobot;

    bool firstNetworkReceived;

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

    void handleNetworkInfo(QStringList list);
};

#endif // COMMUNICATIONMANAGER_H
