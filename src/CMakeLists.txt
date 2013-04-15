include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR}/marlinplugins/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../eel/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libcore/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../libcore/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../libwidgets/)

add_definitions(
    ${VARKA_CFLAGS} ${GAIL_CFLAGS}
    ${GEE_CFLAGS} ${SQLITE_CFLAGS} 
    ${DBUS_GLIB_CFLAGS} ${NOTIFY_CFLAGS}
    "-DGETTEXT_PACKAGE=\"marlin\""
    "-DPIXMAP_DIR=\"${CMAKE_INSTALL_PREFIX}/share/pixmaps/marlin/\""
     -O2
    )
link_libraries(
    ${VARKA_LIBRARIES} ${GAIL_CFLAGS}
    ${GEE_CFLAGS} ${SQLITE_CFLAGS} 
    ${DBUS_GLIB_LIBRARIES} ${NOTIFY_CFLAGS}
    )
vala_precompile(VALA_C
    marlin-deep-count.vala
    View/SettingsDialog.vala
    View/DiskRenderer.vala
    View/DirectoryNotFound.vala
    View/Window.vala
    View/Resources.vala
    View/DbusTags.vala
    View/ViewContainer.vala
    View/ContextView.vala
    View/OverlayBar.vala
    View/PropertiesWindow.vala
    View/Browser.vala
    View/ViewMode.vala
    View/Animations.vala
    View/LocationBar.vala
    View/Chrome/TopMenu.vala
    View/Chrome/ViewSwicher.vala
    View/Chrome/ColorWidget.vala
CUSTOM_VAPIS
    ${CMAKE_CURRENT_BINARY_DIR}/../libwidgets/marlinwidgets.vapi
    marlin.vapi
PACKAGES
    gtk+-3.0
    gio-2.0
    posix
    gee-1.0
    varka
    marlincore
    marlincore-C
GENERATE_HEADER
    marlin-vala
OPTIONS
    --vapidir=${CMAKE_SOURCE_DIR}/libcore/
    --vapidir=${CMAKE_BINARY_DIR}/libcore/
    --thread)
add_executable(marlin
    eel-editable-label.c
    marlin-application.c
    gossip-cell-renderer-expander.c
    marlin-enum-types.c
    marlin-bookmark.c
    marlin-bookmark-list.c
    unity-quicklist-handler.c
    marlin-dnd.c
    marlin-clipboard-manager.c
    marlin-places-sidebar.c
    marlin-window-columns.c
    gof-window-slot.c
    marlin-thumbnailer.c
    marlin-icon-renderer.c
    marlin-text-renderer.c
    marlin-cell-renderer-text-ellipsized.c
    marlin-mime-actions.c
    fm-directory-view.c
    exo-icon-view.c
    exo-tree-view.c
    marlin-main.c
    fm-abstract-icon-view.c
    fm-icon-view.c
    fm-list-view.c
    fm-compact-view.c
    fm-columns-view.c
    marlin-toolbar-editor.c
    marlin-connect-server-dialog.c
    marlin-connect-server-operation.c
    marlin-progress-ui-handler.c
    marlin-progress-info-widget.c
    ${VALA_C} )

add_definitions("-DPLUGIN_DIR=\"${CMAKE_INSTALL_PREFIX}/lib/marlin/plugins/\"")
target_link_libraries(marlin m marlincore marlinwidgets)

OPTION(WITH_UNITY "Add Unity launcher support" ON)
pkg_check_modules(UNITY unity>=4.0.0)

IF(WITH_UNITY AND UNITY_FOUND)
    include_directories(${UNITY_INCLUDE_DIRS})
    target_link_libraries (marlin marlincore marlinwidgets ${UNITY_LIBRARIES})
    add_definitions("-DHAVE_UNITY=1")
ENDIF(WITH_UNITY AND UNITY_FOUND)

install(TARGETS marlin RUNTIME DESTINATION bin)
include(Tests)
