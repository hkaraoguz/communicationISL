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

    myrobot = new Robot(this);

    if(!this->initializeNetwork())
    {
        qDebug()<<"Initialization failed";
    }
    if(myrobot->isCoordinator())
        firstNetworkReceived = true;
    else
        firstNetworkReceived = false;

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
        int iscoord =   result["iscoordinator"].toInt();
        int robotID =   result["robotID"].toInt();

        QString temp = QString::number(robotID);

        QString temp2 = "IRobot"+temp;

        // qDebug()<<temp2;

        myrobot->setName(temp2);

        if(iscoord == 1) myrobot->setCoordinator(true);

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

            if(!robot->isCoordinator())
            {
                connect(robot,SIGNAL(networkInfo(QStringList)),this,SLOT(handleNetworkInfo(QStringList)));
            }

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
    Client* cl = new Client(OUTGOING_CLIENT,this);

    // connect(tempClient, SIGNAL(clientDisconnected(int)),this, SLOT(getClientDisconnected(int)));

    //this->tempSocket = new QTcpSocket(this);

    // QObject::connect(tempClient->socket,SIGNAL(connected()),this,SLOT(handleSocketConnect()));

    // QObject::connect(tempClient->socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(handleSocketError(QAbstractSocket::SocketError)));

    cl->setIP(hostAddress);

    cl->socket->connectToHost(hostAddress,port);

    if(cl->socket->waitForConnected(1000)){

        for(int i = 0; i < robots.size() ; i++){

            //  qDebug()<<robots[i]->getIP();

            //  qDebug()<<tempClient->getClientIP();

            if(robots[i]->getIP() == cl->getIP())
            {
                QObject::disconnect(cl,SIGNAL(clientDisconnected(int)),this,SLOT(getClientDisconnected(int)));

                cl->setParent(robots[i]);

                connect(cl, SIGNAL(clientDisconnected(int)),robots[i], SLOT(getClientDisconnected(int)));

                robots[i]->setOutgoingClient(cl);

                robots[i]->setOutGoingConnected(true);

                qDebug()<<"Outgoing connected";

                //tempClient=0;
                //tempClient->deleteLater();

                return;
            }
        }

    }
    else
    {

        qDebug()<<"Error";

        for(int i = 0; i < robots.size() ; i++){

            if(robots[i]->getIP() == cl->getIP())
            {

                robots[i]->setOutGoingConnected(false);
                break;

            }
        }

        /* tempClient->socket->deleteLater();
        tempClient->socket = 0;*/
        //  tempClient->deleteLater();
        //  tempClient = 0;


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
// Send the information to the coordinator
void CommunicationManager::handleCoordinatorUpdate(navigationISL::robotInfo info)
{
    navigationISL::neighborInfo inf;

    inf.name = myrobot->getName().toStdString();
    inf.posX = info.posX;
    inf.posY = info.posY;

    qDebug()<<"Coordinator update received from nav";

    if(myrobot->isCoordinator())
    {
        this->rosthread->coordinatorUpdatePublisher.publish(inf);

        return;
    }
    for(int i = 0; i < robots.size(); i++)
    {

        if(robots[i]->isCoordinator())
        {


            robots[i]->sendCoordinatorUpdatetoCoordinator(inf);

            break;
        }
    }


}
void CommunicationManager::handleNavigationISLInfo(navigationISL::robotInfo msg)
{
    navigationISL::robotInfo info = msg;

    // qDebug()<<msg.posX<<" "<<msg.posY;

    for(int i = 0; i < neighbors.size(); i++){

        for(int j = 0; j < robots.size(); j++)
        {

            if(neighbors[i] ==  robots[j]->getName())
            {

                robots[j]->sendRobotInfo(info);

                break;


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

            if(myrobot->isCoordinator())
            {
                //  connect(robots.at(i)->incomingclient,SIGNAL(coordinatorUpdate(navigationISL::robotInfo)), this,SLOT(handleCoordinatorInfo));

            }

            qDebug()<<"A new connection";



            return;
        }
    }

    socket->abort();
    // socket->deleteLater();

}
void CommunicationManager::handleNetworkUpdateFromCoordinator(navigationISL::networkInfo info)
{
    QString str =  QString::fromStdString(info.network);

    QStringList nbrs = str.split(",");

    //qDebug()<<nbrs;

    if(myrobot->isCoordinator())
    {
        QString temp = myrobot->getName();

        temp.remove("IRobot");

        int id = temp.toInt();

        if(nbrs.at(id-1).size()> 1)
        {

            this->neighbors.clear();

            neighbors = nbrs.at(id-1).split(";");

        }
        else
        {
            neighbors.clear();

            if(nbrs.at(id-1) != "0")
            {

                neighbors.push_back(nbrs.at(id-1));

            }
        }
        qDebug()<<neighbors;
        //nbrs.removeAt(id-1);

        for(int j = 0; j < nbrs.size(); j++)
        {
            if(j != (id-1))
            {
                if(nbrs.at(j).size()>= 1)
                {

                    QStringList list = nbrs.at(id-1).split(";");

                    QString name = "IRobot";
                    name.append(QString::number(j+1));

                    for(int k = 0; k < this->robots.size(); k++)
                    {
                        if(this->robots.at(k)->getName() == name)
                        {
                            //qDebug()<<list;


                            this->robots.at(k)->sendNetworkInfo(list);
                        }
                    }

                }
               /* else
                {
                    if(nbrs.at(j) != QString::number(0))
                    {
                        this->robots.at(k)->sendNetworkInfo(nbrs);



                    }
                }*/



            }

        }

    }


}
bool CommunicationManager::initializeNetwork()
{

    // Read config file
    QString path = QDir::homePath();
    path.append("/fuerte_workspace/sandbox/configISL.json");
    if(!this->readConfigFile(path)) return false;

    // Initialize TCP server
    this->TcpComm = new tcpComm(this);

    // Start listening incoming connections
    this->TcpComm->myServer->setupServer();

    // Direct incoming connections to the slot
    QObject::connect(this->TcpComm->myServer,SIGNAL(newCommRequest(QTcpSocket*)),this,SLOT(handleNewCommRequest(QTcpSocket*)));

    // If I am the coordinator I should connect to other robots
    if(myrobot->isCoordinator())
    {
        this->connectToRobots();
    }
    // After 5 seconds start to connect with other robots
    // QTimer::singleShot(5000,this,SLOT(connectToRobots()));

    return true;

}
void CommunicationManager::connectToRobots()
{

    // Connect to each robot in the list
    for(int i = 0; i < this->robots.size(); i++)
    {

        this->connectToHost(robots.at(i)->getIP(),1200);


    }



}
void CommunicationManager::handleNetworkInfo(QStringList list)
{
    this->neighbors = list;

    // If this is the first time we received the network info
    if(!firstNetworkReceived){

        firstNetworkReceived = true;
        this->connectToRobots();
    }

    qDebug()<<myrobot->getName()<<" neighbors "<<this->neighbors;
}

