include_directories(${CMAKE_BINARY_DIR}/libcore/)
include_directories(${CMAKE_SOURCE_DIR}/libcore/)

add_definitions(${GTK_CFLAGS} ${GEE_CFLAGS})
link_libraries(${GTK_LIBRARIES} ${GEE_LIBRARIES})

vala_precompile(VALA_C
    plugin.vala
CUSTOM_VAPIS
    ${CMAKE_BINARY_DIR}/libcore/marlincore.vapi
    ${CMAKE_SOURCE_DIR}/libcore/marlincore-C.vapi
    ${CMAKE_BINARY_DIR}/libwidgets/marlinwidgets.vapi
PACKAGES
    gtk+-3.0
    gee-1.0
OPTIONS
    --thread)

add_library(marlin-extended-actions SHARED
    ${VALA_C})
target_link_libraries(marlin-extended-actions marlincore marlinwidgets)
install(TARGETS marlin-extended-actions DESTINATION lib/marlin/plugins/core/)
install(FILES extended-actions.plug DESTINATION lib/marlin/plugins/core/)
