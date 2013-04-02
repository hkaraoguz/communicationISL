#include "robot.h"
#include "communicationmanager.h"
#include "rosThread.h"

Robot::Robot(QObject *parent) :
    QObject(parent)
{
    incomingConnected = false;
    outgoingConnected = false;

    coordinator = false;
}
navigationISL::robotInfo Robot::getRobotInfo(){

    return this->info;

}
void Robot::setRobotInfo(navigationISL::robotInfo info){

    this->info = info;
}
void Robot::setName(QString nam){

    this->name = nam;
}
QString Robot::getName(){

    return name;
}
void Robot::setIncomingClient(Client *client)
{

    incomingclient = client;
}
void Robot::setOutgoingClient(Client *client)
{
    outgoingclient = client;


}
QString Robot::getIP(){
    return IP;
}
void Robot::setIP(QString ip){

    this->IP = ip;
}
bool Robot::isIncomingConnected(){

    return incomingConnected;
}
bool Robot::isOutgoingConnected(){

    return outgoingConnected;
}
void Robot::setOutGoingConnected(bool status){

    outgoingConnected = status;
}
void Robot::setIncomingConnected(bool status){

    incomingConnected = status;
}
void Robot::sendRobotInfo(navigationISL::robotInfo info)
{

    if(outgoingConnected)
    {
        this->outgoingclient->sendRobotInfotoNeighbor(info);

        QFile file("../toRobot.txt");

        if(!file.exists())
        {
            file.open(QFile::WriteOnly);
        }
        else
        {
            file.open(QFile::Append);

        }
        QTextStream stream(&file);

        QString str = this->name;

        str.remove("IRobot");

        int val = Client::RECV_ROBOT_INFO;

        stream << QDateTime::currentDateTime().toTime_t()<<" "<<" "<<val<<" "<<info.posX<<";"<<info.posY<<";"<<info.radius<<" "<<str<<"\n";

        file.close();
    }
}
bool Robot::isCoordinator()
{
    return coordinator;
}
void Robot::setCoordinator(bool status)
{
    coordinator = status;
}
void Robot::sendNetworkInfo(QStringList info)
{
    if(this->outgoingConnected)
    {
        this->outgoingclient->sendNetworkInfo(info);
    }
}
void Robot::getClientDisconnected(int type)
{
    qDebug()<<"Robot "<<this->name<<" client "<<type<<" disconnected"<<"name "<<name;

    if(type == INCOMING_CLIENT)
    {
        //incomingclient->socket->abort();
        this->incomingConnected = false;
        this->incomingclient->deleteLater();
    }
    else
    {
        //outgoingclient->socket->abort();
        this->outgoingConnected = false;
        this->outgoingclient->deleteLater();
    }
}
void Robot::receiveRobotInfo(navigationISL::robotInfo info)
{
    this->info = info;

    navigationISL::neighborInfo ninfo;

    ninfo.name = this->name.toStdString();

    ninfo.posX = this->info.posX;

    ninfo.posY = this->info.posY;

    ninfo.radius = this->info.radius;

    ninfo.targetX = this->info.targetX;

    ninfo.targetY = this->info.targetY;

    CommunicationManager* manager = (CommunicationManager*)this->parent();

    manager->rosthread->neighborInfoPublisher.publish(ninfo);

    QFile file("../fromRobot.txt");

    if(!file.exists())
    {
        file.open(QFile::WriteOnly);
    }
    else
    {
        file.open(QFile::Append);

    }
    QTextStream stream(&file);

    QString str = this->name;

    str.remove("IRobot");

    int val = Client::RECV_ROBOT_INFO;

    stream << QDateTime::currentDateTime().toTime_t()<<" "<<" "<<val<<" "<<info.posX<<";"<<info.posY<<";"<<info.radius<<" "<<str<<"\n";

    file.close();

}
// Receive coordinator update from the client robot
void Robot::receiveCoordinatorUpdate(navigationISL::neighborInfo info)
{


    info.name = name.toStdString();

    qDebug()<<QString::fromStdString(info.name)<<info.posX<<info.posY;

    CommunicationManager* manager = (CommunicationManager*)this->parent();

    manager->rosthread->coordinatorUpdatePublisher.publish(info);

    qDebug()<<"Received a coordinator update";

    QFile file("../CoordinatorReceivedUpdate.txt");

    if(!file.exists())
    {
        file.open(QFile::WriteOnly);
    }
    else
    {
        file.open(QFile::Append);

    }
    QTextStream stream(&file);

    QString str = this->name;

    str.remove("IRobot");

    int val = Client::RECV_COORDINATOR_UPDATE;

    stream << QDateTime::currentDateTime().toTime_t()<<" "<<" "<<val<<" "<<info.posX<<";"<<info.posY<<" "<<str<<"\n";

    file.close();

}
void Robot::sendCoordinatorUpdatetoCoordinator(navigationISL::neighborInfo info)
{
    if(this->isOutgoingConnected())
        this->outgoingclient->sendCoordinatorUpdatetoCoordinator(info);




}
void Robot::receiveNetworkInfoFromCoordinator(QStringList list)
{

  //  CommunicationManager* manager = (CommunicationManager*)this->parent();

    qDebug()<<"Here i am";
    QStringList tempList = list;

    emit networkInfo(tempList);

}
