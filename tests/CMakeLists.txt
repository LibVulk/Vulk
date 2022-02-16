enable_testing()

add_executable(
	${PROJECT_NAME}-unit-tests
	src/Vec2.cpp
	src/Vec3.cpp
	src/Run.cpp
	src/Rect.cpp
	src/Mat3.cpp
)

target_link_libraries(${PROJECT_NAME}-unit-tests PUBLIC ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}-unit-tests PRIVATE ../lib/include)

include(GoogleTest)

gtest_discover_tests(${PROJECT_NAME}-unit-tests)