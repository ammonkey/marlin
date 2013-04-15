set (PKGNAME marlincore)
set (PKGNAME_ MARLINCORE)
set (${PKGNAME_}_VERSION 0.1)
# increase when you broke the API

set (${PKGNAME_}_SOVERSION 0)
set (PREFIX ${CMAKE_INSTALL_PREFIX})
set (DOLLAR "$")
# You hear that? It's kittens being killed by the gods of cmake

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../eel)
vala_precompile(VALA_C
    gof-callwhenready.vala
    gof-directory-async.vala
    gof-preferences.vala
    PluginManager.vala
    Plugin.vala
    marlincore-C.vapi
PACKAGES
    gtk+-3.0
    gio-2.0
    gee-1.0
    gmodule-2.0
OPTIONS
    --thread
GENERATE_VAPI
    marlincore
GENERATE_HEADER
    marlincore
)
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/${PKGNAME}.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}.pc)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/marlincore.h DESTINATION include/marlincore/)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/marlincore.vapi DESTINATION share/vala/vapi)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/marlincore.deps DESTINATION share/vala/vapi)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/marlincore-C.vapi DESTINATION share/vala/vapi)
install (FILES
    eel-accessibility.h
    eel-debug.h
    eel-fcts.h
    eel-gdk-extensions.h
    eel-gdk-pixbuf-extensions.h
    eel-gio-extensions.h
    eel-glib-extensions.h
    eel-gtk-extensions.h
    eel-i18n.h
    eel-pango-extensions.h
    eel-stock-dialogs.h
    eel-string.h
    eel-ui.h
    eel-vfs-extensions.h
    gof-abstract-slot.h
    gof-file.h
    marlin-exec.h
    marlin-file-conflict-dialog.h
    marlin-file-operations.h
    marlin-undostack-manager.h
    marlin-file-changes-queue.h
    marlin-file-utilities.h
    marlin-icons.h
    marlin-abstract-sidebar.h
    marlin-progress-info.h
    marlin-progress-info-manager.h
    marlin-icon-info.h
    fm-list-model.h
    marlin-trash-monitor.h
    DESTINATION include/marlincore/)

add_definitions(
    ${GLIB_CFLAGS} ${GTHREAD_CFLAGS} ${GIO_CFLAGS}
    ${PANGO_CFLAGS} ${GTK_CFLAGS} ${GAIL_CFLAGS}
    ${GEE_CFLAGS} ${SQLITE_CFLAGS} ${NOTIFY_CFLAGS}
    "-DUI_DIR=\"${CMAKE_INSTALL_PREFIX}/share/marlin/ui/\""
    -Wall 
    -Wno-unused-but-set-variable -Wno-unused-function
    )
link_libraries(
    ${GLIB_LIBRARIES} ${GTHREAD_LIBRARIES} ${GIO_LIBRARIES}
    ${PANGO_LIBRARIES} ${GTK_LIBRARIES} ${GAIL_LIBRARIES}
    ${GEE_LIBRARIES} ${SQLITE_LIBRARIES} ${NOTIFY_LIBRARIES}
    )
add_library (${PKGNAME} SHARED
    eel-fcts.c
    eel-i18n.c
    eel-gtk-extensions.c
    eel-gdk-extensions.c
    eel-gdk-pixbuf-extensions.c
    eel-pango-extensions.c
    eel-glib-extensions.c
    eel-gio-extensions.c
    eel-stock-dialogs.c
    eel-debug.c
    eel-string.c
    eel-accessibility.c
    eel-ui.c
    eel-vfs-extensions.c
    marlin-file-operations.c
    marlin-undostack-manager.c
    marlin-file-changes-queue.c
    marlin-file-utilities.c
    marlin-file-conflict-dialog.c
    fm-list-model.c
    marlin-progress-info.c
    marlin-progress-info-manager.c
    marlin-abstract-sidebar.c
    marlin-exec.c
    gof-abstract-slot.c
    gof-file.c
    marlin-icon-info.c
    marlin-trash-monitor.c
    ${VALA_C}
)
if(BUILD_STATIC MATCHES "Yes")
add_library(${PKGNAME}-static STATIC
    eel-fcts.c
    eel-i18n.c
    eel-gtk-extensions.c
    eel-gdk-extensions.c
    eel-gdk-pixbuf-extensions.c
    eel-pango-extensions.c
    eel-glib-extensions.c
    eel-gio-extensions.c
    eel-stock-dialogs.c
    eel-debug.c
    eel-string.c
    eel-accessibility.c
    eel-ui.c
    eel-vfs-extensions.c
    marlin-file-operations.c
    marlin-undostack-manager.c
    marlin-file-changes-queue.c
    marlin-file-utilities.c
    marlin-file-conflict-dialog.c
    fm-list-model.c
    marlin-progress-info.c
    marlin-progress-info-manager.c
    marlin-abstract-sidebar.c
    marlin-exec.c
    gof-abstract-slot.c
    gof-file.c
    marlin-icon-info.c
    marlin-trash-monitor.c
    ${VALA_C}
)

SET_TARGET_PROPERTIES(${PKGNAME}-static PROPERTIES OUTPUT_NAME ${PKGNAME})
install (TARGETS ${PKGNAME}-static DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)
endif(BUILD_STATIC MATCHES "Yes")
set_target_properties (${PKGNAME} PROPERTIES
    VERSION ${MARLINCORE_VERSION}
    SOVERSION ${MARLINCORE_SOVERSION}
                       )
install (TARGETS ${PKGNAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)
add_executable(marlincore_tests
    tests/tests.c
    tests/marlincore-tests-gof.c
    tests/marlincore-tests-file.c
    tests/marlincore-tests-icon-info.c
)
add_test(core marlincore_tests)
target_link_libraries(marlincore_tests marlincore)
#target_link_libraries(marlincore ${DEPS_LC_LIBRARIES})
