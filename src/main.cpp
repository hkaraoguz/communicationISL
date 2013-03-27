#include <ros/ros.h>

#include <QApplication>

#include "communicationmanager.h"

#include "rosThread.h"

#include <QThread>
int main(int argc,char** argv){

    ros::init(argc,argv,"communicationISL");

    QApplication app(argc,argv);

    CommunicationManager manager;

    RosThread* rosthread = new RosThread(&manager);

    manager.rosthread = rosthread;

    QThread thr;

    rosthread->moveToThread(&thr);

    QObject::connect(&thr,SIGNAL(finished()),rosthread,SLOT(deleteLater()));

    QObject::connect(&thr,SIGNAL(started()),rosthread,SLOT(work()));

    thr.start();


    return app.exec();

  //  ros::NodeHandle n;

//    ros::Subscriber amclSub = n.subscribe("amcl_pose",2,amclPoseCallback);

 //   ros::spin();

   // tcpComm comm;

   // comm.myServer->setupServer();

}


