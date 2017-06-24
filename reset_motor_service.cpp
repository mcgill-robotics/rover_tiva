#include <stdbool.h>
#include <stdint.h>
#include <string>



#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>

#include <ros/ros.h>
#include <rover_common/ResetMotor.h>



std_msgs::Bool reset_msg;
ros::Publisher reset_publisher;

bool reset_cb(const rover_common::ResetMotorRequest& request, rover_common::ResetMotorResponse& response){
  response.Success = false;
  ROS_DEBUG("Sending initial message");
  ros::NodeHandle nh_temp;

  reset_msg.data = request.Enable;
  reset_publisher = nh_temp.advertise<std_msgs::Bool>(request.MotorName, 10);
  reset_publisher.publish(reset_msg);

  ros::Duration(0.5).sleep();

  // delete reset_publisher;
  response.Success = true;
  return response.Success;
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "arm_motor_reset_server");
  ros::NodeHandle nh;
  ros::ServiceServer service = nh.advertiseService<rover_common::ResetMotorRequest,
          rover_common::ResetMotorResponse>("arm_motor_reset_server", &reset_cb);
  ros::spin();
}
