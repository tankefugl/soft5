project (soft-tools-plugins-hd5 C CXX)

set (CMAKE_THREAD_PREFER_PTHREAD  TRUE)
set (CMAKE_INCLUDE_CURRENT_DIR    ON)
set (CMAKE_AUTOMOC                ON)

include_directories (
  ${soft-storage-hdf5_INCLUDE_DIRS}
  "../common")

set (LINK_LIBS ${LINK_LIBS} ${soft-storage-hdf5_LIBS})
aux_source_directory (. SRC_LIST)

add_definitions(-DQT_PLUGIN)
add_library (${PROJECT_NAME} SHARED ${SRC_LIST})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_constexpr)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${SCRIPT_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${SCRIPT_PLUGINS_INSTALL_DIR})
