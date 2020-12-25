workspace "Naja"
    configurations { "Debug"
                   , "Release"
                   }

project "Naja"
    location "src"
    kind "ConsoleApp"
    language "C"
    links "flex_bison"
    
    targetdir "bin/%{prj.name}"
    objdir    "bin-int/%{prj.name}"

    includedirs { "%{prj.location}" }

    files { "%{prj.location}/flex_bison/**.c"
          , "%{prj.location}/flex_bison/**.h"
          , "%{prj.location}/Naja/**.c"
          , "%{prj.location}/Naja/**.h"
          }

    buildoptions { "-lfl" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"


project "flex_bison"
    location "src/flex_bison"
    kind "Utility"

    prebuildcommands
    { "bison -Wconflicts-sr -d -o %{prj.location}/parser.c %{prj.location}/parser.y"
    , "flex -o %{prj.location}/scanner.c %{prj.location}/scanner.l"
    }