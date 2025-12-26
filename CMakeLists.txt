cmake_minimum_required(VERSION 3.20)
project(ESRO VERSION 0.1 LANGUAGES CXX)

# -- Options ---------------------------------------------------------------
option(ENABLE_SANITIZERS "Build with ASan/UBSan (debug only)" OFF)
option(BUILD_TESTS "Build unit tests (Catch2)" OFF)

# -- Compiler settings ----------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Make multi-config generators consistent for outputs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable warnings (tweak for your compiler)
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Optional sanitizers (only for non-MSVC)
if(ENABLE_SANITIZERS AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
    message(STATUS "Building with Address/Undefined sanitizers")
    add_compile_options(-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
    link_libraries(-fsanitize=address -fsanitize=undefined)
endif()

# -- Source layout -----------------------------------------------------

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE ALL_CPP_REL
    RELATIVE "${CMAKE_SOURCE_DIR}"
    "${SRC_DIR}/*.cpp"
)

# Add or remove patterns to match your repo layout
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/\\.git/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/out/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/build/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/\\.vs/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/thirdparty/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/tests/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/examples/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/platform/vulkan/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/network/.*")
# Explicitly exclude Windows Recycle Bin artifacts (defensive)
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/\\$Recycle\\.Bin/.*")
list(FILTER ALL_CPP_REL EXCLUDE REGEX ".*/\\$.*")  # any $-prefixed filename

# Build an absolute list of source files
set(GAME_SOURCES "")
foreach(_rel IN LISTS ALL_CPP_REL)
    list(APPEND GAME_SOURCES "${CMAKE_SOURCE_DIR}/${_rel}")
endforeach()

# If nothing found, stop with a clear message (helps debugging)
if (NOT GAME_SOURCES)
    message(FATAL_ERROR "No game sources found under ${SRC_DIR}. Check your src/ layout and the exclude filters.")
endif()

# -- Target ----------------------------------------------------------------
# Release
# add_executable(${PROJECT_NAME} WIN32 ${GAME_SOURCES}) 
# Debug
add_executable(${PROJECT_NAME} ${GAME_SOURCES}) 
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${INCLUDE_DIR}
        ${SRC_DIR}
        ${CMAKE_SOURCE_DIR}/src/include
        ${CMAKE_SOURCE_DIR}/src
)

# Define a macro for test builds so code can enable test-only accessors
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<BOOL:${BUILD_TESTS}>:BUILD_TESTS=1>)

# -- FastNoiseLite (header-only) -----------------------------------------
# If you put FastNoise in src/include/FastNoise/FastNoiseLite.h nothing special needed.

# -- SFML -----------------------------------------------------------------
# Prefer find_package; user should install SFML or provide SFML_DIR.
# On Windows, easiest is to download SFML and set SFML_DIR to its cmake folder.
set(_sfml_ok FALSE)

# -----------------------
# Robust SFML discovery
# -----------------------
# You can set SFML_STATIC_LIBRARIES to ON if you built static SFML libs.
# e.g. -DSFML_STATIC_LIBRARIES=ON on command line or set in CMakeSettings.

message(STATUS "DEBUG: SFML_DIR (before find_package) = ${SFML_DIR}")

# Try SFML 3 style (capitalized components). This matches the SFMLConfig.cmake you shared.
set(_sfml_ok FALSE)

option(SFML_STATIC_LIBRARIES "Link SFML statically (static libs) instead of shared (DLLs)" ON)

# If user requested static SFML, ensure SFML headers know it
if(SFML_STATIC_LIBRARIES)
    message(STATUS "SFML static linking requested (SFML_STATIC_LIBRARIES=ON)")
    add_compile_definitions(SFML_STATIC)
endif()


# First try explicit SFML 3 call (capitalized component names)
find_package(SFML COMPONENTS Graphics Window System Main QUIET)
if (SFML_FOUND)
    message(STATUS "Found SFML via: find_package(SFML COMPONENTS Graphics Window System)")
    set(_sfml_ok TRUE)
    set(_sfml_targets SFML::Graphics SFML::Window SFML::System SFML::Main)
endif()

# If not found, try lowercase (SFML 2.x style)
if (NOT _sfml_ok)
    find_package(SFML 2.5 COMPONENTS graphics window system main QUIET)
    if (SFML_FOUND)
        message(STATUS "Found SFML via: find_package(SFML 2.5 COMPONENTS graphics window system)")
        set(_sfml_ok TRUE)
        # legacy names may export sfml-graphics etc. CMake will handle targets if exported.
        if (TARGET sfml-graphics)
            set(_sfml_targets sfml-graphics sfml-window sfml-system)
        elseif (TARGET SFML::Graphics)
            set(_sfml_targets SFML::Graphics SFML::Window SFML::System SFML::Main)
        endif()
    endif()
endif()

# If still not found, give helpful diagnostics and fail
if (NOT _sfml_ok)
    message(STATUS "DEBUG: Searching SFML_DIR = ${SFML_DIR}")
    file(GLOB_RECURSE _sfml_listing RELATIVE "${SFML_DIR}" "${SFML_DIR}/*")
    message(STATUS "DEBUG: ${SFML_DIR} contains (first 30):")
    foreach(_i RANGE 0 29)
        list(GET _sfml_listing ${_i} _item QUIET)
        if (_item)
            message(STATUS "  ${_item}")
        endif()
    endforeach()
    message(FATAL_ERROR "SFML not found. Please set SFML_DIR to the folder that contains SFMLConfig.cmake (e.g. .../lib/cmake/SFML) and ensure you call find_package with components. Example: -DSFML_DIR=\"C:/path/to/SFML/lib/cmake/SFML\"")
endif()

# Link discovered targets
if (DEFINED _sfml_targets)
    message(STATUS "Linking SFML targets: ${_sfml_targets}")
    target_link_libraries(${PROJECT_NAME} PRIVATE ${_sfml_targets})
else()
    # fallback
    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)
endif()

# -- Resources: copy res/ to output ---------------------------------------
# So `res/tile.png` works when running the exe from build dir
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)

# On Windows, optionally copy SFML DLLs if you ship them in libs/SFML/bin/x64
if (WIN32)
    set(SFML_DLL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/SFML/bin/x64)
    if (EXISTS ${SFML_DLL_SOURCE_DIR})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SFML_DLL_SOURCE_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endif()
endif()

# -- Installation (optional) ----------------------------------------------
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/res DESTINATION .) # installs res folder

# -- Tests (optional) -----------------------------------------------------
if (BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(catch2)

    file(GLOB_RECURSE TEST_SOURCES ${SRC_DIR}/tests/*.cpp)
    if (TEST_SOURCES)
        add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})
        target_include_directories(${PROJECT_NAME}_tests PRIVATE ${INCLUDE_DIR} ${SRC_DIR})
        target_link_libraries(${PROJECT_NAME}_tests PRIVATE Catch2::Catch2WithMain)
        # Link your game logic if tests depend on it
        target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME})
        add_test(NAME unit_tests COMMAND ${PROJECT_NAME}_tests)
    else()
        message(WARNING "BUILD_TESTS enabled but no test sources found in ${SRC_DIR}/tests")
    endif()
endif()

# -- Useful helper targets ------------------------------------------------
# make it easy to run the game from build dir
add_custom_target(run
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# -- Notes / tips ---------------------------------------------------------
# * If you keep third-party headers in src/include/thirdparty, they are found automatically.
# * If you link SFML statically, you may need to define SFML_STATIC and add additional libs.
# * If you use other libs (e.g., FastNoise as a static lib), add find_package or target_link_libraries accordingly.
# * If globbing causes issues with IDEs, replace `file(GLOB_RECURSE ...)` with an explicit list of sources.

# ---------------------------
# Ensure includes & compile flags are attached to the target
# ---------------------------
# Define an include root variable (useful and safe)
set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/include")

# Expose include directories to the target (IntelliSense needs this)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_INCLUDE_DIR}
        ${SRC_DIR}                    # your src root
        ${CMAKE_SOURCE_DIR}           # project root (defensive)
)

# Make sure the target knows language features & visibility
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
if(MSVC)
    # Keep /W4 set but also give IntelliSense the same defines
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

