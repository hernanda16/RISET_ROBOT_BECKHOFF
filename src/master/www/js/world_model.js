var MainAppField = angular.module("MainAppField", []);

MainAppField.controller("FieldController", function ($scope, $interval) {
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
  };

  // Logic for pausing or resuming stream
  $scope.PauseStartStream = function (index) {
    if ($scope.vision_stream[index]) {
      $scope.vision_stream[index] = false;
      if (index == 0) $scope.frame_field_rgb = "None";
      if (index == 1) $scope.frame_field_binary = "None";
    } else {
      $scope.vision_stream[index] = true;
      if (index == 0)
        $scope.frame_field_rgb =
          "http://" +
          window.location.hostname +
          ":9901" +
          "/stream?topic=" +
          $scope.ns +
          "/vision_field_roi&type=ros_compressed";
      else if (index == 1)
        $scope.frame_field_binary =
          "http://" +
          window.location.hostname +
          ":9901" +
          "/stream?topic=" +
          $scope.ns +
          "/vision_testing&type=ros_compressed";
    }
  };
  console.log("field Controller Loaded");
  $scope.frame_field_rgb =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_field_roi&type=ros_compressed";
  $scope.frame_field_binary =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_testing&type=ros_compressed";

  // Threshold Parameters
  $scope.keeper_obs_threshold_param = [
    { label: "Y-", value: 0, min: 0, max: 255 },
    { label: "Y+", value: 255, min: 0, max: 255 },
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
  $interval(function () {}, 1000);

  // ROS Connection
  $scope.ros = new ROSLIB.Ros({
    url: "ws://" + window.location.hostname + ":9900",
  });

  // ROS Connection Status
  $scope.ros.on("connection", function () {
    console.log("Connected to websocket server.");
  });

  console.log("ROS Connected");

  //========
  // Topics
  //========
  $scope.top_field_yuv = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "yuv_keeper_obs_thresh_final",
    messageType: "std_msgs/UInt16MultiArray",
  });

  $scope.top_camera_params = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "camera_params",
    messageType: "std_msgs/UInt16MultiArray",
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

  $scope.ser_field_yuv = new ROSLIB.Service({
    ros: $scope.ros,
    name: "yuv_keeper_obs_ser",
    serviceType: "iris_msgs/yuv_init",
  });

  $scope.ser_field_yuv.callService($scope.req_yuv, (result) => {
    for (var i = 0; i < $scope.keeper_obs_threshold_param.length; i++) {
      $scope.keeper_obs_threshold_param[i].value = result.yuv_threshold[i];
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

  // Publish the threshold parameters
  $scope.PublishThresholdParams = function () {
    var params = [];
    for (var i = 0; i < $scope.keeper_obs_threshold_param.length; i++) {
      params.push($scope.keeper_obs_threshold_param[i].value);
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

  // ------- VISION MAIN STREAM ON --------//
  $scope.pub_vision_main_data_on = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "pub_export_data",
    messageType: "std_msgs/Int16MultiArray",
  });

  $scope.msg_vision_main_data_on = new ROSLIB.Message({
    data: [1, 0],
  });

  $scope.exportClickedMain = function () {
    $scope.pub_vision_main_data_on.publish($scope.msg_vision_main_data_on);
  };

  // --------- VISION ALL STREAM ON ------- //
  $scope.pub_vision_all_data_on = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "pub_export_data",
    messageType: "std_msgs/Int16MultiArray",
  });

  $scope.msg_vision_all_data_on = new ROSLIB.Message({
    data: [0, 1],
  });

  $scope.exportClickedAll = function () {
    $scope.pub_vision_all_data_on.publish($scope.msg_vision_all_data_on);
  };

  // Subscrive from Master
  $scope.sub_toggle_vision = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_toggle",
    messageType: "std_msgs/Int8",
  });

  $scope.sub_toggle_vision.subscribe((msg) => {
    let myToggle = angular.element(document.querySelector("#toggle"));
    if (Number(msg.data) === 1) {
      myToggle.attr("checked", "checked");
      $scope.variablechange = "VISION MAIN ON AND VISION ALL OFF";
    } else {
      myToggle.removeAttr("checked");
      $scope.variablechange = "VISION ALL ON AND VISION MAIN OFF";
    }
  });

  $scope.initToggle = function () {
    $scope.toggleSetter();
  };

  // Toggle Setter
  $scope.toggleSetter = function () {
    let myToggle = angular.element(document.querySelector("#toggle"));
    if ($scope.checkedToggle) {
      myToggle.attr("checked", "checked");
      $scope.variablechange = "VISION MAIN ON AND VISION ALL OFF";
    } else {
      myToggle.removeAttr("checked");
      $scope.variablechange = "VISION ALL ON AND VISION MAIN OFF";
    }
  };

  $scope.toGrapher = function () {
    var field_url =
      "http://" + window.location.hostname + ":9999/master/grapher.html";
    window.location.replace(field_url);
  };
  $scope.toField = function () {
    var field_url =
      "http://" + window.location.hostname + ":9999/master/field.html";
    window.location.replace(field_url);
  };
  $scope.toBall = function () {
    var field_url =
      "http://" + window.location.hostname + ":9999/master/ball.html";
    window.location.replace(field_url);
  };
  $scope.toLine = function () {
    var field_url =
      "http://" + window.location.hostname + ":9999/master/line.html";
    window.location.replace(field_url);
  };
  $scope.toICP = function () {
    var field_url =
      "http://" + window.location.hostname + ":9999/master/lapangan.html";
    window.location.replace(field_url);
  };
  $scope.toExport = function () {
    var field_url =
      "http://" + window.location.hostname + ":9999/master/index3.html";
    window.location.replace(field_url);
  };
  $scope.toGoalKeeper = function () {
    var goalKeeperUrl =
      "http://" + window.location.hostname + ":9999/master/goalkeeper.html";
    window.location.replace(goalKeeperUrl);
  };
  $scope.toVirtualPoints = function () {
    var field_url =
      "http://" + window.location.hostname + ":9999/master/virtual_points.html";
    window.location.replace(field_url);
  };
  $scope.toggleVision = function (e) {
    console.log("i am triggered");
    console.log(e.checked);
    if (e.checked) {
      checkedToggle = 1;
      $scope.msg_vision_main_data_on.data = [1, 0, checkedToggle];
      $scope.exportClickedMain();
      $scope.variablechange = "VISION MAIN ON AND VISION ALL OFF";
    }
    if (!e.checked) {
      checkedToggle = 0;
      $scope.msg_vision_all_data_on.data = [0, 1, checkedToggle];
      $scope.exportClickedAll();
      $scope.variablechange = "VISION ALL ON AND VISION MAIN OFF";
    }
  };
});
