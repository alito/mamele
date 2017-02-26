-- license:BSD-3-Clause

---------------------------------------------------------------------------
--
--   learningenvironment.lua
--
--   Library objects for the learning environment
--
---------------------------------------------------------------------------

if _OPTIONS["with-learning-environment"] then
project "learningenvironment"
    kind "StaticLib"

    configuration { }

    includedirs {
        MAME_DIR .. "learning_environment",
        MAME_DIR .. "src/emu",
        MAME_DIR .. "src/osd",
        MAME_DIR .. "src/lib",
        MAME_DIR .. "src/lib/util",
    }

    files {
        MAME_DIR .. "learning_environment/learning-environment.cpp",
        MAME_DIR .. "learning_environment/learning-environment-utils.cpp",
    }
end
