find_package(MSGFMT REQUIRED)
FILE(GLOB_RECURSE PO_FILES *.po)

add_translations(kcm_cgc ${PO_FILES})