-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   nl.lua
--
--   Compiles all drivers using netlist code
--   Use make SUBTARGET=nl to build
--
---------------------------------------------------------------------------


--------------------------------------------------
-- Specify all the CPU cores necessary for the
-- drivers referenced in nl.lst.
--------------------------------------------------

CPUS["Z80"] = true
CPUS["M6502"] = true
CPUS["M6800"] = true
CPUS["M6803"] = true
CPUS["M6809"] = true
CPUS["MCS48"] = true
CPUS["I8085"] = true
CPUS["MCS51"] = true
--CPUS["M6800"] = true
--CPUS["M6809"] = true
--CPUS["M680X0"] = true
CPUS["M680X0"] = true
--CPUS["TMS9900"] = true
--CPUS["COP400"] = true
CPUS["F8"] = true

--------------------------------------------------
-- Specify all the sound cores necessary for the
-- drivers referenced in nl.lst.
--------------------------------------------------

--SOUNDS["SAMPLES"] = true
SOUNDS["DAC"] = true
SOUNDS["DISCRETE"] = true
SOUNDS["AY8910"] = true
SOUNDS["MSM5205"] = true
--SOUNDS["ASTROCADE"] = true
SOUNDS["TMS5220"] = true
SOUNDS["OKIM6295"] = true
SOUNDS["UPD7759"] = true
--SOUNDS["HC55516"] = true
--SOUNDS["YM3812"] = true
--SOUNDS["CEM3394"] = true
--SOUNDS["VOTRAX"] = true
SOUNDS["YM2151"] = true
SOUNDS["YM2413"] = true
SOUNDS["BEEP"] = true
SOUNDS["VOLT_REG"] = true
SOUNDS["SPEAKER"] = true
SOUNDS["DIGITALKER"] = true
SOUNDS["SN76477"] = true

--------------------------------------------------
-- specify available video cores
--------------------------------------------------

VIDEOS["FIXFREQ"] = true
VIDEOS["PWM_DISPLAY"] = true

--------------------------------------------------
-- specify available machine cores
--------------------------------------------------

MACHINES["INPUT_MERGER"] = true
MACHINES["NETLIST"] = true
MACHINES["Z80DMA"] = true
MACHINES["Z80DAISY"] = true
MACHINES["GEN_LATCH"] = true
MACHINES["AY31015"] = true
MACHINES["KB3600"] = true
MACHINES["COM8116"] = true

MACHINES["TTL74145"] = true
MACHINES["TTL74259"] = true
MACHINES["6522VIA"] = true

MACHINES["6821PIA"] = true
MACHINES["I8255"] = true
MACHINES["WATCHDOG"] = true
MACHINES["EEPROMDEV"] = true
MACHINES["UPD4701"] = true
MACHINES["CXD1095"] = true
--MACHINES["TTL74148"] = true
--MACHINES["TTL74153"] = true
--MACHINES["TTL7474"] = true
--MACHINES["RIOT6532"] = true
--MACHINES["PIT8253"] = true
--MACHINES["Z80CTC"] = true
--MACHINES["68681"] = true
--MACHINES["BANKDEV"] = true
MACHINES["F3853"] = true
MACHINES["MB14241"] = true

--------------------------------------------------
-- specify available bus cores
--------------------------------------------------

-- not needed by nl.lua but build system wants at least one bus
BUSES["CENTRONICS"] = true

--------------------------------------------------
-- This is the list of files that are necessary
-- for building all of the drivers referenced
-- in nl.lst
--------------------------------------------------

function createProjects_mame_nl(_target, _subtarget)
	project ("mame_netlist")
	targetsubdir(_target .."_" .. _subtarget)
	kind (LIBTYPE)
	uuid (os.uuid("drv-mame-nl"))
	addprojectflags()
	precompiledheaders_novs()

	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/devices",
		MAME_DIR .. "src/lib/netlist",
		MAME_DIR .. "src/mame",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "3rdparty",
		GEN_DIR  .. "mame/layout",
		ext_includedir("rapidjson"),
	}

files{
	MAME_DIR .. "src/mame/drivers/pong.cpp",
	MAME_DIR .. "src/mame/machine/nl_pongf.cpp",
	MAME_DIR .. "src/mame/machine/nl_pongf.h",
	MAME_DIR .. "src/mame/machine/nl_pongdoubles.cpp",
	MAME_DIR .. "src/mame/machine/nl_pongdoubles.h",
	MAME_DIR .. "src/mame/machine/nl_breakout.cpp",
	MAME_DIR .. "src/mame/machine/nl_breakout.h",
	MAME_DIR .. "src/mame/machine/nl_rebound.cpp",
	MAME_DIR .. "src/mame/machine/nl_rebound.h",
	MAME_DIR .. "src/mame/machine/nl_hazelvid.cpp",
	MAME_DIR .. "src/mame/machine/nl_hazelvid.h",

	MAME_DIR .. "src/mame/drivers/atarittl.cpp",
	MAME_DIR .. "src/mame/machine/nl_stuntcyc.cpp",
	MAME_DIR .. "src/mame/machine/nl_stuntcyc.h",
	MAME_DIR .. "src/mame/machine/nl_gtrak10.cpp",
	MAME_DIR .. "src/mame/machine/nl_gtrak10.h",

	MAME_DIR .. "src/mame/drivers/hazeltin.cpp",

	MAME_DIR .. "src/mame/drivers/1942.cpp",
	MAME_DIR .. "src/mame/includes/1942.h",
	MAME_DIR .. "src/mame/video/1942.cpp",
	MAME_DIR .. "src/mame/audio/nl_1942.cpp",
	MAME_DIR .. "src/mame/audio/nl_1942.h",

	MAME_DIR .. "src/mame/drivers/gamemachine.cpp",
	MAME_DIR .. "src/mame/audio/nl_gamemachine.h",
	MAME_DIR .. "src/mame/audio/nl_gamemachine.cpp",

	MAME_DIR .. "src/mame/drivers/popeye.cpp",
	MAME_DIR .. "src/mame/includes/popeye.h",
	MAME_DIR .. "src/mame/video/popeye.cpp",
	MAME_DIR .. "src/mame/audio/nl_popeye.cpp",
	MAME_DIR .. "src/mame/audio/nl_popeye.h",

	MAME_DIR .. "src/mame/drivers/mario.cpp",
	MAME_DIR .. "src/mame/includes/mario.h",
	MAME_DIR .. "src/mame/audio/nl_mario.cpp",
	MAME_DIR .. "src/mame/audio/nl_mario.h",
	MAME_DIR .. "src/mame/video/mario.cpp",
	MAME_DIR .. "src/mame/audio/mario.cpp",

	MAME_DIR .. "src/mame/drivers/m62.cpp",
	MAME_DIR .. "src/mame/includes/m62.h",
	MAME_DIR .. "src/mame/video/m62.cpp",
	MAME_DIR .. "src/mame/audio/irem.cpp",
	MAME_DIR .. "src/mame/audio/irem.h",
	MAME_DIR .. "src/mame/audio/nl_kidniki.cpp",
	MAME_DIR .. "src/mame/audio/nl_kidniki.h",

  MAME_DIR .. "src/mame/machine/mw8080bw.cpp",
  MAME_DIR .. "src/mame/drivers/mw8080bw.cpp",
  MAME_DIR .. "src/mame/includes/mw8080bw.h",
  MAME_DIR .. "src/mame/audio/mw8080bw.h",
  MAME_DIR .. "src/mame/audio/mw8080bw.cpp",
  MAME_DIR .. "src/mame/video/mw8080bw.cpp",
	MAME_DIR .. "src/mame/audio/nl_gunfight.cpp",
	MAME_DIR .. "src/mame/audio/nl_gunfight.h",
	MAME_DIR .. "src/mame/audio/nl_280zzzap.cpp",
	MAME_DIR .. "src/mame/audio/nl_280zzzap.h",

	MAME_DIR .. "src/mame/audio/cheekyms.cpp",
	MAME_DIR .. "src/mame/audio/cheekyms.h",
	MAME_DIR .. "src/mame/audio/nl_cheekyms.cpp",
	MAME_DIR .. "src/mame/audio/nl_cheekyms.h",
	MAME_DIR .. "src/mame/drivers/cheekyms.cpp",
	MAME_DIR .. "src/mame/includes/cheekyms.h",
	MAME_DIR .. "src/mame/video/cheekyms.cpp",

	MAME_DIR .. "src/mame/drivers/galaxian.cpp",
	MAME_DIR .. "src/mame/includes/galaxian.h",
	MAME_DIR .. "src/mame/audio/galaxian.cpp",
	MAME_DIR .. "src/mame/audio/galaxian.h",
	MAME_DIR .. "src/mame/video/galaxian.cpp",
	MAME_DIR .. "src/mame/audio/nl_konami.h",
	MAME_DIR .. "src/mame/audio/nl_konami.cpp",

	MAME_DIR .. "src/mame/audio/cclimber.cpp",
	MAME_DIR .. "src/mame/audio/cclimber.h",

	MAME_DIR .. "src/mame/audio/nl_zac1b11142.cpp",
	MAME_DIR .. "src/mame/audio/nl_zacc1b11142.h",
	MAME_DIR .. "src/mame/audio/zaccaria.cpp",
	MAME_DIR .. "src/mame/audio/zaccaria.h",
	MAME_DIR .. "src/mame/drivers/zaccaria.cpp",
	MAME_DIR .. "src/mame/includes/zaccaria.h",
	MAME_DIR .. "src/mame/video/zaccaria.cpp",

	MAME_DIR .. "src/mame/drivers/cocoloco.cpp",
	MAME_DIR .. "src/mame/audio/nl_cocoloco.h",
	MAME_DIR .. "src/mame/audio/nl_cocoloco.cpp",

	MAME_DIR .. "src/mame/drivers/palestra.cpp",
	MAME_DIR .. "src/mame/machine/nl_palestra.cpp",
	MAME_DIR .. "src/mame/machine/nl_palestra.h",

	MAME_DIR .. "src/mame/drivers/segas16b.cpp",
	MAME_DIR .. "src/mame/includes/segas16b.h",
	MAME_DIR .. "src/mame/video/segas16b.cpp",
	MAME_DIR .. "src/mame/audio/nl_segas16b.cpp",
	MAME_DIR .. "src/mame/audio/nl_segas16b.h",
	MAME_DIR .. "src/mame/machine/315_5195.cpp",
	MAME_DIR .. "src/mame/machine/315_5195.h",
	MAME_DIR .. "src/mame/machine/fd1089.cpp",
	MAME_DIR .. "src/mame/machine/fd1089.h",
	MAME_DIR .. "src/mame/machine/fd1094.cpp",
	MAME_DIR .. "src/mame/machine/fd1094.h",
	MAME_DIR .. "src/mame/machine/segaic16.cpp",
	MAME_DIR .. "src/mame/machine/segaic16.h",
	MAME_DIR .. "src/mame/video/sega16sp.cpp",
	MAME_DIR .. "src/mame/video/sega16sp.h",
	MAME_DIR .. "src/mame/machine/mc8123.cpp",
	MAME_DIR .. "src/mame/machine/mc8123.h",
	MAME_DIR .. "src/mame/video/segaic16.cpp",
	MAME_DIR .. "src/mame/video/segaic16.h",
	MAME_DIR .. "src/mame/video/segaic16_road.cpp",
	MAME_DIR .. "src/mame/video/segaic16_road.h",

	MAME_DIR .. "src/mame/drivers/testpat.cpp",
	MAME_DIR .. "src/mame/machine/nl_tp1983.cpp",
	MAME_DIR .. "src/mame/machine/nl_tp1983.h",
	MAME_DIR .. "src/mame/machine/nl_tp1985.cpp",
	MAME_DIR .. "src/mame/machine/nl_tp1985.h",

	-- Skeletons ...
	MAME_DIR .. "src/mame/drivers/a1supply.cpp",
	MAME_DIR .. "src/mame/drivers/aleisttl.cpp",
	MAME_DIR .. "src/mame/drivers/bailey.cpp",
	MAME_DIR .. "src/mame/drivers/chicago.cpp",
	MAME_DIR .. "src/mame/drivers/crazybal.cpp",
	MAME_DIR .. "src/mame/drivers/electra.cpp",
	MAME_DIR .. "src/mame/drivers/exidyttl.cpp",
	MAME_DIR .. "src/mame/drivers/fungames.cpp",
	MAME_DIR .. "src/mame/drivers/meadwttl.cpp",
	MAME_DIR .. "src/mame/drivers/monacogp.cpp",
	MAME_DIR .. "src/mame/drivers/pse.cpp",
	MAME_DIR .. "src/mame/drivers/ramtek.cpp",
	MAME_DIR .. "src/mame/drivers/segattl.cpp",
	MAME_DIR .. "src/mame/drivers/taitottl.cpp",
	MAME_DIR .. "src/mame/drivers/usbilliards.cpp",
}
end

function linkProjects_mame_nl(_target, _subtarget)
	links {
		"mame_netlist",
	}
end
