# Seek Thermal ROS 2 Driver (PIR324)

A ROS 2 C++ driver for the Seek Thermal PIR324 (CompactPro) thermal camera using the open-source `libseek-thermal` library.

## 1. System Dependencies
Ensure your system is up to date and install the following:
* **ROS 2** (Humble, Iron, Jazzy, or Rolling)
* **OpenCV**
* **libusb-1.0**
* **cv_bridge**

```bash
sudo apt install build-essential cmake libusb-1.0-0-dev libopencv-dev ros-$ROS_DISTRO-cv-bridge
```

## 2. Install libseek-thermal
You must install the driver library from source before building the ROS 2 package. You can find the repository here: https://github.com/OpenThermal/libseek-thermal

```bash
git clone [https://github.com/OpenThermal/libseek-thermal.git](https://github.com/OpenThermal/libseek-thermal.git)
cd libseek-thermal
mkdir build && cd build
cmake ..
make
sudo make install
sudo ldconfig
```

## 3. USB Permissions (Udev Rules)
To access the camera without root/sudo, create a udev rule:

1. Create the file: /etc/udev/rules.d/99-seekthermal.rules

2. Paste the following:

```
SUBSYSTEM=="usb", ATTRS{idVendor}=="289d", ATTRS{idProduct}=="0010", MODE="0666", GROUP="plugdev"
SUBSYSTEM=="usb", ATTRS{idVendor}=="289d", ATTRS{idProduct}=="0011", MODE="0666", GROUP="plugdev"
```

Reload rules:

```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```

## 4. Build the ROS 2 Package
```bash
cd ~/ros2_ws
colcon build --packages-select seek_thermal_ros2
source install/setup.bash
```

## 5. Usage Examples
**Run with default settings**

```bash
ros2 run seek_thermal_ros2 seek_thermal_driver
```

**Passing the namespace paramater**

```bash
ros2 run seek_thermal_ros2 seek_thermal_driver --ros-args -p ns:=namespace
```
