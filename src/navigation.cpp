#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <tf/transform_broadcaster.h>
#include <actionlib/client/simple_action_client.h>

#define X_POS 0
#define Y_POS 1
#define W_POS 2

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseActionClient;

class Navigation{
    private:

	ros::NodeHandle nh;

        void move_to_goal(double x, double y, double theta){
            ROS_INFO("Received goal coordinates: X: %f , Y: %f, Theta: %f", x, y, theta);
            MoveBaseActionClient actionClient("move_base", true);

            ROS_INFO("Waiting for the move_base action server...");
            while(!actionClient.waitForServer(ros::Duration(5.0))){
		        ROS_INFO("Waiting for the move_base action server");
	        }

            move_base_msgs::MoveBaseGoal goalMsg;
   	        ROS_INFO("Action server on");
            goalMsg.target_pose.header.frame_id = "base_link";
            goalMsg.target_pose.header.stamp = ros::Time::now();
            goalMsg.target_pose.pose.position.x = x;
            goalMsg.target_pose.pose.position.y = y;

            geometry_msgs::Quaternion theta_quaternions = tf::createQuaternionMsgFromYaw(theta);
            goalMsg.target_pose.pose.orientation = theta_quaternions;

            actionClient.sendGoal(goalMsg);
            actionClient.waitForResult();

            if (actionClient.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
                ROS_INFO("You have successfully reached the goal");
            } else {
                ROS_ERROR("You have not reached the goal.");
            }
        }

    public:
        void init(){
            std::string waypointsPath;
            ros::param::get("waypoint_file", waypointsPath);
            this->load_waypoints(waypointsPath);
        }

        void load_waypoints(std::string filename){
            std::ifstream file(filename);
            std::string line;

            if (!file.is_open()) {
                ROS_ERROR("Failed to open waypoints file");
                return;
            }

            while (std::getline(file, line)) {
                std::vector<std::string> waypoint;
                std::stringstream ss(line);
                std::string cell;

                while (std::getline(ss, cell, ',')){
                    waypoint.push_back(cell);
                }

                ROS_INFO("Navigating to waypoint: X: %f , Y: %f, Theta: %f",
                            std::stod(waypoint.at(X_POS)), std::stod(waypoint.at(Y_POS)), std::stod(waypoint.at(W_POS)));

                this->move_to_goal(std::stod(waypoint.at(X_POS)),std::stod(waypoint.at(Y_POS)),std::stod(waypoint.at(W_POS)));
            }

            file.close();
        }
};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "navigation");
    Navigation navigation;
    navigation.init();

    return 0;
}