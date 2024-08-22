#!/usr/bin/python3

import subprocess
import rospy
import signal
import os
import rospkg

http_server_process = None

def start_http_server():
    global http_server_process
    try:
        rospack = rospkg.RosPack()
        target_directory = rospack.get_path('master') + "/.."; 
        os.chdir(target_directory)

        # Run the python -m http.server command as a subprocess
        http_server_process = subprocess.Popen(["python3", "-m", "http.server"])
        # Wait for the subprocess to complete
        http_server_process.wait()
    except KeyboardInterrupt:
        rospy.loginfo("HTTP server terminated by user.")
    except Exception as e:
        rospy.logerr(f"Error starting HTTP server: {e}")

def signal_handler(signum, frame):
    global http_server_process
    if http_server_process is not None:
        rospy.loginfo("Terminating HTTP server.")
        http_server_process.terminate()

if __name__ == "__main__":
    try:
        # Initialize ROS node (if needed)
        rospy.init_node("http_server_node", anonymous=True)

        # Set up a signal handler for Ctrl+C
        signal.signal(signal.SIGINT, signal_handler)

        # Start the HTTP server
        start_http_server()
    except rospy.ROSInterruptException:
        pass
