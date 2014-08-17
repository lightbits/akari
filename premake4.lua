solution "kyoko"
	configurations {"Debug", "Release"}

	configuration "windows"
		defines {"WIN32", "_WIN32"}
		links {"opengl32", "glew32", "SDL2", "SDL2main"}

	configuration {"Debug"}
		defines "DEBUG"
		flags "Symbols"
		targetsuffix "D"
		targetdir "bin/"

	configuration {"Release"}
		defines "NDEBUG"
		flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"}
		targetdir "bin/"

	-- Include demo projects
	include "00base"
	include "01cube"
	include "02model"
	include "03scattering"
	include "04texture"
	include "05gfx2d"
	include "06arkanoid"
	
	-- Specify the base graphics library
	project "kyoko"
		kind "StaticLib"
		language "c++"
		files {"./src/**.cpp", "./src/**.cc", "./src/**.h"}

		includedirs {"../../glm", "../../sdl/include", "../../glew/include"}
		libdirs {"../../sdl/lib/x86", "../../glew/lib/x86"}