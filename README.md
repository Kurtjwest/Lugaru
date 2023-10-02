# Lugaru

Fork of Lugaru with modern conan and cmake based build system and many bugfixes.
![Banner](./Docs/banner1.jpg)
Current goal is a total refactroing of the game into separate modules and removal of all globals.
![Banner](./Docs/banner2.jpg)

## Requirements

Development is done with
* Conan 2.x
* CMake 3.27.x
* Windows: Visual Studio 2022 Community Edition
* Linux: Make

## Building

Conan install for debug and release builds
```
conan install . --build=missing --settings=build_type=Debug
conan install . --build=missing --settings=build_type=Release
```

CMake project generation

On Windows
```
cmake --preset conan-default
```

On Linux
```
cmake --preset conan-debug
cmake --preset conan-release
```

On Windows, open the generaed solution file in the build folder.
On Linux use
```
cmake --build build/Debug
cmake --build build/Release
```

## Design

The game follows a layered design where a layer can access any layer below but not on the same level.
```
      App
---------------
     Audio
---------------
   Foundation
```

Dependencies
```
Animation -> Graphic
          -> Objects
          -> Audio
          -> Foundation

Environment -> Graphic
            -> Foundation

Graphic -> Environment
        -> Foundation
        -> Objects
```
       