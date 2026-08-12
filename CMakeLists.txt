cmake_minimum_required (VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Get project name from directory
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get all files in src folder
file(
    GLOB_RECURSE SOURCE_CXX_FILES
    "src/*.cpp"
)

# Add source
if (CMAKE_BUILD_TYPE STREQUAL "Release")
	# Don't include console in release builds
	add_executable(${PROJECT_NAME} WIN32 ${SOURCE_CXX_FILES})
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_executable(${PROJECT_NAME} ${SOURCE_CXX_FILES})
endif()


# Clone, build and link SDL3
set(LIBS "")
set(STATIC_LINK FALSE)

# Force SDL3 to build as a static library
if (STATIC_LINK)
  set(SDL_SHARED OFF CACHE BOOL "Build shared libraries" FORCE)
  set(SDL_STATIC ON CACHE BOOL "Build static libraries" FORCE)
endif()

if (EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-s USE_SDL=3")
  # set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s USE_SDL=3 -s USE_WEBGL2=1 -o index.html --shell-file ${CMAKE_SOURCE_DIR}/web/shell_minimal.html")

  # Without minimal template
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s USE_SDL=3 -s USE_WEBGL2=1 -o index.html")

	set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
  include(FetchContent)
  set(FETCHCONTENT_QUIET FALSE)

  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.4.12 # grab latest stable release
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(SDL3)

  # Statically Link SDL3
  if (STATIC_LINK)
    set(LIBS ${LIBS} SDL3::SDL3-static)
  else()
    # Copy DLL
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different 
      "$<TARGET_FILE:SDL3::SDL3>" 
      "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )

    set(LIBS ${LIBS} SDL3::SDL3)
  endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})