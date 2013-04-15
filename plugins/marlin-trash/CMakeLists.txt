include_directories(${CMAKE_SOURCE_DIR}/libcore/)
include_directories(${CMAKE_BINARY_DIR}/libcore/)

vala_precompile(VALA_C
    plugin.vala
PACKAGES
    gtk+-3.0
    gee-1.0
    marlincore-C
    marlincore
OPTIONS
    --thread
    --vapidir=${CMAKE_SOURCE_DIR}/libcore/
    --vapidir=${CMAKE_BINARY_DIR}/libcore/
)

add_definitions(${GTK_CFLAGS} ${GEE_CFLAGS})
link_libraries(${GTK_LIBRARIES} ${GEE_CFLAGS})
add_library(marlintrash SHARED 
    ${VALA_C})
target_link_libraries(marlintrash marlincore)
install(TARGETS marlintrash DESTINATION lib/marlin/plugins/core/)
install(FILES marlintrash.plug DESTINATION lib/marlin/plugins/core/)
include_directories(${CMAKE_BINARY_DIR}/marlinplugins/)
