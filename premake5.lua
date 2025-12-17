local function is_linux()
	return os.target() == "linux"
end

local function is_windows()
	return os.target() == "windows"
end

local function is_macos()
	return os.target() == "macosx"
end

local function is_clang()
	return _OPTIONS["cc"] == "clang" or _OPTIONS["cc"] == "clangcl"
end

local function workspace_location()
	if is_windows() then
		return "build/windows"
	elseif is_linux() then
		if is_clang() then
			return "build/linux-clang"
		else
			return "build/linux-gcc"
		end
	elseif is_macos() then
		if is_clang() then
			return "build/macos-clang"
		else
			return "build/macos"
		end
	end
	return "build"
end

workspace("ECSLib")
location(workspace_location())
architecture("x64")
configurations({ "Debug", "Release", "Dist" })
warnings("Extra")

filter("system:windows")
defines({ "AE_WINDOWS" })

filter("system:macosx")
defines({ "AE_MACOS" })

filter("system:linux")
defines({ "AE_LINUX" })

filter({})

filter("configurations:Debug")
defines({ "AE_DEBUG" })
symbols("On")

filter("configurations:Release")
defines({ "AE_RELEASE", "NDEBUG" })
optimize("Speed")

filter("configurations:Dist")
defines({ "AE_DIST", "NDEBUG" })
optimize("Speed")
symbols("Off")

filter({})

filter({ "action:vs*", "configurations:Debug" })
runtime("Debug")

filter({ "action:vs*", "configurations:Release or configurations:Dist" })
runtime("Release")

filter("action:vs*")
startproject("Sandbox")
filter({})

filter({ "system:linux or system:macosx" })
if is_clang() then
	print(">> Using Clang toolchain")
	buildoptions({
		"-Wall",
		"-Wextra",
		"-Wpedantic",
		"-fcolor-diagnostics",
	})
else
	print(">> Using GCC toolchain")
	buildoptions({
		"-Wall",
		"-Wextra",
		"-Wpedantic",
		"-fdiagnostics-color=always",
	})
end
filter({})

include("ecs-project.lua")

project("Sandbox")
kind("ConsoleApp")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({ "sandbox/src/**.cpp", "sandbox/src/**.h" })

includedirs({
	"dep/log-lib/log-lib/include",
	"ecs-lib/include",
	"sandbox/src",
})

links({ "Log", "ECS" })

pchheader(path.getabsolute("sandbox/src/general/pch.h"))
pchsource("sandbox/src/general/pch.cpp")

local function own_source_files()
	local files = {}

	local function add(glob)
		for _, f in ipairs(os.matchfiles(glob)) do
			table.insert(files, f)
		end
	end

	add("ecs-lib/src/**.cpp")
	add("ecs-lib/src/**.c")
	add("ecs-lib/include/**.h")
	add("ecs-lib/include/**.hpp")

	add("sandbox/src/**.cpp")
	add("sandbox/src/**.c")
	add("sandbox/src/**.h")
	add("sandbox/src/**.hpp")

	return files
end

newaction({
	trigger = "format",
	description = "Run clang-format on all non-vendor C/C++ files",

	execute = function()
		local files = own_source_files()
		if #files == 0 then
			print("No source files found to format.")
			return
		end

		local cmd = "clang-format -i"

		for _, f in ipairs(files) do
			cmd = cmd .. " " .. f
		end

		print("Running:", cmd)
		os.execute(cmd)
	end,
})

local function run_clang_tidy(fix_mode)
	local all_files = own_source_files()
	local cpp_files = {}

	for _, f in ipairs(all_files) do
		local ext = path.getextension(f)
		if ext == ".cpp" or ext == ".cc" or ext == ".cxx" then
			table.insert(cpp_files, f)
		end
	end

	if #cpp_files == 0 then
		print("No C++ source files found to lint.")
		return
	end

	local build_dir = "."

	local mode_label = fix_mode and "lint + fix" or "lint"
	print(string.format("Running clang-tidy (%s) on %d files...", mode_label, #cpp_files))

	for _, f in ipairs(cpp_files) do
		local extra = fix_mode and "-fix -fix-errors" or ""
		local cmd = string.format('clang-tidy -quiet %s -p "%s" "%s"', extra, build_dir, f)

		local result = os.execute(cmd)

		if result ~= 0 then
			print("")
			if fix_mode then
				print("clang-tidy reported issues (and may have applied fixes) in " .. f .. ".")
			else
				print("Issues found in " .. f .. " (see diagnostics above).")
			end
			print("")
		end
	end

	if fix_mode then
		print("clang-tidy auto-fix pass completed. Review changes with git diff.")
	else
		print("clang-tidy lint completed.")
	end
end

newaction({
	trigger = "lint",
	description = "Run clang-tidy on all non-vendor C++ source files",
	execute = function()
		run_clang_tidy(false)
	end,
})

newaction({
	trigger = "lint-fix",
	description = "Run clang-tidy with automatic fixes on all non-vendor C++ source files",
	execute = function()
		print("NOTE: This will modify your source files in-place. Make sure your tree is clean (git status).")
		run_clang_tidy(true)
	end,
})

newaction({
	trigger = "gmake-gcc",
	description = "Generate GNU Makefiles (gmake) using GCC on *nix",
	execute = function()
		os.execute("premake5 gmake --cc=gcc")
	end,
})

newaction({
	trigger = "gmake-clang",
	description = "Generate GNU Makefiles (gmake) using Clang on *nix",
	execute = function()
		os.execute("premake5 gmake --cc=clang")
	end,
})
