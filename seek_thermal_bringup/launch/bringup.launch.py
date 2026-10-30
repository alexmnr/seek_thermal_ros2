from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch_ros.actions import Node


def launch_setup(context):
    # Load parameters
    log_level = context.launch_configurations["log_level"]
    ns = context.launch_configurations["ns"]
    camera_model = context.launch_configurations["camera_model"]
    ffc_file = context.launch_configurations["ffc_file"]
    frame_id = context.launch_configurations["frame_id"]
    image_topic = context.launch_configurations["image_topic"]
    frame_rate = context.launch_configurations["frame_rate"]
    normalize = context.launch_configurations["normalize"]
    reconnect_interval = context.launch_configurations["reconnect_interval"]

    # Print parameters
    print("")
    print("Starting Seek Thermal driver with parameters:")
    print(" log_level:           " + log_level)
    if ns == "":
        print(" ns:                  " + "/")
    else:
        print(" ns:                  " + "/" + ns)
    print(" camera_model:        " + camera_model)
    if ffc_file != "":
        print(" ffc_file:            " + ffc_file)
    print(" frame_id:            " + frame_id)
    print(" image_topic:         " + image_topic)
    print(" frame_rate:          " + frame_rate)
    print(" normalize:           " + normalize)
    print(" reconnect_interval:  " + reconnect_interval)
    print("")

    nodes = []

    nodes.append(
        Node(
            package="seek_thermal_driver",
            executable="seek_thermal_driver_node",
            namespace=ns,
            parameters=[
                {
                    "camera_model": camera_model,
                    "ffc_file": ffc_file,
                    "frame_id": frame_id,
                    "image_topic": image_topic,
                    "frame_rate": float(frame_rate),
                    "normalize": normalize.lower() in ("true", "1"),
                    "reconnect_interval": float(reconnect_interval),
                }
            ],
            arguments=["--ros-args", "--log-level", log_level],
            output="screen",
        )
    )

    return nodes


def generate_launch_description():
    declared_arguments = [
        DeclareLaunchArgument(
            "log_level",
            default_value="error",
            description="Log Level to use for all nodes",
            choices=["info", "debug", "error", "warn"],
        ),
        DeclareLaunchArgument(
            "ns",
            default_value="",
            description="Namespace for all nodes",
        ),
        DeclareLaunchArgument(
            "camera_model",
            default_value="seek_pro",
            description="Seek Thermal camera model (seek_pro for CompactPRO/PIR324, seek for Compact/CompactXR)",
            choices=["seek_pro", "seek"],
        ),
        DeclareLaunchArgument(
            "ffc_file",
            default_value="",
            description="Optional path to additional flat-field calibration (FFC) image file",
        ),
        DeclareLaunchArgument(
            "frame_id",
            default_value="seek_thermal_frame",
            description="TF frame ID stamped in image header",
        ),
        DeclareLaunchArgument(
            "image_topic",
            default_value="image_raw",
            description="Topic name to publish images on",
        ),
        DeclareLaunchArgument(
            "frame_rate",
            default_value="15.0",
            description="Target publishing frame rate in Hz",
        ),
        DeclareLaunchArgument(
            "normalize",
            default_value="true",
            description="Whether to normalize the 16-bit thermal frame to the full 0-65535 range",
        ),
        DeclareLaunchArgument(
            "reconnect_interval",
            default_value="2.0",
            description="Seconds between reconnect attempts when sensor is disconnected",
        ),
    ]

    return LaunchDescription(declared_arguments + [OpaqueFunction(function=launch_setup)])
