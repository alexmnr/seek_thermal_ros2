# seek_thermal_ros2

ROS 2 packages for interfacing with Seek Thermal cameras (e.g., Seek Thermal CompactPRO / PIR324 and Compact) using the open-source `libseek-thermal` library.

## Packages

- **[`seek_thermal_driver`](seek_thermal_driver/)**: Core C++ driver node that connects to an attached Seek Thermal camera, handles disconnect/reconnect cycles, and publishes thermal images as ROS 2 `sensor_msgs/msg/Image` (`mono16`) topics.
- **[`seek_thermal_bringup`](seek_thermal_bringup/)**: Launch package providing [`bringup.launch.py`](seek_thermal_bringup/launch/bringup.launch.py) for starting and configuring the driver with integrated namespacing and parameters.

## Prerequisites

### 1. System Dependencies

Ensure your system is up to date and install the following:
* **ROS 2** (Humble, Iron, Jazzy, or Rolling)
* **OpenCV**
* **libusb-1.0**
* **cv_bridge**

```bash
sudo apt install build-essential cmake libusb-1.0-0-dev libopencv-dev ros-$ROS_DISTRO-cv-bridge
```

### 2. Install `libseek-thermal`

Install the `libseek-thermal` driver library from source before building the ROS 2 packages (`https://github.com/OpenThermal/libseek-thermal`):

```bash
git clone https://github.com/OpenThermal/libseek-thermal.git
cd libseek-thermal
mkdir build && cd build
cmake ..
make
sudo make install
sudo ldconfig
```

### 3. USB Permissions (Udev Rules)

To access the camera without root/sudo, create `/etc/udev/rules.d/99-seekthermal.rules` with:

```text
SUBSYSTEM=="usb", ATTRS{idVendor}=="289d", ATTRS{idProduct}=="0010", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="289d", ATTRS{idProduct}=="0011", MODE="0666", GROUP="plugdev"
```

Reload rules:

```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```

## Building

From your ROS 2 workspace root:

```bash
cd ~/ros2_ws
colcon build --packages-select seek_thermal_driver seek_thermal_bringup
source install/setup.bash
```

## Usage

### Quick Start

Launch the driver with default settings (published on `image_raw`):

```bash
ros2 launch seek_thermal_bringup bringup.launch.py
```

### Common Configurations

- **Launch with a namespace:**
  ```bash
  ros2 launch seek_thermal_bringup bringup.launch.py ns:=thermal
  ```

- **Stream unnormalized raw 16-bit sensor values:**
  ```bash
  ros2 launch seek_thermal_bringup bringup.launch.py normalize:=false
  ```

- **Specify camera model or custom topic:**
  ```bash
  ros2 launch seek_thermal_bringup bringup.launch.py camera_model:=seek_pro image_topic:=image_raw
  ```

### Launch Arguments

| Argument | Default | Description |
|---|---|---|
| `ns` | `""` | Namespace for nodes and topics |
| `camera_model` | `"seek_pro"` | Camera model (`seek_pro` for CompactPRO/PIR324, `seek` for Compact/CompactXR) |
| `ffc_file` | `""` | Optional path to flat-field calibration (FFC) image file |
| `image_topic` | `"image_raw"` | ROS 2 Image topic name |
| `frame_id` | `"seek_thermal_frame"` | TF frame ID in message header |
| `frame_rate` | `"15.0"` | Target publishing frame rate in Hz |
| `normalize` | `"true"` | Normalize 16-bit thermal frame to `0`–`65535` range |
| `reconnect_interval` | `"2.0"` | Interval in seconds between reconnect attempts when disconnected |
| `log_level` | `"error"` | Logging level (`info`, `debug`, `warn`, `error`) |

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
