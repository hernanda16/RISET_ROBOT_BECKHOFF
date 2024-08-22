var MainAppLine = angular.module('MainAppLine', []);

MainAppLine.controller('LineController', function ($scope, $interval) {
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
                $scope.frame_line_rgb = "None";
            if (index == 1)
                $scope.frame_line_raw = "None";
            if (index == 2)
                $scope.frame_line_binary = "None";
        } else {
            $scope.vision_stream[index] = true;
            if (index == 0)
                $scope.frame_line_rgb = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/display&type=ros_compressed";
            else if (index == 1)
                $scope.frame_line_raw = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/debug/line&type=ros_compressed";
            else if (index == 2)
                $scope.frame_line_binary = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/debug/line_pcl&type=ros_compressed";
        }
    }
    console.log("line Controller Loaded");
    $scope.frame_line_rgb = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/display&type=ros_compressed";
    $scope.frame_line_binary = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/debug/line_pcl&type=ros_compressed";
    $scope.frame_line_raw = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/vision/debug/line&type=ros_compressed";

    // Threshold Parameters
    $scope.line_threshold_params = [
        { label: "Y-", value: 0, min: 0, max: 255 },
        { label: "Y+", value: 255, min: 0, max: 255 },
        { label: "U-", value: 0, min: 0, max: 255 },
        { label: "U+", value: 255, min: 0, max: 255 },
        { label: "V-", value: 0, min: 0, max: 255 },
        { label: "V+", value: 255, min: 0, max: 255 },
    ];

    $scope.LST_params = [
        { label: "min_rad", value: 0, min: 0, max: 600 },
        { label: "max_rad", value: 0, min: 0, max: 600 },
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
    $scope.top_line_yuv = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "/yuv_line_thresh_final",
        messageType: "std_msgs/UInt16MultiArray"
    });

    $scope.top_line_scan_hsv = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "lst_params",
        messageType: "std_msgs/UInt16MultiArray",
    });

    //=====
    // MSG
    //=====
    var line_msg = new ROSLIB.Message({
        layout: { dim: [], data_offset: 0 },
        data: [0, 0, 0, 0, 0, 0],
    });

    $scope.lst_param_msg = new ROSLIB.Message({
        layout: { dim: [], data_offset: 0 },
        data: [90, 200],
    });

    //================
    //-----Services
    //================
    $scope.req_yuv = new ROSLIB.ServiceRequest({});
    $scope.req_lst = new ROSLIB.ServiceRequest({});

    $scope.ser_line_yuv = new ROSLIB.Service({
        ros: $scope.ros,
        name: "yuv_line_thresh_ser",
        serviceType: "iris_msgs/yuv_init",
    });

    $scope.ser_line_yuv.callService($scope.req_yuv, (result) => {
        for (var i = 0; i < $scope.line_threshold_params.length; i++) {
            $scope.line_threshold_params[i].value = result.yuv_threshold[i];
            // console.log(result.yuv_threshold[i]);
        }
    });

    $scope.ser_lst_params = new ROSLIB.Service({
        ros: $scope.ros,
        name: "lst_params_ser",
        serviceType: "iris_msgs/yuv_init",
    });

    $scope.ser_lst_params.callService($scope.req_lst, (result) => {
        for (var i = 0; i < $scope.LST_params.length; i++) {
            $scope.LST_params[i].value = result.yuv_threshold[i];
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
        console.log("msgdata filed", result.yuv_threshold[2]);
        let myToggle = angular.element(document.querySelector('#toggle'));
        if (Number(result.yuv_threshold[2]) === 1) {
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "LINE MANUAL";
        } else {
            myToggle.removeAttr('checked');
            $scope.variablechange = "LINE AUTO";
        }
    });

    // Publish the threshold parameters
    $scope.PublishThresholdParams = function () {
        var params = [];
        for (var i = 0; i < $scope.line_threshold_params.length; i++) {
            params.push($scope.line_threshold_params[i].value);
        }
        line_msg.data = params;
        $scope.top_line_yuv.publish(line_msg);
    };

    $scope.pub_lstParams = function (object) {
        for (var i = 0; i < 2; i++) $scope.lst_param_msg.data[i] = object[i].value;
        $scope.top_line_scan_hsv.publish($scope.lst_param_msg);
    };

    // ------- MANUAL THREHOLD --------//
    $scope.pub_manual_threshold = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "vision_switcher_line",
        messageType: "std_msgs/Int8"
    });

    $scope.msg_manual_threshold = new ROSLIB.Message({
        data: 1
    });

    $scope.manualThresh = function () {
        $scope.pub_manual_threshold.publish($scope.msg_manual_threshold);
    };

    // --------- LUT THRESHOLD ------- //
    $scope.pub_lut_threshold = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "vision_switcher_line",
        messageType: "std_msgs/Int8"
    });

    $scope.msg_lut_threshold = new ROSLIB.Message({
        data: 0
    });

    $scope.lutThresh = function () {
        $scope.pub_lut_threshold.publish($scope.msg_lut_threshold);
    };

    // Subscrive from Master
    $scope.sub_toggle_vision = new ROSLIB.Topic({
        ros: $scope.ros,
        name: "vision_toggle",
        messageType: "std_msgs/Int8"
    });

    $scope.sub_toggle_vision.subscribe((msg) => {
        let myToggle = angular.element(document.querySelector('#toggle'));
        if (Number(msg.data) === 1) {
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "LINE MANUAL";
        } else {
            myToggle.removeAttr('checked');
            $scope.variablechange = "LINE LUT";
        }
    })

    $scope.initToggle = function () {
        $scope.toggleSetter();
    };

    // Toggle Setter
    $scope.toggleSetter = function () {
        let myToggle = angular.element(document.querySelector('#toggle'));
        if ($scope.checkedToggle) {
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "LINE MANUAL";
        }
        else {
            myToggle.removeAttr('checked');
            $scope.variablechange = "LINE LUT";
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
            $scope.manualThresh();
            $scope.variablechange = "LINE MANUAL";
        }
        if (!e.checked) {
            checkedToggle = 0;
            $scope.msg_lut_threshold.data = 0;
            $scope.lutThresh();
            $scope.variablechange = "LINE LUT";
        }
    }
}
);