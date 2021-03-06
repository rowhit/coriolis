PROJECT(PARSEDRIVEDTR)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

SET(CMAKE_MODULE_PATH "$ENV{VLSISAPD_USER_TOP}/share/cmake/Modules"
                      "$ENV{VLSISAPD_TOP}/share/cmake/Modules"
   )

FIND_PACKAGE(VLSISAPD REQUIRED)
FIND_PACKAGE(Libxml2 REQUIRED)

IF(DTR_FOUND)
    INCLUDE_DIRECTORIES(${DTR_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
    ADD_EXECUTABLE(driveDtr driveDtr.cpp)
    ADD_EXECUTABLE(parseDtr parseDtr.cpp)
    TARGET_LINK_LIBRARIES(driveDtr ${DTR_LIBRARY} ${LIBXML2_LIBRARIES})
    TARGET_LINK_LIBRARIES(parseDtr ${DTR_LIBRARY} ${LIBXML2_LIBRARIES})
ENDIF(DTR_FOUND)
