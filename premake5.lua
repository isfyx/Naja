workspace "Naja"
    configurations { "Debug"
                   , "Release"
                   }
project "Naja"
    location   "src"
    kind       "ConsoleApp"
    language   "C++"
    cppdialect "C++14"
    toolset    "clang"
    
    targetdir "bin/%{cfg.buildcfg}"
    objdir    "bin-int/%{cfg.buildcfg}"

    includedirs { "src" }

    files { "%{prj.location}/**.cpp"
          , "%{prj.location}/**.hpp"
          }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"