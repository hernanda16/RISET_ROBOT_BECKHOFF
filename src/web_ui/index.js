const WIDTH = 1200;
const HEIGHT = 800;

// let regional = 1;
let regional = 0;

var odom_robot = [0, 0, 0];

var line_pcl_measure_x = [];
var line_pcl_measure_y = [];

var line_pcl_reference_x = [];
var line_pcl_reference_y = [];

var obs_merged_x = [];
var obs_merged_y = [];

var final_bola_x;
var final_bola_y;

var target_intercept_x = 100;
var target_intercept_y = 200;

var ball_waypoint_x = [];
var ball_waypoint_y = [];

var icp_score = 0;

var robot_movement_target_x = [0, 0, 0];

var ball_status = 0;

var scan_min = 0;
var scan_max = 0;

var status_offset = false;

var mouse_pos_x = 0;
var mouse_pos_y = 0;
var mouse_theta = 0;

var icp_target_offset = [0, 0, 0];

var titik_final_positioning_E_x = 0;
var titik_final_positioning_E_y = 0;

var ws_robot = new ROSLIB.Ros({
  url: "ws://" + window.location.hostname + ":9900",
});

var c = document.getElementById("myCanvas");
var ctx = c.getContext("2d");

// slider --------------------------------------------------
var slider_min = document.getElementById("slider_min");
var slider_max = document.getElementById("slider_max");

scan_min = document.getElementById("value_min"); // value dari slider min
scan_max = document.getElementById("value_max"); // value dari slider max

scan_min.innerHTML = slider_min.value; // Display the default slider value
scan_max.innerHTML = slider_max.value; // Display the default slider value

slider_min.oninput = function () {
  scan_min.innerHTML = this.value;
};

slider_max.oninput = function () {
  scan_max.innerHTML = this.value;
};

// ---------------------------------------------------

ws_robot.on("connection", function () {
  console.log("Connection made Robot 1!");
  is_robot_connected = 1;
});

ws_robot.on("close", function () {
  console.log("Connection closed Robot 1!");
  is_robot_connected = 0;
});

// ---------------------------------------------------

var sub_odom_robot = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/robot/pose",
  messageType: "geometry_msgs/Pose2D",
});

var sub_pcl_measure = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/line/pos",
  messageType: "sensor_msgs/PointCloud",
});

var sub_pcl_reference = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/obs/pcl",
  messageType: "sensor_msgs/PointCloud",
});

var sub_icp_score = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/line/icp_score",
  messageType: "std_msgs/Float32",
});

var sub_bola_pos = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/ball/final_pos",
  messageType: "geometry_msgs/Pose2D",
});

var sub_obs_merged = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/obs/shared_all",
  messageType: "sensor_msgs/PointCloud",
});

var sub_ball_waypoint = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/ball/waypoints",
  messageType: "sensor_msgs/PointCloud",
});

var sub_robot_movement_target = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/master/robot/movement_target",
  messageType: "geometry_msgs/Pose2D",
});

var sub_ball_status = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/ball/status",
  messageType: "std_msgs/Int32",
});

var sub_icp_target_offset = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/robot/icp_target_offset",
  messageType: "geometry_msgs/Pose2D",
});
var sub_target_intercept = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/master/ball/intercept",
  messageType: "geometry_msgs/Pose2D",
});

var sub_titik_final_positioning = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/world_model/positioning/titik_final_positioning",
  messageType: "std_msgs/Float32MultiArray",
});

// ---------------------------------------------------

sub_target_intercept.subscribe(function (message) {
  target_intercept_x = message.x;
  target_intercept_y = message.y;
});

sub_titik_final_positioning.subscribe(function (message) {
  titik_final_positioning_E_x = message.data[0];
  titik_final_positioning_E_y = message.data[1];

  console.log(titik_final_positioning_E_x, " ", titik_final_positioning_E_y);
});

// ---------------------------------------------------

sub_odom_robot.subscribe(function (message) {
  odom_robot[0] = message.x;
  odom_robot[1] = message.y;
  odom_robot[2] = message.theta;
});

sub_pcl_measure.subscribe(function (message) {
  line_pcl_measure_x = [];
  line_pcl_measure_y = [];
  for (let i = 0; i < message.points.length; i++) {
    line_pcl_measure_x.push(message.points[i].x);
    line_pcl_measure_y.push(message.points[i].y);
  }
});

sub_pcl_reference.subscribe(function (message) {
  line_pcl_reference_x = [];
  line_pcl_reference_y = [];
  for (let i = 0; i < message.points.length; i++) {
    line_pcl_reference_x.push(message.points[i].x);
    line_pcl_reference_y.push(message.points[i].y);
  }
});

sub_bola_pos.subscribe(function (message) {
  final_bola_x = message.x;
  final_bola_y = message.y;
});

sub_icp_score.subscribe(function (message) {
  icp_score = message.data;
});

sub_obs_merged.subscribe(function (message) {
  obs_merged_x = [];
  obs_merged_y = [];
  for (let i = 0; i < message.points.length; i++) {
    obs_merged_x.push(message.points[i].x);
    obs_merged_y.push(message.points[i].y);
  }
});

sub_ball_waypoint.subscribe(function (message) {
  ball_waypoint_x = [];
  ball_waypoint_y = [];
  for (let i = 0; i < message.points.length; i++) {
    ball_waypoint_x.push(message.points[i].x);
    ball_waypoint_y.push(message.points[i].y);
  }
});

sub_robot_movement_target.subscribe(function (message) {
  robot_movement_target_x[0] = message.x;
  robot_movement_target_x[1] = message.y;
  robot_movement_target_x[2] = message.theta;
});

sub_ball_status.subscribe(function (message) {
  ball_status = message.data;
});

sub_icp_target_offset.subscribe(function (message) {
  icp_target_offset[0] = message.x;
  icp_target_offset[1] = message.y;
  icp_target_offset[2] = message.theta;
});

// ---------------------------------------------------

var pub_line_distance_threshold = new ROSLIB.Topic({
  ros: ws_robot,
  name: "/web_ui/line_distance_threshold",
  messageType: "std_msgs/UInt16MultiArray",
});

var msg_line_distance_threshold = new ROSLIB.Message({
  data: [scan_min, scan_max], // data yang dikirim
});

function publish_line_distance_threshold() {
  pub_line_distance_threshold.publish(msg_line_distance_threshold);
}

// button offset
function offset() {
  console.log("on");
  status_offset = true;
}

// jika mouse ditekan
document.addEventListener("mousedown", function (event) {
  if (event.target.classList.contains("lapangan")) {
    if (status_offset) {
      let rect = c.getBoundingClientRect();

      mouse_pos_x = event.clientX - rect.left - 100;
      mouse_pos_y = event.clientY - rect.top - 100;
    }
  }
});

// jika mouse dilepas
document.body.addEventListener("mouseup", function (event) {
  if (event.target.classList.contains("lapangan")) {
    if (status_offset) {
      let rect = c.getBoundingClientRect();

      let drag_x = event.clientX - rect.left - 100 - mouse_pos_x;
      let drag_y = event.clientY - rect.top - 100 - mouse_pos_y;

      mouse_theta = Math.floor((180 / Math.PI) * Math.atan2(-drag_y, drag_x));

      // console.log(
      //   "pos(" + mouse_pos_x + "," + mouse_pos_y + ")\tth:" + mouse_theta
      // );

      status_offset = false;
      // console.log("off");
    }
  }

  // publish msg_line_distance_threshold slider ketika mouse diangkat dari slider
  if (event.target.classList.contains("slider")) {
    publish_line_distance_threshold();
  }
});

// --------------------INT MAIN-----------------------

setInterval(() => {
  drawPose();
  // console.log(odom_robot[0], odom_robot[1], odom_robot[2]);
  // console.log(icp_target_offset[0], icp_target_offset[1], icp_target_offset[2]);
}, 50);

// ---------------------------------------------------

function drawPose() {
  ctx.clearRect(0, 0, c.width, c.height);
  ctx.save();

  if (regional) {
    c.width = 800;
    c.height = 1000;

    let back = new Path2D();
    back.moveTo(cm2px_x(-100), cm2px_y(-100));
    back.lineTo(cm2px_x(1200), cm2px_y(-100));
    back.lineTo(cm2px_x(1200), cm2px_y(900));
    back.lineTo(cm2px_x(-100), cm2px_y(900));
    back.lineTo(cm2px_x(-100), cm2px_y(-100));
    // back.stroke();
    back.closePath();

    // ctx.rotate(90 * Math.PI / 180);

    ctx.fillStyle = "green";
    ctx.fill(back, "evenodd");

    ctx.lineWidth = 2;
    ctx.strokeStyle = "white";

    ctx.moveTo(0, 0);
    ctx.lineTo(0, 1000);
    ctx.lineTo(800, 1000);
    ctx.lineTo(800, 0);
    ctx.lineTo(0, 0);

    ctx.moveTo(100, 100);
    ctx.lineTo(700, 100);
    ctx.lineTo(700, 900);
    ctx.lineTo(100, 900);
    ctx.lineTo(100, 100);

    ctx.moveTo(700, 400);
    ctx.lineTo(250, 400);

    ctx.moveTo(700, 600);
    ctx.lineTo(250, 600);

    ctx.moveTo(700, 600);
    ctx.lineTo(250, 600);

    ctx.moveTo(100, 300);
    ctx.lineTo(250, 300);
    ctx.lineTo(250, 700);
    ctx.lineTo(100, 700);

    //kotak base robot
    ctx.moveTo(100 + 362.5, 100);
    ctx.lineTo(100 + 362.5, 175);
    ctx.lineTo(100 + 437.5, 175);
    ctx.lineTo(100 + 437.5, 100);

    ctx.moveTo(100 + 362.5, 900);
    ctx.lineTo(100 + 362.5, 825);
    ctx.lineTo(100 + 437.5, 825);
    ctx.lineTo(100 + 437.5, 900);

    //garis base robot
    //------------------
    ctx.moveTo(100 + 362.5 + 37.5, 175);
    ctx.lineTo(100 + 362.5 + 37.5, 400);

    ctx.moveTo(100 + 362.5 + 37.5, 900 - 75);
    ctx.lineTo(100 + 362.5 + 37.5, 600);
    //-------------------

    ctx.moveTo(700, 500);
    ctx.arc(700, 500, 100, 0.5 * Math.PI, 1.5 * Math.PI);

    ctx.moveTo(100, 100);
    ctx.arc(100, 100, 50, 0, 0.5 * Math.PI);

    ctx.moveTo(100, 900);
    ctx.arc(100, 900, 50, 1.5 * Math.PI, 2 * Math.PI);

    // ctx.moveTo(cm2px_x(400), cm2px_y(700));
    // ctx.arc(cm2px_x(400), cm2px_y(700), 10, 0 * Math.PI, 2 * Math.PI);

    // ctx.moveTo(0, 800);
    // ctx.arc(0, 800, 50, Math.PI, 2 * Math.PI);

    // ctx.moveTo(1020, 150);
    // ctx.lineTo(1020, 660);

    // ctx.moveTo(0, 200);
    // ctx.lineTo(50, 200);
    // ctx.lineTo(50, 600);
    // ctx.lineTo(0, 600);

    ctx.stroke();
    //
  } else {
    //
    let back = new Path2D();
    back.moveTo(cm2px_x(-100), cm2px_y(-100));
    back.lineTo(cm2px_x(1300), cm2px_y(-100));
    back.lineTo(cm2px_x(1300), cm2px_y(900));
    back.lineTo(cm2px_x(-100), cm2px_y(900));
    back.lineTo(cm2px_x(-100), cm2px_y(-100));
    // back.stroke();
    back.closePath();

    ctx.fillStyle = "green";
    ctx.fill(back, "evenodd");

    let lap = new Path2D();
    lap.moveTo(cm2px_x(0), cm2px_y(0));
    lap.lineTo(cm2px_x(1200), cm2px_y(0));
    lap.lineTo(cm2px_x(1200), cm2px_y(800));
    lap.lineTo(cm2px_x(0), cm2px_y(800));
    lap.lineTo(cm2px_x(0), cm2px_y(0));
    // lap.stroke();
    lap.closePath();

    ctx.fillStyle = "green";
    ctx.fill(lap, "evenodd");

    ctx.lineWidth = 2;
    ctx.strokeStyle = "white";
    ctx.moveTo(cm2px_x(0), cm2px_y(0));
    ctx.lineTo(cm2px_x(1200), cm2px_y(0));
    ctx.lineTo(cm2px_x(1200), cm2px_y(800));
    ctx.lineTo(cm2px_x(0), cm2px_y(800));
    ctx.lineTo(cm2px_x(0), cm2px_y(0));

    // garis tengah
    ctx.moveTo(cm2px_x(1200 / 2), cm2px_y(0));
    ctx.lineTo(cm2px_x(1200 / 2), cm2px_y(800));

    // lingkaran tengah besar
    ctx.moveTo(cm2px_x(1200 / 2), cm2px_y(800 / 2));
    ctx.arc(
      cm2px_x(1200 / 2),
      cm2px_y(800 / 2),
      260 / 2,
      0.5 * Math.PI,
      1.5 * Math.PI
    );
    ctx.arc(
      cm2px_x(1200 / 2),
      cm2px_y(800 / 2),
      260 / 2,
      1.5 * Math.PI,
      2.5 * Math.PI
    );

    // lingkaran tengah kecil
    ctx.moveTo(cm2px_x(1200 / 2), cm2px_y(800 / 2));
    ctx.arc(cm2px_x(1200 / 2), cm2px_y(800 / 2), 10, 0, 2 * Math.PI);

    //lingkaran corner
    ctx.moveTo(cm2px_x(WIDTH), cm2px_y(HEIGHT));
    ctx.arc(cm2px_x(WIDTH), cm2px_y(HEIGHT), 50, 1 * Math.PI, 1.5 * Math.PI);

    ctx.moveTo(cm2px_x(0), cm2px_y(HEIGHT));
    ctx.arc(cm2px_x(0), cm2px_y(HEIGHT), 50, 1.5 * Math.PI, 2 * Math.PI);

    ctx.moveTo(cm2px_x(0), cm2px_y(0));
    ctx.arc(cm2px_x(0), cm2px_y(0), 50, 2 * Math.PI, 2.5 * Math.PI);

    ctx.moveTo(cm2px_x(WIDTH), cm2px_y(0));
    ctx.arc(cm2px_x(WIDTH), cm2px_y(0), 50, 0.5 * Math.PI, 1 * Math.PI);

    // kotak gawang
    ctx.moveTo(cm2px_x(0), cm2px_y(400 - 100 - 50));
    ctx.lineTo(cm2px_x(50), cm2px_y(400 - 100 - 50));
    ctx.lineTo(cm2px_x(50), cm2px_y(400 + 100 + 50));
    ctx.lineTo(cm2px_x(0), cm2px_y(400 + 100 + 50));

    ctx.moveTo(cm2px_x(WIDTH), cm2px_y(400 - 100 - 50));
    ctx.lineTo(cm2px_x(WIDTH - 50), cm2px_y(400 - 100 - 50));
    ctx.lineTo(cm2px_x(WIDTH - 50), cm2px_y(400 + 100 + 50));
    ctx.lineTo(cm2px_x(WIDTH), cm2px_y(400 + 100 + 50));

    // kotak penalti
    ctx.moveTo(cm2px_x(0), cm2px_y(400 - 100 - 150));
    ctx.lineTo(cm2px_x(180), cm2px_y(400 - 100 - 150));
    ctx.lineTo(cm2px_x(180), cm2px_y(400 + 100 + 150));
    ctx.lineTo(cm2px_x(0), cm2px_y(400 + 100 + 150));

    ctx.moveTo(cm2px_x(WIDTH), cm2px_y(400 - 100 - 150));
    ctx.lineTo(cm2px_x(WIDTH - 180), cm2px_y(400 - 100 - 150));
    ctx.lineTo(cm2px_x(WIDTH - 180), cm2px_y(400 + 100 + 150));
    ctx.lineTo(cm2px_x(WIDTH), cm2px_y(400 + 100 + 150));

    // kotak gawang belakang
    ctx.moveTo(cm2px_x(0), cm2px_y(400 - 100));
    ctx.lineTo(cm2px_x(-25), cm2px_y(400 - 100));
    ctx.lineTo(cm2px_x(-25), cm2px_y(400 + 100));
    ctx.lineTo(cm2px_x(0), cm2px_y(400 + 100));

    ctx.moveTo(cm2px_x(WIDTH + 0), cm2px_y(400 - 100));
    ctx.lineTo(cm2px_x(WIDTH + 25), cm2px_y(400 - 100));
    ctx.lineTo(cm2px_x(WIDTH + 25), cm2px_y(400 + 100));
    ctx.lineTo(cm2px_x(WIDTH + 0), cm2px_y(400 + 100));

    ctx.stroke();

    for (let i = 1; i <= 3; i++) {
      //
      let y = cm2px_y(i * 200);

      ctx.moveTo(cm2px_x(200), y);
      ctx.arc(cm2px_x(200), y, 3, 0, 2 * Math.PI);
      ctx.moveTo(cm2px_x(WIDTH / 2), y);
      ctx.arc(cm2px_x(WIDTH / 2), y, 3, 0, 2 * Math.PI);
      ctx.moveTo(cm2px_x(WIDTH - 200), y);
      ctx.arc(cm2px_x(WIDTH - 200), y, 3, 0, 2 * Math.PI);
      ctx.stroke();
    }

    ctx.stroke();
  }

  // ICP target offset
  drawRobot(
    ctx,
    icp_target_offset[0],
    icp_target_offset[1],
    icp_target_offset[2],
    "yellow",
    0
  );

  // robot
  drawRobot(ctx, odom_robot[0], odom_robot[1], odom_robot[2], "cyan", 1);

  // bola
  ctx.fillStyle = "red";
  ctx.beginPath();
  ctx.arc(cm2px_x(final_bola_y), cm2px_y(final_bola_x), 10, 0, 2 * Math.PI);
  ctx.fill();
  ctx.stroke();

  // draw line measure point cloud
  ctx.beginPath();
  ctx.fillStyle = "yellow";
  for (let i = 0; i < line_pcl_measure_x.length; i++) {
    ctx.fillRect(
      cm2px_x(line_pcl_measure_y[i]),
      cm2px_y(line_pcl_measure_x[i]),
      10,
      10
    ); // kotak robot
  }

  // draw line measure point cloud
  ctx.beginPath();
  ctx.fillStyle = "purple";
  for (let i = 0; i < line_pcl_reference_x.length; i++) {
    ctx.fillRect(
      cm2px_x(line_pcl_reference_y[i]) - 5,
      cm2px_y(line_pcl_reference_x[i]) - 5,
      10,
      10
    ); // kotak robot
  }

  // draw obs merged
  ctx.beginPath();
  ctx.fillStyle = "black";
  for (let i = 0; i < obs_merged_x.length; i++) {
    ctx.fillRect(
      cm2px_x(obs_merged_y[i] - 26),
      cm2px_y(obs_merged_x[i] - 26),
      52,
      52
    ); // kotak robot
  }

  // // draw target intercept

  // draw target intercept
  // drawRobot(
  //   ctx,
  //   cm2px_x(target_intercept_y),
  //   cm2px_y(target_intercept_x),
  //   odom_robot[2],
  //   "blue"
  // );

  // draw ball waypoint
  // if (ball_status > 0) {
  ctx.beginPath();
  ctx.fillStyle = "orange";
  for (let i = 0; i < ball_waypoint_x.length; i++) {
    ctx.arc(
      cm2px_x(ball_waypoint_y[i]),
      cm2px_y(ball_waypoint_x[i]),
      5,
      0,
      2 * Math.PI
    );
    ctx.fill();
  }
  // }

  ctx.beginPath();
  ctx.fillStyle = "blue";
  ctx.arc(
    cm2px_x(target_intercept_y),
    cm2px_y(target_intercept_x),
    30,
    0,
    2 * Math.PI
  );
  ctx.fill();

  // ctx.beginPath();
  // ctx.fillStyle = "orange";
  // ctx.arc(
  //   cm2px_x(titik_final_positioning_E_y),
  //   cm2px_y(titik_final_positioning_E_x),
  //   30,
  //   0,
  //   2 * Math.PI
  // );
  // ctx.fill();

  ctx.restore();

  // icp score
  ctx.font = "30px Arial";
  ctx.fillStyle = "black";
  ctx.fillText("ICP Score: " + icp_score, 10, 50);
}

function drawRobot(ctx, x, y, yaw, color, is_special = 0) {
  const x_scaled = cm2px_x(y);
  const y_scaled = cm2px_y(x);

  ctx.beginPath();
  ctx.arc(x_scaled, y_scaled, 26, 0, 2 * Math.PI);
  ctx.fillStyle = color;
  ctx.fill();

  if (is_special == 1) {
    ctx.lineWidth = 5;
    ctx.strokeStyle = "#FFFF";
  } else {
    ctx.lineWidth = 3.5;
    ctx.strokeStyle = "#000";
  }

  ctx.stroke();

  // Draw a line indicating the orientation of the robot
  const angle = yaw; // Change this angle based on the orientation
  var lineLength = 26;
  if (is_special == 1) {
    lineLength = 1000;
  }
  const dx = lineLength * Math.sin(angle * (Math.PI / 180));
  const dy = lineLength * Math.cos(angle * (Math.PI / 180));

  ctx.beginPath();
  ctx.moveTo(x_scaled, y_scaled);
  ctx.lineTo(x_scaled + dx, y_scaled + dy);
  ctx.strokeStyle = "white";
  ctx.stroke();
}

function cm2px_x(x) {
  return x + 100;
}

function cm2px_y(y) {
  return y + 100;
}
