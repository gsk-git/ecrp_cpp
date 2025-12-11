cmake_minimum_required(VERSION 3.20)
project(ESRO VERSION 0.1 LANGUAGES CXX)

# -- Options ---------------------------------------------------------------
option(ENABLE_SANITIZERS "Build with ASan/UBSan (debug only)" OFF)
option(BUILD_TESTS "Build unit tests (Catch2)" OFF)

# -- Compiler settings ----------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# -- Source layout --------------------------------------------------------
# Assumed layout:
#  ProjectRoot/
#    src/
#      include/         (your headers and thirdparty headers, e.g. FastNoise)
#      main.cpp
#      config.cpp
#      ... other .cpp files ...
#    res/               (resources: images, fonts, tile.png, etc.)
#    libs/              (optional: SFML libs if you ship them locally)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE ALL_CPP_REL
    RELATIVE "${CMAKE_SOURCE_DIR}"
    "${SRC_DIR}/*.cpp"
)

# Exclude common folders that should not be part of the build
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
add_executable(${PROJECT_NAME} ${GAME_SOURCES}) 

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${INCLUDE_DIR}
        ${SRC_DIR}
        ${CMAKE_SOURCE_DIR}/src/include
        ${CMAKE_SOURCE_DIR}/src
)

# Define a macro for test builds so code can enable test-only accessors
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<BOOL:${BUILD_TESTS}>:OFF>)

# -- FastNoiseLite (header-only) -----------------------------------------
# If you put FastNoise in src/include/FastNoise/FastNoiseLite.h nothing special needed.

# -- SFML -----------------------------------------------------------------
# Prefer find_package; user should install SFML or provide SFML_DIR.
# On Windows, easiest is to download SFML and set SFML_DIR to its cmake folder.
set(_sfml_ok FALSE)

find_package(SFML 2.5 COMPONENTS graphics window system QUIET)
if (SFML_FOUND)
    message(STATUS "Found SFML 2.5-style package")
    set(_sfml_ok TRUE)
    # legacy targets are typically: sfml-graphics, sfml-window, sfml-system
    if (TARGET sfml-graphics)
        set(_sfml_targets sfml-graphics sfml-window sfml-system)
    endif()
endif()

# If not found, try SFML 3+ style (capitalized components / namespaced targets)
if (NOT _sfml_ok)
    find_package(SFML COMPONENTS Graphics Window System QUIET)
    if (SFML_FOUND)
        message(STATUS "Found SFML 3-style package")
        set(_sfml_ok TRUE)
        # modern exported targets often use namespaces like SFML::Graphics
        # we prefer those if they exist
        if (TARGET SFML::Graphics)
            set(_sfml_targets SFML::Graphics SFML::Window SFML::System)
        endif()
    endif()
endif()

if (NOT _sfml_ok)
    message(FATAL_ERROR "SFML not found. Install SFML 2.5.x or SFML 3 and set SFML_DIR appropriately.")
endif()

# Link the targets we discovered (or try fallbacks)
if (DEFINED _sfml_targets)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${_sfml_targets})
else()
    # Last resort: try commonly used names (some packages export these)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system SFML::Graphics SFML::Window SFML::System)
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
