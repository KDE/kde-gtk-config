set(SOURCES "window_decorations.scss" "adwaita_colors.scss" "build_decorations_css.sh")
set(DECORATIONS_CSS_PATH "${PROJECT_BINARY_DIR}/kded/scss")
set(CSS_OUTPUT "${DECORATIONS_CSS_PATH}/window_decorations.css" "${DECORATIONS_CSS_PATH}/adwaita_colors.css")

add_custom_command(
    OUTPUT ${CSS_OUTPUT}
    DEPENDS ${SOURCES}
    COMMAND ./build_decorations_css.sh "${DECORATIONS_CSS_PATH}"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/kded/scss"
)
add_custom_target(BreezeGTKWindowDecorationsCSS ALL DEPENDS ${CSS_OUTPUT})

install(
    FILES ${CSS_OUTPUT}
    DESTINATION "${KDE_INSTALL_FULL_DATAROOTDIR}/themes/Breeze"
)
