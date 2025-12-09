function(add_aoc_day_test_regex year day test result1 result2) 
  set(result1 ".*AOC ${year} ${day} part 1 solution is ${result1}.*\n") 
  set(result2 ".*AOC ${year} ${day} part 2 solution is ${result2}.*\n")
  set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION
                                     "${result1}${result2}")
  add_test_regex(${test} "All heap blocks were freed -- no leaks are possible")
endfunction()

function(add_test_regex test result)
  get_property(
    TEST_REGEX
    TEST ${test}
    PROPERTY PASS_REGULAR_EXPRESSION)
  if(TEST_REGEX)
    set(TEST_REGEX "${TEST_REGEX}.*[\n]+.*${result}")
  else()
    set(TEST_REGEX ".*[\n]+.*${result}")
  endif()
  set_property(TEST ${test} PROPERTY PASS_REGULAR_EXPRESSION "${TEST_REGEX}")
endfunction()

function(add_negative_test_regex test result)
  get_property(
    TEST_REGEX
    TEST ${test}
    PROPERTY FAIL_REGULAR_EXPRESSION)
  if(TEST_REGEX)
    set(TEST_REGEX "${TEST_REGEX}.*[\n]+.*${result}")
  else()
    set(TEST_REGEX ".*[\n]+.*${result}")
  endif()
  set_property(TEST ${test} PROPERTY FAIL_REGULAR_EXPRESSION "${TEST_REGEX}")
endfunction()
