/*
 * Copyright (C) 2018 Learning Algorithms and Systems Laboratory, EPFL, Switzerland
 * Author:  Mahdi Khoramshahi
 * email:   mahdi.khoramshahi@epfl.ch
 * website: lasa.epfl.ch
 *
 * This work was supported by the European Communitys Horizon 2020 Research and Innovation 
 * programme ICT-23-2014, grant agreement 643950-SecondHands.
 *
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef __CYCLE_MOTION_GENERATOR_H__
#define __CYCLE_MOTION_GENERATOR_H__

#include "ros/ros.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Twist.h"
//#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/PointStamped.h"
#include "geometry_msgs/Quaternion.h"
#include "nav_msgs/Path.h"
#include "std_msgs/Float64MultiArray.h"


#include <vector>

#include "MathLib.h"
#include "CDDynamics.h"

#include <mutex>
#include <pthread.h>

#include <dynamic_reconfigure/server.h>
#include <ds_motion_generator/CYCLE_paramsConfig.h>

class CycleMotionGenerator {


private:

	std::vector<double> Cycle_Target_;

	double Cycle_radius_;
	double Cycle_radius_scale_;

	double Cycle_speed_;
	double Cycle_speed_offset_;

	double Convergence_Rate_;
	double Convergence_Rate_scale_;

	double Velocity_limit_;

	// Filter variables
	std::unique_ptr<CDDynamics> CCDyn_filter_;

	double dt_;
	double Wn_;
	MathLib::Vector accLimits_;
	MathLib::Vector velLimits_;


	// ROS variables
	ros::NodeHandle nh_;
	ros::Rate loop_rate_;

	ros::Subscriber sub_real_pose_;
	ros::Publisher pub_desired_twist_;
	ros::Publisher pub_desired_twist_filtered_;
	ros::Publisher pub_target_;
	ros::Publisher pub_DesiredPath_;
	ros::Publisher pub_desiredOrientation_;
	ros::Subscriber sub_object_state_;

	std::string input_topic_name_;
	std::string output_topic_name_;
	std::string output_filtered_topic_name_;

	geometry_msgs::Pose msg_real_pose_;
	geometry_msgs::Pose msg_desired_pose_;
    geometry_msgs::Twist msg_desired_velocity_;
    geometry_msgs::Twist msg_desired_velocity_filtered_;
	geometry_msgs::Quaternion msg_desired_orientation;

	nav_msgs::Path msg_DesiredPath_;
	int MAX_FRAME = 200;



	//dynamic reconfig settig
	dynamic_reconfigure::Server<ds_motion_generator::CYCLE_paramsConfig> dyn_rec_srv_;
	dynamic_reconfigure::Server<ds_motion_generator::CYCLE_paramsConfig>::CallbackType dyn_rec_f_;


	// Class variables
	std::mutex mutex_;
    pthread_t _thread;
    bool _startThread;

	MathLib::Vector real_pose_;
	MathLib::Vector desired_pose_;
	MathLib::Vector target_pose_;
	MathLib::Vector target_offset_;

	MathLib::Vector desired_velocity_;
	MathLib::Vector desired_velocity_filtered_;

	MathLib::Vector object_position_;
	MathLib::Vector object_speed_;

	bool _firstPosition = false;
	bool _firstObject = false;

	bool _outputVelocity = true;

	double Alpha;


public:
	CycleMotionGenerator(ros::NodeHandle &n,
	                     double frequency,
	                     std::string input_topic_name,
	                     std::string output_topic_name,
	                     std::string output_filtered_topic_name,
	                     std::vector<double> CenterRotation,
	                     double radius,
	                     double RotationSpeed,
	                     double ConvergenceRate);

	bool Init();

	void Run();

	void setDesiredOrientation(geometry_msgs::Quaternion msg); 


private:

	bool InitializeDS();

	bool InitializeROS();

	void UpdateRealPosition(const geometry_msgs::Pose::ConstPtr& msg);
	void UpdateObjectState(const std_msgs::Float64MultiArray::ConstPtr& msg);


	void ComputeDesiredVelocity();

	void PublishDesiredVelocity();

	void PublishFuturePath();


	void DynCallback(ds_motion_generator::CYCLE_paramsConfig &config, uint32_t level);


 	static void* startPathPublishingLoop(void* ptr);

    void pathPublishingLoop();   

    MathLib::Vector getDesiredVelocity(MathLib::Vector pose);

};


#endif
