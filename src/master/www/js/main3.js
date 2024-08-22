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
          "/vision_field_final_threshold&type=ros_compressed";
      } else if (index_stream == 1) {
        $scope.frame_fieldRawThreshold =
          "http://" +
          window.location.hostname +
          ":9901" +
          "/stream?topic=" +
          $scope.ns +
          "/vision_field/raw_threshold&type=ros_compressed";
      } else if (index_stream == 2) {
        $scope.frame_ballRawThreshold =
          "http://" +
          window.location.hostname +
          ":9901" +
          "/stream?topic=" +
          $scope.ns +
          "/vision_ball_final_threshold&type=ros_compressed";
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

  // $scope.regresi_pixel_data = function()  {
  //   const regresi_jarak_awal = document.querySelector("#jarak-awal");
  //   const regresi_selisih_jarak = document.querySelector("#selisih-jarak");
  //   const regresi_textarea = document.querySelector("#regression-area");
  //   regresi_textarea.scrollTop = regresi_textarea.scrollHeight
  //   if ($scope.jarak_sekarang === 0) {
  //     $scope.jarak_sekarang = regresi_jarak_awal.value;
  //   }
  //   $scope.pixel_sekarang = $scope.jarak_bola_pada_lapangan;
  //   if ($scope.array_regresi.length!=0) {
  //     $scope.array_regresi = regresi_textarea.value.split('\n')
  //   }

  //   $scope.array_regresi.push(`${$scope.jarak_sekarang} ${$scope.pixel_sekarang}`)
  //   regresi_textarea.value = $scope.array_regresi.join('\n')
  //   $scope.jarak_sekarang = Number($scope.jarak_sekarang) + Number(regresi_selisih_jarak.value)
  // }

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
    document.querySelector("#formated").value = `max_orde : ${array_regresi.length - 1
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
    if ($scope.menu_activeIndex != 7) {
      $scope.frame_original_bgr = "None";
    } else {
      $scope.frame_original_bgr =
        "http://" +
        window.location.hostname +
        ":9901" +
        "/stream?topic=" +
        $scope.ns +
        "/vision_frame_bgr&type=ros_compressed";
    }
    // console.log($scope.frame_original_bgr);

    // let imgs = document.getElementsByTagName('img');

    // for(let img of imgs)
    // {
    //   let imgsrc = img.src;
    //   img.src = '';

    //   setTimeout(() => {
    //     img.src = imgsrc;
    //   }, 500);
    // }
  };

  $scope.field_thresholdParameter = [
    { label: "H-", value: 0, min: 0, max: 180 },
    { label: "H+", value: 180, min: 0, max: 180 },
    { label: "S-", value: 0, min: 0, max: 255 },
    { label: "S+", value: 255, min: 0, max: 255 },
    { label: "V-", value: 0, min: 0, max: 255 },
    { label: "V+", value: 255, min: 0, max: 255 },
  ];

  $scope.ball_thresholdParameter = [
    { label: "H-", value: 0, min: 0, max: 180 },
    { label: "H+", value: 180, min: 0, max: 180 },
    { label: "S-", value: 0, min: 0, max: 255 },
    { label: "S+", value: 255, min: 0, max: 255 },
    { label: "V-", value: 0, min: 0, max: 255 },
    { label: "V+", value: 255, min: 0, max: 255 },
  ];

  $scope.obverse_thresholdParameter = [
    { label: "H-", value: 0, min: 0, max: 180 },
    { label: "H+", value: 180, min: 0, max: 180 },
    { label: "S-", value: 0, min: 0, max: 255 },
    { label: "S+", value: 255, min: 0, max: 255 },
    { label: "V-", value: 0, min: 0, max: 255 },
    { label: "V+", value: 255, min: 0, max: 255 },
  ];

  $scope.line_thresholdParameter = [
    { label: "H-", value: 0, min: 0, max: 180 },
    { label: "H+", value: 180, min: 0, max: 180 },
    { label: "L-", value: 0, min: 0, max: 255 },
    { label: "L+", value: 255, min: 0, max: 255 },
    { label: "S-", value: 0, min: 0, max: 255 },
    { label: "S+", value: 255, min: 0, max: 255 },
  ];

  $scope.camera_parameter = [
    { label: "Bright", value: 0, min: 0, max: 255 },
    { label: "Contr", value: 0, min: 0, max: 255 },
    { label: "Satur", value: 0, min: 0, max: 255 },
  ];

  //============
  //-----Updater
  //============
  $interval(function () { }, 1000);

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

  //--------------------------------------------------------------------------------------------------------------------
  //====================================================================================================================

  $scope.frame_fieldRawThreshold =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_field/raw_threshold&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_ballRawThreshold =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_ball/raw_threshold&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_obstacleDisplayOut =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_field_final_threshold&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_obverseRawThreshold =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_obverse/raw_threshold&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_obverseDisplayOut =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_obverse/display_out&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_original_bgr =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_frame_bgr&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_whitePoints =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_line/white_points&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_whiteLine =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_line/white_line&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_localizationDebuger =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/localization/debuger&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_line_ch =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_line_raw_threshold_main&type=ros_compressed";
    
  // $scope.frame_line_ch =
  //   "http://" +
  //   window.location.hostname +
  //   ":9901" +
  //   "/stream?topic=" +
  //   $scope.ns +
  //   "/vision_line_final_threshold_main&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_field_result_display =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_field_thresh_main&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_field_raw =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_field_raw_thresh_main&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_ball_result =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_ball_binary_main&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_convexed_ch =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/CaptureImage/pub_convexed&type=ros_compressed";

  //====================================================================================================================

  $scope.frame_captured_image =
    "http://" +
    window.location.hostname +
    ":9901" +
    "/stream?topic=" +
    $scope.ns +
    "/vision_capture_image&type=ros_compressed";
  // "/vision_captured_image&type=ros_compressed";

  //--------------------------------------------------------------------------------------------------------------------
  //====================================================================================================================

  $scope.pub_CaptureImage = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "CaptureImage/getImage",
    messageType: "std_msgs/Bool",
  });

  $scope.msg_CaptureImage = new ROSLIB.Message({
    data: true,
  });

  $scope.CaptureImage = function () {
    $scope.pub_CaptureImage.publish($scope.msg_CaptureImage);

  }

  //====================================================================================================================

  $scope.pub_cutTrigger = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "CaptureImage/cutTrigger",
    messageType: "std_msgs/Bool",
  });

  $scope.msg_cutTrigger = new ROSLIB.Message({
    data: true,
  });

  $scope.CutContour = function () {
    $scope.pub_cutTrigger.publish($scope.msg_cutTrigger);

  }

  //====================================================================================================================

  $scope.pub_clearAllTrigger = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "CaptureImage/clearAll",
    messageType: "std_msgs/Bool",
  });

  $scope.msg_clearAllTrigger = new ROSLIB.Message({
    data: true,
  });

  $scope.ClearAllContour = function () {
    $scope.pub_clearAllTrigger.publish($scope.msg_clearAllTrigger);

  }

  //====================================================================================================================

  $scope.pub_ambilLagiTrigger = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "CaptureImage/ambilLagi",
    messageType: "std_msgs/Bool",
  });

  $scope.msg_ambilLagiTrigger = new ROSLIB.Message({
    data: true,
  });

  $scope.AmbilLagi = function () {
    $scope.pub_ambilLagiTrigger.publish($scope.msg_ambilLagiTrigger);
  }

  //====================================================================================================================

  $scope.pub_exportCSVLapangan = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "CaptureImage/exportCSVLapangan",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_exportCSVLapangan = new ROSLIB.Message({
    data: true,
  });

  $scope.ExportCSVLapangan = function () {
    $scope.pub_exportCSVLapangan.publish($scope.msg_exportCSVLapangan);
    alert("data yuv LAPANGAN sudah dikirim ke node");
  }

  //====================================================================================================================

  $scope.pub_exportCSVBola = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "CaptureImage/exportCSVBola",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_exportCSVBola = new ROSLIB.Message({
    data: true,
  });

  $scope.ExportCSVBola = function () {
    $scope.pub_exportCSVBola.publish($scope.msg_exportCSVBola);
    alert("data yuv BOLA sudah dikirim ke node");
  }

  //====================================================================================================================

  $scope.pub_exportCSVGaris = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "CaptureImage/exportCSVGaris",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_exportCSVGaris = new ROSLIB.Message({
    data: true,
  });

  $scope.ExportCSVGaris = function () {
    $scope.pub_exportCSVGaris.publish($scope.msg_exportCSVGaris);
    alert("data yuv GARIS sudah dikirim ke node");
  }

  //====================================================================================================================

  $scope.pub_exportLUTLapangan = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_lut/exportLUTLapangan",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_exportLUTLapangan = new ROSLIB.Message({
    data: true,
  });

  $scope.ExportLUTLapangan = function () {
    $scope.pub_exportLUTLapangan.publish($scope.msg_exportLUTLapangan);
    alert("data lut LAPANGAN sudah dikirim ke node");
  }

  //====================================================================================================================

  $scope.pub_exportLUTBola = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_lut/exportLUTBola",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_exportLUTBola = new ROSLIB.Message({
    data: true,
  });

  $scope.ExportLUTBola = function () {
    $scope.pub_exportLUTBola.publish($scope.msg_exportLUTBola);
    alert("data lut BOLA sudah dikirim ke node");
  }

  //====================================================================================================================

  $scope.pub_exportLUTGaris = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_lut/exportLUTGaris",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_exportLUTGaris = new ROSLIB.Message({
    data: true,
  });

  $scope.ExportLUTGaris = function () {
    $scope.pub_exportLUTGaris.publish($scope.msg_exportLUTGaris);
    alert("data lut GARIS sudah dikirim ke node");
  }

  //====================================================================================================================

  $scope.pub_triggerLUTLapangan = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_lut/get_field_lut_filename",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_triggerLUTLapangan = new ROSLIB.Message({
    data: true,
  });

  $scope.triggerLUTLapangan = function () {
    $scope.pub_triggerLUTLapangan.publish($scope.msg_triggerLUTLapangan);
  }

  //====================================================================================================================

  $scope.pub_triggerLUTBola = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_lut/get_ball_lut_filename",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_triggerLUTBola = new ROSLIB.Message({
    data: true,
  });

  $scope.triggerLUTBola = function () {
    $scope.pub_triggerLUTBola.publish($scope.msg_triggerLUTBola);
  }

  //====================================================================================================================

  $scope.pub_triggerLUTGaris = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_lut/get_line_lut_filename",
    messageType: "std_msgs/Bool",
  })

  $scope.msg_triggerLUTGaris = new ROSLIB.Message({
    data: true,
  });

  $scope.triggerLUTGaris = function () {
    $scope.pub_triggerLUTGaris.publish($scope.msg_triggerLUTGaris);
  }

  //====================================================================================================================

  $scope.pub_export_data = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "pub_export_data",
    messageType: "std_msgs/Bool"
  });

  $scope.msg_export_data = new ROSLIB.Message({
    data: true,
  });

  $scope.exportClicked = function () {
    $scope.pub_export_data.publish($scope.msg_export_data);
  };

  //--------------------------------------------------------------------------------------------------------------------
  //====================================================================================================================

  $scope.pilihLutLap = function (value) {

    $scope.pub_pilihLUTLapDisp = new ROSLIB.Topic({
      ros: $scope.ros,
      name: "vision_lut/trigger_field_lut",
      messageType: "std_msgs/UInt32",
    })

    $scope.msg_pilihLUTLapDisp = new ROSLIB.Message({
      data: value,
    });

    $scope.kirimLUTLap = function () {
      $scope.pub_pilihLUTLapDisp.publish($scope.msg_pilihLUTLapDisp);
    }

    $scope.kirimLUTLap();
  }

  //====================================================================================================================

  $scope.nama_lut_lap = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_field_lut_name",
    messageType: "std_msgs/UInt32MultiArray",
  });

  //====================================================================================================================

  $scope.nama_lut_lap.subscribe((msg) => {
    $scope.list_lut_lapangan = msg.data;

    $scope.lut_lap_len = $scope.list_lut_lapangan.length;
    $scope.lut_lap_str = [];
    $scope.lut_lap_str_display = [];

    //=========================================================

    for (var i = 0; i < $scope.lut_lap_len; i++) {
      $scope.lut_lap_str[i] = $scope.list_lut_lapangan[i].toString();
      $scope.lut_lap_str_display[i] = $scope.lut_lap_str[i][1] + $scope.lut_lap_str[i][2] + "-" + $scope.lut_lap_str[i][3] + $scope.lut_lap_str[i][4] + "-" + $scope.lut_lap_str[i][5] + $scope.lut_lap_str[i][6] + "-" + $scope.lut_lap_str[i][7] + $scope.lut_lap_str[i][8];
    }

    //=========================================================
    document.getElementById("tabelLapangan").innerHTML = ""

    for (var i = 0; i < $scope.lut_lap_len; i++) {
      $scope.tr = document.createElement("tr");
      $scope.tr.classList.add("border-b");

      $scope.td1 = document.createElement("td");
      $scope.td1.innerHTML = i + 1;
      $scope.td1.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.td2 = document.createElement("td");
      $scope.td2.innerHTML = $scope.lut_lap_str_display[i];
      $scope.td2.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.td3 = document.createElement("td");
      $scope.td3.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.a = document.createElement("a");
      $scope.a.setAttribute("href", "javascript:$scope.pilihLutLap(" + $scope.list_lut_lapangan[i] + ");");

      $scope.btn = document.createElement("button");
      $scope.btn.innerHTML = "Pilih";
      $scope.btn.classList.add("w-1/2", "bg-rose-500", "px-3", "py-2", "my-2", "rounded", "rounded-lg", "mx-auto");
      $scope.btn.setAttribute("id", "lutlap" + i);

      document.getElementById("tabelLapangan").appendChild($scope.tr).appendChild($scope.td1);
      document.getElementById("tabelLapangan").appendChild($scope.tr).appendChild($scope.td2);
      document.getElementById("tabelLapangan").appendChild($scope.tr).appendChild($scope.td3).appendChild($scope.a).appendChild($scope.btn);
    }

  });

  //--------------------------------------------------------------------------------------------------------------------
  //====================================================================================================================

  $scope.pilihLutBola = function (value) {

    $scope.pub_pilihLUTBolaDisp = new ROSLIB.Topic({
      ros: $scope.ros,
      name: "vision_lut/trigger_ball_lut",
      messageType: "std_msgs/UInt32",
    })

    $scope.msg_pilihLUTBolaDisp = new ROSLIB.Message({
      data: value,
    });

    $scope.kirimLUTBola = function () {
      $scope.pub_pilihLUTBolaDisp.publish($scope.msg_pilihLUTBolaDisp);
    }

    $scope.kirimLUTBola();
  }

  //====================================================================================================================

  $scope.nama_lut_bola = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_ball_lut_name",
    messageType: "std_msgs/UInt32MultiArray",
  });

  //====================================================================================================================

  $scope.nama_lut_bola.subscribe((msg) => {
    $scope.list_lut_bola = msg.data;

    $scope.lut_bola_len = $scope.list_lut_bola.length;
    $scope.lut_bola_str = [];
    $scope.lut_bola_str_display = [];

    //=========================================================

    for (var i = 0; i < $scope.lut_bola_len; i++) {
      $scope.lut_bola_str[i] = $scope.list_lut_bola[i].toString();
      $scope.lut_bola_str_display[i] = $scope.lut_bola_str[i][1] + $scope.lut_bola_str[i][2] + "-" + $scope.lut_bola_str[i][3] + $scope.lut_bola_str[i][4] + "-" + $scope.lut_bola_str[i][5] + $scope.lut_bola_str[i][6] + "-" + $scope.lut_bola_str[i][7] + $scope.lut_bola_str[i][8];
    }

    //=========================================================

    document.getElementById("tabelBola").innerHTML = ""

    for (var i = 0; i < $scope.lut_bola_len; i++) {
      $scope.tr = document.createElement("tr");
      $scope.tr.classList.add("border-b");

      $scope.td1 = document.createElement("td");
      $scope.td1.innerHTML = i + 1;
      $scope.td1.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.td2 = document.createElement("td");
      $scope.td2.innerHTML = $scope.lut_bola_str_display[i];
      $scope.td2.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.td3 = document.createElement("td");
      $scope.td3.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.a = document.createElement("a");
      $scope.a.setAttribute("href", "javascript:$scope.pilihLutBola(" + $scope.list_lut_bola[i] + ");");

      $scope.btn = document.createElement("button");
      $scope.btn.innerHTML = "Pilih";
      $scope.btn.classList.add("w-1/2", "bg-rose-500", "px-3", "py-2", "my-2", "rounded", "rounded-lg", "mx-auto");
      $scope.btn.setAttribute("id", "lutbola" + i);

      document.getElementById("tabelBola").appendChild($scope.tr).appendChild($scope.td1);
      document.getElementById("tabelBola").appendChild($scope.tr).appendChild($scope.td2);
      document.getElementById("tabelBola").appendChild($scope.tr).appendChild($scope.td3).appendChild($scope.a).appendChild($scope.btn);
    }

  });

  //--------------------------------------------------------------------------------------------------------------------
  //====================================================================================================================

  $scope.pilihLutGaris = function (value) {

    $scope.pub_pilihLUTGarisDisp = new ROSLIB.Topic({
      ros: $scope.ros,
      name: "vision_lut/trigger_line_lut",
      messageType: "std_msgs/UInt32",
    })

    $scope.msg_pilihLUTGarisDisp = new ROSLIB.Message({
      data: value,
    });

    $scope.kirimLUTGaris = function () {
      $scope.pub_pilihLUTGarisDisp.publish($scope.msg_pilihLUTGarisDisp);
    }

    $scope.kirimLUTGaris();
  }

  //--------------------------------------------------------------------------------------------------------------------

  $scope.nama_lut_garis = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "vision_line_lut_name",
    messageType: "std_msgs/UInt32MultiArray",
  });

  //--------------------------------------------------------------------------------------------------------------------

  $scope.nama_lut_garis.subscribe((msg) => {
    $scope.list_lut_garis = msg.data;

    $scope.lut_garis_len = $scope.list_lut_garis.length;
    $scope.lut_garis_str = [];
    $scope.lut_garis_str_display = [];

    //=========================================================

    for (var i = 0; i < $scope.lut_garis_len; i++) {
      $scope.lut_garis_str[i] = $scope.list_lut_garis[i].toString();
      $scope.lut_garis_str_display[i] = $scope.lut_garis_str[i][1] + $scope.lut_garis_str[i][2] + "-" + $scope.lut_garis_str[i][3] + $scope.lut_garis_str[i][4] + "-" + $scope.lut_garis_str[i][5] + $scope.lut_garis_str[i][6] + "-" + $scope.lut_garis_str[i][7] + $scope.lut_garis_str[i][8];
    }

    //=========================================================


    document.getElementById("tabelGaris").innerHTML = ""

    for (var i = 0; i < $scope.lut_garis_len; i++) {
      $scope.tr = document.createElement("tr");
      $scope.tr.classList.add("border-b");

      $scope.td1 = document.createElement("td");
      $scope.td1.innerHTML = i + 1;
      $scope.td1.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.td2 = document.createElement("td");
      $scope.td2.innerHTML = $scope.lut_garis_str_display[i];
      $scope.td2.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.td3 = document.createElement("td");
      $scope.td3.classList.add("text-sm", "text-gray-900", "font-light", "px-6", "py-4", "whitespace-nowrap", "ng-binding");

      $scope.a = document.createElement("a");
      $scope.a.setAttribute("href", "javascript:$scope.pilihLutGaris(" + $scope.list_lut_garis[i] + ");");

      $scope.btn = document.createElement("button");
      $scope.btn.innerHTML = "Pilih";
      $scope.btn.classList.add("w-1/2", "bg-rose-500", "px-3", "py-2", "my-2", "rounded", "rounded-lg", "mx-auto");
      $scope.btn.setAttribute("id", "lutbola" + i);

      document.getElementById("tabelGaris").appendChild($scope.tr).appendChild($scope.td1);
      document.getElementById("tabelGaris").appendChild($scope.tr).appendChild($scope.td2);
      document.getElementById("tabelGaris").appendChild($scope.tr).appendChild($scope.td3).appendChild($scope.a).appendChild($scope.btn);
    }




  });

  //--------------------------------------------------------------------------------------------------------------------
  //====================================================================================================================

  $scope.pointxy = []
  $scope.point_x_sampling;
  $scope.point_y_sampling;

  $scope.pub_xysampling = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "CaptureImage/coordinateXY",
    messageType: "std_msgs/UInt16MultiArray",
  });

  $scope.get_x_y_data = function () {
    if (1) {
      $(document).ready(function () {
        $("#get_sampling_data").on("click", function (event) {

          var offset = findPos(document.getElementById("get_sampling_data"));
          //=============================================================
          var x = event.clientX - offset.X;
          var y = event.clientY - offset.Y;
          console.log("curr: ", event.clientX, " ", event.clientY);
          console.log("func: ", offset.X, " ", offset.Y);

          $scope.point_x_sampling = Math.round(x);
          $scope.point_y_sampling = Math.round(y);

          console.log("res: ", $scope.point_x_sampling, " ", $scope.point_y_sampling);

          var $img = $(this);
          var currentClickPosX = event.pageX - $img.offset().left;
          var currentClickPosY = event.pageY - $img.offset().top;

          var currentWidth = $img.width();
          var currentHeight = $img.height();

          var naturalWidth = this.naturalWidth;
          var naturalHeight = this.naturalHeight;

          var naturalClickPosX = ((naturalWidth / currentWidth) * currentClickPosX).toFixed(0);
          var naturalClickPosY = ((naturalHeight / currentHeight) * currentClickPosY).toFixed(0);
          $scope.pointxy.push(Math.round(naturalClickPosX));
          $scope.pointxy.push(Math.round(naturalClickPosY));

          // $scope.pointxy.push(Math.round($scope.point_x_sampling))
          // $scope.pointxy.push(Math.round($scope.point_y_sampling))

          //=============================================================

          $scope.msg_xysampling = new ROSLIB.Message({
            layout: { dim: [], data_offset: 0 },
            data: $scope.pointxy,
          });

          $scope.pub_xysampling.publish($scope.msg_xysampling);
          $scope.pointxy = [];

          //=============================================================

        });
      });
    }

  };


  $scope.get_x_y_data_baru = function () {
    if (1) {
      $(document).ready(function () {
        $("#get_sampling_data").on("click", function (event) {

          //=============================================================
          //   this.offsetLeft 1096
          // this.offsetTop //132
          var x = event.pageX - this.offsetTop;
          var y = event.pageY - this.offsetLeft;
          // console.log(event.pageX, " ",  event.pageY);
          // console.log(this.offsetTop, " ", this.offsetLeft);

          $scope.point_x_sampling = Math.round(x);
          $scope.point_y_sampling = Math.round(y);
          // console.log($scope.point_x_sampling);
          // console.log($scope.point_y_sampling);



          $scope.pointxy.push(Math.round($scope.point_x_sampling))
          $scope.pointxy.push(Math.round($scope.point_y_sampling))



          //=============================================================

          $scope.msg_xysampling = new ROSLIB.Message({
            layout: { dim: [], data_offset: 0 },
            data: $scope.pointxy,
          });

          $scope.pub_xysampling.publish($scope.msg_xysampling);
          $scope.pointxy = [];

          //=============================================================

        });
      });
    }

  };

  function findPos(element) {
    // var curleft = 0;
    // var curtop = 0;

    // if (obj.offsetParent) {
    //   do {
    //     curleft += obj.offsetLeft;
    //     curtop += obj.offsetTop;
    //   } while (obj = obj.offsetParent);

    //   return { X: curleft, Y: curtop };
    // }

    var xPosition = 0;
    var yPosition = 0;

    while (element) {
      xPosition += (element.offsetLeft - element.scrollLeft + element.clientLeft);
      yPosition += (element.offsetTop - element.scrollTop + element.clientTop);
      element = element.offsetParent;
    }
    return { X: xPosition, Y: yPosition };
  }

  //--------------------------------------------------------------------------------------------------------------------
  //====================================================================================================================

  $scope.camera_params = [
    { label: "Brightness", value: 100, min: 0, max: 255 },
    { label: "Contrast", value: 150, min: 0, max: 255 },
    { label: "Saturation", value: 255, min: 0, max: 255 },
  ];

  $scope.top_camera_params = new ROSLIB.Topic({
    ros: $scope.ros,
    name: "camera_params",
    messageType: "std_msgs/UInt16MultiArray"
  });
  var camera_params_msg = new ROSLIB.Message({
    layout: { dim: [], data_offset: 0 },
    data: [0, 0, 0],
  });
  $scope.req_camera_params = new ROSLIB.ServiceRequest({});

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
    }
    else {
      myToggle.removeAttr('checked');
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
    $scope.pub_export_data.publish($scope.msg_export_data);
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


