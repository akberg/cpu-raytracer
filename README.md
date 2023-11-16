# Ray Tracer application

Basic ray tracer application based on the Ray Tracing in One Weekend book. Doing this project to refresh my knowledge about ray tracing, and to have a small reference ray tracer that I know well for testing with acceleration structures.

## Mesh

Trying to balance between polygon meshes and supporting implicit shapes. Start by doing acceleration with only triangles. Later support could extend to allow all primitive objects.

In the end, what the ray tracing renderer needs is an iterable structure (list or tree) that yields hittable objects (implementing `hit(Ray, f64, f64, HitRecord&)`)

How is that normally done, when rendering complex meshes with ray tracing? Storing each triangle in isolation with all its information needed gets very costly:
```cpp
Triangle(
    vec3 v0,
    vec3 v1,
    vec3 v2,
    Material m
); // -> 80 bytes
Triangle(
    int i0,
    int i1,
    int i2,
    Mesh* mesh,
    Material* mat
); // -> 28 bytes
```

## Project structure

### src

Project source code

* `camera.hpp|cpp` - Camera properties, the renderer is initiated from `Camera::render(Hittable)`.
* `color.hpp` - converting and writing colour (not in use anymore)
* `hittable.hpp` - Interface `Hittable` for use with ray tracing.
* `hittableList.hpp` - List implementation of `Hittable`. (Move to "acceleration"?)
* `image.hpp` - Implement writing of PPM image format. To be replaced with some `stb`.
* `ray.hpp|cpp` - Ray tracing specifics, called per sample by `Camera::render(Hittable)`.
* `rtweekend.hpp` - Defines types, includes and utilities used here and there. Could be better organised.
* `texture.hpp|cpp` - Interface `Texture`, and includes some texture implementations: Solid Colour, Checker.
* `material.hpp|cpp` - Interface `Material`, and includes some material implementations.

Results using unity.tri and camera settings:
```
// 1
    cam.imageWidth      = 200;
    cam.aspectRatio     = 1.0;
    cam.vfov            = 80;
    cam.samplesPerPixel = 1;
    cam.maxDepth        = 2; // default 10
    cam.lookAt          = Vec3(-1.1, 0.0, 0.0);
    cam.lookFrom        = Vec3(-1.3, 0.2, 2.4);
    cam.vup             = Vec3(0.0, 1.0, 0.0);
    cam.focusDist       = 1.5;
    cam.defocusAngle    = 0.0;
```
```
// 2
    cam.imageWidth      = 200;
    cam.aspectRatio     = 1.0;
    cam.vfov            = 80;
    cam.samplesPerPixel = 2;
    cam.maxDepth        = 3; // default 10
    cam.lookAt          = Vec3(-1.1, 0.0, 0.0);
    cam.lookFrom        = Vec3(-1.3, 0.2, 2.4);
    cam.vup             = Vec3(0.0, 1.0, 0.0);
    cam.focusDist       = 1.5;
    cam.defocusAngle    = 0.0;
```
Running on my MSI laptop Fedora 38 in performance mode.

### basic (blikker part 1)

1: build: 31ms, nodes used: 10510, render: 249ms

### SAH (blikker part 2)

1: build: 138638ms, nodes used: 24147, render: 119s -> More than double render speed, for an insane increase of build time.
2 (32 bit): build: 134970ms, nodes used: 24479, render: 338ms
  (64 bit): build: 130693ms, nodes used: 24147, render: 289ms

It seems a 32-bit design causes more issues than benefits in this case (TBF, the guide is getting quite old, so this part might be a bit outdated).