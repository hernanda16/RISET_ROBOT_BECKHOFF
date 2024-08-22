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

  //================================================

  // Simple Timer
  $interval(function () {}, 1000);

  // ROS
  $scope.ros = new ROSLIB.Ros({
    url: "ws://" + window.location.hostname + ":9900",
  });

  // Global variables
  var prev_cur_x = 0;
  var prev_cur_y = 0;
  var x_offset_frame = 0;
  var y_offset_frame = 35;

  // Subscribe to frame
  $scope.frame_field =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision/display&type=ros_compressed";

  // Get cam offset
  $scope.req_cam_offset = new ROSLIB.ServiceRequest({});

  $scope.ser_cam_offset = new ROSLIB.Service({
    ros: $scope.ros,
    name: "cam_offset",
    serviceType: "iris_msgs/cam_offset",
  });

  $scope.ser_cam_offset.callService($scope.req_cam_offset, (result) => {
    x_offset_frame = result.data[0];
    y_offset_frame = result.data[1];//TODO: fix this
    // x_offset_frame = -4;
    // y_offset_frame = 28;
  });

  $scope.get_x_y_data = function () {
    if (1) {
      $(document).ready(function () {
        $("#get_x_y").on("click", function (event) {
          var offset = findPos(document.getElementById("get_x_y"));
          //=============================================================
          var x = event.clientX - offset.X + window.pageXOffset;
          var y = event.clientY - offset.Y + window.pageYOffset;
          // console.log("curr: ", event.clientX, " ", event.clientY);
          // console.log("func: ", offset.X, " ", offset.Y);
          // console.log("window screen: ", window.pageXOffset, " ", window.pageYOffset);

          // console.log("res: ", x, " ", y);

          var $img = $(this);
          var x_frame = ((this.naturalWidth / $img.width()) * x).toFixed(0);
          var y_frame = ((this.naturalHeight / $img.height()) * y).toFixed(0);

          // console.log("field: ", x_frame, " ", y_frame);

          var x_center_frame = this.naturalWidth / 2 + x_offset_frame;
          var y_center_frame = this.naturalHeight / 2 + y_offset_frame;

          var x_frame_final = x_frame - x_center_frame;
          var y_frame_final = y_center_frame - y_frame;

          // console.log("dif: ",x_frame_final ," ",y_frame_final, " ", prev_cur_x, " ", prev_cur_y);

          // Calc
          //      if(x_frame_final != prev_cur_x && y_frame_final != prev_cur_y){
          // console.log("final: ",x_frame_final, " ", y_frame_final);

          // Get polar coordinates
          var angle_frame =
            (Math.atan2(y_frame_final, x_frame_final) * 180) / Math.PI;
          var dist_frame = Math.sqrt(
            x_frame_final * x_frame_final + y_frame_final * y_frame_final
          );

          console.log(dist_frame.toFixed(3) + " " + angle_frame.toFixed(3));

          // copy to clipboard
          var dummy = document.createElement("textarea");
          document.body.appendChild(dummy);
          dummy.value = dist_frame.toFixed(3) + " " + angle_frame.toFixed(3);
          dummy.select();
          document.execCommand("copy");
          document.body.removeChild(dummy);

          //=============================================================

          prev_cur_x = x_frame_final;
          prev_cur_y = y_frame_final;
        });
      });
    }
  };

  function findPos(element) {
    var xPosition = 0;
    var yPosition = 0;

    while (element) {
      xPosition += element.offsetLeft - element.scrollLeft + element.clientLeft;
      yPosition += element.offsetTop - element.scrollTop + element.clientTop;
      element = element.offsetParent;
    }
    return { X: xPosition, Y: yPosition };
  }
});
