# pkgconfig, real C code

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libcore/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../libwidgets/)

add_definitions(
    ${GLIB_CFLAGS} ${GIO_CFLAGS} ${GTHREAD_CFLAGS} ${GEE_CFLAGS} ${SQLITE_CFLAGS} 
    "-DGETTEXT_PACKAGE=\"marlin\""
     -O2
    )
link_libraries(${GLIB_LIBRARIES} ${GIO_LIBRARIES} ${GTHREAD_LIBRARIES} ${GEE_LIBRARIES} ${SQLITE_LIBRARIES})
vala_precompile(VALA_C
    marlind-tagging.vala
PACKAGES
    gio-2.0
    gee-1.0
    sqlite3
    )
add_executable(marlin-daemon
    ${VALA_C} )
#target_link_libraries(marlin-daemon ${DEPS_LIBRARIES})
install(TARGETS marlin-daemon RUNTIME DESTINATION bin)
