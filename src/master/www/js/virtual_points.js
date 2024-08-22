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
  $scope.virtual_points_params = [
    { label: "goal k thresh", value: 0, min: 0, max: 255 },
    { label: "friend k thresh", value: 0, min: 0, max: 255 },
    { label: "obs rad k thresh", value: 0, min: 0, max: 255 },
    { label: "obs rad k thresh", value: 0, min: 0, max: 255 },
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


  // SERVICESSSSSS
  $scope.req_k_passing = new ROSLIB.ServiceRequest({});

  $scope.ser_k_passing = new ROSLIB.Service({
      ros: $scope.ros,
      name: "k_passing_ser",
      serviceType: "iris_msgs/yuv_init",
  });

  $scope.ser_k_passing.callService($scope.req_k_passing, (result) => {
      for (var i = 0; i < $scope.virtual_points_params.length; i++) {
          $scope.virtual_points_params[i].value = result.yuv_threshold[i];
          // console.log(result.yuv_threshold[i]);
      }
  });


  // --------- VISION ALL STREAM ON ------- //
  $scope.pub_vision_all_data_on = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "pub_export_data",
    messageType: "std_msgs/Int16MultiArray"
  });

  $scope.pub_virtual_points_params = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "k_passing",
    messageType: "std_msgs/UInt16MultiArray"
    });

    var virtual_points_msg = new ROSLIB.Message({
        data: [0, 0, 0, 0]
    });

  $scope.msg_vision_all_data_on = new ROSLIB.Message({
    data: [0, 1]
  });

  $scope.exportClickedAll = function () {
    $scope.pub_vision_all_data_on.publish($scope.msg_vision_all_data_on);
  };

  // Subscribe from Master
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
  $scope.frame_field = "http://" + window.location.hostname + ":9901" + "/stream?topic=" + $scope.ns +
   "/passing_point&type=ros_compressed";

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
  $scope.toVirtualPoints = function () {
    var field_url = "http://" + window.location.hostname + ":9999/master/virtual_points.html"
    window.location.replace(field_url)
  };
  $scope.toExport = function () {
    var field_url = "http://" + window.location.hostname + ":9999/master/index3.html"
    window.location.replace(field_url)
  };

  $scope.PublishThresholdParams = function () {
    var params = [];
    for (var i = 0; i < $scope.virtual_points_params.length; i++) {
      params.push($scope.virtual_points_params[i].value);
    }
    virtual_points_msg.data = params;
    $scope.pub_virtual_points_params.publish(virtual_points_msg);
    console.log(virtual_points_msg);
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

});