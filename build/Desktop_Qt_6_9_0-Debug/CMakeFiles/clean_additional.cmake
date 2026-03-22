# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/RobotMotionVisualizer_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/RobotMotionVisualizer_autogen.dir/ParseCache.txt"
  "RobotMotionVisualizer_autogen"
  )
endif()
