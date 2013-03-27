//#include "navigationController.h"
#include <QThread>
#include <QObject>
#include <ros/ros.h>
#include "navigationISL/robotInfo.h"
#include "communicationmanager.h"
#include <QTimer>

#define numOfRobots 5

class RosThread:public QObject
{
    Q_OBJECT

public:

    RosThread();

    RosThread(CommunicationManager* currentmanager);

   // RosThread(int argc, char **argv, std::string nodeName);

public:

     void shutdownROS();

     //friend class CommunicationManager;

     friend class Robot;

private:

     CommunicationManager* manager;

     bool shutdown;

     ros::NodeHandle n;

     ros::Subscriber robotInfoSubscriber;

     ros::Publisher neighborInfoPublisher;


public slots:
     void work();

signals:
   void rosFinished();
   void  rosStarted();
   void  rosStartFailed();

};
