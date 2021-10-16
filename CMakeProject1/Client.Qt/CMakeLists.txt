set(TARGET Client.Qt)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE_FILES 
        main.cpp)

if (WIN32)
    list(APPEND EXTRA_LIBS ws2_32 wsock32 winmm dwmapi)
elseif (UNIX AND NOT APPLE)
    list(APPEND EXTRA_LIBS xcb)
endif ()

# This needs to be an install or after install operation
# At this point it disrupts CI build so I commented it out =)

if (WIN32)
   set(Qt5_DIR $ENV{CMAKE_PREFIX_PATH})
    message(STATUS ${Qt5_DIR})
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        file(COPY "${Qt5_DIR}/bin/Qt5Cored.dll" "${Qt5_DIR}/bin/Qt5Guid.dll"
                "${Qt5_DIR}/bin/Qt5Widgetsd.dll" "${Qt5_DIR}/plugins/platforms/qminimald.dll"
                "${Qt5_DIR}/plugins/platforms/qoffscreend.dll" "${Qt5_DIR}/plugins/platforms/qwindowsd.dll"
                DESTINATION "${CMAKE_BINARY_DIR}/bin")
        message("Install Debug .dll's")
    elseif (CMAKE_BUILD_TYPE MATCHES Release)
        file(COPY "${Qt5_DIR}/bin/Qt5Core.dll" "${Qt5_DIR}/bin/Qt5Gui.dll"
                "${Qt5_DIR}/bin/Qt5Widgets.dll" "${Qt5_DIR}/plugins/platforms/qminimal.dll"
                "${Qt5_DIR}/plugins/platforms/qoffscreen.dll" "${Qt5_DIR}/plugins/platforms/qwindows.dll"
                DESTINATION "${CMAKE_BINARY_DIR}/bin")
        message("Install Release .dll's")
    endif ()
    file(DOWNLOAD
            "https://drive.google.com/u/0/uc?id=1_SrQNoI8lwsMD6bXFnQBAzP7PWGVeilo&export=download"
            "${CMAKE_BINARY_DIR}/bin/glib-2.0-0.dll"
            SHOW_PROGRESS)
endif ()
