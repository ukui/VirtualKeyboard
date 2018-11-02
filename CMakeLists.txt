find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
qt5_add_resources(VirtualKeyboard_SRC
	src/keyboard.qrc)

set(VirtualKeyboard_SRC
	${VirtualKeyboard_SRC}
	src/cursormonitor.cpp
	src/keyboardwidget.cpp
	src/virtualkeyboard.cpp
	src/x11keyboard.cpp
	src/keyboard.qrc)

include_directories(
	${Qt5Core_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS}
	)

add_library(VirtualKeyboard OBJECT ${VirtualKeyboard_SRC})
