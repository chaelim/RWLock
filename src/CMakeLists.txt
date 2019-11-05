project(RWLock)

include_directories(../include)
include(../cmake/BuildSettings.cmake)

file(GLOB SRCFILES *.cpp)
file(GLOB INCFILES ../include/*.h)
add_library (RWLock ${SRCFILES} ${INCFILES} )
