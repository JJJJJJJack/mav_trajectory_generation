#include <mav_trajectory_generation_example/waypoints_planner.h>

WaypointsPlanner::WaypointsPlanner(ros::NodeHandle& nh) :

    nh_(nh),
    max_v_(2.0),
    max_a_(2.0),
    current_velocity_(Eigen::Vector3d::Zero()),
    current_pose_(Eigen::Affine3d::Identity()) {
      
  // Load params
  if (!nh_.getParam(ros::this_node::getName() + "/max_v", max_v_)){
    ROS_WARN("[waypoints_planner] param max_v not found");
  }
  if (!nh_.getParam(ros::this_node::getName() + "/max_a", max_a_)){
    ROS_WARN("[waypoints_planner] param max_a not found");
  }

  // create publisher for RVIZ markers
  pub_markers_ =
      nh.advertise<visualization_msgs::MarkerArray>("trajectory_markers", 0);

  pub_trajectory_ =
      nh.advertise<mav_planning_msgs::PolynomialTrajectory>("trajectory",
                                                              0);

  // subscriber for Odometry
  sub_odom_ =
      nh.subscribe("uav_pose", 1, &WaypointsPlanner::uavOdomCallback, this);
}

// Callback to get current Pose of UAV
void WaypointsPlanner::uavOdomCallback(const nav_msgs::Odometry::ConstPtr& odom) {

  // store current position in our planner
  tf::poseMsgToEigen(odom->pose.pose, current_pose_);

  // store current vleocity
  tf::vectorMsgToEigen(odom->twist.twist.linear, current_velocity_);
}

// Method to set maximum speed.
void WaypointsPlanner::setMaxSpeed(const double max_v) {
  max_v_ = max_v;
}

// Plans a trajectory from the current position to the a goal position and velocity
// we neglect attitude here for simplicity
bool WaypointsPlanner::planTrajectory(const Eigen::VectorXd& goal_pos,
                                    const Eigen::VectorXd& goal_vel,
                                    mav_trajectory_generation::Trajectory* trajectory) {


  // 3 Dimensional trajectory => through carteisan space, no orientation
  const int dimension = 3;

  // Array for all waypoints and their constrains
  mav_trajectory_generation::Vertex::Vector vertices;

  // Optimze up to 4th order derivative (SNAP)
  const int derivative_to_optimize =
      mav_trajectory_generation::derivative_order::SNAP;
  // we have 3 vertices:
  // Start = current position
  // end = desired position and velocity
  mav_trajectory_generation::Vertex start(dimension), middle(dimension),middle1(dimension),middle2(dimension),middle3(dimension),middle4(dimension),end(dimension);


  /******* Configure start point *******/
  // set start point constraints to current position and set all derivatives to zero
  Eigen::Vector3d start_pos;
  start_pos << 0, 0, -1;
  start.makeStartOrEnd(start_pos,
                       derivative_to_optimize);

  // set start point's velocity to be constrained to current velocity
  start.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
                      current_velocity_);

  // add waypoint to list
  vertices.push_back(start);


  /******* Configure middle point *******/

  // Powerloop
  // Eigen::Vector3d middle_position(1,0,-2.0);//2.0 to 1.87
  // Eigen::Vector3d middle_velocity(-2,0,0);
  // Eigen::Vector3d middle_acceleration(0,0,12);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION,
  //                     middle_position);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
  //                       middle_velocity);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::ACCELERATION,
  //                     middle_acceleration);
  // vertices.push_back(middle);

  // Multiloop
  // Eigen::Vector3d middle_position(1,0,-2.0);//2.0 to 1.87
  // Eigen::Vector3d middle_velocity(-2,0,0);
  // Eigen::Vector3d middle_acceleration(0,0,12);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION,
  //                     middle_position);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
  //                       middle_velocity);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::ACCELERATION,
  //                     middle_acceleration);
  // vertices.push_back(middle);
  // Eigen::Vector3d middle1_position(1,0,-1.15); // bottom
  // Eigen::Vector3d middle1_velocity(2,0,0);
  // Eigen::Vector3d middle1_acceleration(0,0,-3);
  // middle1.addConstraint(mav_trajectory_generation::derivative_order::POSITION,
  //                     middle1_position);
  // // middle1.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
  // //                       middle1_velocity);
  // middle1.addConstraint(mav_trajectory_generation::derivative_order::ACCELERATION,
  //                     middle1_acceleration);
  // // loop 2
  // vertices.push_back(middle1); // bottom
  // vertices.push_back(middle); // top
  // // loop 3
  // vertices.push_back(middle1); // bottom
  // vertices.push_back(middle); // top
  // // loop 4
  // vertices.push_back(middle1); // bottom
  // vertices.push_back(middle); // top

  // OneGate
  // Eigen::Vector3d middle_position(1,1,-2.0);
  // Eigen::Vector3d middle_acceleration(0,-6,9.8);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION,
  //                     middle_position);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::ACCELERATION,
  //                     middle_acceleration);
  // vertices.push_back(middle);

  // TestHeightPerformance
  Eigen::Vector3d middle_position(0,0,-2.0);
  Eigen::Vector3d middle_velocity(0,0,0);
  Eigen::Vector3d middle_acceleration(0,0,0);
  middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION,
                      middle_position);
  middle.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
                        middle_velocity);
  middle.addConstraint(mav_trajectory_generation::derivative_order::ACCELERATION,
                      middle_acceleration);
  vertices.push_back(middle);
  Eigen::Vector3d middle1_position(0,0,-1.0);
  Eigen::Vector3d middle1_velocity(0,0,0);
  Eigen::Vector3d middle1_acceleration(0,0,0);
  middle1.addConstraint(mav_trajectory_generation::derivative_order::POSITION,
                      middle1_position);
  middle1.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
                        middle1_velocity);
  middle.addConstraint(mav_trajectory_generation::derivative_order::ACCELERATION,
                      middle1_acceleration);
  vertices.push_back(middle1);
  vertices.push_back(middle);
  vertices.push_back(middle1);
  vertices.push_back(middle);
  vertices.push_back(middle1);


  // TestHorizontalPerformance
  // Eigen::Vector3d middle_position(2,0,-1);
  // Eigen::Vector3d middle_velocity(0,-2,0);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::POSITION,
  //                     middle_position);
  // middle.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
  //                       middle_velocity);
  // vertices.push_back(middle);
  // Eigen::Vector3d middle1_position(0,0,-1.0);
  // Eigen::Vector3d middle1_velocity(0,2,0);
  // middle1.addConstraint(mav_trajectory_generation::derivative_order::POSITION,
  //                     middle1_position);
  // middle1.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
  //                       middle1_velocity);
  // vertices.push_back(middle1);
  // vertices.push_back(middle);
  // vertices.push_back(middle1);
  // vertices.push_back(middle);
  // vertices.push_back(middle1);
  // vertices.push_back(middle);
  // vertices.push_back(middle1);


  /******* Configure end point *******/
  // set end point constraints to desired position and set all derivatives to zero
  end.makeStartOrEnd(goal_pos,
                     derivative_to_optimize);

  // set start point's velocity to be constrained to current velocity
  end.addConstraint(mav_trajectory_generation::derivative_order::VELOCITY,
                    goal_vel);
  // add waypoint to list
  vertices.push_back(end);

  // setimate initial segment times
  std::vector<double> segment_times;
  segment_times = estimateSegmentTimes(vertices, max_v_, max_a_);

  // Set up polynomial solver with default params
  mav_trajectory_generation::NonlinearOptimizationParameters parameters;

  // set up optimization problem
  const int N = 10;
  mav_trajectory_generation::PolynomialOptimizationNonLinear<N> opt(dimension, parameters);
  opt.setupFromVertices(vertices, segment_times, derivative_to_optimize);

  // constrain velocity and acceleration
  opt.addMaximumMagnitudeConstraint(mav_trajectory_generation::derivative_order::VELOCITY, max_v_);
  opt.addMaximumMagnitudeConstraint(mav_trajectory_generation::derivative_order::ACCELERATION, max_a_);

  // solve trajectory
  opt.optimize();

  // get trajectory as polynomial parameters
  opt.getTrajectory(&(*trajectory));
  
  return true;
}

bool WaypointsPlanner::publishTrajectory(const mav_trajectory_generation::Trajectory& trajectory){
  // send trajectory as markers to display them in RVIZ
  visualization_msgs::MarkerArray markers;
  double distance =
      0.2; // Distance by which to seperate additional markers. Set 0.0 to disable.
  std::string frame_id = "world";

  mav_trajectory_generation::drawMavTrajectory(trajectory,
                                               distance,
                                               frame_id,
                                               &markers);
  pub_markers_.publish(markers);

  // send trajectory to be executed on UAV
  mav_planning_msgs::PolynomialTrajectory msg;
  mav_trajectory_generation::trajectoryToPolynomialTrajectoryMsg(trajectory,
                                                                 &msg);
  msg.header.frame_id = "world";
  ROS_INFO_STREAM(msg);
  pub_trajectory_.publish(msg);


  return true;
}  

