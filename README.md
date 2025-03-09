# FFL Arian Fork
This is my fork of [AboodXD's FFL decompilation](https://github.com/aboood40091/ffl), which was originally decompiled from FFL 1.3.10 in New SUPER MARIO BROS. U v1.3.0.


<details>
<summary>View the original README contents here.
TL;DR, FFL is the library that renders Miis and manages Mii data on Wii U and is only accessible to approved developers.</summary>

# FFL
Decompilation of Wii U Face Library v1.3.10, used in New Super Mario Bros. U v1.3.0.

FFL is a library that is part of nn (**N**intendo SDK C++ API **N**amespace?), used for dealing with Mii data.
While most of nn is already open to developers as part of the Cafe SDK by Nintendo, FFL is a closed-source library that is sent to developers on demand.

This decompilation aims to provide more information on Mii data, as well as to further push the progress on the [New Super Mario Bros. U decompilation project](https://github.com/aboood40091/red-pro2).

See [Sead](https://github.com/aboood40091/sead) for matching policy.
</details>

Specifically, this is Abood's port to RIO (OpenGL 3.3) which works on PC alongside Wii U.

## Dependencies

Both of the below are originally by [Abood](https://github.com/aboood40091), but I am linking to my forks since that's what is expected.

* [RIO](https://github.com/ariankordi/rio)
  - This branch of the FFL decomp relies on it.
  - This is a framework to abstract functionality between Wii U (RIO_IS_CAFE) and PC (RIO_IS_WIN).
  - Although you can build without RIO _sources_, you will always need RIO's headers, for now.

* [ninTexUtils](https://github.com/ariankordi/ninTexUtils) (NOT the `python` branch but the cpp one)
  - This is for "deswizzling" Wii U textures/converting textures with GX2 default tile mode to linear tile mode.
  - Required for FFL resources (FFLResHigh.dat, FFLResMiddle.dat), or else textures will look wrong because they are tiled.
  - If you don't want this, build with `FFL_NO_NINTEXUTILS` but you will have to use AFL resource files from Miitomo.
    * You can actually get AFLResHigh_2_3.dat here: https://web.archive.org/web/20180502054513/http://download-cdn.miitomo.com/native/20180125111639/android/v2/asset_model_character_mii_AFLResHigh_2_3_dat.zip

## Building

Also see the [FFL-Testing CMakeLists](https://github.com/ariankordi/FFL-Testing/blob/master/CMakeLists.txt).

1. Clone this repo and dependencies.

```
git clone https://github.com/ariankordi/ffl  # This repo.
git clone https://github.com/ariankordi/rio  # RIO framework.
git clone https://github.com/ariankordi/ninTexUtils  # ninTexUtils for deswizzling.
```

The CMake assumes that RIO is located at `../rio`, but you can also pass `-DFFL_WITH_RIO=path/to/rio` (Exact same for ninTexUtils)

2. Choose a build mode. Here are your options:

* For a RIO program/game.
  - Such as [FFL-Testing](https://github.com/ariankordi/FFL-Testing).
  - `-DFFL_MODE=for-rio`
* For an OpenGL program, bundling RIO code with it.
  - Such as the [FFL raylib samples](https://github.com/ariankordi/ffl-raylib-samples).
  - `-DFFL_MODE=opengl-33`, `-DFFL_MODE=opengl-es2`
* Without RIO or OpenGL.
  - Standalone, for something like [FFLSharp](https://github.com/ariankordi/FFLSharp).
  - Will most likely use the dynamic library: `-DFFL_BUILD_SHARED=1`

3. Head into the `ffl` folder and use CMake to build.

* `cmake -S . -B build` (Add any extra args at the end)
* `cmake --build build`

4. Your library is in `build/`

```
> ls build
CMakeCache.txt  CMakeFiles/  cmake_install.cmake  libffl.so*  Makefile
                                                  ^^^^^^^^^^
```
* In my case on Linux, it's in `libffl.so` but I think on Windows it will be `ffl.dll`.
  - For the shared library of course not static
  - There will be extra suffixes if it's for RIO or OpenGL.
  - Copy this to your program's directory or include the CMakeLists as a dependency.

More instructions are TBD.

## New Changes
Over time, I've added new features that are not included in Abood's original decomp. As of writing (December 2024), these include:
* [Support for building on 64-bit, on non-Windows platforms](https://github.com/ariankordi/ffl/commit/dc1650bb24f16f028b5f97125a08da0f70331f85) and [on MSVC](https://github.com/ariankordi/ffl/commit/49996a583bb15d1af5561633ddc670f246a4ac31). Also proper [support for flipped Y](https://github.com/ariankordi/ffl/commit/e4a5876219d617200ed4cf54884812bcb5a7d049) (OpenGL <4.5)
* [Color tables for Switch Miis](https://github.com/ariankordi/ffl/commit/7d2dded274f5ab6283df2568223e9f50d0d5464b) imported from nn::mii.
  - Note that methods to convert from Switch structures are not in here at the moment, they live here: [github.com/ariankordi/FFL-Testing, renderer-server-prototype branch: /src/DataUtils.cpp](https://github.com/ariankordi/FFL-Testing/blob/renderer-server-prototype/src/DataUtils.cpp)
* Allow using the [Miitomo AFL resource files](https://github.com/ariankordi/ffl/commit/d740192adb655a1a3d6e2bcae0feda5ea8fd25fc) alongside FFL resources from Wii U.
  - This means that, if you need a resource file, you can [just use the Miitomo one downloadable from web.archive.org.](http://web.archive.org/web/20180502054513/http://download-cdn.miitomo.com/native/20180125111639/android/v2/asset_model_character_mii_AFLResHigh_2_3_dat.zip)
  - Additionally added [Miitomo exclusive expressions](https://github.com/ariankordi/ffl/commit/2bdfc351f16af84e279956d57e2be5cb42c946e8).
* Flags to remove unneeded functionality such as [FFL_NO_DATABASE_FILE](https://github.com/ariankordi/ffl/commit/3d499fc66f686ce5ed181a8459f3fd288df32f70), [FFL_NO_NINTEXUTILS](https://github.com/ariankordi/ffl/commit/5174db9dda7f330e96ed278844e11c76642b1e50), [FFL_NO_FS](https://github.com/ariankordi/ffl/commit/7ffa02c1ff4eae2d731b5aa4de453547f0f27f35),
  - Applying FFL_NO_DATABASE_FILE is probably essential if you're on a PC - FFL will try to load the database file when it is initialized.
  - Note that with FFL_NO_NINTEXUTILS, you can't load Wii U resources - just ones from Miitomo.
* Fixes for [calling FFL from C](https://github.com/ariankordi/ffl/commit/bd25dafc2029067368b28f6c96a8732d2157a712) (broken in the RIO port) and [OpenGL ES 2.0](https://github.com/ariankordi/ffl/commit/c5286b4822ca884dc2b5eb58c61ce54acac55bb6).
* Various [functions](https://github.com/ariankordi/ffl/commit/7840fb118f64a85838d31805491e33d8665ebfa6) such as FFLSetScale, and [enum definitions](https://github.com/ariankordi/ffl/commit/1b46640a60e6ba8f4a0ac0fcbb11195c43912a72) missing from the decomp.
* Minor [adjustments](https://github.com/ariankordi/ffl/commit/948780c0713f4366d9e4da30f3d616b03f83d652) [and](https://github.com/ariankordi/ffl/commit/5e843d33891878a64fbef820a76483be30305d4e) [optimizations](https://github.com/ariankordi/ffl/commit/d7eb341aa484decc9d532445c46d47a32c346ad8) and accuracy tweaks for the mask texture.
* WIP: Exports and a texture callback system to assist using FFL without RIO or OpenGL (todo, detail this?)

## Definitions
This documents all of the definitions that this supports to add/remove functionality, as of December 2024.

* FFL_ADD_GLAD_GL_IMPLEMENTATION - Adds GLAD implementation, enabling FFLGladLoadGL (dynamic linking)
* FFL_ENABLE_NEW_MASK_ONLY_FLAG  - Enables new mask only flag which does not initialize shapes.
* FFL_NO_DATABASE_DEFAULT    - disables default guest Miis
* FFL_NO_DATABASE_FILE       - Disables opening and use of hidden/official databases.
* FFL_NO_DATABASE_RANDOM     - Disables use of random database and FFLiGetRandomCharInfo.
* FFL_NO_MIDDLE_DB           - Disables FFLMiddleDB functionality (^^)
* FFL_NO_RENDER_TEXTURE      - Do not use FFLiRenderTexture (breaks FFLInitCharModelGPUStep)
* FFL_NO_FS                  - Disables FFL's use of RIO filesystem.
  - This is used for databases and non-cached resource loading.
* FFL_NO_NINTEXUTILS         - Disables ninTexUtils, which is needed for Wii U/FFLRes resource file support.
  - Specifically, this library deswizzles Wii U format textures. You can still use AFL resources with this.
* FFL_NO_DRAW_MASK_ALPHA_VALUES - Skip drawing alpha values on mask (FFL default behavior)...
  - This makes zero difference to the mask's appearance for me.
* FFL_MLC_PATH - Takes a quoted string. Defines the MLC path if FS is not disabled.
* FFL_PART_INDEX_WRAP - When out of bounds part indexes are passed in (invalid CharInfo)...
  - ... This option will choose to wrap that part index.

#### Not documented (slash useless):
* FFL_USE_ADJUST_MTX
* FFL_USE_MINIZ
* FFL_USE_MODULATE_EYEBROW_EX
* FFL_LOG_CHARMODEL_CLEANUP

## License Note
All of the changes that I have made to FFL in this repo are hereby licensed under [the Unlicense](https://choosealicense.com/licenses/unlicense/). However, this does not account for changes made by Abood when porting the decompilation, or the library to begin with.
