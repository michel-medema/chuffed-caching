cmake_minimum_required(VERSION 3.14)
project(chuffed_test LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        f8d7d77 # v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests_run
        doctest.cpp

        caching/keys/constraints/CMinimumTests.cpp
        caching/keys/constraints/CLinearLETests.cpp
        caching/keys/EquivalencePartTests.cpp
        #caching/keys/DominancePartTests.cpp
        #caching/keys/VariableDomainTests.cpp
        caching/cache/CacheMocks.h
        caching/cache/CacheTests.h
        caching/cache/CacheTests.cpp
        caching/cache/ThresholdTests.cpp
        caching/cache/UnboundedCacheTests.cpp
        caching/cache/EventCacheTests.cpp
        caching/cache/policies/LeastRecentlyUsedTests.cpp
        caching/cache/BoundedCacheTests.cpp
        caching/cache/policies/MockEvictionPolicy.h
        caching/cache/policies/RandomReplacementTests.cpp
        caching/cache/policies/FIFOTests.cpp
        caching/KeySizeTests.cpp
)
target_link_libraries(tests_run chuffed_fzn chuffed gmock_main)