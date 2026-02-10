cmake_minimum_required(VERSION 3.20)
project(rscn-drivers
    VERSION 1.0.0
    DESCRIPTION "RSCN OS Driver Manager"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/hardwaredetector.cpp
    src/driverprofile.cpp
    src/packagemanager.cpp
)

set(HEADERS
    src/mainwindow.h
    src/hardwaredetector.h
    src/driverprofile.h
    src/packagemanager.h
)

set(RESOURCES
    resources/resources.qrc
)

qt_add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
)

# Install targets
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES assets/rscn-drivers.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES assets/rscn-drivers.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)

install(FILES assets/org.rscn.drivers.policy
    DESTINATION ${CMAKE_INSTALL_DATADIR}/polkit-1/actions
)
