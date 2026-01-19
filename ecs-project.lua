-- Reusable ECS project definition
-- Can be included by parent projects via: include("path/to/ecs-lib/ecs-project.lua")
-- This will also include the Log project dependency (if not already included)

local ecs_lib_dir = path.getdirectory(_SCRIPT)
local ecs_lib_src = ecs_lib_dir .. "/ecs-lib"
local log_lib_dir = ecs_lib_dir .. "/dep/log-lib"

-- Only include log-lib if not already included by parent project
if not LOG_LIB_INCLUDED then
    include(log_lib_dir .. "/log-project.lua")
end

-- Use exported path from log-lib if available, otherwise use local path
local log_include_dir = LOG_LIB_INCLUDE_DIR or (log_lib_dir .. "/log-lib/include")

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
        log_include_dir,
        ecs_lib_src .. "/include",
        ecs_lib_src .. "/src"
    })

    links({ "Log" })

    pchheader(path.getabsolute(ecs_lib_src .. "/src/general/pch.h"))
    pchsource(ecs_lib_src .. "/src/general/pch.cpp")
