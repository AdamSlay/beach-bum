# BEACH BUM (wip)
This is a simple 2D game engine written in C++ using SDL2. 
I'm working on this as a learning experience to get a better understanding of how game engines work, specifically graphics/rendering. 
Also, I love video games and I came up with this little character a while back, so I wanted to use him for something.
The platforms and background are procedurally generated, so the game is different on each run. The player can move left and right,
and jump. If the player falls off the ground, they will respawn at the starting position. It's really just a minimum viable platformer.

It is a work in progress and is not yet ready for use.

Here are some gifs of the current progress: 

![Beach Bum Example Gif](assets/bb_example.gif)

a different background and set of platforms is generated on this run:

![Beach Bum Example Gif 2](assets/bb_example_2.gif)

## Installation
This project uses [vcpkg](https://github.com/microsoft/vcpkg).

Follow the [install instructions](https://github.com/microsoft/vcpkg#quick-start-unix) to install ```vcpkg``` into the project folder. Then run the following to install the project dependencies:
```shell
    ./vcpkg install sdl2
    ./vcpkg install sdl2-image
    ./vcpkg install nlohmann-json
```

if you're using an IDE to build and run your project, you'll need to add the following flag to your cmake options:
```shell
-DCMAKE_TOOLCHAIN_FILE=[project_dir or .]/scripts/buildsystems/vcpkg.cmake
```