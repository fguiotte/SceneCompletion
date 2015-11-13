PROJECT(sceneCompletion)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

find_package(OpenCV REQUIRED)

SET(SRC 
    main.cpp
    )

SET(HDR
    )

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC} ${HDR})
ADD_EXECUTABLE(test grabcut.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(test ${OpenCV_LIBS})
