var mainApp = angular.module("mainApp", []);


mainApp.controller("mainAppController", function ($scope, $interval) {
    $scope.ns = "";

    // Accessing the scope as global variable
    window.scope = $scope;

    $interval(function () { }, 1000);

    $scope.ros = new ROSLIB.Ros({
        url: 'ws://' + window.location.hostname + ':9900'
    });

    // ROS Connection Status
    $scope.ros.on('connection', function () {
        console.log('Connected to websocket server.');
    });

    $scope.checkedToggle = false;

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
        let myToggle = angular.element( document.querySelector('#toggle'));
        if(Number(msg.data) === 1){
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "VISION MAIN ON AND VISION ALL OFF";
        } else {
            myToggle.removeAttr('checked');
            $scope.variablechange = "VISION ALL ON AND VISION MAIN OFF";
        }
    })

    $scope.initToggle = function(){
        $scope.toggleSetter();
    };

    // Toggle Setter
    $scope.toggleSetter = function(){
        let myToggle = angular.element( document.querySelector('#toggle'));
        if($scope.checkedToggle){
            myToggle.attr('checked', 'checked');
            $scope.variablechange = "VISION MAIN ON AND VISION ALL OFF";
        }
        else{
            myToggle.removeAttr('checked');
            $scope.variablechange = "VISION ALL ON AND VISION MAIN OFF";
        }
    }

    // Callback Button Clicked
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
    $scope.toMain = function () {
        $scope.exportClickedMain();
        console.log("VISION MAIN ON");
    }
    $scope.toAll = function () {
        $scope.exportClickedAll();
        console.log("VISION ALL ON");
    }
    $scope.toGoalKeeper = function () {
        var goalKeeperUrl = "http://" + window.location.hostname + ":9999/master/goalkeeper.html"
        window.location.replace(goalKeeperUrl)
    }
    $scope.toVirtualPoints = function () {
        var field_url = "http://" + window.location.hostname + ":9999/master/virtual_points.html"
        window.location.replace(field_url)
      };
    $scope.toggleVision = function(e){
        console.log("i am triggered");
        console.log(e.checked);
        if(e.checked)
        {
            checkedToggle = 1;
            $scope.msg_vision_main_data_on.data = [1, 0, checkedToggle];
            $scope.exportClickedMain();
            $scope.variablechange = "VISION MAIN ON AND VISION ALL OFF";
        }
        if(!e.checked){
            checkedToggle =  0;
            $scope.msg_vision_all_data_on.data = [0, 1, checkedToggle];
            $scope.exportClickedAll();
            $scope.variablechange = "VISION ALL ON AND VISION MAIN OFF";
        }
    }
});

