#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
using namespace std;
ofstream outputfile;
ostringstream temp;
bool start = true;
struct tm* local_time()
{
    time_t now = time(0);
    tm* ltm = localtime(&now);
    ltm->tm_year += 1900;
    ltm->tm_mon += 1;
//     cout << ltm->tm_year << endl;
//     cout << ltm->tm_mon << endl;
//     cout << ltm->tm_mday << endl;
//     cout << ltm->tm_hour << endl;
//     cout << ltm->tm_min << endl;
//     cout << ltm->tm_sec << endl;
    return ltm;
}
void odomCB(const nav_msgs::Odometry::ConstPtr& msg)
{
    if(msg->pose.pose.position.x == 0&&start)
    {
        cout<<"aaaaa"<<endl;
        return;
    }
    double px = msg->pose.pose.position.x;
    double py = msg->pose.pose.position.y;
    double pz = msg->pose.pose.position.z;
    double w = msg->pose.pose.orientation.w;
    double x = msg->pose.pose.orientation.x;
    double y = msg->pose.pose.orientation.y;
    double z = msg->pose.pose.orientation.z;
    Eigen::Quaternion<double> q = Eigen::Quaternion<double>(w,x,y,z);
    //Eigen::Quaterniond q = Eigen::Quaterniond(vector4d(x,y,z,w));
    Eigen::Matrix3d R = q.normalized().toRotationMatrix();
    
    outputfile <<
    R(0,0)<<' '<<R(0,1)<<' '<<R(0,2)<<' '<<px<<' '<<
    R(1,0)<<' '<<R(1,1)<<' '<<R(1,2)<<' '<<py<<' '<<
    R(2,0)<<' '<<R(2,1)<<' '<<R(2,2)<<' '<<pz<<endl;
    

}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "odom2txt");
    ros::NodeHandle nh_;
    ///检查目录是否存在，不存在就建一个
//     if(access("/home/xcy/.robot_log", 00))
//     system("mkdir /home/xcy/.robot_log");
    string topic_name = "/lio_sam/mapping/odometry";
    if(argc>1)
    {
        topic_name = argv[1];
    }
    ostringstream temp;
    string filename;
    tm* _p_fopen_time = local_time();
    temp <<
         "/home/xcy/" <<
         _p_fopen_time->tm_year <<
         setfill('0') << setw(2) << _p_fopen_time->tm_mon <<
         setfill('0') << setw(2) << _p_fopen_time->tm_mday <<
         setfill('0') << setw(2) << _p_fopen_time->tm_hour <<
         setfill('0') << setw(2) << _p_fopen_time->tm_min <<
         setfill('0') << setw(2) << _p_fopen_time->tm_sec <<
         ".txt";
    filename = temp.str();
    temp.clear();
    temp.str("");
    outputfile.open(filename.c_str(), ios::trunc);
    ros::Subscriber log_sub_ = nh_.subscribe<nav_msgs::Odometry>(topic_name, 10, odomCB);
    //system("find /home/xcy/.robot_log -name \"*.txt\" -mtime +7 -exec rm -fv {} '\;'");
    ros::spin();
    outputfile.close();
    return 0;
}

