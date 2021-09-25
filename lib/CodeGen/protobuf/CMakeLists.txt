INCLUDE(FindProtobuf)
# Create protobuf .h and .cc files, and put them in a library for use
find_package(Protobuf REQUIRED)
add_definitions(-DGOOGLE_PROTOBUF_NO_RTTI)

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS IRInfo.proto)
# FILE(GLOB proto_files ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)  
# FOREACH(proto_file ${proto_files})
#     STRING(REGEX REPLACE "[^/]proto" "" proto_file_name ${proto_file})
#     LIST(APPEND PROTO_SRCS "${proto_file_name}.pb.cc")
#     LIST(APPEND PROTO_HDRS "${proto_file_name}.pb.h")
# 	ADD_CUSTOM_COMMAND(
# 		OUTPUT "${proto_file_name}.pb.h" "${proto_file_name}.pb.cc"
# 		COMMAND protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
#                        --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} ${proto_file} 
# 		DEPENDS ${proto_file}
# 	)
# ENDFOREACH(proto_file)


add_llvm_component_library(irproto
  ${PROTO_SRCS}
  )

target_link_libraries(irproto ${PROTOBUF_LIBRARY})