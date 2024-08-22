var mainApp = angular.module("mainApp", []);

function cekColor(elem, value, limit) {
  if (Math.abs(value) > limit) {
    elem.className = "";
    elem.className = "badge badge-danger";
  } else {
    elem.className = "";
    elem.className = "badge badge-success";
  }
}

mainApp.controller("mainAppController", function ($scope, $interval) {
  $scope.ns = "";

  // Agar scope dapat diakses sebagai global variabel
  window.$scope = $scope;

  //Area buat resume jeda video
  $scope.stream_vision = [true, true, true];

  // pub_ball_thresh = IT.advertise("/vision_ball_binary", 1);
  // pub_field_final_threshold_ = IT.advertise("/vision_field_thresh", 1);
  // pub_ball_final_threshold_ = IT.advertise("/vision_ball_final_threshold", 1);
  // pub_line_final_threshold_ = IT.advertise("/vision_line_final_threshold", 1);

  $scope.pause_start_stream = function (index_stream) {
    console.log($scope.stream_vision[index_stream]);
    if ($scope.stream_vision[index_stream]) {
      $scope.stream_vision[index_stream] = false;
      if (index_stream == 0) {
        $scope.frame_obstacleDisplayOut = "None";
      } else if (index_stream == 1) {
        $scope.frame_fieldRawThreshold = "None";
      } else if (index_stream == 2) {
        $scope.frame_ballRawThreshold = "None";
      } else if (index_stream == 3) {
        $scope.frame_lineRawThreshold = "None";
      }
    } else {
      $scope.stream_vision[index_stream] = true;
      if (index_stream == 0) {
        $scope.frame_obstacleDisplayOut =
          "http://" +
          window.location.hostname +
          ":9901" +
          "/stream?topic=" +
          $scope.ns +
          "/vision_field_thresh&type=ros_compressed";
      } else if (index_stream == 1) {
        $scope.frame_fieldRawThreshold =
          "http://" +
          window.location.hostname +
          ":9901" +
          "/stream?topic=" +
          $scope.ns +
          "/vision_field_thresh&type=ros_compressed";
      } else if (index_stream == 2) {
        $scope.frame_ballRawThreshold =
          "http://" +
          window.location.hostname +
          ":9901" +
          "/stream?topic=" +
          $scope.ns +
          "/vision_ball_binary&type=ros_compressed";
      } else if (index_stream == 3) {
        $scope.frame_lineRawThreshold =
          "http://" +
          window.location.hostname +
          ":9901" +
          "/stream?topic=" +
          $scope.ns +
          "/vision_line_final_threshold&type=ros_compressed";
      }
    }
  };

  $scope.get_stream_status = function (index_stream) {
    return $scope.stream_vision[index_stream];
  };

  $scope.stream_button_state = function (index_stream) {
    if (!$scope.stream_vision[index_stream]) {
      return { text: "Resume", css: "hover:bg-sky-700 bg-sky-500" };
    } else {
      return { text: "Pause", css: "hover:bg-rose-700 bg-rose-500" };
    }
  };

  // ---------------------------------------

  //Area Untuk Regresi
  $scope.jarak_sekarang = 0;
  $scope.array_regresi = [];

  $scope.reset_regresi = function () {
    const regresi_textarea = document.querySelector("#regression-area");
    regresi_textarea.value = "";
    $scope.jarak_sekarang = 0;
    $scope.array_regresi = [];
  };

  $scope.convert_format = function () {
    const array_regresi = document
      .querySelector("#data-regresi")
      .value.split("\n");
    const converted_format = array_regresi.map((current, index) => {
      return `orde_${index}: ${current}`;
    });
    document.querySelector("#formated").value = `max_orde : ${
      array_regresi.length - 1
    }\n${converted_format.join("\n")}`;
    console.log(array_regresi);
  };

  // -----------------------------------------

  // UNTUK SUBSCRIBE TOPIC

  $scope.menu_activeIndex = 1;

  $scope.menu_buttonState = function (index) {
    if (index == $scope.menu_activeIndex) return "bg-white text-black";
    else return "hover:text-black";
  };

  $scope.menu_contentState = function (index) {
    if (index == $scope.menu_activeIndex) return true;
    else return false;
  };

  $scope.changeActiveIndex = function (index) {
    $scope.menu_activeIndex = index;
  };

  $scope.field_thresholdParameter = [
    { label: "Y-", value: 0, min: 0, max: 255 },
    { label: "Y+", value: 255, min: 0, max: 255 },
    { label: "U-", value: 0, min: 0, max: 255 },
    { label: "U+", value: 255, min: 0, max: 255 },
    { label: "V-", value: 0, min: 0, max: 255 },
    { label: "V+", value: 255, min: 0, max: 255 },
  ];

  $scope.ball_thresholdParameter = [
    { label: "Y-", value: 0, min: 0, max: 255 },
    { label: "Y+", value: 255, min: 0, max: 255 },
    { label: "U-", value: 0, min: 0, max: 255 },
    { label: "U+", value: 255, min: 0, max: 255 },
    { label: "V-", value: 0, min: 0, max: 255 },
    { label: "V+", value: 255, min: 0, max: 255 },
  ];

  $scope.line_thresholdParameter = [
    { label: "Y-", value: 0, min: 0, max: 255 },
    { label: "Y+", value: 255, min: 0, max: 255 },
    { label: "U-", value: 0, min: 0, max: 255 },
    { label: "U+", value: 255, min: 0, max: 255 },
    { label: "V-", value: 0, min: 0, max: 255 },
    { label: "V+", value: 255, min: 0, max: 255 },
  ];

  $scope.camera_parameter = [
    { label: "Bright", value: 0, min: 0, max: 255 },
    { label: "Contr", value: 0, min: 0, max: 255 },
    { label: "Satur", value: 0, min: 0, max: 255 },
  ];

  $scope.LST_params = [
    { label: "min_rad", value: 0, min: 0, max: 600 },
    { label: "max_rad", value: 0, min: 0, max: 600 },
  ];

  //============
  //-----Updater
  //============
  $interval(function () {}, 1000);

  //========
  //-----ROS
  //========
  $scope.ros = new ROSLIB.Ros({
    url: "ws://" + window.location.hostname + ":9900",
  });

  // GET AUTO NAMESPACES
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

  $scope.frame_fieldRawThreshold =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_raw&type=ros_compressed";

  $scope.frame_ballRawThreshold =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_ball_final_threshold&type=ros_compressed";

  $scope.frame_lineRawThreshold =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_line_final_threshold&type=ros_compressed";

  $scope.frame_obstacleDisplayOut =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_field_final_threshold&type=ros_compressed";

  $scope.frame_obverseRawThreshold =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_obverse/raw_threshold&type=ros_compressed";

  $scope.frame_obverseDisplayOut =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_obverse/display_out&type=ros_compressed";

  $scope.frame_original_bgr =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_frame_bgr&type=ros_compressed";

  //========================
  //-----HSV Threshold Topic
  //========================
  $scope.top_field_hsv = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "yuv_field_thresh_final",
    messageType: "std_msgs/UInt16MultiArray",
  });
  $scope.top_ball_hsv = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "yuv_ball_thresh_final",
    messageType: "std_msgs/UInt16MultiArray",
  });
  $scope.top_line_hsv = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "yuv_line_thresh_final",
    messageType: "std_msgs/UInt16MultiArray",
  });

  $scope.top_line_scan_hsv = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "lst_params",
    messageType: "std_msgs/UInt16MultiArray",
  });

  // $scope.top_calibrate_hsv = new ROSLIB.Topic({
  //   ros: $scope.ros,
  //   name: "vision_calibrate/hsv_threshold",
  //   messageType: "std_msgs/UInt16MultiArray"
  // });
  //========================
  //-----HLS Threshold Topic
  //========================
  $scope.top_line_hls = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_line/hls_threshold",
    messageType: "std_msgs/UInt16MultiArray",
  });

  //========================
  //-----HSV Threshold Msg
  //========================
  $scope.msg_hsv = new ROSLIB.Message({
    layout: { dim: [], data_offset: 0 },
    data: [0, 0, 0, 0, 0, 0],
  });
  //========================
  //-----HLSThreshold Msg
  //========================
  $scope.msg_hls = new ROSLIB.Message({
    layout: { dim: [], data_offset: 0 },
    data: [0, 0, 0, 0, 0, 0],
  });


  $scope.lst_param_msg = new ROSLIB.Message({
    layout: { dim: [], data_offset: 0 },
    data: [90, 200],
  });


  //============================
  //-----HSV Threshold Publisher
  //============================
  $scope.pub_field_threshold_value = function (object) {
    for (var i = 0; i < 6; i++) {
      $scope.msg_hsv.data[i] = object[i].value;
    }
    $scope.top_field_hsv.publish($scope.msg_hsv);
  };

  $scope.pub_ball_threshold_value = function (object) {
    for (var i = 0; i < 6; i++) $scope.msg_hsv.data[i] = object[i].value;
    $scope.top_ball_hsv.publish($scope.msg_hsv);
  };

  $scope.pub_line_threshold_value = function (object) {
    for (var i = 0; i < 6; i++) $scope.msg_hsv.data[i] = object[i].value;
    $scope.top_line_hsv.publish($scope.msg_hsv);
  };

  $scope.pub_lstParams = function (object) {
    for (var i = 0; i < 2; i++) $scope.lst_param_msg.data[i] = object[i].value;
    $scope.top_line_scan_hsv.publish($scope.lst_param_msg);
  };

  //==================================
  //-----YUV Threshold Service Request
  //==================================
  $scope.req_yuv = new ROSLIB.ServiceRequest({});
  $scope.req_lst = new ROSLIB.ServiceRequest({});

  //=================================
  //-----YUV Threshold Service Client
  //=================================
  $scope.ser_field_hsv = new ROSLIB.Service({
    ros: $scope.ros,
    name: "yuv_field_thresh_ser",
    messageType: "iris_msgs/yuv_init",
  });
  $scope.ser_field_hsv.callService($scope.req_yuv, (res) => {
    for (var i = 0; i < 6; i++) {
      $scope.field_thresholdParameter[i].value = res.yuv_threshold[i];
    }
  });

  $scope.ser_ball_hsv = new ROSLIB.Service({
    ros: $scope.ros,
    name: "yuv_ball_thresh_ser",
    messageType: "iris_msgs/yuv_init",
  });
  $scope.ser_ball_hsv.callService($scope.req_yuv, (res) => {
    for (var i = 0; i < 6; i++)
      $scope.ball_thresholdParameter[i].value = res.yuv_threshold[i];
  });

  $scope.ser_line_hsv = new ROSLIB.Service({
    ros: $scope.ros,
    name: "yuv_line_thresh_ser",
    messageType: "iris_msgs/yuv_init",
  });
  $scope.ser_line_hsv.callService($scope.req_yuv, (res) => {
    for (var i = 0; i < 6; i++) {
      $scope.line_thresholdParameter[i].value = res.yuv_threshold[i];
      console.log(res.yuv_threshold[i]);
    }
  });

  /////////////////////////////////
  //CAMERA REALTIME CHANGE AREAA///
  /////////////////////////////////

  $scope.top_cameraConfig = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "camera_config",
    messageType: "std_msgs/UInt16MultiArray",
  });

  $scope.msg_cameraConfig = new ROSLIB.Message({
    layout: { dim: [], data_offset: 0 },
    data: [0, 0, 0],
  });

  $scope.pub_cameraConfig = function (object) {
    for (let i = 0; i < $scope.camera_parameter.length; i++) {
      $scope.msg_cameraConfig.data[i] = object[i].value;
    }
    $scope.top_cameraConfig.publish($scope.msg_cameraConfig);
  };

  $scope.req_camera_config = new ROSLIB.ServiceRequest({});
  $scope.ser_camera_config = new ROSLIB.Service({
    ros: $scope.ros,
    name: "ser_camera_config",
    messageType: "iris_msgs/cameryuvonfig",
  });

  $scope.ser_camera_config.callService($scope.req_camera_config, (res) => {
    for (var i = 0; i < 3; i++)
      $scope.camera_parameter[i].value = res.camera_config[i];
  });

  ///////////////////////////////////////////////////////

  $scope.set_default_thresshold_bola = function () {
    $scope.ball_thresholdParameter[0].value = 0;
    $scope.ball_thresholdParameter[1].value = 13;
    $scope.ball_thresholdParameter[2].value = 156;
    $scope.ball_thresholdParameter[3].value = 255;
    $scope.ball_thresholdParameter[4].value = 145;
    $scope.ball_thresholdParameter[5].value = 255;
    $scope.pub_ball_threshold_value($scope.ball_thresholdParameter);
  };

  $scope.set_default_thresshold_lapangan = function () {
    $scope.field_thresholdParameter[0].value = 0;
    $scope.field_thresholdParameter[1].value = 110;
    $scope.field_thresholdParameter[2].value = 52;
    $scope.field_thresholdParameter[3].value = 255;
    $scope.field_thresholdParameter[4].value = 80;
    $scope.field_thresholdParameter[5].value = 255;
    $scope.pub_field_threshold_value($scope.field_thresholdParameter);
  };

  $scope.set_default_thresshold_garis = function () {
    $scope.line_thresholdParameter[0].value = 0;
    $scope.line_thresholdParameter[1].value = 0;
    $scope.line_thresholdParameter[2].value = 0;
    $scope.line_thresholdParameter[3].value = 255;
    $scope.line_thresholdParameter[4].value = 0;
    $scope.line_thresholdParameter[5].value = 255;
    $scope.pub_line_threshold_value($scope.line_thresholdParameter);
  };

  $scope.set_default_camera_setting = function () {
    $scope.camera_parameter[0].value = 162;
    $scope.camera_parameter[1].value = 0;
    $scope.camera_parameter[2].value = 255;
    $scope.pub_cameraConfig($scope.camera_parameter);
  };

  $scope.set_default_LST_params = function () {
    $scope.LST_params[0].value = 70;
    $scope.LST_params[1].value = 210;
    $scope.pub_lstParams($scope.LST_params);
  };
});
