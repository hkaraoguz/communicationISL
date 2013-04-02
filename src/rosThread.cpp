#include "rosThread.h"
#include <navigationISL/neighborInfo.h>

RosThread::RosThread()
{
    shutdown = false;

}
RosThread::RosThread(CommunicationManager *currentmanager)
{
    manager = currentmanager;

}

/*RosThread::RosThread(int argc, char **argv, std::string nodeName){

    //  ros::init(argc, argv, nodeName);

 //   ros::init(argc,argv,nodeName);
}*/

void RosThread::work(){

    if(!ros::ok()){

        emit rosStartFailed();

        return;
    }

     emit rosStarted();

     robotInfoSubscriber = n.subscribe("navigationISL/robotInfo",1,&CommunicationManager::handleNavigationISLInfo,this->manager);

     coordinatorUpdateSubscriber = n.subscribe("navigationISL/coordinatorUpdate",1,&CommunicationManager::handleCoordinatorUpdate,this->manager);

     // Sends the received neighbor info to the navigationNode
     neighborInfoPublisher = n.advertise<navigationISL::neighborInfo>("communicationISL/neighborInfo",1,true);
  //  this->amclSub = n.subscribe("amcl_pose",2,&RosThread::amclPoseCallback,this);

     // Publishes the received coordinator update from a robot to the coordinator node
     coordinatorUpdatePublisher = n.advertise<navigationISL::neighborInfo>("communicationISL/coordinatorUpdate",1);

     networkUpdateSubscriber = n.subscribe("coordinatorISL/networkInfo",1,&CommunicationManager::handleNetworkUpdateFromCoordinator,this->manager);
    /* navigationISL::robotInfo inf;
     inf.neighbors.resize(1);
     inf.posX = 2;
     inf.posY =3;*/
    // ros::AsyncSpinner spinner(2);

   //  spinner.start();

    ros::Rate loop(30);

    while(ros::ok()){

        //coordinatorUpdatePublisher.publish(inf);

          //  NavigationController::robotContoller(vel, numOfRobots, bin, bt, b_rs, ro, kkLimits);

             //   ros::spinOnce();

             //   loop.sleep();

        ros::spinOnce();
        loop.sleep();

    }

    emit rosFinished();


}
void RosThread::shutdownROS()
{
    ros::shutdown();
   // shutdown = true;


}


