# Lugaru

![Banner](./Docs/banner3.jpg)

Fork of Lugaru with modern conan and cmake based build system and many bugfixes.
Current goal is a total refactroing of the game into separate modules and removal of all globals.

## Requirements

![Banner](./Docs/banner4.jpg)

Development is done with
* Conan 2.x
* CMake 3.27.x
* Windows: Visual Studio 2022 Community Edition
* Linux: Make

## Building

![Banner](./Docs/banner1.jpg)

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

![Banner](./Docs/banner5.jpg)

The game follows a layered design.
Each layer consists of one or more modules. 
A module can access any module in a layer below or on the same layer but not above.
```
-----------------------------------------------
|                   App                       | Application Layer
-----------------------------------------------
|                   Game                      | Game Logic Layer
-----------------------------------------------
| Graphic | Animation | Objects | Environment | Graphics Layer
-----------------------------------------------
|                   Audio                     | Audio Layer
-----------------------------------------------
|  Math  |  Platform  |  Thirdparty  |  Utils | Foundation Layer
-----------------------------------------------
```

Graphics layer module dependencies
```
Animation -> Graphic
          -> Objects
          -> Audio
          -> Foundation

Environment -> Graphic
            -> Objects
            -> Foundation

Graphic -> Environment
        -> Objects
        -> Foundation

Objects -> Environment
        -> Graphic
        -> Animation
        -> Audio
        -> Foundation
```

## License

![Banner](./Docs/banner2.jpg)

The source code is distributed under the GNU General Public License version 2
or (at your option) any later version (GPLv2+).

The assets (campaigns, graphical and audio assets, etc.) in the `Data` folder
are distributed under the Creative Commons Attribution - Share Alike license,
some in version 3.0 Unported (CC-BY-SA 3.0) and others in version 4.0
International (CC-BY-SA 4.0) as described in the `CONTENT-LICENSE.txt` file.