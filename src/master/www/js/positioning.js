var MainAppPositioning = angular.module('MainAppPositioning', []);

MainAppPositioning.controller('PositioningController', function ($scope, $interval) {
    $scope.ns = "";

    // Accessing the scope as global variable
    window.scope = $scope;

    // Area for pause or resume video stream
    $scope.vision_stream = [true, true, true, true];

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
                $scope.field_layer_1 = "None";
            if (index == 1)
                $scope.field_layer_2 = "None";
            if (index == 2)
                $scope.field_layer_3 = "None";
            if (index == 3)
                $scope.field_layer_total = "None";
        } else {
            $scope.vision_stream[index] = true;
            if (index == 0)
                $scope.field_layer_1 = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/world_model/positioning/field_layer_1&type=ros_compressed";
            else if (index == 1)
                $scope.field_layer_2 = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/world_model/positioning/field_layer_2&type=ros_compressed";
            else if (index == 2)
                $scope.field_layer_3 = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/world_model/positioning/field_layer_3&type=ros_compressed";
            else if (index == 3)
                $scope.field_layer_total = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/world_model/positioning/field_layer_total&type=ros_compressed";
        }
    }
    console.log("positioning Controller Loaded");
    $scope.field_layer_1 = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/world_model/positioning/field_layer_1&type=ros_compressed";
    $scope.field_layer_2 = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/world_model/positioning/field_layer_2&type=ros_compressed";
    $scope.field_layer_3 = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/world_model/positioning/field_layer_3&type=ros_compressed";
    $scope.field_layer_total = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/world_model/positioning/field_layer_total&type=ros_compressed";

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

    $scope.toGrapher = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/grapher.html"
        window.location.replace(field_url)
    };
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
    };
    $scope.toVirtualPoints = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/virtual_points.html"
        window.location.replace(field_url)
    };
    $scope.toPositioning = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/positioning.html"
        window.location.replace(field_url)
    };


});