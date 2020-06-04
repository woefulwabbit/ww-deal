cmake_minimum_required (VERSION 2.8.12)

add_library(wwdeal wwdeal.c)
target_include_directories(wwdeal PUBLIC .)

add_executable(wwdeal-example example.c)
target_link_libraries(wwdeal-example wwdeal)
