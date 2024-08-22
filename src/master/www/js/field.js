var MainAppField = angular.module('MainAppField', []);

MainAppField.controller('FieldController', function ($scope, $interval) {
    $scope.ns = "";

    // Accessing the scope as global variable
    window.scope = $scope;

    // Area for pause or resume video stream
    $scope.vision_stream = [true, true];

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
                $scope.frame_field_rgb = "None";
            if (index == 1)
                $scope.frame_field_binary = "None";
            if (index == 2)
                $scope.frame_field_3 = "None";
        } else {
            $scope.vision_stream[index] = true;
            if (index == 0)
                $scope.frame_field_rgb = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/display&type=ros_compressed";
            else if (index == 1)
                $scope.frame_field_binary = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "vision/debug/obs&type=ros_compressed";
            else if (index == 2)
                $scope.frame_field_3 = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/debug/field&type=ros_compressed";
        }
    }
    console.log("field Controller Loaded");
    $scope.frame_field_rgb = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/display&type=ros_compressed";
    $scope.frame_field_binary = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "vision/debug/obs&type=ros_compressed";
    $scope.frame_field_3 = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/debug/field&type=ros_compressed";

    // Threshold Parameters
    $scope.field_threshold_params = [
        { label: "Y-", value: 0, min: 0, max: 180 },
        { label: "Y+", value: 180, min: 0, max: 180 },
        { label: "U-", value: 0, min: 0, max: 255 },
        { label: "U+", value: 255, min: 0, max: 255 },
        { label: "V-", value: 0, min: 0, max: 255 },
        { label: "V+", value: 255, min: 0, max: 255 },
    ];

    // Threshold Parameters
    $scope.camera_params = [
        { label: "Brightness", value: 100, min: 0, max: 255 },
        { label: "Contrast", value: 150, min: 0, max: 255 },
        { label: "Saturation", value: 255, min: 0, max: 255 },
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
    $scope.top_field_yuv = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "/yuv_field_thresh_final",
        messageType: "std_msgs/UInt16MultiArray"
    });

    $scope.top_camera_params = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "camera_params",
        messageType: "std_msgs/UInt16MultiArray"
    });

    //=====
    // MSG
    //=====
    var field_msg = new ROSLIB.Message({
        layout: { dim: [], data_offset: 0 },
        data: [0, 0, 0, 0, 0, 0],
    });

    var camera_params_msg = new ROSLIB.Message({
        layout: { dim: [], data_offset: 0 },
        data: [0, 0, 0],
    });


    //================
    //-----Services
    //================
    $scope.req_yuv = new ROSLIB.ServiceRequest({});
    $scope.req_camera_params = new ROSLIB.ServiceRequest({});
    $scope.req_toggle_datas = new ROSLIB.ServiceRequest({});

    $scope.ser_field_yuv = new ROSLIB.Service({
        ros: $scope.ros,
        name: "yuv_field_thresh_ser",
        serviceType: "iris_msgs/yuv_init",
    });

    $scope.ser_field_yuv.callService($scope.req_yuv, (result) => {
        for (var i = 0; i < $scope.field_threshold_params.length; i++) {
            $scope.field_threshold_params[i].value = result.yuv_threshold[i];
        }
    });

    $scope.ser_camera_params = new ROSLIB.Service({
        ros: $scope.ros,
        name: "camera_params_ser",
        serviceType: "iris_msgs/camera_param_init",
    });

    $scope.ser_camera_params.callService($scope.req_yuv, (result) => {
        for (var i = 0; i < $scope.camera_params.length; i++) {
            $scope.camera_params[i].value = result.data[i];
        }
    });

    $scope.ser_toggle_datas = new ROSLIB.Service({
        ros: $scope.ros,
        name: "vision_toggle",
        serviceType: "iris_msgs/YuvThresholdInit",
    });

    $scope.ser_toggle_datas.callService($scope.req_toggle_datas, (result) => {
        console.log("msgdata filed", result.yuv_threshold[0]);
        let myToggle = angular.element(document.querySelector('#toggle'));
        if (Number(result.yuv_threshold[0]) === 1) {
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "FIELD MANUAL";
        } else {
            myToggle.removeAttr('checked');
            $scope.variablechange = "FIELD AUTO";
        }
    });

    // Publish the threshold parameters
    $scope.PublishThresholdParams = function () {
        var params = [];
        for (var i = 0; i < $scope.field_threshold_params.length; i++) {
            params.push($scope.field_threshold_params[i].value);
        }
        field_msg.data = params;
        $scope.top_field_yuv.publish(field_msg);
    };

    // Publish the camera parameters
    $scope.PublishCameraParams = function () {
        var params = [];
        for (var i = 0; i < $scope.camera_params.length; i++) {
            params.push($scope.camera_params[i].value);
        }
        camera_params_msg.data = params;
        $scope.top_camera_params.publish(camera_params_msg);
    };


    // * Index 0-5 is for field color threshold
    // * Index 6-11 is for ball color threshold
    // * Index 12-17 is for line color threshold
    // * Index 18-23 is for GoalKeeper color threshold
    // * Index 24-25 is for line range scanning

    // ------- VISION MAIN STREAM ON --------//
    $scope.pub_manual_threshold = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "vision_switcher_field",
        messageType: "std_msgs/Int8"
    });

    $scope.msg_manual_threshold = new ROSLIB.Message({
        // [index, message]
        data: 1
    });

    $scope.manualThresh = function () {
        $scope.pub_manual_threshold.publish($scope.msg_manual_threshold);
    };

    // --------- VISION ALL STREAM ON ------- //
    $scope.pub_auto_threshold = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "vision_switcher_field",
        messageType: "std_msgs/Int8"
    });

    $scope.msg_auto_threshold = new ROSLIB.Message({
        data: 0
    });

    $scope.autoThresh = function () {
        $scope.pub_auto_threshold.publish($scope.msg_auto_threshold);
    };


    $scope.initToggle = function () {
        $scope.toggleSetter();
    };

    // Toggle Setter
    $scope.toggleSetter = function () {
        let myToggle = angular.element(document.querySelector('#toggle'));
        if ($scope.checkedToggle) {
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "FIELD MANUAL";
        }
        else {
            myToggle.removeAttr('checked');
            $scope.variablechange = "FIELD AUTO";
        }
    }
    $scope.toggleVision = function (e) {
        console.log("i am triggered");
        console.log(e.checked);
        if (e.checked) {
            checkedToggle = 1;
            $scope.msg_manual_threshold.data = 1;
            $scope.manualThresh();
            $scope.variablechange = "FIELD AUTO";
        }
        if (!e.checked) {
            checkedToggle = 0;
            $scope.msg_auto_threshold.data = 0;
            $scope.autoThresh();
            $scope.variablechange = "FIELD MANUAL";
        }
    }
}
);