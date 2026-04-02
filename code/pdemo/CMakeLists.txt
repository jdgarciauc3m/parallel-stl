cmake_minimum_required(VERSION 4.0)
project(pdemo LANGUAGES CXX)

# 1. Essential: Set the C++ standard (oneDPL requires 17 or higher)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Find the oneDPL package
# This works after you've run 'source /opt/intel/oneapi/setvars.sh'
find_package(oneDPL REQUIRED)

# 3. Find oneTBB (required for par/par_unseq policies on CPU)
find_package(TBB REQUIRED)

add_executable(demo1 demo1.cpp)

# 4. Link the oneDPL and TBB targets
# Note: oneDPL is a header-only library, but the target handles include paths
target_link_libraries(demo1 oneDPL TBB::tbb)