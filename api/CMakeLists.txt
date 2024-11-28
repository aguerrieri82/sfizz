string(TIMESTAMP MAN_TODAY "%Y-%m-%d")


add_library(sfizz_api SHARED api.cpp)
target_link_libraries(sfizz_api PRIVATE
	sfizz::cxxopts
	sfizz::fmidi
	sfizz::internal
	st_audiofile_formats
)

sfizz_enable_lto_if_needed(sfizz_api)

install(TARGETS sfizz_api
	DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT "api" OPTIONAL
)
