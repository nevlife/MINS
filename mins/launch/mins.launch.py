"""ros2 launch mins mins.launch.py config:=scv [transport:=flux]

The node is named mins, the name its stage in CAPA_SPEC uses.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    return LaunchDescription(
        [
            DeclareLaunchArgument("config"),
            DeclareLaunchArgument("transport", default_value="dds", choices=["dds", "flux"]),
            Node(
                package="mins",
                executable="subscribe",
                name="mins",
                output="screen",
                parameters=[
                    {
                        "config_path": PathJoinSubstitution(
                            [FindPackageShare("mins"), "config", LaunchConfiguration("config"), "config.yaml"]
                        ),
                        "transport": LaunchConfiguration("transport"),
                    }
                ],
            ),
        ]
    )
