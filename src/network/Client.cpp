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
    //id = clientID; // get ID

//	TcpComm = (tcpComm*)parent; // get Parent class pointer

	//socket = new QTcpSocket(this); // get socket

	socket = sock;
	socket->setParent(this);
	
	//socket->setParent(this);

	socket->setReadBufferSize(0);

	type = clientType;

    //add = socket->peerAddress();

    IP = socket->peerAddress().toString(); // get IP

	//QHostInfo::lookupHost(IP,this, SLOT(getHostName(QHostInfo)));

   // connect(this, SIGNAL(sendClientInfo(QList<QString>, int)), this->parent(), SLOT(getClientInfo(QList<QString>, int)));

    connect(this, SIGNAL(clientDisconnected(int)),this->parent(), SLOT(getClientDisconnected(int)));

	//connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

	connect(socket, SIGNAL(disconnected()), this, SLOT(getSocketDisconnected()));

	connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));

	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displaySocketError(QAbstractSocket::SocketError)));

    //connect(this,SIGNAL(imageReceived(QImage)), TcpComm, SLOT(getClientImageReceived(QImage)));
    connect(this,SIGNAL(neighborInfo(navigationISL::robotInfo)),this->parent(),SLOT(receiveRobotInfo(navigationISL::robotInfo)));

    connect(this,SIGNAL(coordinatorUpdate(navigationISL::robotInfo)),this->parent(),SLOT(receiveCoordinatorUpdate(navigationISL::robotInfo)));

    messageDataSizeBuf = new char[50];
	
	messageBuf = new char[50];

	clientSocketError = QAbstractSocket::UnknownSocketError; // initially no error

	clearBuffers();

	speedCounter = 0;

	speedTest = false;

    qDebug()<<"Client IP is: "<<IP;

    QString str = "Hello this is server!!";

    this->sendData(str.toAscii());

}
// outgoing client constructor
Client::Client(int clientType, QObject* parent):QObject(parent)
{
    //id = clientID;

    //TcpComm = (tcpComm*)parent;
	//TcpComm = comm;

    socket = new QTcpSocket(this);
	    
	socket->setReadBufferSize(0);

	type = clientType;

    //hostName = TcpComm->getHostName();

    //connect(this, SIGNAL(sendClientInfo(QList<QString>,int)), this->parent(), SLOT(getClientInfo(QList<QString>,int)));

    connect(this, SIGNAL(clientDisconnected(int)),this->parent(), SLOT(getClientDisconnected(int)));

    connect(socket, SIGNAL(disconnected()), this, SLOT(getSocketDisconnected()));

	connect(socket,SIGNAL(readyRead()),this,SLOT(receiveData()));

	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displaySocketError(QAbstractSocket::SocketError)));

    messageDataSizeBuf = new char[250];
    messageBuf = new char[250];

	clientSocketError = QAbstractSocket::UnknownSocketError; // initially no error

	clearBuffers();


	/*	QString str;

	str.setNum(id);

	list<<str<<hostName<<IP;*/

	//emit(sendClientInfo(list,type));
}
Client::~Client(){

	delete[] messageBuf;
	delete[] messageDataSizeBuf;

}
void Client::clearBuffers(){

	delete[]messageBuf;
	delete[]messageDataSizeBuf;

    messageBuf =  new char[250];
    messageDataSizeBuf  = new char[250];

	for(int i = 0; i < 50; i++){

		messageBuf[i]  = 0;
		messageDataSizeBuf[i] = 0;

	}

}

void Client::receiveData(){
	
	
	myRecDataBA = socket->readAll(); // read the length of the data received

    myRecData = QString::fromAscii(myRecDataBA);

    QStringList list = myRecData.split(",");

    qDebug()<<"List size"<<list.size();

    for(int i = 0; i < list.size();i++){

        qDebug()<<list[i]<<" "<<i;
    }
    if(list.size()>0){

        if(list.at(0) == "AA"){

            int task = list.at(1).toInt();


            myRecData.clear();
            myRecDataBA.clear();

            myRecData = list.at(list.size()-1);

            this->handleTask(task,1);


        }
    }
    /*int dot = 0; // "." points out that rest of the package is just data

	if(myRecDataBA.at(0) == -86){ // if control byte is correct

		//myRecDataBA = socket->read(1); // read task

        int task = (int)((myRecDataBA.at(1)-48) + (myRecDataBA.at(2)-48));
		

		for(int i = 0; i < myRecDataBA.size(); i++){
		
			if(myRecDataBA.at(i) == '.'){
				
				dot = i; // found the end of data size information rest is just data
				break;
			
			}
		
		}
		int dataSize = 0;
		int z = 0;

		//////Calculate data size/////
		for(int i = dot-1; i >= 2; i--){
		
			/// -48 is used to calculate numbers correctly 1 = 49 ascii 
			
			dataSize += (int)((myRecDataBA.at(i)-48)*pow((double)10, (double)z));
			
			z++;
		
		}
		////////////////////////////


		myRecDataBA.remove(0,dot+1); // remove the first bytes, only the data is left
		
	
		if(dataSize != 0){

			

			myRecData = QString::fromAscii(myRecDataBA.data());

		}

		handleTask(task, dataSize);

		myRecData.clear();

		myRecDataBA.clear();


	}
	else{ // if package is not correct don't do anything clear buffers
	
		myRecData.clear();

		myRecDataBA.clear();
	
	}
*/
	

}
void Client::sendData(QByteArray data){

	socket->write(data); 
	//socket->waitForBytesWritten(5000);

}
/// Displays socket error in a MessageBox
void Client::displaySocketError(QAbstractSocket::SocketError socketError){

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
int Client::getClientID(){

	return id;


}
void Client::setClientID(int clientid){


	id = clientid;

	QList<QString>list;

	QString str;

	str.setNum(id);

	list<<str<<hostName<<IP;

	emit(sendClientInfo(list,type));


}
void Client::getHostName(const QHostInfo &host)
{
	if (host.error() != QHostInfo::NoError) {
		qDebug() << "Lookup failed:" << host.errorString();
		return;
	}

	hostName = host.hostName();
	host.addresses();

	QString ids;
	ids.setNum(id);

	clientInfo<<ids<<hostName<<IP;

	emit(sendClientInfo(clientInfo, type));

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
void Client::sendRobotInfo(navigationISL::robotInfo info)
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

    sendData(dat);


}
void Client::receiveRobotInfo()
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
void Client::sendCoordinatorUpdate(navigationISL::robotInfo info)
{
    QByteArray data;

    QString temp = QString::number(info.posX);

    data.append(temp);

    data.append(";");

    temp = QString::number(info.posY);

    data.append(temp);

    int dataSize = data.size();

    QByteArray dat = makeDataPackage(RECV_COORDINATOR_UPDATE,dataSize,data);

    sendData(dat);

}
void Client::receiveCoordinatorUpdate()
{
    navigationISL::robotInfo robotInfo;

    QStringList result = myRecData.split(";");

    if(result.size() > 1)
    {
        robotInfo.posX = result.at(0).toFloat();

        robotInfo.posY = result.at(1).toFloat();


    }


}
void Client::setClientIP(QString ip){

	IP = ip;

	QList<QString>list;

	QString str;

	str.setNum(id);

	list<<str<<hostName<<IP;

    //emit(sendClientInfo(list,type));

}
void Client::getSocketDisconnected(){

    // socket->deleteLater();
	// if remote host has closed the connection

//	socket->deleteLater();
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
QString Client::getClientIP(){


	return IP;

}
QString Client::getClientHostName(){

	return hostName;

}
void Client::setClientHostName(QString Name){

	hostName = Name;

	QList<QString>list;

	QString str;

	str.setNum(id);

	list<<str<<hostName<<IP;

	emit(sendClientInfo(list,type));

}
void Client::handleTask(int task , int dataSize){

	switch(task)
	{
	case SEND_HOST_NAME:
		sendHostName();
		break;
	case RECV_HOST_NAME:
		setClientHostName(receiveHostName());
		break;

	case RECV_IMAGE:
		receiveImage();
		break;
	case RECV_IMAGE_DSIZE:
		receiveImageDataSize();
		break;
    case RECV_ROBOT_INFO:
        receiveRobotInfo();
        break;
    case RECV_COORDINATOR_UPDATE:
        receiveCoordinatorUpdate();
        break;
	default:
		break;
	}	

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
QString Client::receiveHostName(){


	return myRecData;


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

	emit(acknowledgeReceived(id));
}
