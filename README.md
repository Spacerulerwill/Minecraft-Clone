# Building - 64 bit only (CMake)

## Windows

    git clone https://github.com/Spacerulerwill/Minecraft-Clone.git --recursive
    cd Minecraft-Clone/tools/windows
    configure.bat
    cd ../../build
    MinecraftClone.sln

Now you can compile it like any other visual studio project

## Linux

    git clone https://github.com/Spacerulerwill/Minecraft-Clone.git --recursive
    cd Minecraft-Clone/tools/unix

From here you can either run `configure_debug.sh` or `configure_release.sh`

    ./configure_debug.sh
    ./build.sh

And then to run the program

    ./run.sh
