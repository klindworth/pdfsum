cmake_minimum_required(VERSION 3.0)

project(pdfsum)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Boost REQUIRED)

INCLUDE_DIRECTORIES("/usr/include/poppler/qt5")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES 
	main.cpp 
	pdfview.cpp 
	pdfmarker.cpp
	documentsettings.cpp
	maindialog.cpp
	documentpage.cpp
	summarizedocument.cpp
	latexexportdialog.cpp
	documentmarkergui.cpp
	prerenderthread.cpp
	automarkthread.cpp
	latexrunner.cpp
	autocut.cpp
	pdfmarkeritem.cpp
	automarker.cpp
)

set(HEADERS 
	pdfview.h
	pdfmarker.h
	documentsettings.h
	maindialog.h
	documentpage.h
	summarizedocument.h
	latexexportdialog.h
	documentmarkergui.h
	prerenderthread.h
	automarkthread.h
	latexrunner.h
	autocut.h
	pdfmarkeritem.h
	automarker.h
	document_units.h
)

set(FORMS
	MainDialog.ui
	ExportDialog.ui
	DocumentMarkerGui.ui
	autocutdialog.ui
)

add_definitions(${QT_DEFINITIONS})

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets Qt5::Xml ${Boost_LIBRARIES} poppler-qt5
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS} -Wall -Wextra -std=c++14 -Og -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS} -Wall -Wextra -Wnoexcept -std=c++14 -DNDEBUG -O3")
