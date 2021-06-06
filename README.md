# Engine2
Second version of working through a graphics engine

Some conventions...

LH coorindates

VS Constant Buffers
0 = scene
1 = model

PS Constant Buffers
0 = scene
1 = material
2 = shadow lighting

PS Textures/Samplers
0 = Skybox
1 = Shadow map
2 = Point lights (structured buffer)


Notes:
- Steps to get Nvidia PhysX working is in the Physics.h file.
