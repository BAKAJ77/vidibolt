workspace "vidibolt"
    configurations { "Debug", "Release" }
    architecture "x86_64"

--------------------------------------------------------------------------------------------------------
---- Project file of VIDIBOLT core

project "vidibolt"
    location "%{prj.name}"

    -- General project settings
    kind "SharedLib"
    staticruntime "off"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{prj.name}/"
    objdir "objs/%{cfg.buildcfg}-%{cfg.architecture}/%{prj.name}/"

    includedirs { "%{prj.name}/src", "libs/openssl/include" }
    files { "%{prj.name}/src/**.h", "%{prj.name}/src/**.cpp", "%{prj.name}/src/**.tpp" }
    links { "libcrypto", "libssl" }

    defines { "VOLT_EXPORTS" }

    -- Project platform define macro based on identified system
    filter "system:windows"
        defines { "VOLT_PLATFORM_WINDOWS" }

    filter "system:macosx"
        defines { "VOLT_PLATFORM_MACOSX" }

    -- Project settings with values unique to the Debug/Release configurations
    filter "configurations:Debug"
        targetname "libvolt-dbg"
        libdirs { "bin/openssl/debug" }
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        targetname "libvolt"
        libdirs { "bin/openssl/release" }
        defines { "NDEBUG" }
        optimize "Speed"

--------------------------------------------------------------------------------------------------------
---- Project files for testing the VIDIBOLT core

project "blockchain_test"
    location "test" -- Set the location of project files relative to this premake script file

    -- General project settings
    kind "ConsoleApp"
    staticruntime "off"
    language "C++"
    cppdialect "C++17"

    targetdir "%{prj.location}/bin/%{cfg.buildcfg}-%{cfg.architecture}/"
    objdir "%{prj.location}/objs/%{cfg.buildcfg}-%{cfg.architecture}/%{prj.name}"

    includedirs { "%{prj.location}/src", "vidibolt/src" }
    files { "%{prj.location}/src/**.h", "%{prj.location}/src/**.cpp", "%{prj.location}/src/**.tpp" }

    libdirs { "bin/vidibolt" }

    -- Project platform define macro based on identified system
    filter "system:windows"
        defines { "VOLT_PLATFORM_WINDOWS" }

    filter "system:macosx"
        defines { "VOLT_PLATFORM_MACOSX" }

    -- Project settings with values unique to the Debug/Release configurations
    filter "configurations:Debug"
        links { "libvolt-dbg" }
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        links { "libvolt" }
        defines { "NDEBUG" }
        optimize "Speed"

    -- Post build commands for project unique to platforms and configurations
    filter { "system:windows", "configurations:Debug" }
        postbuildcommands { "copy ..\\bin\\vidibolt\\libvolt-dbg.dll bin\\%{cfg.buildcfg}-%{cfg.architecture}\\libvolt-dbg.dll",
            "copy ..\\bin\\openssl\\debug\\libcrypto-3-x64.dll bin\\%{cfg.buildcfg}-%{cfg.architecture}\\libcrypto-3-x64.dll",
            "copy ..\\bin\\openssl\\debug\\libssl-3-x64.dll bin\\%{cfg.buildcfg}-%{cfg.architecture}\\libssl-3-x64.dll" }

    filter { "system:windows", "configurations:Release" }
        postbuildcommands { "copy ..\\bin\\vidibolt\\libvolt.dll bin\\%{cfg.buildcfg}-%{cfg.architecture}\\libvolt.dll",
            "copy ..\\bin\\openssl\\release\\libcrypto-3-x64.dll bin\\%{cfg.buildcfg}-%{cfg.architecture}\\libcrypto-3-x64.dll",
            "copy ..\\bin\\openssl\\release\\libssl-3-x64.dll bin\\%{cfg.buildcfg}-%{cfg.architecture}\\libssl-3-x64.dll" }

    filter { "not system:windows", "configurations:Debug" }
        postbuildcommands { "cp ../bin/vidibolt/libvolt-dbg.dll bin/%{cfg.buildcfg}-%{cfg.architecture}/libvolt-dbg.dll",
            "cp ../bin/openssl/debug/libcrypto-3-x64.dll bin/%{cfg.buildcfg}-%{cfg.architecture}/libcrypto-3-x64.dll",
            "cp ../bin/openssl/debug/libssl-3-x64.dll bin/%{cfg.buildcfg}-%{cfg.architecture}/libssl-3-x64.dll" }

    filter { "not system:windows", "configurations:Release" }
        postbuildcommands { "cp ../bin/vidibolt/libvolt-dbg.dll bin/%{cfg.buildcfg}-%{cfg.architecture}/libvolt-dbg.dll",
            "cp ../bin/openssl/release/libcrypto-3-x64.dll bin/%{cfg.buildcfg}-%{cfg.architecture}/libcrypto-3-x64.dll",
            "cp ../bin/openssl/release/libssl-3-x64.dll bin/%{cfg.buildcfg}-%{cfg.architecture}/libssl-3-x64.dll" }

--------------------------------------------------------------------------------------------------------
