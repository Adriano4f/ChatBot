# Each test links only the module it exercises, through its public headers
add_executable(test_hash test_hash.c)
target_link_libraries(test_hash PRIVATE Hash)

add_executable(test_tokenise test_tokenise.c)
target_link_libraries(test_tokenise PRIVATE LI_Unit)

foreach(test_name test_hash test_tokenise)
  target_include_directories(${test_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
