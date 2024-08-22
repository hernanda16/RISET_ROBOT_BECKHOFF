var MainAppRealsense = angular.module('MainAppRealsense', []);

MainAppRealsense.controller('RealsenseController', function($scope, $interval) {
    $scope.ns = "";

    // Accessing the scope as global variable
    window.scope = $scope;

    // Area for pause or resume video stream
    $scope.vision_stream = [true, true, true];

    // Button State
    $scope.GetButtonState = function(index){
        if(!$scope.vision_stream[index]){
            return {text: "Resume", css: "hover:bg-sky-700 bg-sky-500" };
        }else{
            return { text: "Pause", css: "hover:bg-rose-700 bg-rose-500" };
        }
    }    

    // Logic for pausing or resuming stream
    $scope.PauseStartStream = function(index){
        if($scope.vision_stream[index]){
            $scope.vision_stream[index] = false;
            if(index == 0)
                $scope.frame_realsense_rgb = "None";
            if (index == 1)
                $scope.frame_realsense_ball = "None";
            if (index == 2)
                $scope.frame_realsense_obstacle = "None";
        } else{
            $scope.vision_stream[index] = true;
            if(index == 0)
                $scope.frame_realsense_rgb = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision_bgr_rs&type=ros_compressed";
            if (index == 1)
                $scope.frame_realsense_ball = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision_ball_binary_rs&type=ros_compressed";
            if (index == 2)
                $scope.frame_realsense_obstacle = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision_obs_binary_rs&type=ros_compressed";
        }
    }
    console.log("realsense Controller Loaded");
    $scope.frame_realsense_rgb = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision_bgr_rs&type=ros_compressed";
    $scope.frame_realsense_ball = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision_ball_binary_rs&type=ros_compressed";
    $scope.frame_realsense_obstacle = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision_obs_binary_rs&type=ros_compressed";

    //---Threshold Parameters
    //=======================
    $scope.realsense_obs_threshold_params = [
        { label: "H-", value: 0, min: 0, max: 255 },
        { label: "H+", value: 255, min: 0, max: 255 },
        { label: "S-", value: 0, min: 0, max: 255 },
        { label: "S+", value: 255, min: 0, max: 255 },
        { label: "V-", value: 0, min: 0, max: 255 },
        { label: "V+", value: 255, min: 0, max: 255 },
    ];

    $scope.realsense_ball_threshold_params = [
        { label: "H-", value: 0, min: 0, max: 255 },
        { label: "H+", value: 255, min: 0, max: 255 },
        { label: "S-", value: 0, min: 0, max: 255 },
        { label: "S+", value: 255, min: 0, max: 255 },
        { label: "V-", value: 0, min: 0, max: 255 },
        { label: "V+", value: 255, min: 0, max: 255 },
    ];
    
    //============
    //-----Updater
    //============
    $interval(function() {}, 1000);

    // ROS Connection
    $scope.ros = new ROSLIB.Ros({
        url: "ws://" + window.location.hostname + ":9900",
    });

    $scope.simParam = new ROSLIB.Param({
        ros: $scope.ros,
        name: "is_sim",
      });
    
      $scope.simParam.get((value) => {
        if (value == true) {
          $scope.ros.getParams((params) => {
            let robotName = "";
            params.forEach((param) => {
              robotName = param.includes("robot_name")
                ? param.match(/(?<=\/).*(?=\/)/)
                : robotName;
            });
    
            $scope.ns = "/" + robotName;
            console.info(
              "This Robot Run In [SIMULATION] Mode with [ns:" + $scope.ns + "]."
            );
          });
        } else if (value == false) {
          $scope.ns = "";
          console.info("This Robot Run In [HARDWARE] with NO [ns].");
        }
      });   

    // ROS Connection Status
    $scope.ros.on('connection', function() {
        console.log('Connected to websocket server.');
    });

    console.log("ROS Connected");

    //========
    // Topics
    //========
    $scope.realsense_ball = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "rs_vision_ball_thresh",
        messageType: "std_msgs/UInt16MultiArray"
    });
    $scope.realsense_obs = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "rs_vision_obs_thresh",
        messageType: "std_msgs/UInt16MultiArray"
    });


    //=====
    // MSG
    //=====
    $scope.realsense_ball_msg = new ROSLIB.Message({
        layout: { dim: [], data_offset: 0 },
        data: [0, 0, 0, 0, 0, 0],
    });
    $scope.realsense_obs_msg = new ROSLIB.Message({
        layout: { dim: [], data_offset: 0 },
        data: [0, 0, 0, 0, 0, 0],
    });

    //================
    //-----Services
    //================
    $scope.req_hsv = new ROSLIB.ServiceRequest({});

    $scope.ser_realsense_ball = new ROSLIB.Service({
        ros: $scope.ros,
        name: "vision_ball_rs",
        serviceType: "iris_msgs/RealsenseThreshold",
    });

    $scope.ser_realsense_obs = new ROSLIB.Service({
        ros: $scope.ros,
        name: "vision_obs_rs",
        serviceType: "iris_msgs/RealsenseThreshold",
    });

    $scope.ser_realsense_ball.callService($scope.req_hsv, (result) =>  {
        for (var i = 0; i < $scope.realsense_ball_threshold_params.length; i++) {
            $scope.realsense_ball_threshold_params[i].value = result.data[i];
            console.log(result.data[i]);
        }
    });

    $scope.ser_realsense_obs.callService($scope.req_hsv, (result) =>  {
        for (var i = 0; i < $scope.realsense_obs_threshold_params.length; i++) {
            $scope.realsense_obs_threshold_params[i].value = result.data[i];
            console.log(result.data[i]);
        }
    });

    // Publish the threshold parameters
    $scope.PublishBallThresholdParams = function(object){
        for(var i = 0; i < 6; i++)
            $scope.realsense_ball_msg.data[i] = object[i].value ;
        $scope.realsense_ball.publish($scope.realsense_ball_msg);
        console.log("Published realsense Threshold Params");
    }

    // Publish the threshold parameters
    $scope.PublishObsThresholdParams = function(object){
        for(var i = 0; i < 6; i++) $scope.realsense_obs_msg.data[i] = object[i].value ;
        $scope.realsense_obs.publish($scope.realsense_obs_msg);
        console.log("Published realsense Threshold Params");
    }  
});