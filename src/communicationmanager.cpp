#include "communicationmanager.h"
#include <QTimer>
#include <QString>
#include <QTreeWidget>
#include <qjson/parser.h>
#include "rosThread.h"
CommunicationManager::CommunicationManager(QObject *parent) :
    QObject(parent)
{
   // robots.resize(1);

    if(!this->initializeNetwork())
    {
        qDebug()<<"Initialization failed";
    }

  //  Robot* robot = new Robot(this);

 //   robot->setIP("127.0.1.1");

  //  this->robots.push_back(robot);

  //  QTimer::singleShot(2000,this,SLOT(connecttt()));



   // this->connectToHost("192.168.68.225",1200);
}
bool CommunicationManager::readConfigFile(QString filename)
{
    QFile file(filename);

    if(!file.exists()) return false;

    if(!file.open(QFile::ReadOnly))return false;

    QJson::Parser parser;

    bool ok;

    QVariantMap result = parser.parse(&file,&ok).toMap();

    if(!ok){

        file.close();
        qDebug()<<"Fatal reading error";

        return false;
    }
    else
    {
       // qDebug()<<result["numrobots"].toString();

        int numrobots = result["numrobots"].toInt();

        this->robots.resize(numrobots);

        QVariantMap nestedMap = result["Robots"].toMap();

        //QVariantList nestedList = nestedMap["Robot"].toList();

        //qDebug()<<nestedList.at(0).toStringList();
        int count = 0;
        foreach (QVariant plugin, nestedMap["Robot"].toList()) {

            Robot* robot = new Robot(this);

            robot->setIP(plugin.toMap()["ip"].toString());

            robot->setName(plugin.toMap()["name"].toString());

            int coord = plugin.toMap()["coordinator"].toInt();

            if(coord == 1) robot->setCoordinator(true);

            this->robots[count] = robot;

            count++;
           // qDebug() << "\t-" << plugin.toMap()["ip"].toString();
        }



    }

    file.close();
    return true;

   /* QTreeWidget wg;

    configReader = new QXmlStreamReader(&file);

   // configReader->setDevice(&file);

    while (!configReader->atEnd()) {

        configReader->readNext();

        if(configReader->isStartElement())
        {

            if(configReader->name().toString() == "numrobot")
            {
                qDebug()<<configReader->readElementText();


            }

            else if(configReader->name().toString() =="robot"){

                        configReader->readNext();

                        if(configReader->isStartElement()){

                            if(configReader->name().toString() == "name"){

                                qDebug()<<configReader->readElementText();
                            }
                        }
                    }





        }

       // qDebug()<<configReader->tokenString();



       }
       if (configReader->hasError()) {

           file.close();
           return false;
       }
       file.close();
       return true;

*/

}
void CommunicationManager::connecttt(){

  //  qDebug()<<"Error";
  //  this->connectToHost("193.140.195.52",1200);
   this->connectToHost("localhost",1200);
  //  QTimer::singleShot(2000,this,SLOT(connecttt()));

}
void CommunicationManager::connectToHost(QString hostAddress, quint16 port)
{
    tempClient = new Client(OUTGOING_CLIENT,this);

    //this->tempSocket = new QTcpSocket(this);

   // QObject::connect(tempClient->socket,SIGNAL(connected()),this,SLOT(handleSocketConnect()));

   // QObject::connect(tempClient->socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(handleSocketError(QAbstractSocket::SocketError)));

    tempClient->setIP(hostAddress);

    tempClient->socket->connectToHost(hostAddress,port);


    if(tempClient->socket->waitForConnected(1000)){

        for(int i = 0; i < robots.size() ; i++){

          //  qDebug()<<robots[i]->getIP();

          //  qDebug()<<tempClient->getClientIP();

            if(robots[i]->getIP() == tempClient->getIP())
            {
                tempClient->setParent(robots[i]);

                robots[i]->setOutgoingClient(tempClient);

                robots[i]->setOutGoingConnected(true);

                break;
            }
        }

    }
    else
    {


        qDebug()<<"Error";

        for(int i = 0; i < robots.size() ; i++){

            if(robots[i]->getIP() == tempClient->getIP())
            {

                robots[i]->setOutGoingConnected(false);

            }
        }

        tempClient=0;


    }
}
void CommunicationManager::handleSocketConnect()
{
   // Robot* robot = new Robot(this);

   // tempClient->setParent(robot);

   // robot->setOutgoingClient(tempClient);



  //  this->robots.push_back(robot);
   /* int id = TcpComm->findEmptyOutgoingClientSlot();

    if(id >= 0){

        this->TcpComm->outgoingClients[id] = this->tempClient;

        this->TcpComm->outgoingClients[id]->setClientID(id);

        this->TcpComm->outgoingClientSlots[id] = true;

       // this->tempClient = 0;
       // this->tempClient->deleteLater();


    /*Client* aClient = this->TcpComm->outgoingClientss.at(0);

   // QObject::connect(this->TcpComm->outgoingClientss.at(0),SIGNAL(destroyed()),this,SLOT(hebele()));

   // tempClient = 0;
   // delete tempClient;
    QString str;
    str = "hehe";

    QByteArray arr;

    arr.append("sss");

    //arr.append(str.toUtf8());

    aClient->sendData(arr);*/
    //this->TcpComm->

        qDebug()<<"Connected";
    //}
   /* else
    {

        tempClient->socket->abort();
        tempClient->socket->deleteLater();
    }*/
}
void CommunicationManager::handleSocketError(QAbstractSocket::SocketError error){

    qDebug()<<"Error";

    for(int i = 0; i < robots.size() ; i++){

        if(robots[i]->getIP() == tempClient->getIP())
        {

            robots[i]->setOutGoingConnected(false);
            break;

        }
    }

}
void CommunicationManager::getClientDisconnected(int type)
{
    qDebug()<<"Error client disconnected";
    for(int i = 0; i < robots.size() ; i++){

        if(robots[i]->getIP() == tempClient->getIP())
        {

            robots[i]->setOutGoingConnected(false);
            break;

        }
    }

}
void CommunicationManager::handleCoordinatorUpdate(navigationISL::robotInfo info)
{
    for(int i = 0; i < robots.size(); i++)
    {

        if(robots[i]->isCoordinator())
        {

            robots[i]->sendCoordinatorUpdate(info);

            break;
        }
    }


}
void CommunicationManager::handleNavigationISLInfo(navigationISL::robotInfo msg)
{

    qDebug()<<msg.posX<<" "<<msg.posY;

    for(int i = 0; i < msg.neighbors.size(); i++){

        for(int j = 0; j < robots.size(); j++)
        {

            if(msg.neighbors[i].compare(robots[j]->getName().toStdString()) == 0)
            {
                if(robots[j]->isOutgoingConnected())
                {
                    robots[j]->sendRobotInfo(msg);

                    break;
                }

            }
        }


    }

}
void CommunicationManager::handleNewCommRequest(QTcpSocket *socket)
{
    for(int i = 0; i < robots.size(); i++){

        if(robots.at(i)->getIP() == socket->peerAddress().toString()){

            Client* aClient = new Client(socket,INCOMING_CLIENT,robots.at(i));

            robots.at(i)->setIncomingClient(aClient);

            robots.at(i)->setIncomingConnected(true);

            qDebug()<<"A new connection";

            return;
        }
    }

    socket->abort();
   // socket->deleteLater();

}
bool CommunicationManager::initializeNetwork()
{

    // Read config file
    QString path = QDir::homePath();
    path.append("/fuerte_workspace/sandbox/configISL.json");
    if(!this->readConfigFile(path)) return false;

    // Setup TCP server
    this->TcpComm = new tcpComm(this);

    this->TcpComm->myServer->setupServer();

    // Direct incoming connections to the slot
    QObject::connect(this->TcpComm->myServer,SIGNAL(newCommRequest(QTcpSocket*)),this,SLOT(handleNewCommRequest(QTcpSocket*)));

    // Connect to each robot in the list
    for(int i = 0; i < this->robots.size(); i++)
    {

        this->connectToHost(robots.at(i)->getIP(),1200);


    }

    return true;

}

