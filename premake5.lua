workspace "Naja"
    configurations { "Debug"
                   , "Release"
                   }
project "Naja"
    location   "src/Naja"
    kind       "ConsoleApp"
    language   "C++"
    cppdialect "C++14"
    toolset    "clang"
    links      "Lexer"
    
    targetdir "bin/%{prj.name}"
    objdir    "bin-int/%{prj.name}"

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

project "Lexer"
    location   "src/Lexer"
    kind       "SharedLib"
    language   "C++"
    cppdialect "C++14"
    toolset    "clang"
    
    targetdir "bin/%{prj.name}"
    objdir    "bin-int/%{prj.name}"

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
