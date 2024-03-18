# TODO
* Add partial chunk unloading (chunk vertices are loaded on vram but not no voxels stored on ram)
* Indirect drawing (one draw call per stack? maybe one for the whole world?)
* Convert chunk stacks to use deque's allowing for infinite vertical build height
* Add rotatable block directions
* Fix collision detection bugs
* Optimise collision detection
* Make chunk unloading non blocking
* Add compression for chunk unloading
* Switch to a new audio library? (Seems to be broken on linux mint but it may just be my distro)
* Chunk LOD system
* Untie physics from framerate
* Add water physics plus underwater shader
* Water fluid simulation