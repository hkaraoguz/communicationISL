#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include <navigationISL/robotInfo.h>
#include <navigationISL/neighborInfo.h>
#include <QString>
#include "Client.h"

class Robot : public QObject
{
    Q_OBJECT
public:

    explicit Robot(QObject *parent = 0);

    void setRobotInfo(navigationISL::robotInfo info);

    void sendCoordinatorUpdatetoCoordinator(navigationISL::neighborInfo info);

    navigationISL::robotInfo getRobotInfo();

    void setName(QString nam);

    QString getName();

    void setIncomingClient(Client* client);

    void setOutgoingClient(Client* client);

    QString getIP();

    void setIP(QString ip);

    bool isOutgoingConnected();

    bool isIncomingConnected();

    void setOutGoingConnected(bool status);

    void setIncomingConnected(bool status);

    void sendRobotInfo(navigationISL::robotInfo info);

    bool isCoordinator();

    void setCoordinator(bool status);
private:

    navigationISL::robotInfo info;

    QString IP;

    Client* incomingclient;

    Client* outgoingclient;

    bool outgoingConnected;

    bool incomingConnected;

    QString name;

    bool coordinator;
    
signals:
    
public slots:
    void getClientDisconnected(int type);

    // Receive robot info from a neighbor
    void receiveRobotInfo(navigationISL::robotInfo info);

    // Receive a coordinator update info from a robot
    void receiveCoordinatorUpdate(navigationISL::neighborInfo info);


    
};

#endif // ROBOT_H
