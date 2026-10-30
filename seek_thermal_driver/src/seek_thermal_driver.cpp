#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <seek.h>
#include <sensor_msgs/msg/image.hpp>

class SeekThermalDriver : public rclcpp::Node
{
public:
  explicit SeekThermalDriver(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
  : Node("seek_thermal_driver", options)
  {
    // Declare parameters
    this->declare_parameter<std::string>("camera_model", "seek_pro");
    this->declare_parameter<std::string>("ffc_file", "");
    this->declare_parameter<std::string>("frame_id", "seek_thermal_frame");
    this->declare_parameter<std::string>("image_topic", "image_raw");
    this->declare_parameter<double>("frame_rate", 15.0);
    this->declare_parameter<bool>("normalize", true);
    this->declare_parameter<double>("reconnect_interval", 2.0);

    // Retrieve parameters
    camera_model_ = this->get_parameter("camera_model").as_string();
    ffc_file_ = this->get_parameter("ffc_file").as_string();
    frame_id_ = this->get_parameter("frame_id").as_string();
    image_topic_ = this->get_parameter("image_topic").as_string();
    frame_rate_ = this->get_parameter("frame_rate").as_double();
    normalize_ = this->get_parameter("normalize").as_bool();
    reconnect_interval_ = this->get_parameter("reconnect_interval").as_double();

    publisher_ = this->create_publisher<sensor_msgs::msg::Image>(image_topic_, 10);

    init_camera_instance();

    RCLCPP_INFO(
      this->get_logger(),
      "Seek Thermal Driver initialized. Namespace: '%s', topic: '%s', frame_id: '%s', model: '%s'",
      this->get_namespace(), image_topic_.c_str(), frame_id_.c_str(), camera_model_.c_str());

    open_camera();

    double rate = frame_rate_ > 0.0 ? frame_rate_ : 15.0;
    auto period = std::chrono::duration<double>(1.0 / rate);
    timer_ = this->create_wall_timer(
      std::chrono::duration_cast<std::chrono::nanoseconds>(period),
      std::bind(&SeekThermalDriver::capture_frame, this));
  }

  ~SeekThermalDriver() override
  {
    close_camera();
  }

private:
  void init_camera_instance()
  {
    if (camera_model_ == "seek" || camera_model_ == "compact") {
      seek_cam_ = std::make_shared<LibSeek::SeekThermal>(ffc_file_);
    } else {
      seek_cam_ = std::make_shared<LibSeek::SeekThermalPro>(ffc_file_);
    }
  }

  bool open_camera()
  {
    if (!seek_cam_) {
      init_camera_instance();
    }

    if (seek_cam_->isOpened()) {
      return true;
    }

    if (!seek_cam_->open()) {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), *this->get_clock(), 5000,
        "Could not open Seek Thermal camera (%s). Retrying...", camera_model_.c_str());
      last_reconnect_attempt_ = this->now();
      return false;
    }

    RCLCPP_INFO(this->get_logger(), "Successfully connected to Seek Thermal camera (%s).", camera_model_.c_str());
    return true;
  }

  void close_camera()
  {
    if (seek_cam_ && seek_cam_->isOpened()) {
      seek_cam_->close();
    }
  }

  void capture_frame()
  {
    if (!seek_cam_ || !seek_cam_->isOpened()) {
      if ((this->now() - last_reconnect_attempt_).seconds() >= reconnect_interval_) {
        open_camera();
      }
      return;
    }

    cv::Mat frame;
    if (!seek_cam_->grab()) {
      RCLCPP_WARN(
        this->get_logger(),
        "Failed to grab frame from Seek Thermal camera. Resetting connection...");
      close_camera();
      last_reconnect_attempt_ = this->now();
      return;
    }

    seek_cam_->retrieve(frame);

    cv::Mat output_frame;
    if (normalize_) {
      cv::normalize(frame, output_frame, 0, 65535, cv::NORM_MINMAX, CV_16UC1);
    } else {
      output_frame = frame;
    }

    auto msg = cv_bridge::CvImage(std_msgs::msg::Header(), "mono16", output_frame).toImageMsg();
    msg->header.stamp = this->now();
    msg->header.frame_id = frame_id_;

    publisher_->publish(*msg);
  }

  std::string camera_model_;
  std::string ffc_file_;
  std::string frame_id_;
  std::string image_topic_;
  double frame_rate_{15.0};
  bool normalize_{true};
  double reconnect_interval_{2.0};

  rclcpp::Time last_reconnect_attempt_{0, 0, RCL_ROS_TIME};
  std::shared_ptr<LibSeek::SeekCam> seek_cam_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SeekThermalDriver>(rclcpp::NodeOptions()));
  rclcpp::shutdown();
  return 0;
}
