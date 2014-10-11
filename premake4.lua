solution "akari"
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
	include "07gui"
	include "08dithering"
	include "09collision"
	include "10terrain"
	include "11sdffont"
	include "12menu"
	include "13tilemap"
	
	-- Specify the base graphics library
	project "akari"
		kind "StaticLib"
		language "c++"
		files {"./src/**.cpp", "./src/**.cc", "./src/**.h"}

		includedirs {"../../glm", "../../sdl/include", "../../glew/include"}
		libdirs {"../../sdl/lib/x86", "../../glew/lib/x86"}