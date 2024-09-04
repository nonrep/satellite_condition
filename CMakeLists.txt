cmake_minimum_required(VERSION 3.14)

project(KA_states_client LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LOGGER_FILES    ${CMAKE_SOURCE_DIR}/logger/logger_boost.h
                    ${CMAKE_SOURCE_DIR}/logger/logger_boost_qtwrap.h)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS WebSockets)
find_package(Boost COMPONENTS system filesystem thread chrono locale log log_setup REQUIRED)
# add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
# INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

add_executable(KA_states_client
  main.cpp
  client.h
  client.cpp
  configmanager/configmanager.cpp
  configmanager/configmanager.h
  configmanager/jsonagent.cpp
  configmanager/jsonagent.h
  logger/logger_boost.h
  logger/logger_boost_qtwrap.h
)
target_link_libraries(KA_states_client Qt${QT_VERSION_MAJOR}::Core Qt5::WebSockets)
target_link_libraries(KA_states_client ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
                    ${Boost_REGEX_LIBRARY})
target_link_libraries(KA_states_client ${framework_LIBRARIES} ${Boost_LIBRARIES})


include(GNUInstallDirs)
install(TARGETS KA_states_client
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
