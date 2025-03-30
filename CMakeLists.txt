cmake_minimum_required(VERSION 3.15)
project(ffl LANGUAGES CXX)

# ------------------ Options & Config -------------------

option(FFL_NO_NINTEXUTILS "Disable ninTexUtils (then you can only use AFL resources)" OFF)
option(FFL_NO_ZLIB "Disable zlib (then you need to disable zlib on a resource made with FFLResource.py)" OFF)
option(FFL_USE_BROTLI "Enable brotli support for unofficial resource files. Do NOT enable this if you are not going to use a resource explicitly packed with brotli." OFF)
option(FFL_NO_FS "Disables FFL's use of RIO filesystem, for databases and non-cached resource loading." OFF)
# This should probably be changeable at runtime:
set(FFL_MLC_PATH "./" CACHE STRING "MLC path for FFL to use to search for resource and database files.")


# Disable database functionality:
option(FFL_NO_DATABASE_FILE "Disable functionality to open databases in FFL." OFF)
option(FFL_NO_DATABASE_RANDOM "Disable random database in FFL." OFF)
if(EMSCRIPTEN)
    option(FFL_NO_DATABASE_DEFAULT "Disable default database (6 entries) in FFL." ON) # Save 1.6 ms when initializing FFL...
else()
    option(FFL_NO_DATABASE_DEFAULT "Disable default database (6 entries) in FFL." OFF)
endif()
option(FFL_NO_MIDDLE_DB "Disable MiddleDB functionality in FFL. NOTE that you cannot disable MiddleDB if you need any other databases" OFF)

# Enabled by default in FFL-Testing:
option(FFL_NO_DRAW_MASK_ALPHA_VALUES "Skips clearing alpha (fill)/drawing alpha values when drawing mask. Not default FFL behavior." OFF)
option(FFL_ENABLE_NEW_MASK_ONLY_FLAG "Enable functionality for FFLInitCharModelCPUStep to create the mask without any other shapes, causing a potential crash." OFF)

# Misc:
option(FFL_USE_ADJUST_MTX "Changes behavior of FFLiAdjustShape, so that instead of performing operations on the CPU to adjust scale/translate on shapes, it will instead set pAdjustMatrix and let the caller do that adjustment itself. Needed for using half float types." OFF)
option(FFL_PART_INDEX_WRAP "When this is enabled, out of bounds parts IDs will.. wrap. Invalid parts IDs will still map to something. In other words, It will make pretty much force all invalid CharInfo to render." OFF)
option(FFL_USE_EXTRA_RESOURCES "Expand resource types to add two more: very high (FFLResPoster.dat) and low (meant to be CFL_Res.dat quality)." OFF)

# Target output options:
option(FFL_BUILD_STATIC "Build FFL as a static library (.a, .lib)" OFF)
option(FFL_BUILD_SHARED "Build FFL as a shared/dynamically linked library (.dll, .so, .dylib)" ON) # Not available with Emscripten
# Note that FFL_ADD_GLAD_GL_IMPLEMENTATION is enabled when FFL_BUILD_SHARED is used.
if(EMSCRIPTEN)
    # By default when using Emscripten, this will be built
    # as an executable that functions as an Emscripten module.
    option(FFL_BUILD_EMSCRIPTEN_MODULE "Build as an Emscripten module" ON)
    # Emscripten-specific option to build for WASM or not.
    option(FFL_BUILD_WASM "Build for WebAssembly" ON)
endif()

set(CMAKE_CACHE_ARGS_INCLUDE_HELP TRUE) # Include option descriptions with cmake -LH.

# Define build modes for FFL.
# Standalone (default, no RIO/GL), for-rio without RIO but for RIO,
# and finally opengl-33 and opengl-es2 both with minimal RIO sources.
set(FFL_MODE "" CACHE STRING "FFL build mode (blank for standalone, for-rio, opengl-33, opengl-es2)")
set_property(CACHE FFL_MODE PROPERTY STRINGS "" "for-rio" "opengl-33" "opengl-es2")


# Paths to RIO and ninTexUtils.
set(FFL_WITH_RIO "${CMAKE_CURRENT_SOURCE_DIR}/../rio" CACHE PATH "Path to RIO directory (must have include/ and src/)")
set(FFL_WITH_NINTEXUTILS "${CMAKE_CURRENT_SOURCE_DIR}/../ninTexUtils" CACHE PATH "Path to ninTexUtils directory (must have include/ and src/)")

# Use debug as the primary build target.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ------------------ Compiler Settings -------------------

# RIO/FFL need C++17, but MSVC wants it to be C++20.
if(MSVC)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")  # Warning level 3.
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-private-field -Wno-missing-braces")
endif()

# Set flags to reduce dependence on libstdc++ in release mode.
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-exceptions -fno-rtti -fno-threadsafe-statics")
# Only libstdc++ funcs used are: __cxa_pure_virtual, new/delete

# ------------------ Detect RIO -------------------

if(FFL_MODE MATCHES "for-rio|opengl")
    if(FFL_WITH_RIO STREQUAL "")
        message(FATAL_ERROR "FFL_WITH_RIO (RIO repository) must be set for mode ${FFL_MODE}.")
    endif()
endif()

if(NOT EXISTS "${FFL_WITH_RIO}/include")
    message(FATAL_ERROR "RIO include folder not found at path: ${FFL_WITH_RIO}. Set FFL_WITH_RIO to the correct path. RIO includes are needed for all modes.")
endif()

# Even standalone mode needs RIO headers for structs, inlined
# functions, and macros, but not necessarily the runtime code.
include_directories(${FFL_WITH_RIO}/include)

# ------------------ ninTexUtils (Optional) -------------------

if(NOT FFL_NO_NINTEXUTILS)
    if(FFL_WITH_NINTEXUTILS STREQUAL "")
        message(FATAL_ERROR "FFL_WITH_NINTEXUTILS must be set when ninTexUtils is enabled. Provide correct path or set FFL_NO_NINTEXUTILS=ON. Use ninTexUtils cpp branch for this.")
    endif()
    if(NOT EXISTS "${FFL_WITH_NINTEXUTILS}/include" OR NOT EXISTS "${FFL_WITH_NINTEXUTILS}/src")
        message(FATAL_ERROR "Invalid ninTexUtils path: ${FFL_WITH_NINTEXUTILS}. Ensure it contains include/ and src/. Try pulling the cpp branch.")
    endif()
endif()

# ------------------ zlib, brotli -------------------

if(NOT FFL_NO_ZLIB)
    find_package(ZLIB QUIET)
    if(NOT ZLIB_FOUND)
        message(WARNING "zlib not found. Proceeding without zlib. Please note that this will not work with resources that are not uncompressed (all publicly available ones are)")
        set(FFL_NO_ZLIB ON)
    endif()
else()
    message(WARNING "zlib disabled. Will proceed without zlib features (FFL_NO_ZLIB).")
endif()

if(FFL_USE_BROTLI)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(BROTLI_DEC libbrotlidec libbrotlicommon REQUIRED)
endif()

# ------------------ Source Files -------------------

file(GLOB_RECURSE FFL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Includes are added to the target at the end.

if(NOT FFL_NO_NINTEXUTILS) # Add ninTexUtils source
    # Only two ninTexUtils funcs are used in FFL: GX2CalcSurfaceSizeAndAlignment, GX2CopySurface
    list(APPEND FFL_SOURCES
        "${FFL_WITH_NINTEXUTILS}/src/ninTexUtils/gx2/gx2Surface.cpp"
        "${FFL_WITH_NINTEXUTILS}/src/ninTexUtils/gx2/tcl/addrlib.cpp"
    )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(NOT FFL_NO_NINTEXUTILS)
    include_directories(${FFL_WITH_NINTEXUTILS}/include)
endif()


# ------------------ Target Generation -------------------

# Function for making an FFL target, to reduce duplicated code.
function(create_ffl_target target_name library_type)

    if(FFL_MODE MATCHES "opengl") # If the mode starts with opengl...
        # Add a minimal set of RIO sources to make GL calls.
        set(RIO_GL_SOURCES
            "${FFL_WITH_RIO}/src/gpu/rio_Texture.cpp"
            "${FFL_WITH_RIO}/src/gpu/win/rio_Texture2DUtilWin.cpp"
            "${FFL_WITH_RIO}/src/gpu/win/rio_TextureWin.cpp"
            "${FFL_WITH_RIO}/src/gfx/rio_Projection.cpp"
            "${FFL_WITH_RIO}/src/gpu/rio_RenderState.cpp"
            "${FFL_WITH_RIO}/src/gpu/win/rio_RenderStateWin.cpp"
            "${FFL_WITH_RIO}/src/gpu/rio_RenderBuffer.cpp"
            "${FFL_WITH_RIO}/src/math/rio_Matrix.cpp"
            "${FFL_WITH_RIO}/src/gpu/rio_RenderTarget.cpp"
            "${FFL_WITH_RIO}/src/gfx/win/rio_GraphicsWin.cpp"
        )
        list(APPEND FFL_SOURCES ${RIO_GL_SOURCES})
    # Standalone mode does not add RIO sources. It only references RIO headers.
    endif()

    # Create target.
    if(FFL_BUILD_EMSCRIPTEN_MODULE)
        # If making Emscripten module, instead build an executable.
        message(STATUS "Building an executable instead of a library in order to build an Emscripten module. To disable this, turn off FFL_BUILD_EMSCRIPTEN_MODULE.")
        add_executable(${target_name} ${FFL_SOURCES})
    else()
        # Create a library target (static or shared)
        add_library(${target_name} ${library_type} ${FFL_SOURCES})
    endif()

    target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    # Define RIO_DEBUG/RIO_RELEASE if not using RIO.
    if(NOT FFL_MODE STREQUAL "for-rio")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug") # Only use RIO_DEBUG for all debug modes.
            target_compile_definitions(${target_name} PRIVATE RIO_DEBUG)
        else()
            target_compile_definitions(${target_name} PRIVATE RIO_RELEASE)
        endif()
    endif()

    if(ZLIB_FOUND AND NOT FFL_NO_ZLIB) # Link zlib.
        target_link_libraries(${target_name} PRIVATE ZLIB::ZLIB)
    else()
        target_compile_definitions(${target_name} PRIVATE FFL_NO_ZLIB)
    endif()

    if(FFL_USE_BROTLI) # Link brotli.
        target_link_libraries(${target_name} PRIVATE ${BROTLI_DEC_LIBRARIES} ${BROTLI_COMMON_LIBRARIES})
        target_compile_definitions(${target_name} PRIVATE FFL_USE_BROTLI)
    endif()

    if(FFL_NO_FS) # Exclude RIO filesystem support
        # Make sure to also add FFL_NO_DATABASE_FILE.
        target_compile_definitions(${target_name} PRIVATE FFL_NO_FS FFL_NO_DATABASE_FILE)
    else()
        # Add FFL_MLC_PATH.
        if(FFL_MLC_PATH) # Escape the path.
            #string(REPLACE REGEX "\"" "\\\"" FFL_MLC_PATH_ESCAPED "${FFL_MLC_PATH}")
            target_compile_definitions(${target_name} PRIVATE FFL_MLC_PATH="${FFL_MLC_PATH}")
            message(STATUS "FFL: FFL_MLC_PATH set to: ${FFL_MLC_PATH}")
        endif()
    endif()

    if(NOT FFL_NO_NINTEXUTILS) # Include ninTexUtils.
        target_include_directories(${target_name} PUBLIC ${FFL_WITH_NINTEXUTILS}/include)
    else() # Define FFL_NO_NINTEXUTILS.
        target_compile_definitions(${target_name} PRIVATE FFL_NO_NINTEXUTILS)
    endif()


    # Apply settings defined by options:
    if(FFL_NO_DATABASE_FILE)
        target_compile_definitions(${target_name} PUBLIC FFL_NO_DATABASE_FILE)
    endif()
    if(FFL_NO_DATABASE_RANDOM)
        target_compile_definitions(${target_name} PRIVATE FFL_NO_DATABASE_RANDOM)
    endif()
    if(FFL_NO_DATABASE_DEFAULT)
        target_compile_definitions(${target_name} PUBLIC FFL_NO_DATABASE_DEFAULT)
    endif()
    if(FFL_NO_MIDDLE_DB)
        target_compile_definitions(${target_name} PUBLIC FFL_NO_MIDDLE_DB)
    endif()
    if(FFL_NO_DRAW_MASK_ALPHA_VALUES)
        target_compile_definitions(${target_name} PRIVATE FFL_NO_DRAW_MASK_ALPHA_VALUES)
    endif()
    if(FFL_ENABLE_NEW_MASK_ONLY_FLAG)
        target_compile_definitions(${target_name} PUBLIC FFL_ENABLE_NEW_MASK_ONLY_FLAG)
    endif()
    if(FFL_USE_ADJUST_MTX)
        target_compile_definitions(${target_name} PUBLIC FFL_USE_ADJUST_MTX)
    endif()
    if(FFL_PART_INDEX_WRAP)
        target_compile_definitions(${target_name} PRIVATE FFL_PART_INDEX_WRAP)
    endif()
    if(FFL_USE_EXTRA_RESOURCES)
        target_compile_definitions(${target_name} PUBLIC FFL_USE_EXTRA_RESOURCES)
    endif()


    # Apply mode-specific settings.
    if(FFL_MODE STREQUAL "") # Standalone mode
        # Standalone needs: texture callback, no render texture or GLFW
        target_compile_definitions(${target_name} PRIVATE
            RIO_NO_GLFW_CALLS        # Needed because some RIO headers are still used.
            FFL_USE_TEXTURE_CALLBACK # Need to define a texture callback.
            FFL_NO_RENDER_TEXTURE    # Need to manually manage render textures.
            FFL_NO_FS # Removes dependency on RIO filesystem.
            FFL_NO_DATABASE_FILE # ^^
            FFL_NO_DRAW_MASK_ALPHA_VALUES # << Removes rio::RenderState calls
        )

    elseif(FFL_MODE STREQUAL "for-rio") # Build mode for RIO targets.
        # Do not import GLFW headers when using RIO headers.
        target_compile_definitions(${target_name} PRIVATE RIO_NO_GLFW_IMPORT)
        # Pass along RIO definitions that FFL supports.
        if(RIO_GLES)
            target_compile_definitions(${target_name} PRIVATE RIO_GLES)
        endif()


    # OpenGL (minimal RIO) target.
    elseif(FFL_MODE MATCHES "opengl")
        target_compile_definitions(${target_name} PRIVATE
            RIO_NO_GLFW_CALLS
            RIO_NO_CONTROLLERS_WIN # ^^ Remove GLFW dependency.
            RIO_NO_CLIP_CONTROL    # Assume pre-OpenGL 4.5.
            RIO_NO_TEXTURE2D_FILE_CTOR # Relies on C++ STL.
            #RIO_NO_GL_LOADER # Needed when building alongside smth else?
            FFL_NO_FS # No RIO filesystem is being used
            FFL_NO_DATABASE_FILE # ^^
        )
        if(FFL_BUILD_SHARED)# AND FFL_ADD_GLAD_GL_IMPLEMENTATION)
            # Build GLAD implementation into libffl (FFLGladLoadGL)
            target_compile_definitions(${target_name} PRIVATE FFL_ADD_GLAD_GL_IMPLEMENTATION)
        endif()

        if(FFL_MODE STREQUAL "opengl-es2") # OpenGL ES 2.0/3.0 target.
            target_compile_definitions(${target_name} PRIVATE RIO_GLES)
        endif()
    endif()


    # Set target properties (output name matches target name)
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${target_name})
endfunction()

# ------------------ Build Targets -------------------

if(FFL_MODE STREQUAL "")
    set(FFL_TARGET_NAME "${PROJECT_NAME}")
else()
    set(FFL_TARGET_NAME "${PROJECT_NAME}-${FFL_MODE}")
endif()

if(FFL_BUILD_EMSCRIPTEN_MODULE)
    # Force "ffl-emscripten" suffix for module.
    set(FFL_TARGET_NAME "${FFL_TARGET_NAME}-emscripten")
endif()

# Static library
if(FFL_BUILD_STATIC)
    create_ffl_target("${FFL_TARGET_NAME}" STATIC)
endif()

# Shared library
if(FFL_BUILD_SHARED)
    create_ffl_target("${FFL_TARGET_NAME}" SHARED)
    # Add .def file to define exports for MSVC.
    if(MSVC)
        # Ensure the linker uses the .def file
        set_target_properties("${FFL_TARGET_NAME}" PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/ffl.def")
    endif()

endif()

# ------------ Emscripten Module Options -----------
# Apply Emscripten-specific options if building a module.
if(FFL_BUILD_EMSCRIPTEN_MODULE)
    # Path to exports JSON for Emscripten.
    set(EXPORTS_JSON ${PROJECT_SOURCE_DIR}/emscripten-exports.json)
    # Set emcc flags: exports, zlib, remove/addFunction...
    set(LINK_FLAGS
        -s EXPORTED_FUNCTIONS=\"@${EXPORTS_JSON}\"
        -s MALLOC=emmalloc # Not tested if this is good
        -s USE_ZLIB=1

        # Allow addFunction.
        -s ALLOW_TABLE_GROWTH=1
        -s STRICT=1
        # Required explicit parameters for strict mode:
        -s EXPORTED_RUNTIME_METHODS=\"['addFunction','removeFunction','HEAP8','HEAPU8','HEAPU16','HEAPU32','HEAP32','HEAPF32']\"
        -s INCOMING_MODULE_JS_API=\"['locateFile', 'onRuntimeInitialized']\"
        -s EXPORT_NAME="ModuleFFL" # Custom export name.

        -s ENVIRONMENT=web
        # -s MINIMAL_RUNTIME=1 # depends on calledRun/onRuntimeInitialized (need to use modularize)
        -s ERROR_ON_UNDEFINED_SYMBOLS=0 # $stackTrace
        -s FILESYSTEM=0
        -s DYNAMIC_EXECUTION=2
        -s MODULARIZE=1

        -s INITIAL_HEAP=1MB -s ALLOW_MEMORY_GROWTH=1
        #--memoryprofiler
        #-s SAFE_HEAP=1
    )
    if (FFL_BUILD_WASM)
        set(LINK_FLAGS ${LINK_FLAGS} -s WASM=1)
    else()
        set(LINK_FLAGS ${LINK_FLAGS} -s WASM=0)
    endif()

    string(REPLACE ";" " " LINK_FLAGS_STR "${LINK_FLAGS}")
    set_target_properties(${FFL_TARGET_NAME}
        PROPERTIES
        LINK_FLAGS "${LINK_FLAGS_STR}"
        LINK_DEPENDS "${EXPORTS_JSON}"
        #SUFFIX ".js"
    )
endif()

# ------------------ Installation (Optional) -------------------
# You can add install rules if you want to make these libraries installable
# install(TARGETS ffl ffl_shared
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
#     PUBLIC_HEADER DESTINATION include
# )

# ------------------ Summary -------------------
message(STATUS "FFL: Build mode: ${FFL_MODE}")
if(FFL_NO_NINTEXUTILS)
    message(STATUS "FFL: ninTexUtils disabled")
else()
    message(STATUS "FFL: ninTexUtils enabled (path: ${FFL_WITH_NINTEXUTILS})")
endif()

if(FFL_NO_ZLIB)
    message(STATUS "FFL: zlib disabled or not found")
else()
    message(STATUS "FFL: zlib enabled")
endif()

if(FFL_USE_BROTLI)
    message(STATUS "FFL: brotli enabled")
endif()

if(FFL_MODE STREQUAL "for-rio")
    message(STATUS "FFL: Building for RIO using includes from: ${FFL_WITH_RIO}")
elseif(FFL_MODE MATCHES "opengl")
    message(STATUS "FFL: Building for OpenGL with minimal RIO sources from: ${FFL_WITH_RIO}")
elseif(FFL_MODE STREQUAL "")
    message(STATUS "FFL: Building ffl standalone using RIO includes from: ${FFL_WITH_RIO}")
endif()

message(STATUS "FFL: Static library = ${FFL_BUILD_STATIC}")
message(STATUS "FFL: Shared library = ${FFL_BUILD_SHARED}")
message(STATUS "FFL: Build type: ${CMAKE_BUILD_TYPE}")

# force mfs building for web to not distribute a 9 GB binary
if(EMSCRIPTEN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "If you are building for the web, PLEASE make sure you know that debug is the default build mode, so use release (-DCMAKE_BUILD_TYPE=Release) for smaller binaries.")
endif()
