# TileMapRendererExperiments
Two methods, one based on this:
https://github.com/davudk/OpenGL-TileMap-Demos

One using vertex pulling (better) - this is the one actually used. Store integer indices for the entire big map in an openGL texture, has code that draws chunks of the map that are visible to the camera (chunks of definable size.
