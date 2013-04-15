include_directories(${CMAKE_SOURCE_DIR}/libcore/)
include_directories(${CMAKE_BINARY_DIR}/libcore/)

vala_precompile(VALA_C
    plugin.vala
PACKAGES
    gtk+-3.0
    gee-1.0
    posix
    marlincore-C
    marlincore
OPTIONS
    --thread
    --vapidir=${CMAKE_SOURCE_DIR}/libcore/
    --vapidir=${CMAKE_BINARY_DIR}/libcore/
)
add_definitions(${GTK_CFLAGS} ${GIO_CFLAGS} ${GEE_CFLAGS})
link_libraries(${GTK_LIBRARIES} ${GIO_LIBRARIES} ${GEE_LIBRARIES})
add_library(marlinctags SHARED 
    ${VALA_C})
target_link_libraries(marlinctags marlincore)
install(TARGETS marlinctags DESTINATION lib/marlin/plugins/core/)
install(FILES marlinctags.plug DESTINATION lib/marlin/plugins/core/)
include_directories(${CMAKE_BINARY_DIR}/marlinplugins/)
