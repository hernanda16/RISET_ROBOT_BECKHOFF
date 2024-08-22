. devel/setup.bash
export AUTORUN=0

# if the first argument is 0 then run without robot
if [ "$1" = "0" ]; then
    echo "Running without robot"
    roslaunch master launcher.launch with_robot:=0
else 
    echo "Running with robot"
    roslaunch master launcher.launch with_robot:=1
fi