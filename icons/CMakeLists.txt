install(FILES thumbnail_frame.png DESTINATION share/pixmaps/marlin/)
install(FILES marlin.svg view-list-column-symbolic.svg view-list-compact-symbolic.svg view-list-details-symbolic.svg view-list-icons-symbolic.svg DESTINATION share/icons/hicolor/scalable/apps/)

option (ICON_UPDATE "Run gtk-update-icon-cache after the install." ON)
if (ICON_UPDATE)
  install (CODE "message(\"-- Updating icon cache...\")")
  install (CODE "execute_process(COMMAND gtk-update-icon-cache -f -t ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor)")
endif ()
