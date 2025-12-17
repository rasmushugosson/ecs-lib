-- Reusable ECS project definition
-- Can be included by parent projects via: include("path/to/ecs-lib/ecs-project.lua")
-- This will also include the Log project dependency

local ecs_lib_dir = path.getdirectory(_SCRIPT)
local ecs_lib_src = ecs_lib_dir .. "/ecs-lib"
local log_lib_dir = ecs_lib_dir .. "/dep/log-lib"

include(log_lib_dir .. "/log-project.lua")

project("ECS")
    kind("StaticLib")
    language("C++")
    cppdialect("C++23")
    objdir("obj/%{prj.name}/%{cfg.buildcfg}")
    targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

    files({
        ecs_lib_src .. "/src/**.cpp",
        ecs_lib_src .. "/src/**.h",
        ecs_lib_src .. "/include/**.h"
    })

    includedirs({
        log_lib_dir .. "/log-lib/include",
        ecs_lib_src .. "/include",
        ecs_lib_src .. "/src"
    })

    links({ "Log" })

    pchheader(path.getabsolute(ecs_lib_src .. "/src/general/pch.h"))
    pchsource(ecs_lib_src .. "/src/general/pch.cpp")
