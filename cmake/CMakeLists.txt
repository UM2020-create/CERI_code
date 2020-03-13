#cmake_minimum_required (VERSION 2.8)
add_executable(bin/arc2sum src/arc2sum.c)
add_executable(bin/hyp2sum src/hyp2sum.c)
TARGET_LINK_LIBRARIES(bin/arc2sum  m)
TARGET_LINK_LIBRARIES(bin/hyp2sum  m)



