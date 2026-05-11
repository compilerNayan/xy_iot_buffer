cmake_minimum_required(VERSION 3.10)
project(JwtAuthenticatorDemo LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files from src/
file(GLOB SRC_FILES src/*.cpp src/*.h)

add_executable(jwt_demo ${SRC_FILES})

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Find ArduinoJson
# Option 1: If installed via package manager (vcpkg, apt, brew)
find_package(ArduinoJson REQUIRED)

# Option 2: If vendored (downloaded manually into src/libs/ArduinoJson)
# include_directories(${CMAKE_SOURCE_DIR}/src/libs/ArduinoJson/src)

# Link libraries
target_link_libraries(jwt_demo PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Include directories
target_include_directories(jwt_demo PRIVATE ${ARDUINOJSON_INCLUDE_DIRS})
