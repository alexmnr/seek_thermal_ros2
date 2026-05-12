#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.hpp> 
#include <opencv2/opencv.hpp>
#include <seek.h>

class SeekThermalDriver : public rclcpp::Node {
public:
    // Using NodeOptions allows the node to accept namespaces from launch files
    SeekThermalDriver(const rclcpp::NodeOptions & options = rclcpp::NodeOptions()) 
    : Node("seek_thermal_driver", options) {
        
        // 1. Declare a parameter for the namespace/prefix
        // If you don't provide one, it defaults to "seek_pir324"
        this->declare_parameter<std::string>("ns", "thermal");
        std::string ns = this->get_parameter("ns").as_string();

        // 2. Create the publisher using the parameter as a prefix
        std::string topic_name = ns + "/image_raw";
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>(topic_name, 10);
        
        seek_ptr_ = std::make_shared<LibSeek::SeekThermalPro>();

        if (!seek_ptr_->open()) {
            RCLCPP_ERROR(this->get_logger(), "Could not open Seek Thermal Camera!");
            return;
        }

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(66), std::bind(&SeekThermalDriver::capture_frame, this));
        
        RCLCPP_INFO(this->get_logger(), "Node started with namespace: %s", ns.c_str());
    }

private:
    void capture_frame() {
        cv::Mat frame, fixed_frame;
        if (seek_ptr_->grab()) {
            seek_ptr_->retrieve(frame);
            cv::normalize(frame, fixed_frame, 0, 65535, cv::NORM_MINMAX, CV_16UC1);

            auto msg = cv_bridge::CvImage(std_msgs::msg::Header(), "mono16", fixed_frame).toImageMsg();
            msg->header.stamp = this->now();
            msg->header.frame_id = "thermal_link";
            
            publisher_->publish(*msg);
        }
    }

    std::shared_ptr<LibSeek::SeekThermalPro> seek_ptr_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    // Ensure we pass arguments so the node can see ROS parameters
    rclcpp::spin(std::make_shared<SeekThermalDriver>(rclcpp::NodeOptions()));
    rclcpp::shutdown();
    return 0;
}
