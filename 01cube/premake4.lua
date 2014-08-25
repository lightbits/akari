project(path.getname(os.getcwd()))
	kind "ConsoleApp"
	language "c++"
	objdir "../obj"
	files { "**.cpp", "**.cc", "**.h"} -- recursively add files
	includedirs {"../../../glm", "../../../sdl/include", "../../../glew/include", "../src"}
	libdirs {"../../../sdl/lib/x86", "../../../glew/lib/x86"}
	links { "akari" }