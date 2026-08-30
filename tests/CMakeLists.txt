set(DANA_TESTS
  test_hashmap
  test_tokenizer
  test_vocab
  test_ngram
  test_chat
)

set(test_hashmap_MODULE   Collections)
set(test_tokenizer_MODULE Text)
set(test_vocab_MODULE     Vocab)
set(test_ngram_MODULE     NGram)
set(test_chat_MODULE      Chat)

# Each test links only the module it exercises, through its public headers
foreach(test_name IN LISTS DANA_TESTS)
  add_executable(${test_name} ${test_name}.c)
  target_link_libraries(${test_name} PRIVATE ${${test_name}_MODULE})
  target_include_directories(${test_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

target_link_libraries(test_chat PRIVATE Trainer)
target_link_libraries(test_ngram PRIVATE Trainer)
