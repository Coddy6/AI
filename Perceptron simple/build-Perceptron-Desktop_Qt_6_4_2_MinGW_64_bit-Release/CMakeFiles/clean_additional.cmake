# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Perceptron_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Perceptron_autogen.dir\\ParseCache.txt"
  "Perceptron_autogen"
  )
endif()
