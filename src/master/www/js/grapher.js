var mainApp = angular.module("mainApp", []);

mainApp.controller("mainAppController", function ($scope, $interval) {
  $scope.ns = "";

  // Agar scope dapat diakses sebagai global variabel
  window.$scope = $scope;

  $scope.menu_activeIndex = 1;

  $scope.menu_buttonState = function (index) {
    if (index == $scope.menu_activeIndex) return "btn-success";
    else return "btn-outline-success";
  };

  $scope.menu_contentState = function (index) {
    if (index == $scope.menu_activeIndex) return true;
    else return false;
  };

  $scope.changeActiveIndex = function (index) {
    $scope.menu_activeIndex = index;
  };


  //================================================


  // label params
  $scope.line_thresholdParameter = [
    { label: "H-", value: 0, min: 0, max: 180 },
    { label: "H+", value: 180, min: 0, max: 180 },
    { label: "L-", value: 0, min: 0, max: 255 },
    { label: "L+", value: 255, min: 0, max: 255 },
    { label: "S-", value: 0, min: 0, max: 255 },
    { label: "S+", value: 255, min: 0, max: 255 }
  ];

  // Simple Timer
  $interval(function () {
  }, 1000);

  // ROS
  $scope.ros = new ROSLIB.Ros({
    url: "ws://" + window.location.hostname + ":9900"
  });

  // ------- VISION MAIN STREAM ON --------//
  $scope.pub_vision_main_data_on = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "pub_export_data",
    messageType: "std_msgs/Int16MultiArray"
  });

  $scope.msg_vision_main_data_on = new ROSLIB.Message({
    data: [1, 0]
  });

  $scope.exportClickedMain = function () {
    $scope.pub_vision_main_data_on.publish($scope.msg_vision_main_data_on);
  };

  // --------- VISION ALL STREAM ON ------- //
  $scope.pub_vision_all_data_on = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "pub_export_data",
    messageType: "std_msgs/Int16MultiArray"
  });

  $scope.msg_vision_all_data_on = new ROSLIB.Message({
    data: [0, 1]
  });

  $scope.exportClickedAll = function () {
    $scope.pub_vision_all_data_on.publish($scope.msg_vision_all_data_on);
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
      $scope.variablechange = "VISION MAIN ON AND VISION ALL OFF";
    } else {
      myToggle.removeAttr('checked');
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
      $scope.variablechange = "VISION MAIN ON AND VISION ALL OFF";
    }
    else {
      myToggle.removeAttr('checked');
      $scope.variablechange = "VISION ALL ON AND VISION MAIN OFF";
    }
  }

  // Subscribe to frame
  $scope.frame_field = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/potential_debugger&type=ros_compressed";
  $scope.frame_potential_2d = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns + "/grapher_2d";

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
  $scope.toPotential = function () {
    var field_url = "http://" + window.location.hostname + ":9999/master/potential.html"
    window.location.replace(field_url)
  };
  $scope.toExport = function () {
    var field_url = "http://" + window.location.hostname + ":9999/master/index3.html"
    window.location.replace(field_url)
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
  }

  // Publisher
  var startGrapher = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "potential/start_grapher",
    messageType: "std_msgs/UInt8"
  });

  $scope.checkedToggle = 0;

  // if the web is loaded, publish the message
  $scope.ros.on('connection', function () {
    console.log('Connected to websocket server.');
    $scope.checkedToggle = 1;
    var msg = new ROSLIB.Message({
      data: $scope.checkedToggle
    });
    startGrapher.publish(msg);
  });


  $scope.toggleFrame = function (e) {
    if ($scope.checkedToggle === 0) {
      $scope.checkedToggle = 1;
    } else {
      $scope.checkedToggle = 0;
    }
    var msg = new ROSLIB.Message({
      data: $scope.checkedToggle
    });
    startGrapher.publish(msg);
    console.log($scope.checkedToggle);
  };

  // if the web is closed, publish the message
  $scope.ros.on('close', function () {
    console.log('Connection to websocket server closed.');
    var msg = new ROSLIB.Message({
      data: 0
    });
    startGrapher.publish(msg);
  });

  // if there is an error on the backend, an 'error' emit will be emitted.
  $scope.ros.on('error', function (error) {
    console.log('Error connecting to websocket server: ', error);
  });

  // Subscriber to passing points
  $scope.sub_passing_point = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "passing_points",
    messageType: "geometry_msgs/Point32"
  });

  $scope.sub_passing_point.subscribe((msg) => {
    $scope.passing_points = msg;
    $scope.$apply(); 
    if (msg.x === 0 && msg.y === 0) {
      document.getElementById("passing-points").innerHTML = "No passing points";
    } else {
    document.getElementById("passing-points").innerHTML = "x: " + msg.x + " y: " + msg.y;
    }
  });

  // robot pose
  $scope.sub_robot_pose = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "pose_robot",
    messageType: "geometry_msgs/Pose2D"
  });

  $scope.sub_robot_pose.subscribe((msg) => {
    $scope.robot_pose = msg;
    $scope.$apply();
    document.getElementById("robot-pose").innerHTML = "x: " + msg.x + " y: " + msg.y + " theta: " + msg.theta;
  });

  // attacker and assist pose
  $scope.sub_friend_pose = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "friends_pose",
    messageType: "iris_msgs/FriendPose"
  });

  $scope.sub_friend_pose.subscribe((msg) => {
    $scope.friend_pose = msg;
    $scope.$apply();
    document.getElementById("attacker-pose").innerHTML = "x: " + msg.att_pos[0]+ " y: " + msg.att_pos[1];
    document.getElementById("assist-pose").innerHTML = "x: " + msg.assist_pos[0] + " y: " + msg.assist_pos[1];
  });

  // robot role
  $scope.sub_robot_role = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "robot_role",
    messageType: "std_msgs/UInt8"
  });

  $scope.sub_robot_role.subscribe((msg) => {
    $scope.robot_role = msg;
    $scope.$apply();
    if (msg.data === 0) {
      document.getElementById("robot-role").innerHTML = "GK";
    } else if (msg.data === 1) {
      document.getElementById("robot-role").innerHTML = "ATT";
    } else if (msg.data === 2) {
      document.getElementById("robot-role").innerHTML = "DEF";
    } else if (msg.data === 3) {
      document.getElementById("robot-role").innerHTML = "ASSIST";
    } else {
      document.getElementById("robot-role").innerHTML = "DEF 2";
    }
  });

});