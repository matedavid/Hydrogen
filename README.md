Hydrogen
-

Rendering Engine developed using the OpenGL graphics api.

![HydrogenImage](https://github.com/matedavid/Hydrogen/assets/42835499/e3e44617-68bf-4f79-b508-fd326a9c1c58)

### Features:
- PBR materials
- Image based environment lighting
- Model loading

### How to compile
To compile the code, you just need to configure the CMakeLists provided in the following way:
```
mkdir build
cd build
cmake ..
```

Then, just compile the code with `make`, `ninja` or any other alternative. This will compile the Hydrogen library, and a Sandbox project.

### Execute the Sandbox Project
After compiling the code, you need to enter the `build/Sandbox` directory and run the executable: `./Sandbox`. 
