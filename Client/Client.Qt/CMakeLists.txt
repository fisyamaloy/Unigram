set(TARGET Client.Qt)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(STYLE_SOURCES
        Style/Shadow.hpp
        Style/Style.cpp
        Style/Style.hpp
        Style/StyleColor.cpp
        Style/StyleColor.hpp
        Style/StyleFont.cpp
        Style/StyleFont.hpp
        Style/StyleIcon.cpp
        Style/StyleIcon.hpp
        Style/StyleScale.cpp
        Style/StyleScale.hpp
        Style/StyleTypes.cpp
        Style/StyleTypes.hpp

        Style/Palette.cpp
        Style/Palette.hpp
        Style/StyleBasic.hpp
        Style/StyleBasic.cpp
        Style/StyleIcons.cpp
        Style/StyleIcons.hpp
        Style/Styles.hpp
        Style/StyleWidgets.hpp
        Style/StyleWidgets.cpp)

set(UI_WIDGETS_SOURCES
        Widgets/AbstractButton.cpp
        Widgets/AbstractButton.hpp
        Widgets/Buttons.cpp
        Widgets/Buttons.hpp
        Widgets/ComboBox.cpp
        Widgets/ComboBox.hpp
        Widgets/InputFields.cpp
        Widgets/InputFields.hpp
        Widgets/Label.cpp
        Widgets/Label.hpp
        Widgets/ListWidget.cpp
        Widgets/ListWidget.hpp
        Widgets/LoadingSpinner.cpp
        Widgets/LoadingSpinner.hpp
        Widgets/LogoWidget.cpp
        Widgets/LogoWidget.hpp
        Widgets/Menu.cpp
        Widgets/Menu.hpp
        Widgets/MenuItem.cpp
        Widgets/MenuItem.hpp
        Widgets/MessageWidget.cpp
        Widgets/MessageWidget.hpp
        Widgets/ReplyWidget.cpp
        Widgets/ReplyWidget.hpp
        Widgets/ReplyMessageWidget.cpp
        Widgets/ReplyMessageWidget.hpp
        Widgets/Page.cpp
        Widgets/Page.hpp
        Widgets/PopupWidget.cpp
        Widgets/PopupWidget.hpp
        Widgets/ScrollArea.cpp
        Widgets/ScrollArea.hpp
        Widgets/TextEdit.cpp
        Widgets/TextEdit.hpp
        Widgets/TitleWidget.cpp
        Widgets/TitleWidget.hpp)

set(PAGES_SOURCES
        Pages/ChannelCreationPage.cpp
        Pages/ChannelCreationPage.hpp
        Pages/ChannelListPage.cpp
        Pages/ChannelListPage.hpp
        Pages/ChatPage.cpp
        Pages/ChatPage.hpp
        Pages/LoginPage.cpp
        Pages/LoginPage.hpp
        Pages/RegistrationPage.cpp
        Pages/RegistrationPage.hpp)

set(SOURCE_FILES
        Widgets/ChannelListWidget.cpp
        Widgets/ChannelListWidget.hpp
        Widgets/ChatHistory.cpp
        Widgets/ChatHistory.hpp
        Widgets/ChatWidget.cpp
        Widgets/ChatWidget.hpp
        Widgets/ChannelBar.cpp
        Widgets/ChannelBar.hpp
        Widgets/MessageWidget.cpp
        Widgets/MessageWidget.hpp
        Widgets/ReactionLayout.cpp
        Widgets/ReactionLayout.hpp

        Application.cpp
        Application.hpp
        ConnectionManager.cpp
        ConnectionManager.hpp

        main.cpp
        MainWidget.cpp
        MainWidget.hpp
        Settings.cpp
        Settings.hpp
        SettingsWidget.cpp
        SettingsWidget.hpp)

if (WIN32)
    list(APPEND EXTRA_LIBS ws2_32 wsock32 winmm dwmapi)
elseif (UNIX AND NOT APPLE)
    list(APPEND EXTRA_LIBS xcb)
endif ()

add_executable(${TARGET} ${SOURCE_FILES} ${STYLE_SOURCES} ${UI_WIDGETS_SOURCES} ${PAGES_SOURCES}
        resources/resources.qrc)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Boost CONFIG REQUIRED COMPONENTS asio)
find_package(mp3lame CONFIG REQUIRED)

target_link_libraries(${TARGET}
        PRIVATE Qt6::Widgets Qt::Gui Qt6::Multimedia ${EXTRA_LIBS} Boost::asio mp3lame::mp3lame
        PUBLIC Network.Public Client.Core.Static Utility.Public)

target_precompile_headers(${TARGET} INTERFACE QtWidgets.h)

# This needs to be an install or after install operation
# At this point it disrupts CI build so I commented it out =)

#if (WIN32)
#   set(Qt6_DIR $ENV{CMAKE_PREFIX_PATH})
#    message(STATUS ${6})
#    if (CMAKE_BUILD_TYPE MATCHES Debug)
#        file(COPY "${6}/bin/Qt6Cored.dll" "${6}/bin/Qt6Guid.dll"
#                "${6}/bin/Qt6Widgetsd.dll" "${6}/bin/Qt6Sqld.dll"
#                "${6}/plugins/platforms/qminimald.dll" "${6}/plugins/platforms/qoffscreend.dll"
#                "${6}/plugins/platforms/qwindowsd.dll"
#                DESTINATION "${CMAKE_BINARY_DIR}/bin")
#        message("Install Debug .dll's")
#    elseif (CMAKE_BUILD_TYPE MATCHES Release)
#        file(COPY "${6}/bin/Qt6Core.dll" "${6}/bin/Qt6Gui.dll"
#                "${6}/bin/Qt6Widgets.dll" "${6}/bin/Qt6Sqld.dll"
#                "${6}/plugins/platforms/qminimal.dll" "${6}/plugins/platforms/qoffscreen.dll"
#                "${6}/plugins/platforms/qwindows.dll"
#                DESTINATION "${CMAKE_BINARY_DIR}/bin")
#        message("Install Release .dll's")
#    endif ()
#    file(DOWNLOAD
#            "https://drive.google.com/u/0/uc?id=1_SrQNoI8lwsMD6bXFnQBAzP7PWGVeilo&export=download"
#            "${CMAKE_BINARY_DIR}/bin/glib-2.0-0.dll"
#            SHOW_PROGRESS)
#endif ()
