set (PKGNAME marlinwidgets)
set (PKGNAME_ MARLINWIDGETS)
set (${PKGNAME_}_VERSION 0.1)
# increase when you broke the API

set (${PKGNAME_}_SOVERSION 0)
set (PREFIX ${CMAKE_INSTALL_PREFIX})
set (DOLLAR "$")

#install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PKGNAME}.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/)

add_definitions(
    ${VARKA_CFLAGS} ${GTK_CFLAGS} ${GEE_CFLAGS} 
    "-DGETTEXT_PACKAGE=\"${PKGNAME_}\"")
link_libraries(${VARKA_LIBRARIES} ${GTK_LIBRARIES} ${GEE_LIBRARIES})

vala_precompile(VALA_C
    LocationBar.vala
    BreadcrumbsElements.vala
    BreadcrumbsEntry.vala
PACKAGES
    gtk+-3.0
    gee-1.0
    posix
    varka
OPTIONS
    --thread
GENERATE_VAPI
    marlinwidgets
GENERATE_HEADER
    marlinwidgets
)
add_library (${PKGNAME} SHARED
    ${VALA_C}
)
if(BUILD_STATIC MATCHES "Yes")
ADD_LIBRARY(${PKGNAME}-static STATIC ${VALA_C})

SET_TARGET_PROPERTIES(${PKGNAME}-static PROPERTIES OUTPUT_NAME ${PKGNAME})
install (TARGETS ${PKGNAME}-static DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)
endif(BUILD_STATIC MATCHES "Yes")
set_target_properties (${PKGNAME} PROPERTIES
    VERSION ${MARLINWIDGETS_VERSION}
    SOVERSION ${MARLINWIDGETS_SOVERSION}
                       )
install (TARGETS ${PKGNAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)
add_subdirectory(tests)
