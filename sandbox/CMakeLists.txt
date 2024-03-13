cmake_minimum_required(VERSION 3.26)
project(sandbox)

set(SOURCES
	main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME}
					  PROPERTIES
					  CXX_STANDARD 20
					  CXX_STANDARD_REQUIRED YES
					  CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME}
						   PUBLIC ../include
)