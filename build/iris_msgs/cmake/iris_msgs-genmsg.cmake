# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "iris_msgs: 6 messages, 3 services")

set(MSG_I_FLAGS "-Iiris_msgs:/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg;-Istd_msgs:/opt/ros/noetic/share/std_msgs/cmake/../msg;-Igeometry_msgs:/opt/ros/noetic/share/geometry_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(gennodejs REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(iris_msgs_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg" ""
)

get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg" ""
)

get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg" ""
)

get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg" ""
)

get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg" ""
)

get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg" ""
)

get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv" ""
)

get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv" ""
)

get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv" NAME_WE)
add_custom_target(_iris_msgs_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "iris_msgs" "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv" ""
)

#
#  langs = gencpp;geneus;genlisp;gennodejs;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)
_generate_msg_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)
_generate_msg_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)
_generate_msg_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)
_generate_msg_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)
_generate_msg_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)

### Generating Services
_generate_srv_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)
_generate_srv_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)
_generate_srv_cpp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
)

### Generating Module File
_generate_module_cpp(iris_msgs
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(iris_msgs_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(iris_msgs_generate_messages iris_msgs_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_cpp _iris_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(iris_msgs_gencpp)
add_dependencies(iris_msgs_gencpp iris_msgs_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS iris_msgs_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)
_generate_msg_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)
_generate_msg_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)
_generate_msg_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)
_generate_msg_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)
_generate_msg_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)

### Generating Services
_generate_srv_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)
_generate_srv_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)
_generate_srv_eus(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
)

### Generating Module File
_generate_module_eus(iris_msgs
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(iris_msgs_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(iris_msgs_generate_messages iris_msgs_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_eus _iris_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(iris_msgs_geneus)
add_dependencies(iris_msgs_geneus iris_msgs_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS iris_msgs_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)
_generate_msg_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)
_generate_msg_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)
_generate_msg_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)
_generate_msg_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)
_generate_msg_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)

### Generating Services
_generate_srv_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)
_generate_srv_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)
_generate_srv_lisp(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
)

### Generating Module File
_generate_module_lisp(iris_msgs
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(iris_msgs_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(iris_msgs_generate_messages iris_msgs_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_lisp _iris_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(iris_msgs_genlisp)
add_dependencies(iris_msgs_genlisp iris_msgs_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS iris_msgs_generate_messages_lisp)

### Section generating for lang: gennodejs
### Generating Messages
_generate_msg_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)
_generate_msg_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)
_generate_msg_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)
_generate_msg_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)
_generate_msg_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)
_generate_msg_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)

### Generating Services
_generate_srv_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)
_generate_srv_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)
_generate_srv_nodejs(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
)

### Generating Module File
_generate_module_nodejs(iris_msgs
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
  "${ALL_GEN_OUTPUT_FILES_nodejs}"
)

add_custom_target(iris_msgs_generate_messages_nodejs
  DEPENDS ${ALL_GEN_OUTPUT_FILES_nodejs}
)
add_dependencies(iris_msgs_generate_messages iris_msgs_generate_messages_nodejs)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_nodejs _iris_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(iris_msgs_gennodejs)
add_dependencies(iris_msgs_gennodejs iris_msgs_generate_messages_nodejs)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS iris_msgs_generate_messages_nodejs)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)
_generate_msg_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)
_generate_msg_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)
_generate_msg_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)
_generate_msg_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)
_generate_msg_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)

### Generating Services
_generate_srv_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)
_generate_srv_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)
_generate_srv_py(iris_msgs
  "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
)

### Generating Module File
_generate_module_py(iris_msgs
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(iris_msgs_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(iris_msgs_generate_messages iris_msgs_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/bs2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/stm2pc.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/pc2stm.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/vision_status.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/data_robots.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/msg/goalpost.msg" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CameraConfig.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/YuvThresholdInit.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})
get_filename_component(_filename "/run/media/her/Data/IRIS/RISET_IRIS_ROBOT/src/iris_msgs/srv/CamOffset.srv" NAME_WE)
add_dependencies(iris_msgs_generate_messages_py _iris_msgs_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(iris_msgs_genpy)
add_dependencies(iris_msgs_genpy iris_msgs_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS iris_msgs_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/iris_msgs
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_cpp)
  add_dependencies(iris_msgs_generate_messages_cpp std_msgs_generate_messages_cpp)
endif()
if(TARGET geometry_msgs_generate_messages_cpp)
  add_dependencies(iris_msgs_generate_messages_cpp geometry_msgs_generate_messages_cpp)
endif()

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/iris_msgs
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_eus)
  add_dependencies(iris_msgs_generate_messages_eus std_msgs_generate_messages_eus)
endif()
if(TARGET geometry_msgs_generate_messages_eus)
  add_dependencies(iris_msgs_generate_messages_eus geometry_msgs_generate_messages_eus)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/iris_msgs
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_lisp)
  add_dependencies(iris_msgs_generate_messages_lisp std_msgs_generate_messages_lisp)
endif()
if(TARGET geometry_msgs_generate_messages_lisp)
  add_dependencies(iris_msgs_generate_messages_lisp geometry_msgs_generate_messages_lisp)
endif()

if(gennodejs_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/iris_msgs
    DESTINATION ${gennodejs_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_nodejs)
  add_dependencies(iris_msgs_generate_messages_nodejs std_msgs_generate_messages_nodejs)
endif()
if(TARGET geometry_msgs_generate_messages_nodejs)
  add_dependencies(iris_msgs_generate_messages_nodejs geometry_msgs_generate_messages_nodejs)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs)
  install(CODE "execute_process(COMMAND \"/usr/bin/python3\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/iris_msgs
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_py)
  add_dependencies(iris_msgs_generate_messages_py std_msgs_generate_messages_py)
endif()
if(TARGET geometry_msgs_generate_messages_py)
  add_dependencies(iris_msgs_generate_messages_py geometry_msgs_generate_messages_py)
endif()
