#include <stdbool.h>
#include <stdint.h>
#include <string>



#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>

#include <ros/ros.h>
#include <rover_common/BrakeMotor.h>



std_msgs::Bool brake_msg;
ros::Publisher brake_publisher;

bool brake_cb(const rover_common::BrakeMotorRequest& request, rover_common::BrakeMotorResponse& response){
  ros::NodeHandle nh_temp;

  brake_msg.data = request.Enable;
  brake_publisher = nh_temp.advertise<std_msgs::Bool>(request.MotorName, 1);
  brake_publisher.publish(brake_msg);

  response.Success = true;
  return response.Success;
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "arm_motor_brake_server");
  ros::NodeHandle nh;
  ros::ServiceServer service = nh.advertiseService<rover_common::BrakeMotorRequest,
          rover_common::BrakeMotorResponse>("arm_motor_brake_server", &brake_cb);
  ros::spin();
}
