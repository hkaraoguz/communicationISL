#include "Client.h"
#include "tcpComm.h"
#include <opencv2/opencv.hpp>
#include <QtGui/QMessageBox>
#include <QtNetwork/QHostInfo>
#include <QtCore/QString>
#include <string>
#include <sstream>

using namespace std;

bool firstTime = true;
int iii = 0;
	
Client::Client(QTcpSocket* sock, int clientType, QObject* parent):QObject(parent)
{


	socket = sock;

    socket->setParent(this);

    socket->setReadBufferSize(0);

	type = clientType;

    //add = socket->peerAddress();

    IP = socket->peerAddress().toString(); // get IP

    connect(socket, SIGNAL(disconnected()), this, SLOT(getSocketDisconnected()));

    connect(this, SIGNAL(clientDisconnected(int)),this->parent(), SLOT(getClientDisconnected(int)));

    //connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

	connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));

	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displaySocketError(QAbstractSocket::SocketError)));

    //connect(this,SIGNAL(imageReceived(QImage)), TcpComm, SLOT(getClientImageReceived(QImage)));

    // When neighbor info is received, notify the parent
    connect(this,SIGNAL(neighborInfo(navigationISL::robotInfo)),this->parent(),SLOT(receiveRobotInfo(navigationISL::robotInfo)));

    // When coordinator update is received notify the parent
    connect(this,SIGNAL(coordinatorUpdate(navigationISL::neighborInfo)),this->parent(),SLOT(receiveCoordinatorUpdate(navigationISL::neighborInfo)));

    connect(this, SIGNAL(networkInfo(QStringList)),this->parent(),SLOT(receiveNetworkInfoFromCoordinator(QStringList)));
    clientSocketError = QAbstractSocket::UnknownSocketError; // initially no error

	speedCounter = 0;

	speedTest = false;

    qDebug()<<"Client IP is: "<<IP;


}
// Outgoing client constructor
Client::Client(int clientType, QObject* parent):QObject(parent)
{
    socket = new QTcpSocket(this);
	    
	socket->setReadBufferSize(0);

	type = clientType;



    connect(socket, SIGNAL(disconnected()), this, SLOT(getSocketDisconnected()));

    //connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));

	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displaySocketError(QAbstractSocket::SocketError)));



	clientSocketError = QAbstractSocket::UnknownSocketError; // initially no error

}
Client::~Client()
{


}
QByteArray Client::makeDataPackage(int task, int dataSize, QByteArray data)
{

    QByteArray package;

    QString str;

    str = "AA";

    package.append(str); // control byte

    package.append(",");

    str.setNum(task);

    package.append(str); //task

    package.append(",");

    str.setNum(dataSize);

    package.append(str);

    package.append(",");

    package.append(data); //data

    return package;


}
void Client::receiveData(){
	
    // Read the buffer;
    myRecDataBA = socket->readAll();

    // Convert to QString
    myRecData = QString::fromAscii(myRecDataBA);

    // Split the data (Comma seperated format)
    QStringList list = myRecData.split(",",QString::SkipEmptyParts);

    // Incoming data parts
    qDebug()<<"Number of incoming data parts"<<list.size();
    qDebug()<<list;
  /*  for(int i = 0; i < list.size();i++){

        qDebug()<<list[i]<<" "<<i;
    }*/

    // If list contains anything, process it
    if(list.size()>0)
    {
        // If the control byte is correct
        if(list.at(0) == "AA")
        {
            // Read the task
            int task = list.at(1).toInt();

            // Clear the buffers
            myRecData.clear();
            myRecDataBA.clear();

            // The end part contains the whole data
            myRecData = list.at(list.size()-1);

            // Handle data
            this->handleTask(task,1);

            myRecData.clear();

        }
    }

	

}

void Client::sendData(QByteArray data){

	socket->write(data); 
   // socket->waitForBytesWritten(500);

}
void Client::handleTask(int task , int dataSize){

    switch(task)
    {
    case SEND_HOST_NAME:
        sendHostName();
        break;
    case RECV_HOST_NAME:
        receiveHostName();
        break;

    case RECV_IMAGE:
        receiveImage();
        break;
    case RECV_IMAGE_DSIZE:
        receiveImageDataSize();
        break;
    case RECV_ROBOT_INFO:
        receiveRobotInfoFromNeighbor();
        break;
    case RECV_COORDINATOR_UPDATE:
        receiveCoordinatorUpdateFromClient();
        break;
    case RECV_NETWORK_INFO:
        receiveNetworkInfo();
        break;
    default:
        break;
    }

}
void Client::getSocketDisconnected()
{

    // socket->deleteLater();
    // if remote host has closed the connection

   // socket->deleteLater();
    emit(clientDisconnected(type));


    /*	if(clientSocketError == QAbstractSocket::RemoteHostClosedError){

    socket->deleteLater();

    emit(clientDisconnected(id,type));

    }
    else if(clientSocketError == QAbstractSocket::UnknownSocketError){


    socket->deleteLater();

    emit(clientDisconnected(id,type));

    }
    else if(clientSocketError == QAbstractSocket::HostNotFoundError){

    socket->deleteLater();

    emit(clientDisconnected(id,type));

    }
    else if(clientSocketError == QAbstractSocket::SocketTimeoutError){

    socket->deleteLater();

    emit(clientDisconnected(id,type));
    }
    else if(clientSocketError == QAbstractSocket::co*/

}
// Displays socket error in a MessageBox
void Client::displaySocketError(QAbstractSocket::SocketError socketError){

    qDebug()<<"Socket Error!!!";

    if(type == 0)
    emit(clientDisconnected(type));

   // socket->deleteLater();

    return;

    QMessageBox information;

	switch (socketError) {
	 case QAbstractSocket::RemoteHostClosedError:
		 clientSocketError = socketError;
		 break;
	 case QAbstractSocket::HostNotFoundError:

		 information.setWindowTitle(tr("QtServer"));
		 information.setText(tr("The host was not found. Please check the "
			 "host name and port settings."));
		 information.exec();
		 break;
	 case QAbstractSocket::ConnectionRefusedError:

		 information.setWindowTitle(tr("QtServer"));
		 information.setText(tr("The connection was refused by the peer. "
			 "Make sure the fortune server is running, "
			 "and check that the host name and port "
			 "settings are correct."));
		 information.exec();
		 break;

	 default:

		 information.setWindowTitle(tr("QtServer"));
		 information.setText(tr("The following error occurred: %1.")
			 .arg(socket->errorString()));
		 information.exec();
	}

	socket->deleteLater();
    emit(clientDisconnected(type));
}
QString Client::getHostName(){

    return hostName;

}
void Client::setHostName(QString Name){

    hostName = Name;


}

void Client::sendRobotInfotoNeighbor(navigationISL::robotInfo info)
{
    QByteArray data;

    QString temp = QString::number(info.neighbors.size());

    data.append(temp);

   // data.append(info.neighbors.size());

    data.append(";");

    for(int i = 0; i < info.neighbors.size();i++){

        QString temp = QString::fromStdString(info.neighbors[i]);

        data.append(temp);

        data.append(";");


    }
    temp = QString::number(info.posX);

    data.append(temp);

    data.append(";");

    temp = QString::number(info.posY);

    data.append(temp);

    data.append(";");

    temp = QString::number(info.radius);

    data.append(temp);

    data.append(";");

    temp = QString::number(info.targetX);

    data.append(temp);

    data.append(";");

    temp = QString::number(info.targetY);

    data.append(temp);

    //data.append(";");

    int dataSize = data.size();

    QByteArray dat = makeDataPackage(RECV_ROBOT_INFO,dataSize,data);

    this->socket->waitForBytesWritten(500);

    sendData(dat);


}
void Client::receiveNetworkInfo()
{
    QStringList list = myRecData.split(";",QString::SkipEmptyParts);

    if(list.size() > 0){
        qDebug()<<list;
        emit networkInfo(list);
    }



}
void Client::receiveRobotInfoFromNeighbor()
{
    navigationISL::robotInfo robotInfo;

    QStringList result = myRecData.split(";");

    if(result.size() > 0)
    {
       int neighborSize = result.at(0).toInt();

       robotInfo.neighbors.resize(neighborSize);

       for(int i = 1; i <= neighborSize;i++)
       {

           robotInfo.neighbors[i-1] = result.at(i).toStdString();


       }

       robotInfo.posX = result.at(neighborSize+1).toFloat();

       robotInfo.posY = result.at(neighborSize+2).toFloat();

       robotInfo.radius = result.at(neighborSize+3).toFloat();

       robotInfo.targetX = result.at(neighborSize+4).toFloat();

       robotInfo.targetY = result.at(neighborSize+5).toFloat();

       emit neighborInfo(robotInfo);
      // qDebug()<<"Target y"<<robotInfo.targetY;
    }


}
void Client::sendCoordinatorUpdatetoCoordinator(navigationISL::neighborInfo info)
{
    QByteArray data;

    QString temp = QString::number(info.posX);

    data.append(temp);

    data.append(";");

    temp = QString::number(info.posY);

    data.append(temp);

    int dataSize = data.size();

    QByteArray dat = makeDataPackage(RECV_COORDINATOR_UPDATE,dataSize,data);

    this->socket->waitForBytesWritten(500);


    sendData(dat);

}
void Client::receiveCoordinatorUpdateFromClient()
{
    navigationISL::neighborInfo info;

    QStringList result = myRecData.split(";");

    qDebug()<<result;

    if(result.size() > 1)
    {
        info.posX = result.at(0).toFloat();

        info.posY = result.at(1).toFloat();


        emit coordinatorUpdate(info);
    }


}
void Client::sendNetworkInfo(QStringList info)
{
    QByteArray data;

    qDebug()<<"Network info sent";

   // QString temp = QString::number(info.neighbors.size());

   // data.append(temp);

   // data.append(info.neighbors.size());

    //data.append(";");

    for(int i = 0; i < info.size();i++){

        data.append(info[i]);

        data.append(";");

    }

    int dataSize = data.size();

    QByteArray dat = makeDataPackage(RECV_NETWORK_INFO,dataSize,data);

    this->socket->waitForBytesWritten(500);

    sendData(dat);


}
void Client::setIP(QString ip){

	IP = ip;



    //emit(sendClientInfo(list,type));

}
QString Client::getIP(){


    return IP;

}
void Client::receiveImage(){

	if(socket->bytesAvailable() < imageDataSize) return;

	myRecDataBA = socket->readAll();

	//myRecDataBA = socket->readAll();
	
	QImage image;

	image.loadFromData(myRecDataBA,"JPG");

	emit(imageReceived(image));

	disconnect(socket,SIGNAL(readyRead()),this,SLOT(receiveImage()));

	connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));

	// QImage* qImage, IplImage* cvImage
/*	IplImage* cvImage = cvCreateImageHeader(cvSize(image.width(), image.height()), IPL_DEPTH_8U, 4);
	
	cvImage->imageData = (char*)image.bits();

	uchar* newdata = (uchar*) malloc(sizeof(uchar) * image.byteCount());
	memcpy(newdata, image.bits(), image.byteCount());
	cvImage->imageData = (char*) newdata;
	
	IplImage* imge = cvCreateImage(cvGetSize(cvImage),8,3);
	
	cvConvertImage(cvImage,imge);

	cvNamedWindow("win");

	cvShowImage("win",imge);

	cvWaitKey(0);

	cvReleaseImage(&cvImage);

	cvReleaseImage(&imge);

	cvDestroyAllWindows();

*/

}
void Client::receiveSPCounter(bool respond){

    if(speedTest || type == INCOMING_CLIENT){

		speedCounter = myRecData.toLong();

		myRecData.clear();

		speedCounter++;

		QString str;

		str.setNum(speedCounter);

		QByteArray dat = str.toAscii();

		QByteArray dat2;

//		dat2 = makeDataPackage(RECV_SP_COUNTER,dat.size(),dat);

//		sendData(dat2);

	}
}
void Client::sendSPCounter(){

	speedTest = true;

	QString str;

	str.setNum(speedCounter);

	QByteArray dat = str.toAscii();

	QByteArray dat2;

//	dat2 = makeDataPackage(RECV_SP_COUNTER,dat.size(),dat);

//	sendData(dat2);


}
void Client::sendHostName(){

    //int dataSize = 0;

	QByteArray dat = hostName.toAscii();

    int dataSize = dat.size();

	dat = makeDataPackage(RECV_HOST_NAME,dataSize,dat);

	sendData(dat);

}
void Client::receiveHostName(){


    this->hostName = myRecData;


}
void Client::sendImage(QImage image){

		QByteArray ba;
        
		QBuffer buffer(&ba);
        
		buffer.open(QIODevice::WriteOnly);
        
		image.save(&buffer, "JPG");
		
		QString str;

		socket->write(ba);

	
}
void Client::sendImageDataSize(QImage image){
	
		QByteArray ba;
        
		QBuffer buffer(&ba);
        
		buffer.open(QIODevice::WriteOnly);
        
		image.save(&buffer, "JPG");

		QString str;

		str.setNum(ba.size());

		QByteArray dummy = str.toAscii();

		QByteArray dat = makeDataPackage(RECV_IMAGE_DSIZE,str.size(),dummy);
		
		sendData(dat);



}
// Remote host wants to send me an image
void Client::receiveImageDataSize(){

	imageDataSize = myRecData.toLong();

	sendAcknowledge(true);

	disconnect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));

	connect(socket,SIGNAL(readyRead()),this,SLOT(receiveImage()));

	
}
void Client::sendAcknowledge(bool status){

	Task t;

	if(status) t = RECV_ACK_TRUE;
	else t = RECV_ACK_FALSE;

	QByteArray dummy, dummy2;
	
	dummy2 = makeDataPackage(t,0,dummy);

	sendData(dummy2);

}
void Client::receiveAcknowledge(){

//	emit(acknowledgeReceived(id));
}
