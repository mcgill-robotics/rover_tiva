cmake_minimum_required(VERSION 2.8.3)
project(rover_tiva)

find_package(catkin REQUIRED COMPONENTS
  message_generation
  rosserial_client
  rosserial_tivac
  std_msgs
  roscpp
  rover_common
  genmsg
)

add_service_files(
  DIRECTORY srv
  FILES ResetMotor.srv BrakeMotor.srv)


rosserial_generate_ros_lib(
  PACKAGE rosserial_tivac
  SCRIPT make_libraries_tiva
)


generate_messages(DEPENDENCIES std_msgs)

catkin_package()

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(reset_motor_service reset_motor_service.cpp)
target_link_libraries(reset_motor_service ${catkin_LIBRARIES})
add_dependencies(reset_motor_service rover_tiva_generate_messages_cpp)

add_executable(brake_motor_service brake_motor_service.cpp)
target_link_libraries(brake_motor_service ${catkin_LIBRARIES})
add_dependencies(brake_motor_service rover_tiva_generate_messages_cpp)



rosserial_configure_client(
  DIRECTORY arm_shoulder
  TOOLCHAIN_FILE ${ROSSERIAL_TIVAC_TOOLCHAIN}
)

rosserial_add_client_target(arm_shoulder arm_shoulder.axf ALL)
rosserial_add_client_target(arm_shoulder flash)
rosserial_add_client_target(arm_shoulder size)
rosserial_add_client_target(arm_shoulder dump)

rosserial_configure_client(
  DIRECTORY arm_elbow
  TOOLCHAIN_FILE ${ROSSERIAL_TIVAC_TOOLCHAIN}
)

rosserial_add_client_target(arm_elbow arm_elbow.axf ALL)
rosserial_add_client_target(arm_elbow flash)
rosserial_add_client_target(arm_elbow size)
rosserial_add_client_target(arm_elbow dump)

rosserial_configure_client(
  DIRECTORY arm_wrist
  TOOLCHAIN_FILE ${ROSSERIAL_TIVAC_TOOLCHAIN}
)

rosserial_add_client_target(arm_wrist arm_wrist.axf ALL)
rosserial_add_client_target(arm_wrist flash)
rosserial_add_client_target(arm_wrist size)
rosserial_add_client_target(arm_wrist dump)

rosserial_configure_client(
  DIRECTORY sampling
  TOOLCHAIN_FILE ${ROSSERIAL_TIVAC_TOOLCHAIN}
)

rosserial_add_client_target(sampling sampling.axf ALL)
rosserial_add_client_target(sampling flash)
rosserial_add_client_target(sampling size)
rosserial_add_client_target(sampling dump)

# Middle wheel encoder firmware.
rosserial_configure_client(
  DIRECTORY drive_middle
  TOOLCHAIN_FILE ${ROSSERIAL_TIVAC_TOOLCHAIN}
)

rosserial_add_client_target(drive_middle drive_middle.axf ALL)
rosserial_add_client_target(drive_middle flash)
rosserial_add_client_target(drive_middle size)
rosserial_add_client_target(drive_middle dump)

# rosserial_add_client_target(drive_left drive_left.axf ALL)
# rosserial_add_client_target(drive_left flash)
# rosserial_add_client_target(drive_left size)
# rosserial_add_client_target(drive_left dump)
# rosserial_add_client_target(drive_right drive_right.axf ALL)
# rosserial_add_client_target(drive_right flash)
# rosserial_add_client_target(drive_right size)
# rosserial_add_client_target(drive_right dump)
# rosserial_add_client_target(sampling sampling.axf ALL)
# rosserial_add_client_target(sampling flash)
# rosserial_add_client_target(sampling size)
# rosserial_add_client_target(sampling dump)
# rosserial_add_client_target(power power.axf ALL)
# rosserial_add_client_target(power flash)
# rosserial_add_client_target(power size)
# rosserial_add_client_target(power dump)
