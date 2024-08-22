var MainAppBall = angular.module('MainAppBall', []);

MainAppBall.controller('BallController', function ($scope, $interval) {
    $scope.ns = "";

    // Accessing the scope as global variable
    window.scope = $scope;

    // Area for pause or resume video stream
    $scope.vision_stream = [true, true, true];

    // Button State
    $scope.GetButtonState = function (index) {
        if (!$scope.vision_stream[index]) {
            return { text: "Resume", css: "hover:bg-sky-700 bg-sky-500" };
        } else {
            return { text: "Pause", css: "hover:bg-rose-700 bg-rose-500" };
        }
    }

    // Logic for pausing or resuming stream
    $scope.PauseStartStream = function (index) {
        if ($scope.vision_stream[index]) {
            $scope.vision_stream[index] = false;
            if (index == 0)
                $scope.frame_ball_rgb = "None";
            if (index == 1)
                $scope.frame_ball_binary = "None";
            if (index == 2)
                $scope.frame_ball_yuv = "None";
        } else {
            $scope.vision_stream[index] = true;
            if (index == 0)
                $scope.frame_ball_rgb = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/display&type=ros_compressed";
            else if (index == 1)
                $scope.frame_ball_binary = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/debug/ball&type=ros_compressed";
            else if (index == 2)
                $scope.frame_ball_yuv = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/display/yuv&type=ros_compressed";
            
        }
    }
    console.log("Ball Controller Loaded");
    $scope.frame_ball_rgb = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/display/ball&type=ros_compressed";
    $scope.frame_ball_binary = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/debug/ball&type=ros_compressed";
    $scope.frame_ball_yuv = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/display/yuv&type=ros_compressed";

    // Threshold Parameters
    $scope.ball_threshold_params = [
        { label: "Y-", value: 0, min: 0, max: 255 },
        { label: "Y+", value: 255, min: 0, max: 255 },
        { label: "U-", value: 0, min: 0, max: 255 },
        { label: "U+", value: 255, min: 0, max: 255 },
        { label: "V-", value: 0, min: 0, max: 255 },
        { label: "V+", value: 255, min: 0, max: 255 },
    ];

    //============
    //-----Updater
    //============
    $interval(function () { }, 1000);

    // ROS Connection
    $scope.ros = new ROSLIB.Ros({
        url: 'ws://' + window.location.hostname + ':9900'
    });

    // ROS Connection Status
    $scope.ros.on('connection', function () {
        console.log('Connected to websocket server.');
    });

    console.log("ROS Connected");

    //========
    // Topics
    //========
    $scope.top_ball_yuv = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "/yuv_ball_thresh_final",
        messageType: "std_msgs/UInt16MultiArray"
    });

    //=====
    // MSG
    //=====
    var ball_msg = new ROSLIB.Message({
        layout: { dim: [], data_offset: 0 },
        data: [0, 0, 0, 0, 0, 0],
    });

    //================
    //-----Services
    //================
    $scope.req_yuv = new ROSLIB.ServiceRequest({});

    $scope.ser_ball_yuv = new ROSLIB.Service({
        ros: $scope.ros,
        name: "yuv_ball_thresh_ser",
        serviceType: "iris_msgs/yuv_init",
    });

    $scope.ser_ball_yuv.callService($scope.req_yuv, (result) => {
        for (var i = 0; i < $scope.ball_threshold_params.length; i++) {
            $scope.ball_threshold_params[i].value = result.yuv_threshold[i];
            // console.log(result.yuv_threshold[i]);
        }
    });

    $scope.req_toggle_datas = new ROSLIB.ServiceRequest({});

    $scope.ser_toggle_datas = new ROSLIB.Service({
        ros: $scope.ros,
        name: "vision_toggle",
        serviceType: "iris_msgs/YuvThresholdInit",
    });

    $scope.ser_toggle_datas.callService($scope.req_toggle_datas, (result) => {
        console.log("msgdata filed", result.yuv_threshold[1]);
        let myToggle = angular.element(document.querySelector('#toggle'));
        if (Number(result.yuv_threshold[1]) === 1) {
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "BALL MANUAL";
        } else {
            myToggle.removeAttr('checked');
            $scope.variablechange = "BALL AUTO";
        }
    });

    // Publish the threshold parameters
    $scope.PublishThresholdParams = function () {
        var params = [];
        for (var i = 0; i < $scope.ball_threshold_params.length; i++) {
            params.push($scope.ball_threshold_params[i].value);
            // console.log($scope.ball_threshold_params[i].value);
        }
        ball_msg.data = params;
        $scope.top_ball_yuv.publish(ball_msg);

        console.log("Published Ball Threshold Params");
    };

    // ------- VISION MAIN STREAM ON --------//
    $scope.pub_manual_threshold = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "vision_switcher_ball",
        messageType: "std_msgs/Int8"
    });

    $scope.msg_manual_threshold = new ROSLIB.Message({
        data: 1
    });

    $scope.mainThresh = function () {
        $scope.pub_manual_threshold.publish($scope.msg_manual_threshold);
    };

    // --------- VISION ALL STREAM ON ------- //
    $scope.pub_lut_threshold = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "vision_switcher_ball",
        messageType: "std_msgs/Int8"
    });

    $scope.msg_lut_threshold = new ROSLIB.Message({
        data: 0
    });

    $scope.lutThresh = function () {
        $scope.pub_lut_threshold.publish($scope.msg_lut_threshold);
    };

    $scope.initToggle = function () {
        $scope.toggleSetter();
    };

    // Toggle Setter
    $scope.toggleSetter = function () {
        let myToggle = angular.element(document.querySelector('#toggle'));
        if ($scope.checkedToggle) {
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "BALL MANUAL";
        }
        else {
            myToggle.removeAttr('checked');
            $scope.variablechange = "BALL LUT";
        }
    }
    $scope.toGrapher = function () {     
var field_url = "http://" + window.location.hostname + ":9999/master/grapher.html"     
window.location.replace(field_url)   };
$scope.toField = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/field.html"
        window.location.replace(field_url)
    };
    $scope.toBall = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/ball.html"
        window.location.replace(field_url)
    };
    $scope.toLine = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/line.html"
        window.location.replace(field_url)
    };
    $scope.toICP = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/lapangan.html"
        window.location.replace(field_url)
    };
    $scope.toExport = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/index3.html"
        window.location.replace(field_url)
    };
    $scope.toGoalKeeper = function () {
        var goalKeeperUrl = "http://" + window.location.hostname + ":9999/master/goalkeeper.html"
        window.location.replace(goalKeeperUrl)
    }
    $scope.toVirtualPoints = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/virtual_points.html"
        window.location.replace(field_url)
      };
    $scope.toggleVision = function (e) {
        console.log("i am triggered");
        console.log(e.checked);
        if (e.checked) {
            checkedToggle = 1;
            $scope.msg_manual_threshold.data = 1;
            $scope.mainThresh();
            $scope.variablechange = "BALL MANUAL";
        }
        if (!e.checked) {
            checkedToggle = 0;
            $scope.msg_lut_threshold.data = 0;
            $scope.lutThresh();
            $scope.variablechange = "BALL LUT";
        }
    }
}
);