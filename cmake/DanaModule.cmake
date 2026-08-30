# dana_module(<name>
#   SOURCES <file> ...            # paths relative to src/<name>/Private
#   [PUBLIC_DEPENDS <module> ...] # modules named in this module's public headers
#   [PRIVATE_DEPENDS <module> ...]# modules used only by its implementation
# )
function(dana_module name)
  cmake_parse_arguments(MODULE "" "" "SOURCES;PUBLIC_DEPENDS;PRIVATE_DEPENDS" ${ARGN})

  if(NOT MODULE_SOURCES)
    message(FATAL_ERROR "dana_module(${name}): SOURCES is required")
  endif()

  set(module_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/${name}")

  list(TRANSFORM MODULE_SOURCES PREPEND "${module_dir}/Private/")

  add_library(${name} STATIC ${MODULE_SOURCES})

  target_include_directories(${name}
    PUBLIC "${module_dir}/Public"
    PRIVATE "${module_dir}/Private"
  )

  target_link_libraries(${name}
    PUBLIC ${MODULE_PUBLIC_DEPENDS}
    PRIVATE ${MODULE_PRIVATE_DEPENDS}
  )
endfunction()
