# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/IDPPCOMRECV_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/IDPPCOMRECV_autogen.dir/ParseCache.txt"
  "IDPPCOMRECV_autogen"
  )
endif()
