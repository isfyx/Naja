workspace "Naja"
    configurations { "Debug"
                   , "Release"
                   }

project "Naja"
    location "Naja"
    kind "ConsoleApp"
    language "C"
    links "flex_bison"
    
    targetdir "bin/%{prj.name}"
    objdir    "bin-int/%{prj.name}"

    files { "%{prj.location}/src/Naja/**.c"
          , "%{prj.location}/src/Naja/**.h"
          , "%{prj.location}/src/flex_bison/parser.c"
          , "%{prj.location}/src/flex_bison/scanner.c"
          }

    buildoptions { "-lfl" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"


project "flex_bison"
    location "Naja/src/flex_bison"
    kind "Utility"

    prebuildcommands
    { "bison -d -o %{prj.location}/parser.c %{prj.location}/parser.y"
    , "flex -o %{prj.location}/scanner.c %{prj.location}/scanner.l"
    }