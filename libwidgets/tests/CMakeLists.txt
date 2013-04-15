include_directories(../../libcore/)
vala_precompile(VALA_C
    tests-pathbar.vala
PACKAGES
    gtk+-3.0
    gee-1.0
    posix
    marlinwidgets
OPTIONS
    --vapidir=${CMAKE_BINARY_DIR}/libwidgets/
    --vapidir=${CMAKE_BINARY_DIR}/libcore/
)

add_definitions(${GTK_CFLAGS} ${GEE_CFLAGS})
link_libraries(${GTK_LIBRARIES} ${GEE_CFLAGS})

add_executable(widgets-test ${VALA_C})

target_link_libraries(widgets-test marlinwidgets)
add_test_executable(widgets-test)
