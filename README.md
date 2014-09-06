### akari ###

A 3D graphics library full of helpful functions and things.

### done ###

* Model loading
* ImGui    
	[ocornut's library](https://github.com/ocornut/imgui)

### todo ###

* SSAO
	* http://floored.com/blog/2013/ssao-screen-space-ambient-occlusion.html  
	* http://developer.download.nvidia.com/SDK/10.5/direct3d/Source/ScreenSpaceAO/doc/ScreenSpaceAO.pdf  
	* http://iquilezles.org/www/articles/ssao/ssao.htm  
	* http://iquilezles.org/www/articles/sphereao/sphereao.htm  
	* http://john-chapman-graphics.blogspot.no/2013/01/ssao-tutorial.html  
	* http://blog.evoserv.at/index.php/2012/12/hemispherical-screen-space-ambient-occlusion-ssao-for-deferred-renderers-using-openglglsl/  
* Ingame shader editor  
	* Using ImGUI  
	* Reload shader on the fly, compilation errors  
	* Parse shader, detect // color 0.0 1.0 and create color slider in GUI
* xBR scaler  
	[description of the algorithm](http://board.byuu.org/viewtopic.php?f=10&t=2248)  
	[implementation](http://sourceforge.net/p/paintown/code/7842/tree/trunk/src/util/graphics/sdl/xbr.cpp)  
	[mame implementation](http://sourceforge.net/projects/hqmame/files/)  
* Scalable height field self shadowing  
	[paper](http://wili.cc/research/hfshadow/)  
* Shadows  
	[slides](http://advancedgraphics.marries.nl/lectures/03_shadows.pdf)
* Voxel cone tracing  
	[video](https://www.youtube.com/user/cheaterproject/videos)  
	[slides](http://www.slideshare.net/mmostajab/voxel-based-globalillumination)  
	[various links](http://d.hatena.ne.jp/hanecci/20131019/p5)  
	[visualizing voxels](http://www.alexandre-pestana.com/voxel-visualization-using-drawindexedinstancedindirect/)  
	[an implementation](https://github.com/otaku690/SparseVoxelOctree)  
	[voxelization](http://www.alexandre-pestana.com/voxelization-using-gpu-hardware-rasterizer/)  
	[shadow casting](http://www.alexandre-pestana.com/dynamic-shadow-casting-point-lights-tiled-deferred-rendering/)  
	[gtc 2012](http://www.icare3d.org/research/GTC2012_Voxelization_public.pdf)
* Tessendorf water  
* Volumetric clouds  
	Compute shader
	Pyroclastic volume
	Sliced volume rendering
	[intel samples](https://github.com/GameTechDev)
	[intel cloud sample](https://software.intel.com/en-us/blogs/2014/03/31/cloud-rendering-sample)
* Volumetric fog  
	http://www.alexandre-pestana.com/volumetric-lights/  
* Terrains + tesselation  
	iq, improved perlin noise
* Motion blur  
* Subsurface scattering  
* Dynamic shadows through voxelization  
	http://www.alexandre-pestana.com/dynamic-shadow-casting-point-lights-tiled-deferred-rendering/
* Depth of field / bokeh  
	http://bartwronski.com/2014/04/07/bokeh-depth-of-field-going-insane-part-1/
* Fourier opacity mapping  
* Adaptive volumetric shadow mapping  
* Spherical harmonics  
* Variance shadow maps  
* Volume rendering  
	[nice paper](http://magnuswrenninge.com/content/pubs/VolumetricMethodsInVisualEffects2010.pdf)
	[intro to volume rendering](http://graphicsrunner.blogspot.no/2009/01/volume-rendering-101.html)
	[gpu gems](http://http.developer.nvidia.com/GPUGems/gpugems_ch39.html)  
* 3D fluid sim  
	[an implementaion](http://scrawkblog.com/2014/01/09/gpu-gems-to-unity-3d-fluid-simulation/)
	
[stuff](http://advancedgraphics.marries.nl/)
[more stuff](http://oglplus.org/oglplus/html/examples.html)
