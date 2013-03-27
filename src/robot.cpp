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
    this->outgoingclient->sendRobotInfo(info);
}
bool Robot::isCoordinator()
{
    return coordinator;
}
void Robot::setCoordinator(bool status)
{
    coordinator = status;
}
void Robot::getClientDisconnected(int type)
{
    qDebug()<<"Robot "<<this->name<<" client "<<type<<" disconncted";
    if(type == INCOMING_CLIENT)
    {
        this->incomingConnected = false;
        this->incomingclient->deleteLater();
    }
    else
    {
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

    CommunicationManager* manager = (CommunicationManager*)this->parent();

    manager->rosthread->neighborInfoPublisher.publish(ninfo);

}
void Robot::receiveCoordinatorUpdate(navigationISL::robotInfo info)
{


}
void Robot::sendCoordinatorUpdate(navigationISL::robotInfo info)
{

    this->outgoingclient->sendCoordinatorUpdate(info);



}
