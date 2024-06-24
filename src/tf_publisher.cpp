#include <math.h>
#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Quaternion.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_datatypes.h>

class Tf_publisher{
    private:
        tf::TransformBroadcaster transformBroadcaster;
        tf::Transform transform;
        ros::NodeHandle n;

    public:
        void publish_tf(const nav_msgs::Odometry::ConstPtr &data){
            double x = data->pose.pose.position.x;
            double y = data->pose.pose.position.y;
            geometry_msgs::Quaternion theta = data->pose.pose.orientation;

            tf::Quaternion q;
            quaternionMsgToTF(theta, q);

            transform.setOrigin( tf::Vector3(x, y, 0.0) );
            transform.setRotation(q);
            transformBroadcaster.sendTransform(tf::StampedTransform(transform, data->header.stamp, "UGV_odom", "base_link"));
        }

        void init(){
            ros::Subscriber bagReader = n.subscribe<nav_msgs::Odometry>("/ugv/odom", 1000, &Tf_publisher::publish_tf,this);
            ros::spin();
        }
};

int main(int argc, char *argv[]){

    ros::init(argc, argv, "tf_publisher");
    Tf_publisher tf_publisher;
    tf_publisher.init();

    return 0;
}