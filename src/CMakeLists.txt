add_library(bf_core
        bf.cpp
        bbf.cpp
        )

set(BF_DEBUG_CXXFLAGS "${BF_CXX_FLAGS};-g")
set(BF_RELEASE_CXXFLAGS "${BF_CXX_FLAGS};-O3")

target_include_directories(bf_core PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)
target_compile_options(bf_core PUBLIC "$<$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CXX>>:${BF_DEBUG_CXXFLAGS}>")
target_compile_options(bf_core PUBLIC "$<$<AND:$<CONFIG:RELEASE>,$<COMPILE_LANGUAGE:CXX>>:${BF_RELEASE_CXXFLAGS}>")

add_executable(bf runBfCommands.cpp)
target_include_directories(bf PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)
target_link_libraries(bf bf_core)
target_compile_options(bf PUBLIC "$<$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CXX>>:${BF_DEBUG_CXXFLAGS}>")
target_compile_options(bf PUBLIC "$<$<AND:$<CONFIG:RELEASE>,$<COMPILE_LANGUAGE:CXX>>:${BF_RELEASE_CXXFLAGS}>")

add_executable(bbf runBBfCommands.cpp)
target_include_directories(bbf PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)
target_link_libraries(bbf bf_core)
target_compile_options(bbf PUBLIC "$<$<AND:$<CONFIG:DEBUG>,$<COMPILE_LANGUAGE:CXX>>:${BF_DEBUG_CXXFLAGS}>")
target_compile_options(bbf PUBLIC "$<$<AND:$<CONFIG:RELEASE>,$<COMPILE_LANGUAGE:CXX>>:${BF_RELEASE_CXXFLAGS}>")
