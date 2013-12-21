
set(MYTGET_SOURCES advio.cpp block.cpp debug.cpp downloader.cpp ftp.cpp ftpparser.cpp ftpplugin.cpp header.cpp http.cpp httpplugin.cpp mytget.cpp plugin.cpp progressbar.cpp proxy.cpp task.cpp tcp.cpp url.cpp utils.cpp)

add_executable(mytget ${MYTGET_SOURCES})
target_link_libraries(mytget ${MYTGET_EXTERNAL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
INSTALL(TARGETS mytget DESTINATION bin)

