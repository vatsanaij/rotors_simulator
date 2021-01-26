/*
 * Copyright 2016 Pavel Vechersky, ASL, ETH Zurich, Switzerland
 * Copyright 2016 Geoffrey Hunter <gbmhunter@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROTORS_GAZEBO_PLUGINS_GPS_PLUGIN_H
#define ROTORS_GAZEBO_PLUGINS_GPS_PLUGIN_H

// SYSTEM
#include <random>

// 3RD PARTY
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/sensors/sensors.hh>

// USER
#include "NavSatFix.pb.h"     // GPS message type generated by protobuf .proto file
#include "TwistStamped.pb.h"  // GPS ground speed message
#include "SITLGps.pb.h"     // GPS message from PX4/sitl_gazebo
#include "rotors_gazebo_plugins/common.h"

namespace gazebo {

// Default values
static constexpr double kDefaultHorPosStdDev = 3.0;
static constexpr double kDefaultVerPosStdDev = 6.0;
static constexpr double kDefaultHorVelStdDev = 0.1;
static constexpr double kDefaultVerVelStdDev = 0.1;
static constexpr double kDefaultUpdateItv = 0.2;
static constexpr double kDefaultDelay = 0.12;
static constexpr bool kDefaultEnableDelay = false;
//static constexpr bool kDefaultRqstFwdToRos = false;

class GazeboGpsPlugin : public SensorPlugin {
 public:
  typedef std::normal_distribution<> NormalDistribution;

  GazeboGpsPlugin();
  virtual ~GazeboGpsPlugin();

 protected:
  void Load(sensors::SensorPtr _sensor, sdf::ElementPtr _sdf);

  /// \brief    Publishes both a NavSatFix and Gazebo message.
  void OnUpdate();

 private:

  std::string namespace_;

  /// \brief    Flag that is set to true once CreatePubsAndSubs() is called, used
  ///           to prevent CreatePubsAndSubs() from be called on every OnUpdate().
  bool pubs_and_subs_created_;
  //bool rqst_fwd_to_ros;

  /// \brief    Creates all required publishers and subscribers, incl. routing of messages to/from ROS if required.
  /// \details  Call this once the first time OnUpdate() is called (can't
  ///           be called from Load() because there is no guarantee GazeboRosInterfacePlugin has
  ///           has loaded and listening to ConnectGazeboToRosTopic and ConnectRosToGazeboTopic messages).
  void CreatePubsAndSubs();

  gazebo::transport::NodePtr node_handle_;

  gazebo::transport::PublisherPtr gz_gps_pub_;

  gazebo::transport::PublisherPtr gz_ground_speed_pub_;

  gazebo::transport::PublisherPtr gz_gps_hil_pub_;

  /// \brief    Name of topic for GPS messages, read from SDF file.
  std::string gps_topic_;

  /// \brief    Name of topic for ground speed messages, read from SDF file.
  std::string ground_speed_topic_;

  /// \brief    Pointer to the parent sensor
  sensors::GpsSensorPtr parent_sensor_;

  /// \biref    Pointer to the world.
  physics::WorldPtr world_;

  /// \brief    Pointer to the sensor link.
  physics::LinkPtr link_;

  /// \brief    Pointer to the update event connection.
  event::ConnectionPtr updateConnection_;

  /// \brief    GPS message to be published on sensor update.
  gz_sensor_msgs::NavSatFix gz_gps_message_;

  /// \brief    Ground speed message to be published on sensor update.
  gz_geometry_msgs::TwistStamped gz_ground_speed_message_;

  /// \brief    GPS message from PX4/sitl_gazebo.
  sensor_msgs::msgs::SITLGps gz_gps_hil_message_;

  /// \brief    GPS noise parameters
  double std_xy;    // meters
  double std_z;     // meters

  /// \brief    GPS delay related
  double gps_update_interval_ = 0.2; // 5hz
  double gps_delay = 0.12;           // 120 ms
  static constexpr int gps_buffer_size_max = 1000;
  std::queue<sensor_msgs::msgs::SITLGps> gps_delay_buffer;
  common::Time last_gps_time_;
  bool enable_delay;

  /// \brief    Normal distributions for ground speed noise in x, y, and z directions.
  NormalDistribution ground_speed_n_[3];

  /// \brief    Random number generator.
  std::random_device random_device_;

  std::mt19937 random_generator_;
};

} // namespace gazebo 

#endif // ROTORS_GAZEBO_PLUGINS_GPS_PLUGIN_H
