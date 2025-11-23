cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GluonApi")
include(FetchContent)

add_library (gluonapi SHARED "src/gluon/main.cpp" "src/gluon/Gluon.cpp" )
add_executable(injector "src/injector/main.cpp" "src/injector/injector.cpp")
target_include_directories(injector PRIVATE ${CMAKE_SOURCE_DIR}/include/injector)

target_include_directories(gluonapi PRIVATE
	${CMAKE_SOURCE_DIR}/include/gluon
    ${CMAKE_SOURCE_DIR}/extern/httplib/include
    ${CMAKE_SOURCE_DIR}/extern/asio/include
    ${CMAKE_SOURCE_DIR}/extern/crow/include
)



FetchContent_Declare(
    safetyhook
    GIT_REPOSITORY "https://github.com/cursey/safetyhook.git"
    GIT_TAG "origin/main"
)
FetchContent_MakeAvailable(safetyhook)


target_link_libraries(gluonapi PRIVATE safetyhook d3d9.lib ws2_32 mswsock)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET gluonapi PROPERTY CXX_STANDARD 23)
  set_property(TARGET injector PROPERTY CXX_STANDARD 23)
  target_link_options(injector PRIVATE "/INCREMENTAL:NO")
  target_link_options(gluonapi PRIVATE "/INCREMENTAL:NO")
endif()

