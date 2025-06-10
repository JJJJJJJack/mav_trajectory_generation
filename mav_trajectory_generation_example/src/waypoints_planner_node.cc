/*
 * Planning trajectory based on waypoint subscribed
 *
 *
 * Launch via
 *   roslaunch mav_trajectory_generation_example waypoints.launch
 *
 * Wait for console to run through all gazebo/rviz messages and then
 * you should see the example below
 *  - After Enter, it receives the current uav position
 *  - After second enter, publishes trajectory information
 *  - After third enter, executes trajectory (sends it to the sampler)
 */

#include  "ros/ros.h"
#include <mav_trajectory_generation_example/waypoints_planner.h>

#include <iostream>

int main(int argc, char** argv) {

  ros::init(argc, argv, "waypoints_planner");

  ros::NodeHandle n;
  WaypointsPlanner planner(n);
  ROS_WARN_STREAM("SLEEPING FOR 5s TO WAIT FOR CLEAR CONSOLE");
  ros::Duration(5.0).sleep();
  ROS_WARN_STREAM("WARNING: CONSOLE INPUT/OUTPUT ONLY FOR DEMONSTRATION!");

  // define set point
  Eigen::Vector3d position, velocity;
  position << 2.0, 0.0, -1.0;
  velocity << 0.0, 0.0, 0.0;

  // THIS SHOULD NORMALLY RUN INSIDE ROS::SPIN!!! JUST FOR DEMO PURPOSES LIKE THIS.
  ROS_WARN_STREAM("PRESS ENTER TO UPDATE CURRENT POSITION AND SEND TRAJECTORY");
  std::cin.get();
  for (int i = 0; i < 10; i++) {
    ros::spinOnce();  // process a few messages in the background - causes the uavPoseCallback to happen
  }
  ROS_WARN_STREAM("Planning start");
  mav_trajectory_generation::Trajectory trajectory;
  planner.planTrajectory(position, velocity, &trajectory);
bool planning_result = planner.publishTrajectory(trajectory);
  ROS_WARN_STREAM("DONE. GOODBYE.");
 std::cerr<<std::endl<<"Planning result"<<planning_result<<std::endl<<std::endl;
 return true;
}