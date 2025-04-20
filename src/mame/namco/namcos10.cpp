// license:BSD-3-Clause
// copyright-holders:smf,windyfairy
/***************************************************************************

Namco System 10 - Arcade PSX Hardware
=====================================
Driver by smf. Board notes by Guru


----------------------------------------
Guru Readme for Namco System 10 Hardware
----------------------------------------
Note! This document is a Work-In-Progress and will be updated from time to time when more dumps are available.

This document covers all the known Namco System 10 games, including....
*Aim For Cash (AFC2 Ver.A)                                       (C) Namco, 2004
Ball Pom Line  (no sticker, ROM VER. B0 FEB 09 2005 15:29:02)    (C) Namco, 2005
*Dice ROM                                                        (C) Namco, 2004
*Dokidoki! Flower                                                (C) Namco, 2004
***Drum Master                                                   (C) Namco, 2001
***Drum Master 2                                                 (C) Namco, 2001
***Drum Master 3                                                 (C) Namco, 2002
***Drum Master 4                                                 (C) Namco, 2003
***Drum Master 5                                                 (C) Namco, 2003
***Drum Master 6                                                 (C) Namco, 2004
GAHAHA Ippatsu-dou (GID2 Ver.A)                                  (C) Namco/Metro, 2000
GAHAHA Ippatsu-dou 2 (GIS1 Ver.A)                                (C) Namco/Metro, 2001
Gamshara (10021 Ver.A)                                           (C) Mitchell, 2003
Gegege no Kitaro Yokai Yokocho Matsuri de Battle Ja (GYM1 Ver.A) (C) Namco, 2007
Gekitoride-Jong Space (10011 Ver.A)                              (C) Namco/Metro, 2001
Golgo 13 Juusei no Requiem (GLT1 Ver.A)                          (C) Namco/8ing/Raizing, 2001
Gunbalina (GNN1 Ver. A)                                          (C) Namco, 2000
*Hard Puncher Hajime no Ippo: The Fighting                       (C) Namco/Taito, 2001
*Hard Puncher Hajime no Ippo 2: The Fighting Round 2 (VER.2.00J) (C) Namco/Taito, 2002
*Honne Hakkenki                                                  (C) Namco, 2001
Keroro Gunsou Pekopon Shinryaku Shirei De Arimasu! (KRG1 Ver.A)  (C) Namco, 2006
**Knock Down 2001 / KO2001 (KD11 Ver.B)                          (C) Namco, 2001
Kono e Tako (RAN Ver.A, 10021 Ver.A reprogrammed Gamshara PCB)   (C) Mitchell 2003
Kotoba no Puzzle Mojipittan (KPM1 Ver.A)                         (C) Namco, 2001
Medal no Tatsujin ((MTL1 SPR0B)                                  (C) Namco, 2005
Medal no Tatsujin 2 (MTA1001 STMPR0A)                            (C) Namco, 2007
Mr Driller 2 (DR21 Ver.A)                                        (C) Namco, 2000
Mr Driller 2 (DR22 Ver.A)                                        (C) Namco, 2000
Mr Driller 2 (DR23 Ver.A)                                        (C) Namco, 2000
Mr Driller G (DRG1 Ver.A)                                        (C) Namco, 2001
NFL Classic Football (NCF3 Ver.A)                                (C) Namco, 2003
Pacman Ball (PMB2 Ver.A)                                         (C) Namco, 2003
Panikuru Panekuru (PPA1 Ver.A)                                   (C) Namco, 2001
*Peter The Shepherd                                              (C) Namco, 2003
*Photo Battle (PBT1 Ver.B)                                       (C) Namco, 2001
Point Blank 3 (GNN2 Ver. A)                                      (C) Namco, 2000
Puzz Ball (PZB1 Ver. A)                                          (C) Namco, 2002
*Puzz Cube                                                       (C) Namco, 2005
Seishun-Quiz Colorful High School (CHS1 Ver.A)                   (C) Namco, 2002
Sekai Kaseki Hakken (Japan, SKH1 Ver.A)                          (C) Namco, 2004
*Shamisen Brothers (KT-SB2 Ver.A + CDROM)                        (C) Kato/Konami, 2003
*Slot no Oujisama / Slot Prince (SLO1 Ver.A),(SLO1 Ver.B)        (C) Namco, 2002
Star Trigon (STT1 Ver.A)                                         (C) Namco, 2002
Sugorotic Japan (STJ1 Ver.C)                                     (C) Namco, 2002
*Taiko no Tatsujin  (with CDROM?)                                (C) Namco, 2001
Taiko no Tatsujin 2 (TK21 Ver.C & CDROM TK21-A)                  (C) Namco, 2001
Taiko no Tatsujin 3 (TK31 Ver.A & *CDROM)                        (C) Namco, 2002
Taiko no Tatsujin 4 (TK41 Ver.A & CDROM TK-4)                    (C) Namco, 2003
Taiko no Tatsujin 5 (TK51 Ver.A & *CDROM)                        (C) Namco, 2003
Taiko no Tatsujin 6 (TK61 Ver.A & CDROM TK-6)                    (C) Namco, 2004
Tsukkomi Yousei Gips Nice Tsukkomi (NTK1 Ver.A)                  (C) Namco/Metro, 2002
Uchuu Daisakusen Chocovader Contactee (CVC1 Ver.A)               (C) Namco, 2002

* - denotes not dumped yet. If a game code is listed a PCB has been seen.
** - denotes incomplete dump due to damaged ROMs. A redump is required.
*** - World region title, probably doesn't exist if game was only released in Japan.

The Namco System 10 system is basically a PSOne on an arcade board with added protection.
Namco System 10 has 2 or 3 PCBs....
MAIN PCB - This is the mother board PCB. It holds the main CPU/GPU & SPU and all sound circuitry, program RAM & video RAM,
           controller/input logic and video output circuitry. Basically everything except the ROMs.
           There are four known revisions of this PCB so far. The differences seem very minor. The 2nd, 3rd and 4th revisions
           have an updated CPLD revision.
           The 3rd and 4th revisions have some updated model Sony chips. The only other _noticeable_ difference is some component
           shuffling in the sound amplification section to accommodate two extra 1000uF capacitors and one 470uF capacitor
           has been replaced by a 1000uF capacitor. Everything else, including all the PLDs appears to be identical.
           Note there are no ROMs on the Main PCB and also no custom Namco chips on System10, which seem to have been
           phased out. Instead, they have been replaced by (custom programmed) CPLDs, probably due to cost-cutting
           measures within the company, or to reduce the cost of System10 to an entry-level no-frills development platform.
MEM PCB  - There are four known types of this PCB (so far).
           The first type uses SSOP56 Flash and TSOP48 mask ROMs.
           The second type uses TSOP48 NAND ROMs and also has a RAM chip on it.
           The third type uses TSOP48 NAND ROMs and has some extra hardware to decode MP3 data which comes from the ROMs or an external CDROM.
           The fourth type is manufactured by Taito and is completely different but appears to use NAND ROMs. Taito uses their PCB numbering system.
           Each game has a multi-letter code assigned to it which is printed on a small sticker and placed on the top side of the MEM PCB.
           This code is proceeded by a number (only '1' & '2' seen so far), then 'Ver.' then A/B/C which denotes the software revision, and in
           some cases a sub-revision such as 1 or 2 (usually only shown in the test mode).
           The first 1 denotes a Japanese version. Other numbers denote a World version.
           For World versions, only the main program changes, the rest of the (graphics) ROMs use the Japanese version ROMs.
           If the version sticker has a red dot it means nothing as several identical versions of the same games exist with and without the red
           dot. A similar red dot has also been seen on Namco System 246 security carts and means nothing. Speculation about a red dot on a sticker
           is pointless and has no impact on the dumps or the emulation.
           Any System 10 MEM PCB can be swapped to run on any System 10 Main PCB regardless of the main board revision.
           The high scores are stored on the MEM PCB (inside the main program Flash ROM or block 1 of the first NAND chip).
           There are no "alt" versions with the same code, this simply means the game was dumped without first resetting the
           high score records and coinage/play statistics info to factory defaults.
           Also, on all System 10 games, there is a sticker with a serial number on it and the program ROMs also contain
           that same serial number. I'm not sure why, they're not exactly _easily_ traceable and no one cares either way ;-)
EXIO PCB - Optional I/O & Extra Controls PCB

           See the Main PCB, ROM Daughterboard PCB and Expansion PCB below for more details.


Main PCB Layout
---------------

Revision 1
SYSTEM10 MAIN PCB 8906960103 (8906970103)

Revision 2
SYSTEM10 MAIN PCB 8906960104 (8906970104)

Revision 3
SYSTEM10 MAIN(B) PCB 8906962400 (8906972400)

Revision 4
SYSTEM10 MAIN(C) PCB 8906962503 (8906972503)
  |----------------------------------------------------------|
  |   LA4705    VR1                     J201                 |
  |                           |----------------------|       |
  |           NJM3414         |----------------------|       |
|-|     J10                                                  |
|       BA3121                                               |
|             NJM3414            54V25632      54V25632    J1|
|                     CXD1178Q                               |
|J JP4                          |---------|   |-------|      |
|A                              |         |   |       |      |
|M         CXA2067AS            |CXD8561CQ|   |CY37128|      |
|M                 53.693175MHz |         |   |VP160  |      |
|A                              |         |   |       |      |
|                               |---------|   |-------|      |
|                                                            |
|                            101.4912MHz                     |
|-|          MAX734  IS41LV16100                             |
  |                              |---------|                 |
  |   DSW1           IS41LV16100 |         |                 |
  |                              |CXD8606BQ|                 |
  |        GAL16V8D              |         |                 |
  |J5      |-|           *       |         |          PST592 |
  |        | |                   |---------| |--------|      |
  |        | |                               |        |      |
  |        | |           *                   |CXD2938Q|      |
  |J4      | |J202             IS41LV16256   |        |      |
  |        | |                               |        |      |
  |        | |      EPM3064                  |--------|      |
  |        | |                                               |
  |        |-|                 PQ30RV21                      |
  |                                                  J103    |
  |----------------------------------------------------------|
Notes:
------
      CXD8606BQ - SONY CXD8606BQ Central Processing Unit / GTE     (QFP208)
                     - replaced by CXD8606CQ on Revision 3 & 4 Main PCB
      CXD8561CQ - SONY CXD8561CQ Graphics Processor Unit           (QFP208)
       CXD2938Q - SONY CXD2938Q  Sound Processor Unit              (QFP208)
                     - replaced with CXD2941R on Revision 4 Main PCB
       CXD1178Q - SONY CXD1178Q  8-bit RGB 3-channel D/A converter (QFP48)
      CXA2067AS - SONY CXA2067AS TV/Video circuit RGB Pre-Driver   (SDIP30)
   CY37128VP160 - CYPRESS CY37128VP160 Complex Programmable Logic Device (TQFP160, stamped 'S10MA1')
                     - replaced by an updated revision on Revision 2 & 3 Main PCB and stamped 'S10MA1B'
        EPM3064 - Altera MAX EPM3064ATC100-10 Complex Programmable Logic Device (TQFP100, stamped 'S10MA2A')
       GAL16V8D - GAL16V8D PAL (PLCC20, stamped 'S10MA3A')
    IS41LV16100 - ISSI IS41LV16100S-50T 1M x16 EDO DRAM (x2, TSOP50(44) Type II)
    IS41LV16256 - ISSI IS41LV16256-50T 256k x16 EDO DRAM (TSOP44(40) Type II)
       54V25632 - OKI 54V25632 256K x32 SGRAM (x2, QFP100)
       PQ30RV31 - Sharp PQ30RV31 5 Volt to 3.3 Volt Voltage Regulator
         LA4705 - LA4705 15W 2-channel Power Amplifier (SIP18)
         MAX734 - MAX734 +12V 120mA Flash Memory Programming Supply Switching Regulator (SOIC8)
         PST592 - PST592J System Reset IC with 2.7V detection circuit (MMP-4A)
         BA3121 - Rohm BA3121 Dual Channel Ground Isolation Amplifier & Noise Eliminator (SOIC8)
        JRC3414 - New Japan Radio Co. Ltd. JRC3414 Single-Supply Dual High Current Operational Amplifier (x2, SOIC8)
           DSW1 - 8 position DIP switch. #1 is TEST. Most of the others might be unused on most games.
            JP4 - 2 position jumper, set to NC, alt. position labelled SYNC (Note: changing the jumper position
                  switches the game between interlaced and non-interlaced mode)
             J1 - 40 Pin IDC connector for a flat 40-wire cable, used for games that have a CDROM/DVDROM drive
             J4 - 10 pin header for extra controls etc  \ (note: custom Namco 48 pin edge connector is not on System10 PCBs)
             J5 - 4 pin header for stereo sound out     /
            J10 - 4 pin header for audio input from ROM board type 3. This audio is mixed with the other main board audio.
           J201 - 100 pin custom Namco connector for mounting of MEM PCB. This connector is surface-mounted, not a thru-hole type.
           J202 - 80 pin custom Namco connector for mounting of another board. This connector is surface-mounted, not a thru-hole type.
                  There are additional boards that plug in here and provide extra functionality. See below for the details.
           J103 - 6-pin JAMMA2 power plug (Note none of the other JAMMA2 standard connectors are present)
            VR1 - Volume potentiometer
              * - Unpopulated position for IS41LV16100 1M x16 EDO DRAM

Additional Notes:
                1. In test mode (Display Test) the screen can be set to interlace or non-interlace mode. The graphics in
                   interlace mode are visually much smoother with noticeable screen flickering. Non-interlace modes gives
                   a much blockier graphic display (i.e. lower resolution) but without screen flickering.
                2. There is no dedicated highscore/options EEPROM present on the PCB. The game stores the settings on the
                   MEM PCB in the program Flash ROM or block 1 of the first NAND Flash.

ROM Daughterboard PCBs
----------------------
This PCB holds all the ROMs.
There are four known types of ROM daughterboards used on System 10 games (so far).
All of the PCBs are the same size (approx 5" x 5") containing one custom connector surface-mounted to the underside of
the PCB, some mask/Flash/NAND ROMs, a CPLD (the customary Namco 'KEYCUS' chip but doing de-scrambling/decryption too). On the
2nd type a RAM chip is also present. The 3rd type has additional hardware to decode MP3 audio and a ROM-less microcontroller.
The 4th type was manufactured by Taito.

********
*Type 1*
********
System10 MEM(M) PCB 8906961000 (8906970700)
|-------------------------------------|
|                                     |
|                       |-------|     +-
|                       |       |     +-
|                       |CY37128|   J1+-
|                       |VP160  |     +-
|                       |       |     +-
|     7E     7D         |-------|     +-
|                                     |
|     6E     6D                       |
|                                     |
|     5E     5D                 5A    |
|                                     |
|     4E     4D                 4A    |
|                                     |
|     3E     3D                       |
|                                     |
|     2E     2D                 2A    |
|                                     |
|     1E     1D                 1A    |
|                                     |
|-------------------------------------|
Notes:
      CY37128VP160 - CY37128VP160 Cypress Complex Programmable Logic Device (TQFP160)
           1A - 5A - Intel Flash DA28F640J5 64Mbit Flash EEPROM (SSOP56)
           1D - 7E - Samsung Electronics K3N9V1000A-YC 128Mbit mask ROM (TSOP48) (see note 3)
                J1 - 6 pin header for programming the CPLD via JTAG

This PCB is used on:

              Software     MEM PCB
Game          Revision     Sticker      KEYCUS   ROMs Populated                        I/O Board
------------------------------------------------------------------------------------------------
Mr Driller 2  DR21/VER.A3  DR21 Ver.A   KC001A   DR21VERA.1A, DR21MA1.1D, DR21MA2.2D   None
Mr Driller 2  DR22/VER.A3  DR22 Ver.A   KC001A   DR22VERA.1A, DR21MA1.1D, DR21MA2.2D   None
Mr Driller 2  DR23/VER.A3  DR23 Ver.A   KC001A   DR23VERA.1A, DR21MA1.1D, DR21MA2.2D   None

      Note
      1. The ROM PCB has locations for 4x 64Mbit program ROMs, but only 1A is populated.
      2. The ROM PCB has locations for 14x 128Mbit GFX ROMs (Total capacity = 2048Mbits) but only 1D and 2D are populated.
      3. These ROMs are only 18mm long, dumping them requires a special custom adapter

********
*Type 2*
********
System10 MEM(N) PCB 8906961402 (8906971402)
|-------------------------------------|
|                                     |
|                    |---------|      +-
|                    |         |      +-
|                    |CY37256  |    J1+-
|     8E     8D      |VP208    |      +-
|                    |         |      +-
|     7E     7D      |---------|      +-
|                                     |
|     6E     6D                       |
|                                     |
|     5E     5D       CY7C1019        |
|                                     |
|     4E     4D                       |
|                                     |
|     3E     3D                       |
|                                     |
|     2E     2D                       |
|                                     |
|     1E     1D                       |
|                                     |
|-------------------------------------|
Notes:
      CY37256VP208 - Cypress CY37256VP208 Complex Programmable Logic Device (TQFP208)
          CY7C1019 - Cypress CY7C1019BV33-15VC or Samsung Electronics K6R1008V1C-JC15 128k x8 bit 3.3V High Speed CMOS Static RAM (SOJ32)
           1D - 8E - Samsung Electronics K9F2808U0B-YCBO 64Mbit or K9F5608U0D 128Mbit NAND Flash ROM (TSOP48)
                J1 - 6 pin header for programming the CPLD via JTAG

This PCB is used on:

     (some names shortened, check GAME line)        MEM PCB
Game (at end of driver for full title      )        Sticker       KEYCUS   ROMs Populated       CD            Notes
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Ball Pom Line                                       missing       KC039A   8E, 8D               N/A           also has a Namco S10 MGEX10 8681960201 PCB
GAHAHA Ippatsudou                                   GID2  Ver.A   KC005A   8E, 8D, 7E           N/A           also has a Namco System10 EXIO 8906960602 (8906970602) PCB. 2 analog joysticks
GAHAHA Ippatsudou 2                                 GIS1  Ver.A   KC008A   8E, 8D, 7E           N/A           also has a Namco System10 EXIO 8906960602 (8906970602) PCB. 2 analog joysticks
Gamshara                                            10021 Ver.A   KC020A   8E, 8D               N/A           I/O board = none
Gegege no Kitaro Yokai Yokocho Matsuri De Batoru Ja GYM1  Ver.A   KC052A   8E, 8D               N/A           also has a Namco S10 MGEX10 8681960200 PCB
Gekitoride-Jong Space                               10011 Ver.A   KC003A   8E, 8D, 7E, 7D       N/A           I/O board = none
Gunbalina                                           GNN1  Ver.A   KC002A   8E, 8D               N/A           also has a Namco System10 EXIO(G) 8906961602 (8906970602) PCB. TMP95C061 not populated
Keroro Gunsou Pekopon Shinryaku Shirei De Arimasu!  KRG1  Ver.A   KC047A1  8E, 8D               N/A           also has a Namco S10 MGEX10 8681960200 PCB
Knock Down 2001                                     KD11  Ver.B   KC011A   8E, 8D               N/A           also has a Namco P-DRIVE PCB 1908961101 (1908971101) with an H8/3002
Kono e Tako                                         10021 Ver.A   KC034A   8E, 8D               N/A           RAN Ver.A in test mode, all seem to be re-programmed Gamshara PCB
Kotoba no Puzzle Mojipittan                         KPM1  Ver.A   KC012A   8E, 8D, 7E           N/A           also has a Namco System10 EXIO 8906960602 (8906970602) PCB
Medal no Tatsujin                                   MTL1  SPR0B   KC043A   8E, 8D               N/A           also has a Namco System10 EXFINAL PCB 8906962603 (8906962703)
Medal no Tatsujin 2                                 MTA1  STMPR0A KC048A   8E, 8D               N/A           also has a Namco System10 EXFINAL PCB 8906962603 (8906962703)
Mr Driller G                                        DRG1  Ver.A   KC007A   8E, 8D, 7E           N/A           I/O board = none
NFL Classic Football                                NCF3  Ver.A   KC027A   8E, 8D, 7E, 7D       N/A           also has a Namco System10 EXIO PCB
Pacman Ball                                         PMB2  Ver.A   KC026A   8E, 8D               N/A           I/O board = ?
Panikuru Panekuru                                   PPA1  Ver.A   KC017A   8E, 8D, 7E           N/A           I/O board = none
Point Blank 3                                       GNN2  Ver.A   KC002A   8E, 8D               N/A           also has a Namco System10 EXIO(G) 8906961602 (8906970602) PCB. TMP95C061 and RAM not populated
Puzz Ball                                           PZB1  Ver.A   KC013A   8E, 8D               N/A           also has a Namco S10 MGEX10 (8681960201) PCB, unverified title
Sekai Kaseki Hakken                                 SKH1  Ver.A   KC035A   8E, 8D               N/A           also has a Namco S10 MGEX10 (8681960201) PCB, unverified title
Star Trigon                                         STT1  Ver.A   KC019A   8E, 8D               N/A           I/O board = none
Sugorotic Japan                                     STJ1  Ver.C   KC014A   8E, 8D               N/A           also has a Namco S10 MGEX10 (8681960201) PCB
Taiko no Tatsujin 2                                 TK21  Ver.C   KC010A   8E, 8D, 7E           TK21-A        KEYCUS is marked KC007A, KC010A is a sticker on top. I/O board = ?. For all TK* games see note 2 and 3
Taiko no Tatsujin 3                                 TK31  Ver.A   KC016A   8E, 8D, 7E           not dumped    I/O board = ?
Taiko no Tatsujin 4                                 TK41  Ver.A   KC024A   8E, 8D, 7E           TK-4          also has a fully populated Namco System10 EXIO 8906961602 (8906970602) PCB
Taiko no Tatsujin 5                                 TK51  Ver.A   KC031A   8E, 8D, 7E           not dumped    also has a fully populated Namco System10 EXIO 8906961602 (8906970602) PCB
Taiko no Tatsujin 6                                 TK61  Ver.A   KC036A   8E, 8D, 7E           TK-6          also has a fully populated Namco System10 EXIO 8906961602 (8906970602) PCB
Utyuu Daisakusen Chocovader Contactee               CVC1  Ver.A   KC022A   8E, 8D, 7E, 7D, 6E   N/A           I/O board = none

Other games verified to use this PCB but NOT DUMPED (move into list above when dumped)
Shamisen Brothers                                   KT-SB2Ver.A   KC038A   8E                   not dumped    I/O board = none
Photo Battle                                        PBT1  Ver.B   KC006A   1D-8E (16)           N/A           also has a Namco System10 EXIO(G) 8906961602 (8906970602) PCB. TMP95C061 and RAM not populated
Slot no Oujisama/Slot Prince (Ver.A & Ver.B seen)   SLO1  Ver.B   KC023A   8E, 8D               N/A           also has a Namco S10 MGEX10 (8681960201) PCB

      Notes:
      1. The ROM PCB has locations for 16x 128Mbit FlashROMs (Total capacity = 2048Mbits) but usually only a few are populated.
      2. All of the Taiko no Tatsujin games require a CDROM disc. The game will not show anything on screen
         if the CD drive & disc is not present and working. The disc contains binary data.
      3. TK21 and TK31 boards did not have an I/O board. It is unknown if an I/O board is required or if it was missing.

********
*Type 3*
********
System10 MEM(P3) PCB 8906962201 (8906972201)
|-------------------------------------|
|TMP95C061       J101     L   K6R1008 |
|                PST575D              +-
|      ZZZZ   |-------|               +-
|VHCT245      |       |    |-------|  +-
|      LCX245 |CY37256|    |       |J1+-
|  07VZ5M     |VP208  |    |CY37256|  +-
|  07VZ5M     |(2)    |    |VP208  |  +-
|             |-------|    |(1)    |  |
|J3                        |-------|  |
|           HY57V641620    DSW(4)     |
|                        LCX245 LCX245|
|         LC82310        LCX245 LCX245|
|   3414      16.9344MHz              |
|                VHC14    L           |
|                         L           |
|                         L           |
|                         L           |
|     0  2  4  6  8  10  12  14       |
|J2                                   |
|     1  3  5  7  9  11  13  15       |
|-------------------------------------|
Notes:
            TMP95C061 - Toshiba TMP95C061 TLCS-900 Series CMOS 16-bit Microcontroller; No internal ROM or RAM (QFP100)
      CY37256VP208(1) - Cypress CY37256VP208 Complex Programmable Logic Device, marked with code 'KC' and a number.
                        This is the Namco KEYCUS chip which is unique to each game (TQFP208)
      CY37256VP208(2) - Cypress CY37256VP208 Complex Programmable Logic Device, marked 'S10MEP2A' (TQFP208)
              K6R1008 - Samsung Electronics K6R1008V1C-JC15 128k x8-bit 3.3V High Speed CMOS Static Ram (SOJ32)
          HY57V641620 - Hyundai HY57V641620 4 Banks x1M x16-bit Synchronous DRAM (TSOP54 Type II)
                 0-15 - Samsung Electronics K9F2808U0A-YCBO 16Mx8-bit (128M-bit) NAND Flash ROM (TSOP48)
                        Note! These ROMs also hold data for high scores, play time and coin history in NAND block 1.
                        They must be reset to factory defaults before dumping so the dump is clean.
              LC82310 - Sanyo LC82310 MP3 decoder IC (QFP64)
                 3414 - New Japan Radio Co. Ltd. JRC3414 Single-Supply Dual High Current Operational Amplifier (SOIC8)
               07VZ5M - Sharp 07VZ5M Variable Voltage Regulator
              PST575D - Mitsumi PST575D System Reset IC. Available in voltage detection C through L with voltages 4.5V-2.3V
                        This D version triggers a reset at 4.2V (MMP-4A)
                 J101 - Multi-pin connector joining to main board
                   J1 - 6 pin header for programming the CPLDs via JTAG
                   J2 - 4 pin connector joined to main board for MP3 audio output from ROM board
                   J3 - 6 pin connector joined to V278 EMI PCB (filter board on outside of metal box) via 16-pin IDC connector
                        This connector is for extra ANALOG controls. Up to 4 potentiometers can be monitored.
                        For Golgo 13 Part 3 the gun connects to ROM board connector J3 pins 4 and 5 which connect to TMP95C061 pin 20 (P90 AN0)
                        and pin 23 (P93 AN3). Pins 2 and 3 of connector J3 connect to TMP95C061 pin 21 (P91 AN1) and pin 22 (P92 AN2) but are not used.
                        J3 connector pinout is:
                                               1: Ground
                                               2: Analog Y2
                                               3: Analog X2
                                               4: Analog Y1
                                               5: Analog X1
                                               6: 5V
                    L - LED (SMD 0603). These LEDs move sideways in one direction from LED2...LED5 all the time. They connect to
                        CY37256VP208(2)
                    Z - LED (SMD 0603). These LEDs flash on/off together slowly when music is not playing and move sideways
                        fast left/right (i.e. Knight Rider/KITT style) when music is playing. They are connected to the TMP95C061
                        pins 40 (PB4), 41 (PB5), 42 (PB6), 43 (PB7)

      Notes:
      1. The ROM PCB has locations for 16x Flash ROMs (Total capacity = 2048Mbits) but usually only a few are populated.
      2. This board outputs audio that is mixed into the normal audio via main board connector J10 and IC BA3121.

This PCB is used on:

                                   MEM PCB
Game                               Sticker      KEYCUS   ROMs Populated           Notes
-------------------------------------------------------------------------------------------------------
Golgo 13 Juusei no Requiem         GLT1 Ver.A   KC009A   0, 1, 2, 3, 4, 5         Cabinet-mounted X,Y gun. I/O board = none. Test mode shows ANALOG X & ANALOG Y and is using 2x 5k-ohm pots for the gun on-screen positioning.
Seishun Quiz Colorful High School  CHS1 Ver.A   KC025A   0 to 13 (14 total)       I/O board = none
Tsukkomi Yousei Gips Nice Tsukkomi NTK1 Ver.A   KC018A   0, 1, 2, 3, 4, 5, 6, 7   Game uses several JAMMA PL1 & PL2 buttons for controls. Main board came with EXIO(G) I/O board but the game works without it
                                                                                  so possible the main board was for a light gun game and an operator swapped the ROM board for this game. Meaning this game doesn't
                                                                                  use an I/O board which makes sense as there are no special controls.
Type 4
TAITO CORPORATION
SYSTEM10 MEM/IO PCB
|-------------------------------------|
|                                     |
|                    |---------|      |
|                    |         |      |
|                    |CY37256  |      |
|    IC6     IC10    |VP208    |      |
|                    |         |    J1|
|    IC5     IC9     |---------|      |
|                                     |
|    IC4     IC8                      |
|                                     |
|                     CY7C1019        |
|                                     |
|                                     |
|                                     |
|                                     |
|                                  CN2|
|                                     |
|                                     |
|                      PAL20V8        |
|                                     |
|-------------------------------------|
Notes:
      CY37256VP208 - Cypress CY37256VP208 Complex Programmable Logic Device (TQFP208)
          CY7C1019 - Cypress CY7C1019BV33-15VC or Samsung Electronics K6R1008V1C-JC15 128k x8 bit 3.3V High Speed CMOS Static RAM (SOJ32)
          IC4-IC10 - Samsung Electronics K9F2808U0B-YCBO 64Mbit or Toshiba NAND Flash ROM (TSOP48)
           PAL20V8 - marked F06-02
                J1 - 10-pin header for programming the CPLD via JTAG
               CN2 - 10-pin connector for controls

This PCB is used on..
Hajime no Ippo Hard Puncher   - CPLD marked F06-01. I/O board = none
Hajime no Ippo Hard Puncher 2 - sticker HAZIMENO IPPO2 K91J0893A, HAZIME'NO IPPO2 VER.2.00J. CPLD marked F06-03. I/O board = none


Expansion Daughterboards
------------------------

Type 1:
This PCB provides input/output capabilities for JVS hook-ups and allows extra controls to be connected. There is actually only one PCB design
but there are several variations where some of the connectors and ICs are not populated if the game does not need that capability. In that case
an extra sticker is applied. However the PCB contains all the locations/pads/holes etc to mount those parts. In most cases seen so far this PCB
is optional but on some games (e.g. Point Blank 3, Photo Battle, Aim For Cash etc) it is required for proper game play.

System10 EXIO PCB 8906960602 (8906970602)
System10 EXIO(G) PCB 8906960602 (8906970602) sticker: 8906961602 (for lightgun games)
      |--------------------------------------------------|
      |   J4         J5                  J3       J2     |
      |                                 ADM485  MC14052  |-|
|-----|LT1181A                                             |
|                                             VHCT245      |
|                  CY37128VP100               VHCT245      |
|J6                                  VHCT245  VHCT245    J1|
|                                    VHCT245               |
|                                    VHCT245               |
|   VHC14      61C256                                      |
|     TMP95C061                                            |
|    22.1184MHz                   VHCT244                |-|
|            LLLL        J7                   VHCT574  J8|
|--------------------------------------------------------|
Notes:
         TMP95C061 - Toshiba TMP95C061 TLCS-900 Series CMOS 16-bit Microcontroller; No internal ROM or RAM (QFP100)
                        - not populated on EXIO(G) PCB.
      CY37128VP160 - CY37128VP100 Cypress Complex Programmable Logic Device, marked 'S10XIO1A' or 'S10XIO1B' or 'S10XIO1C' (TQFP100)
              VHC* - Common 3.3v logic chips
            ADM485 - Analog Devices ADM485 Low Power EIA RS485 transceiver (SOIC8)
            61C256 - ISSI IS61C256AH-15J 32k x8-bit SRAM (SOJ28)
                        - not populated on EXIO(G) PCB.
           LT1181A - Linear Technology LT1181A or Analog Devices ADM202EARW Low Power 5V RS232 Dual Driver/Receiver (SOIC16W)
           MC14052 - OnSemi MC14052 Analog DP4T Multiplexers/Demultiplexer (SOIC16)
                 L - LED (SMD 0603)
                J1 - 48-Way Card Edge Connector
                J2 - USB Connector for JVS External I/O board
                J3 - Dual RCA Jacks marked 'AUDIO', for audio output
             J4/J5 - HD15F DSUB Connector marked 'CRT1/CRT2', for video output
                J6 - DB9 DSUB Connector marked 'RS232C'. Possibly for networking several PCBs together
                J7 - 6 pin header for programming the CPLDs via JTAG
                J8 - 2 pin header for connection of gun. Pin 1:Player 1 Gun Opto. Pin2:Player 2 Gun Opto

This PCB is required by Point Blank 3 since it controls the lightgun opto signal. Only the CPLD and J8 and some minor
logic and other small support parts are populated.
This PCB has been found almost fully populated (minus J6) on some Taiko no Tatsujin games (TK41/TK51/TK61) and on Knock Down 2001, but not
earlier TK games. It appears to be optional or is only used by the later TK41, TK51 and TK61 games or the earlier TK Main PCB was missing that board
which is highly likely given that most of these boards were sold and bought as 'junk'.


Type 2:
System10 EXFINAL PCB 8906962703 (8906972703)
      |----------------------------------------------------|
      |          LLLLLLLL     J3                           |
      |                                                    |
|-----|             40MHz                                  |
|J4                       EPM3128                          |
|                        (S10XFN1A)                        |
|J5     JP1                                                |
|                                            M62023        |
|     COM20022I                           R4543            |
|J6               CY62256                                  |
|       ADM485                                             |
|J8  J7     TMC20070A                                      |
| JP2  JP3                                    BATT         |
|----------------------------------------------------------|
Notes:
      CY62256 - 32kBx8-bit SRAM
      EPM3128 - Altera MAX EPM3128 CPLD labelled S10XFN1A
        R4543 - R4543 RTC
         BATT - 3V coin battery
            L - LED (SMD 0603)
    TMC20070A - TMC 'EC-NET' TMC20070A Network Controller
    COM20022I - SMSC COM20022I 10Mbps ARCNET Controller
           J3 - 6 pin connector for JTAG
           J4 - 3.5mm Audio Jack
           J5 - HD15 DSUB connector (video out?)
           J6 - Optical Network Jacks
        J7/J8 - 3 pin connector
          JP1 - Jumper marked 30m/10m
      JP2/JP3 - Jumper marked Non/Term

This PCB is essentially a network board.
This PCB was found on Medal no Tatsujin and Medal no Tatsujin 2.
Most likely used on all satellite games.


Type 3:
System10 EXUSB PCB 8906962601 (8906972601)
      |----------------------------------------------------|
      | J7                 J6                LLLL     J5   |
      |                                                    |
|-----|   TR2 TR4 TR6 TR8 TR10 TR12 TR14 TR16  USBN9604-28M|
|         TR1 TR3 TR5 TR7 TR9  TR11 TR13 TR15            L |
|                                                          |
|                                              EPM3128     |
|   TD62083                                   (S10EXU1A)   |
|                    TLP283-4                         48MHz|
| J1                        TLP283-4                       |
|                                                          |
|   TD62083                                              J4|
|                          J2        J3         SW1        |
|----------------------------------------------------------|
Notes:
       J1 - Multi-pin connector
       J2 - Multi-pin connector
       J3 - 6-pin JTAG connector
       J4 - HD15 DSUB connector
       J5 - USB type B connector
       J6 - Multi-pin connector
       J7 - 6 pin Power Input connector
      TR* - D1718 Transistor
        L - LED (SMD 0603)
 USBN9604 - National/TI USBN9604-28M USB controller
  EPM3128 - Altera MAX EPM3128 CPLD labelled S10EXU1A
 TLP283-4 - Toshiba TLP283-4 Optocoupler
  TD62083 - Toshiba 8-channel Darlington Sink Driver
      SW1 - 4-position DIP switch

Usage of this PCB is unknown but likely it is used on any game that drives motors such as coin pushers etc.


Type 4:
S10 MGEXIO PCB 8681960201 (8681970201)
      |----------------------------------------------------|
      | TR11  TR13  TR15  TR17  TR19             J8        |
      |   TR12  TR14  TR16  TR18  TR20                     |
|-----|                                                    |
|                             L    EPM3128                 |
|                                     14.??MHz             |
|J3           J4            J5                      M48Z35Y|
|                                   H8/3007                |
|                                                          |
|                                                          |
|                                                   MB3771 |
|       TR1   TR3   TR5   TR7   TR9      L                 |
|  J2     TR2   TR4   TR6   TR8   TR10   L  SW1         J6 |
|----------------------------------------------------------|
Notes:
       J2 - 4-pin connector
   J3/4/5 - Multi-pin connector
       J6 - 4-pin connector
       J8 - 6-pin JTAG connector
      TR* - D1718 Transistor
        L - LED (SMD 0603)
  EPM3128 - Altera MAX EPM3128 CPLD labelled PZB1DR0
      SW1 - 2-position DIP switch
  M48Z35Y - ST Microelectronics M48Z35Y 32kBx8-bit NVRAM
  H8/3007 - Hitachi H8/3007 Microcontroller (2kB internal RAM, no internal ROM)
   MB3771 - Fujitsu MB3771 System Reset IC

Check the MEM(N) PCB above for a list of games that use this PCB.

-----------

Note about the bit scramble order:
For MEM(N) games you can use 0x8508-0x8528 and for MEM(M) games you can use 0x108-0x128 to brute force the scramble order.
Those bytes correspond to the "Sony Computer Entertainment Inc." string in the BIOS.
Note: gjspace swaps between the entire 16-bit space but every other game I've tested swaps in an 8-bit space,
so you can try every permutation of 0-7 bit swaps separately very quickly (<1 sec) first but if that fails you will need to
try all permutations of 0-15 bit swaps which will take much longer or manually work through it.


Dumping notes (DUMP VERIFIERS PLEASE READ):
The following does not apply to MEM(M) boards.

If you are marking a dump as bad because of bad blocks in the dump then please understand how to read the invalid block table and check
if the block wasn't already intentionally avoided, or if the block isn't already redirected in the invalid block table.
Do not mark dumps bad just because they have bad blocks. Bad blocks are expected and were taken into consideration when programming the chips.

These chips are programmed with data that's tailored to the individual chips on the board to avoid bad blocks. If you are redumping a game then
please redump all of the chips and not just any that are marked BAD_DUMP in MAME or else you may run into an issue where the data is not in the
expected block order.

As per the datasheet, the NAND chips used have only a guarantee of 1004 usable blocks out of the 1024, with the 1st block also guaranteed to be good.
The 1st block of the first ROM (8e) will contain a table at the top of that remaps blocks to other blocks and is read in by the BIOS and game.
All games seem to assume that blocks 0 (invalid block table), 1 (EEPROM data section) and 2-9 (BIOS data) are good.

The invalid block table is a table of entries with two 16-bit big endian fields for the bad block index and the new redirected block index.
The invalid block table uses the upper 4 bits of the bad block index value as target the device ID. So if you see a value of 2010 03fe then
it means nand2's block 0x10 needs to be redirected to block 0x3fe. Namco seems to have reserved the blocks after 0x3ec as recovery blocks,
so redirected bad blocks will go into the > 0x3ec region at the end of the NAND. The recovery blocks are used in reverse order starting
from 0x3ff and goes down for every recovery block used. 0001 03ff -> 0002 03fe -> 0003 03fd, etc. The invalid block table ends when ffff ffff is found.
New redirected block indexes do not *have* to start at 0x3ff because Namco intentionally avoided using bad blocks at the very end of the NAND chips.
For example, if the first entry says 0001 03fe instead of the normal 0001 03fff then they intentionally avoided using the last block on the NAND most
likely because it was bad.

You can see the block lookup table in any game by looking at 0x1f500000 in memory. Each NAND device has 0x3ec entries, so you can look up a specific
block for a specific device using: 0x1f500000 + 2 * (0x3ec * device_id + block_idx)

Known issues:
- mrdrilr2, mrdrilr2j: Opening the operator menu sometimes can crash Mr. Driller 2
- nflclsfb: Needs additional I/O for trackball
- g13jnr: Needs MP3 decoder emulation
- sugorotc, sekaikh(?): BGMs stop early and/or crash the game. Seems to be expecting an SPU-related IRQ?
- nicetsuk: Hangs on boot due to suspected issues with the PSX's timers
- knpuzzle: Audio/sound effects can be glitchy, sound glitches on the difficulty select screen for a moment
- panikuru: Audio/sound effects can be glitchy
- Fix medal games I/O and refactor code to separate MGEXIO states from namcos10_state


User data note:
- the games store settings / rankings / bookkeeping data in the first NAND ROM - at 0x4200 for MEM(N) and 0x40000 for MEM(M), the ROMs used should be defaulted where possible
*/

#include "emu.h"

#include "namcos10_exio.h"
#include "ns10crypt.h"

#include "bus/ata/ataintf.h"
#include "bus/ata/cr589.h"
#include "cpu/psx/psx.h"
#include "cpu/tlcs900/tmp95c061.h"
#include "machine/intelfsh.h"
#include "machine/nandflash.h"
#include "machine/ram.h"
#include "machine/ticket.h"
#include "machine/timer.h"
#include "sound/spu.h"
#include "video/psx.h"

#include "screen.h"
#include "speaker.h"


namespace {

class namcos10_state : public driver_device
{
public:
	namcos10_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_decrypter(*this, "decrypter")
		, m_ata(*this, "ata")
		, m_io_update_interrupt(*this)
		, m_io_system(*this, "SYSTEM")
		, m_exio(*this, "exio")
		, m_exio_analog(*this, "EXIO_ANALOG%u", 1U)
		, m_mgexio_hopper(*this, "mgexio_hopper%u", 1U)
		, m_mgexio_outputs(*this, "MGEXIO_OUTPUT%u", 0U)
		, m_mgexio_sensor(*this, "MGEXIO_SENSOR")
	{ }

	INPUT_CHANGED_MEMBER(mgexio_coin_start);

protected:
	using unscramble_func = uint16_t (*)(uint16_t);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void device_resolve_objects() override;

	void namcos10_base(machine_config &config);
	void namcos10_exio(machine_config &config);
	void namcos10_mgexio(machine_config &config);
	void namcos10_exfinalio(machine_config &config);

	void namcos10_map_inner(address_map &map);
	void namcos10_map(address_map &map);

	void namcos10_map_exio_inner(address_map &map);
	void namcos10_map_exio(address_map &map);

	void namcos10_map_mgexio_inner(address_map &map);
	void namcos10_map_mgexio(address_map &map);

	required_device<psxcpu_device> m_maincpu;
	optional_device<ns10_decrypter_device> m_decrypter;

	unscramble_func m_unscrambler;
	std::function<void()> m_psx_remapper;

	required_device<ata_interface_device> m_ata;

	bool m_is_cdrom_dma;

private:
	enum : int8_t {
		I2CP_IDLE,
		I2CP_RECIEVE_BYTE,
		I2CP_RECIEVE_ACK_1,
		I2CP_RECIEVE_ACK_0
	};

	uint16_t cdrom_unk_r();
	void int_ack_w(offs_t offset, uint16_t data);

	uint16_t io_system_r();

	uint16_t int_r(offs_t offset);

	uint16_t exio_ident_r();
	void exio_ident_w(uint16_t data);

	uint16_t i2c_clock_r();
	void i2c_clock_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint16_t i2c_data_r();
	void i2c_data_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void i2c_update();

	TIMER_DEVICE_CALLBACK_MEMBER(io_update_interrupt_callback);
	DECLARE_WRITE_LINE_MEMBER(cdrom_interrupt);
	DECLARE_WRITE_LINE_MEMBER(cdrom_dmarq);

	devcb_write_line m_io_update_interrupt;

	uint16_t m_i2c_host_clock, m_i2c_host_data, m_i2c_dev_clock, m_i2c_dev_data, m_i2c_prev_clock, m_i2c_prev_data;
	int8_t m_i2cp_mode;
	uint8_t m_i2c_byte;
	int32_t m_i2c_bit;

	int32_t m_exio_ident_bit;
	uint32_t m_exio_ident_byte;

	required_ioport m_io_system;

	optional_device<namcos10_exio_base_device> m_exio;

	util::notifier_subscription m_notif_psx_space;
	bool m_remapping_psx_io;

	uint16_t m_int;

	// EXIO
	optional_ioport_array<8> m_exio_analog;

	// MGEXIO
	template <int N> void mgexio_output_w(offs_t offset, uint16_t data);

	optional_device_array<hopper_device, 4> m_mgexio_hopper;
	output_finder<16> m_mgexio_outputs;
	optional_ioport m_mgexio_sensor;

	attotime m_mgexio_coin_start_time[2];
};

class namcos10_memm_state : public namcos10_state
{
public:
	namcos10_memm_state(const machine_config &mconfig, device_type type, const char *tag)
		: namcos10_state(mconfig, type, tag)
		, m_nand(*this, "nand")
	{ }

	void ns10_mrdrilr2(machine_config &config);

	void init_mrdrilr2();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	void namcos10_memm(machine_config &config);

	void namcos10_memm_map_inner(address_map &map);
	void namcos10_memm_map(address_map &map);

	void memm_driver_init();

	void crypto_switch_w(uint16_t data);
	uint16_t range_r(offs_t offset);
	void nand_w(offs_t offset, uint16_t data);
	uint16_t bank_r(offs_t offset);
	void bank_w(offs_t offset, uint16_t data);

	void decrypt_bios_region(int start, int end);

	required_device<intel_28f640j5_device> m_nand;

	uint32_t m_bank_idx;
	uint32_t m_bank_access_nand_direct;
};

class namcos10_memn_state : public namcos10_state
{
public:
	namcos10_memn_state(const machine_config &mconfig, device_type type, const char *tag)
		: namcos10_state(mconfig, type, tag)
		, m_nand(*this, "nand%u", 0U)
	{ }

	void ns10_ballpom(machine_config &config);
	void ns10_chocovdr(machine_config &config);
	void ns10_gahaha(machine_config &config);
	void ns10_gahaha2(machine_config &config);
	void ns10_gamshara(machine_config &config);
	void ns10_gegemdb(machine_config &config);
	void ns10_gjspace(machine_config &config);
	void ns10_kd2001(machine_config &config);
	void ns10_keroro(machine_config &config);
	void ns10_knpuzzle(machine_config &config);
	void ns10_konotako(machine_config &config);
	void ns10_medalnt(machine_config &config);
	void ns10_medalnt2(machine_config &config);
	void ns10_mrdrilrg(machine_config &config);
	void ns10_nflclsfb(machine_config &config);
	void ns10_pacmball(machine_config &config);
	void ns10_panikuru(machine_config &config);
	void ns10_ptblank3(machine_config &config);
	void ns10_puzzball(machine_config &config);
	void ns10_sekaikh(machine_config &config);
	void ns10_startrgn(machine_config &config);
	void ns10_sugorotic(machine_config &config);
	void ns10_taiko2(machine_config &config);
	void ns10_taiko3(machine_config &config);
	void ns10_taiko4(machine_config &config);
	void ns10_taiko5(machine_config &config);
	void ns10_taiko6(machine_config &config);

	void init_ballpom();
	void init_chocovdr();
	void init_gahaha();
	void init_gahaha2();
	void init_gamshara();
	void init_gjspace();
	void init_gunbalina();
	void init_keroro();
	void init_knpuzzle();
	void init_konotako();
	void init_medalnt();
	void init_medalnt2();
	void init_mrdrilrg();
	void init_nflclsfb();
	void init_pacmball();
	void init_panikuru();
	void init_puzzball();
	void init_sekaikh();
	void init_startrgn();
	void init_sugorotic();
	void init_taiko2();
	void init_taiko3();
	void init_taiko4();
	void init_taiko5();
	void init_taiko6();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void namcos10_memn_base(machine_config &config);
	void namcos10_memn(machine_config &config);

	void namcos10_memn_map(address_map &map);

	void namcos10_nand_k9f2808u0b(machine_config &config, int nand_count);
	void namcos10_nand_k9f5608u0d(machine_config &config, int nand_count);

	void memn_driver_init();

	void pio_dma_read(uint32_t *p_n_psxram, uint32_t n_address, int32_t n_size);
	void pio_dma_write(uint32_t *p_n_psxram, uint32_t n_address, int32_t n_size);

private:
	void namcos10_memn_map_inner(address_map &map);

	void nand_copy(uint8_t *nand_base, uint16_t *dst, uint32_t address, int len);

	void crypto_switch_w(uint16_t data);
	uint16_t ctrl_reg_r(offs_t offset);
	void ctrl_reg_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	uint16_t nand_rnb_r();
	void nand_cmd_w(uint8_t data);
	void nand_address_column_w(uint8_t data);
	void nand_address_row_w(uint8_t data);
	void nand_address_page_w(uint8_t data);
	uint16_t nand_data_r();
	void nand_data_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void nand_bank_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	optional_device_array<nand_device, 16> m_nand;

	uint32_t m_ctrl_reg;
	uint32_t m_nand_device_idx;
	uint8_t m_nand_rnb_state[16];
	uint32_t m_nand_address;
};

class namcos10_memp3_state : public namcos10_memn_state
{
public:
	namcos10_memp3_state(const machine_config &mconfig, device_type type, const char *tag)
		: namcos10_memn_state(mconfig, type, tag)
		, m_ram(*this, "maincpu:ram")
		, m_memp3_mcu(*this, "memp3_mcu")
		, m_mcu_ram(*this, "mcu_ram")
		, m_p3_analog(*this, "P3_ANALOG%u", 1U)
	{ }

	void ns10_g13jnr(machine_config &config);
	void ns10_nicetsuk(machine_config &config);
	void ns10_squizchs(machine_config &config);

	void init_g13jnr();
	void init_nicetsuk();
	void init_squizchs();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	void namcos10_memp3_base(machine_config &config);

	void namcos10_memp3_map_inner(address_map &map);
	void namcos10_memp3_map(address_map &map);
	void mcu_map(address_map &map);

	void firmware_write_w(uint16_t data);

	void ram_bank_w(uint16_t data);

	uint16_t unk_status1_r();

	uint16_t unk_status2_r();
	void mcu_int5_w(uint16_t data);

	uint16_t ram_r(offs_t offset);
	void ram_w(offs_t offset, uint16_t data);

	uint16_t io_analog_r(offs_t offset);

	required_device<ram_device> m_ram;
	required_device<tmp95c061_device> m_memp3_mcu;
	required_shared_ptr<uint16_t> m_mcu_ram;
	optional_ioport_array<4> m_p3_analog;

	uint16_t m_mcu_ram_bank;
};

///////////////////////////////////////////////////////////////////////////////////////////////

void namcos10_state::machine_start()
{
	if (m_psx_remapper) {
		m_notif_psx_space = m_maincpu->space(AS_PROGRAM).add_change_notifier(
			[this] (read_or_write mode)
			{
				if (!m_remapping_psx_io)
				{
					m_remapping_psx_io = true;
					m_psx_remapper();
					m_remapping_psx_io = false;
				}
			}
		);
	}

	save_item(NAME(m_i2c_dev_clock));
	save_item(NAME(m_i2c_dev_data));
	save_item(NAME(m_i2c_host_clock));
	save_item(NAME(m_i2c_host_data));
	save_item(NAME(m_i2c_prev_clock));
	save_item(NAME(m_i2c_prev_data));
	save_item(NAME(m_i2cp_mode));
	save_item(NAME(m_i2c_byte));
	save_item(NAME(m_i2c_bit));

	save_item(NAME(m_int));
	save_item(NAME(m_is_cdrom_dma));

	save_item(NAME(m_exio_ident_bit));
	save_item(NAME(m_exio_ident_byte));

	save_item(NAME(m_mgexio_coin_start_time));
}

void namcos10_state::machine_reset()
{
	m_remapping_psx_io = false;

	m_i2c_dev_clock = m_i2c_dev_data = 1;
	m_i2c_host_clock = m_i2c_host_data = 1;
	m_i2c_prev_clock = m_i2c_prev_data = 1;
	m_i2cp_mode = I2CP_IDLE;
	m_i2c_byte = 0x00;
	m_i2c_bit = 0;

	m_int = 0;
	m_is_cdrom_dma = false;

	m_exio_ident_bit = 0;
	m_exio_ident_byte = 0;

	std::fill(std::begin(m_mgexio_outputs), std::end(m_mgexio_outputs), 0);
	std::fill(std::begin(m_mgexio_coin_start_time), std::end(m_mgexio_coin_start_time), attotime::never);
}

void namcos10_state::device_resolve_objects()
{
	m_io_update_interrupt.resolve_safe();
	m_mgexio_outputs.resolve();
}

TIMER_DEVICE_CALLBACK_MEMBER(namcos10_state::io_update_interrupt_callback)
{
	m_int |= 8; // I/O interrupt
	m_io_update_interrupt(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void namcos10_state::namcos10_base(machine_config &config)
{
	/* basic machine hardware */
	CXD8606BQ(config, m_maincpu, XTAL(101'491'200));
	m_maincpu->set_disable_rom_berr(true);
	m_maincpu->subdevice<ram_device>("ram")->set_default_size("4M");

	/* video hardware */
	CXD8561CQ(config, "gpu", XTAL(53'693'175), 0x200000, subdevice<psxcpu_device>("maincpu")).set_screen("screen"); // 2 54V25632s

	SCREEN(config, "screen", SCREEN_TYPE_RASTER);

	/* sound hardware */
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	// CXD2938Q; SPU with CD-ROM controller - also seen in PSone, 101.4912MHz / 2
	// TODO: This must be replaced with a proper CXD2938Q device, CD-ROM functionality of chip not used
	spu_device &spu(SPU(config, "spu", XTAL(101'491'200)/2, m_maincpu.target()));
	spu.set_stream_flags(STREAM_SYNCHRONOUS);
	spu.add_route(0, "lspeaker", 0.75);
	spu.add_route(1, "rspeaker", 0.75);

	// TODO: Trace main PCB to see where JAMMA I/O goes and/or how int10 can be triggered (SM10MA3?)
	m_io_update_interrupt.bind().set("maincpu:irq", FUNC(psxirq_device::intin10));
	TIMER(config, "io_timer").configure_periodic(FUNC(namcos10_state::io_update_interrupt_callback), attotime::from_hz(100));

	// Taiko wants any drive with a name matching "Toshiba", "TEAC", "Matsushita", "LG", or "No Brand"(?)
	// The default CD drive in MAME identifies as "MAME Virtual CDROM" so just give it a Matsushita drive
	ATA_INTERFACE(config, m_ata).options([] (device_slot_interface &device) { device.option_add("cdrom", CR589); }, "cdrom", nullptr, true);
	m_ata->slot(0).set_fixed(true);
	m_ata->irq_handler().set(FUNC(namcos10_state::cdrom_interrupt));
	m_ata->dmarq_handler().set(FUNC(namcos10_state::cdrom_dmarq));
}

WRITE_LINE_MEMBER(namcos10_state::cdrom_interrupt)
{
	if (state)
		m_int |= 4;
	else
		m_int &= ~4;

	m_io_update_interrupt(state);
}

WRITE_LINE_MEMBER(namcos10_state::cdrom_dmarq)
{
	if (!state)
		return;

	// Start of CD-ROM DMA, will continue in pio_dma_read after dmarq is received
	m_is_cdrom_dma = true;
}

void namcos10_state::namcos10_map_inner(address_map &map)
{
	// ram?
	// Contains the NAND block table with redirected blocks generated
	// based on the invalid block table found at the beginning of a game's first NAND
	map(0xf500000, 0xf5fffff).ram().share("share3");

	map(0xfb20000, 0xfb20003).nopw(); // written to when DMA are finished? maybe some kind of IRQ?
	map(0xfb60000, 0xfb60003).noprw(); // ?
	map(0xfba0000, 0xfba0001).r(FUNC(namcos10_state::io_system_r));
	map(0xfba0002, 0xfba0003).rw(FUNC(namcos10_state::exio_ident_r), FUNC(namcos10_state::exio_ident_w));
	map(0xfba0004, 0xfba0007).portr("IN1");
	map(0xfba0008, 0xfba0009).rw(FUNC(namcos10_state::i2c_clock_r), FUNC(namcos10_state::i2c_clock_w));
	map(0xfba000a, 0xfba000b).rw(FUNC(namcos10_state::i2c_data_r), FUNC(namcos10_state::i2c_data_w));
	map(0xfba0012, 0xfba0019).w(FUNC(namcos10_state::int_ack_w));
	map(0xfba001a, 0xfba001b).r(FUNC(namcos10_state::int_r));
	map(0xfbc0000, 0xfbc000f).rw(m_ata, FUNC(ata_interface_device::cs0_r), FUNC(ata_interface_device::cs0_w));
	map(0xfbf0000, 0xfbf000f).r(FUNC(namcos10_state::cdrom_unk_r));
}

void namcos10_state::namcos10_map(address_map &map)
{
	map(0x10000000, 0x1fffffff).m(FUNC(namcos10_state::namcos10_map_inner));
	map(0x90000000, 0x9fffffff).m(FUNC(namcos10_state::namcos10_map_inner));
	map(0xb0000000, 0xbfffffff).m(FUNC(namcos10_state::namcos10_map_inner));
}

uint16_t namcos10_state::cdrom_unk_r()
{
	// Used to check if CD drive is connected/available?
	return 1;
}

void namcos10_state::int_ack_w(offs_t offset, uint16_t data)
{
	m_int &= ~(1 << offset);
}

uint16_t namcos10_state::io_system_r()
{
	return m_io_system->read();
}

uint16_t namcos10_state::int_r(offs_t offset)
{
	// bit = cleared registers
	// 0 = 1fba0012
	// bit 1 is EXIO board-related? Medal no Tatsujin (EXFINAL) uses this to update the EXIO states (ARCNET among other things)
	// bit 2 is for CD-ROM
	// bit 3 is for I/O (must be set to update I/O)
	return m_int;
}

void namcos10_state::exio_ident_w(uint16_t data)
{
	logerror("%s: exio_ident_w %04x\n", machine().describe_context(), data);
	m_exio_ident_bit = 7;
	m_exio_ident_byte = 0;
}

uint16_t namcos10_state::exio_ident_r()
{
	// 0x23 = DEV PCB (mentioned in ballpom)
	// 0x24 = DEV PCB (mentioned in ballpom)
	// 0x30 = EXIO PCB, has CPU and I/O
	// 0x31 = EXIO PCB, has CPU and I/O (different pinout from 0x30, but some games like nflclsfb support both layouts)
	// 0x32 = EXIO PCB, has no CPU but has I/O
	// 0x33 = MGEXIO PCB
	// 0x34 = EXUSB?
	// 0x35 = EXFINAL PCB
	// 0xff = disabled
	const uint8_t resp = m_exio ? m_exio->ident_code() : 0xff;

	uint16_t res = m_exio_ident_byte >= 0x20 ? 0x3 :
		(((resp >> m_exio_ident_bit) & 1) ? 1 : 0) |
		(((resp >> m_exio_ident_bit) & 1) ? 2 : 0);

	m_exio_ident_bit--;
	if(m_exio_ident_bit == -1) {
		m_exio_ident_bit = 7;
		m_exio_ident_byte++;
	}
	return res;
}

uint16_t namcos10_state::i2c_clock_r()
{
	uint16_t res = m_i2c_dev_clock & m_i2c_host_clock & 1;
	// logerror("i2c_clock_r %d (%x)\n", res, m_maincpu->pc());
	return res;
}

void namcos10_state::i2c_clock_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(&m_i2c_host_clock);
	// logerror("i2c_clock_w %d (%x)\n", data, m_maincpu->pc());
	i2c_update();
}

uint16_t namcos10_state::i2c_data_r()
{
	uint16_t res = m_i2c_dev_data & m_i2c_host_data & 1;
	// logerror("i2c_data_r %d (%x)\n", res, m_maincpu->pc());
	return res;
}

void namcos10_state::i2c_data_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(&m_i2c_host_data);
	// logerror("i2c_data_w %d (%x)\n", data, m_maincpu->pc());
	i2c_update();
}

void namcos10_state::i2c_update()
{
	uint16_t clock = m_i2c_dev_clock & m_i2c_host_clock & 1;
	uint16_t data = m_i2c_dev_data & m_i2c_host_data & 1;

	if(m_i2c_prev_data == data && m_i2c_prev_clock == clock)
		return;

	switch(m_i2cp_mode) {
	case I2CP_IDLE:
		if(clock && !data) {
			logerror("i2c: start bit\n");
			m_i2c_byte = 0;
			m_i2c_bit = 7;
			m_i2cp_mode = I2CP_RECIEVE_BYTE;
		}
		break;
	case I2CP_RECIEVE_BYTE:
		if(clock && data && !m_i2c_prev_data) {
			logerror("i2c stop bit\n");
			m_i2cp_mode = I2CP_IDLE;
		} else if(clock && !m_i2c_prev_clock) {
			m_i2c_byte |= (data << m_i2c_bit);
			// logerror("i2c_byte = %02x (%d)\n", m_i2c_byte, m_i2c_bit);
			m_i2c_bit--;
			if(m_i2c_bit < 0) {
				m_i2cp_mode = I2CP_RECIEVE_ACK_1;
				logerror("i2c received byte %02x\n", m_i2c_byte);
				m_i2c_dev_data = 0;
				data = 0;
			}
		}
		break;
	case I2CP_RECIEVE_ACK_1:
		if(clock && !m_i2c_prev_clock) {
			// logerror("i2c ack on\n");
			m_i2cp_mode = I2CP_RECIEVE_ACK_0;
		}
		break;
	case I2CP_RECIEVE_ACK_0:
		if(!clock && m_i2c_prev_clock) {
			// logerror("i2c ack off\n");
			m_i2c_dev_data = 1;
			data = m_i2c_host_data & 1;
			m_i2c_byte = 0;
			m_i2c_bit = 7;
			m_i2cp_mode = I2CP_RECIEVE_BYTE;
		}
		break;
	}
	m_i2c_prev_data = data;
	m_i2c_prev_clock = clock;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void namcos10_state::namcos10_exio(machine_config &config)
{
	namcos10_exio_device &exio(NAMCOS10_EXIO(config, m_exio, 0));

	exio.analog_callback().set([this] (offs_t offset) {
		return m_exio_analog[offset].read_safe(0);
	});

	m_psx_remapper = [this] () {
		m_maincpu->space(AS_PROGRAM).install_device(0x00000000, 0xffffffff, *this, &namcos10_state::namcos10_map_exio);
	};
}

void namcos10_state::namcos10_map_exio_inner(address_map &map)
{
	// TODO: Base registers are probably similar between EXIO and MGEXIO, fill in registers and rename if possible
	// TODO: Figure out what the commented out registers are actually used for
	map(0x06000, 0x0ffff).rw(m_exio, FUNC(namcos10_exio_device::ram_r), FUNC(namcos10_exio_device::ram_w));
	map(0x10000, 0x10003).rw(m_exio, FUNC(namcos10_exio_device::ctrl_r), FUNC(namcos10_exio_device::ctrl_w));
	map(0x18000, 0x18003).rw(m_exio, FUNC(namcos10_exio_device::bus_req_r), FUNC(namcos10_exio_device::bus_req_w));
	map(0x28000, 0x28003).r(m_exio, FUNC(namcos10_exio_device::cpu_status_r));
	// map(0x30000, 0x30003).nopw();
	// map(0x40000, 0x40003).nopw();
	// map(0x48000, 0x48003).nopw();
	map(0x50000, 0x50003).portr("EXIO_IN1");
	map(0x58000, 0x58003).portr("EXIO_IN2");
	// map(0xc0000, 0xc0003).nopw();
	// map(0xc8000, 0xc8003).nopw();
}

void namcos10_state::namcos10_map_exio(address_map &map)
{
	map(0x1fe00000, 0x1fffffff).m(FUNC(namcos10_state::namcos10_map_exio_inner));
	map(0x9fe00000, 0x9fffffff).m(FUNC(namcos10_state::namcos10_map_exio_inner));
	map(0xbfe00000, 0xbfffffff).m(FUNC(namcos10_state::namcos10_map_exio_inner));
}

///////////////////////////////////////////////////////////////////////////////////////////////

void namcos10_state::namcos10_mgexio(machine_config &config)
{
	// Probably stands for Medal Game EXIO. Every game so far is a medal game.
	// The inputs are best effort to get the games booting.
	// sekaikh sometimes throws a divider sol sensor error on boot.
	// I had success faking the pusher motor using a hopper device using 1000ms timer but it's a gross hack.
	// You can use the check sensor I/O to get some feedback from the games but it's hard
	// to understand what's going on or if things are really working as intended.

	// TODO: puzzball wants to see IRQ 2 triggering. Where from MGEXIO does that come? Probably a port

	namcos10_mgexio_device &mgexio(NAMCOS10_MGEXIO(config, m_exio, 0));

	HOPPER(config, m_mgexio_hopper[0], attotime::from_msec(100), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_HIGH);
	HOPPER(config, m_mgexio_hopper[1], attotime::from_msec(100), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_HIGH);
	HOPPER(config, m_mgexio_hopper[2], attotime::from_msec(100), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_HIGH);

	mgexio.port4_read_callback().set([this] (offs_t offset) {
		uint16_t r = 0;
		r |= (m_mgexio_outputs[6] & 1); // divider sol (l) sensor
		r |= (m_mgexio_outputs[7] & 1) << 1; //divider sol (r) sensor
		return r;
	});

	mgexio.port7_read_callback().set([this] (offs_t offset) {
		return m_mgexio_sensor->read();
	});

	mgexio.porta_read_callback().set([this] (offs_t offset) {
		uint16_t r = 0b1111;

		// update coin states
		auto curtime = machine().time();
		for (int i = 0; i < 2; i++) {
			if (m_mgexio_coin_start_time[i] == attotime::never)
				continue;

			auto diff = curtime - m_mgexio_coin_start_time[i];

			if (diff < attotime::from_msec(25))
				r ^= 1 << (2 * i); // L1/R1
			else if (diff < attotime::from_msec(50))
				r ^= 1 << (2 * i + 1); // L2/R2
			else
				m_mgexio_coin_start_time[i] = attotime::never;
		}

		r |= m_mgexio_hopper[0]->line_r() << 4; // hopper (l)
		r |= m_mgexio_hopper[1]->line_r() << 5; // hopper (r)
		r |= m_mgexio_hopper[2]->line_r() << 6; // hopper (c)
		r |= 1 << 7; // TODO: pusher motor, how does this work?
		return r;
	});

	mgexio.port4_write_callback().set([this] (uint16_t data) {
		m_mgexio_outputs[8] = BIT(data, 6); // win lamp
	});

	mgexio.portb_write_callback().set([this] (uint16_t data) {
		m_mgexio_hopper[0]->motor_w(BIT(data, 0));
		m_mgexio_hopper[1]->motor_w(BIT(data, 1));
		m_mgexio_hopper[2]->motor_w(BIT(data, 4));
		m_mgexio_outputs[2] = BIT(data, 2); // lockout sol (l)
		m_mgexio_outputs[3] = BIT(data, 3); // lockout sol (r)
		m_mgexio_outputs[5] = BIT(data, 5); // payout sol
		m_mgexio_outputs[6] = !BIT(data, 6); // divider sol (l)
		m_mgexio_outputs[7] = !BIT(data, 7); // divider sol (r)
	});

	m_psx_remapper = [this] () {
		m_maincpu->space(AS_PROGRAM).install_device(0x00000000, 0xffffffff, *this, &namcos10_state::namcos10_map_mgexio);
	};
}

void namcos10_state::namcos10_map_mgexio_inner(address_map &map)
{
	map(0x00000, 0x0ffff).rw(m_exio, FUNC(namcos10_mgexio_device::ram_r), FUNC(namcos10_mgexio_device::ram_w));
	map(0x10000, 0x10003).rw(m_exio, FUNC(namcos10_mgexio_device::ctrl_r), FUNC(namcos10_mgexio_device::ctrl_w));
	map(0x18000, 0x18003).rw(m_exio, FUNC(namcos10_mgexio_device::bus_req_r), FUNC(namcos10_mgexio_device::bus_req_w));
	/*
	0x20000 (w) - If bit 1 of 1fba001a is set then this gets cleared too
	(pacmball) When the CPU is recognized as being on, 1fba001a is set to 0xfffd and 0x1fe20000 is set to 0.
	When an error occurs then 0x1fe10000 and 1fe18000 are set to 0, and 0x1fe20000 is set to 2.
	*/
	map(0x28000, 0x28003).r(m_exio, FUNC(namcos10_mgexio_device::cpu_status_r));
	map(0x40000, 0x40003).w(FUNC(namcos10_state::mgexio_output_w<0>));
	map(0x41000, 0x41003).w(FUNC(namcos10_state::mgexio_output_w<1>));
	map(0x42000, 0x42003).w(FUNC(namcos10_state::mgexio_output_w<2>));
	map(0x43000, 0x43003).w(FUNC(namcos10_state::mgexio_output_w<3>));
	map(0x44000, 0x44003).w(FUNC(namcos10_state::mgexio_output_w<4>));
	map(0x45000, 0x45003).w(FUNC(namcos10_state::mgexio_output_w<5>));
	map(0x46000, 0x46003).w(FUNC(namcos10_state::mgexio_output_w<6>));
	map(0x47000, 0x47003).w(FUNC(namcos10_state::mgexio_output_w<7>));
}

void namcos10_state::namcos10_map_mgexio(address_map &map)
{
	map(0x1fe00000, 0x1fffffff).m(FUNC(namcos10_state::namcos10_map_mgexio_inner));
	map(0x9fe00000, 0x9fffffff).m(FUNC(namcos10_state::namcos10_map_mgexio_inner));
	map(0xbfe00000, 0xbfffffff).m(FUNC(namcos10_state::namcos10_map_mgexio_inner));
}

template <int N>
void namcos10_state::mgexio_output_w(offs_t offset, uint16_t data)
{
	switch (N) {
		case 0: m_mgexio_outputs[10] = data != 0; break; // led 2
		case 1: m_mgexio_outputs[9] = data != 0; break;  // led 1
		case 2: m_mgexio_outputs[12] = data != 0; break; // led 4
		case 3: m_mgexio_outputs[11] = data != 0; break; // led 3
		case 4: m_mgexio_outputs[14] = data != 0; break; // led 6
		case 5: m_mgexio_outputs[13] = data != 0; break; // led 5
		case 6: m_mgexio_outputs[15] = data != 0; break; // led 7
		default: break;
	}
}

INPUT_CHANGED_MEMBER(namcos10_state::mgexio_coin_start)
{
	if (newval && m_mgexio_coin_start_time[param] == attotime::never)
		m_mgexio_coin_start_time[param] = machine().time();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void namcos10_state::namcos10_exfinalio(machine_config &config)
{
	// TODO: Implement EXFINAL I/O board
	// Only seen on Medal no Tatsujin?
}

///////////////////////////////////////////////////////////////////////////////////////////////
// MEM(M)

void namcos10_memm_state::crypto_switch_w(uint16_t data)
{
	logerror("%s: crypto_switch_w: %04x\n", machine().describe_context(), data);
	if (!m_decrypter.found())
		return;

	if (BIT(data, 15) != 0)
		m_decrypter->activate(data & 0xf);
	else
		m_decrypter->deactivate();
}

uint16_t namcos10_memm_state::bank_r(offs_t offset)
{
	// Probably not correct but it works as a predictor
	m_bank_access_nand_direct = 1;
	return 0;
}

void namcos10_memm_state::bank_w(offs_t offset, uint16_t data)
{
	/*
	Can address banks 0x00 to 0x1f
	Bank 0x00-0x08 are mapped to the NAND
	Bank 0x10-0x18 are mapped to .1d?
	Bank 0x18-0x1f are mapped to .2e?
	*/

	m_bank_idx = offset;
	m_bank_access_nand_direct = offset >= 0x10;
}

uint16_t namcos10_memm_state::range_r(offs_t offset)
{
	if (m_bank_access_nand_direct == 0) {
		// Direct flash access
		return m_nand->read((0x100000 * m_bank_idx) + offset);
	}

	const uint16_t *bank_data;
	if (m_bank_idx < 0x10) {
		bank_data = &((uint16_t*)memregion("nand")->base())[0x100000 * m_bank_idx];
	} else {
		bank_data = &((uint16_t*)memregion("data")->base())[0x100000 * (m_bank_idx - 0x10)];
	}

	uint16_t data = bank_data[offset];

	if (m_decrypter.found() && m_decrypter->is_active())
		return m_decrypter->decrypt(data);

	if (m_bank_idx < 0x10) {
		// TODO: Need more MEM(M) samples to verify if all of the data ROMs are unscrambled normally
		data = m_unscrambler(data ^ 0xaaaa);
	}

	return data;
}

void namcos10_memm_state::nand_w(offs_t offset, uint16_t data)
{
	if (m_bank_access_nand_direct != 0)
		return;

	m_nand->write(offset, data);
}

void namcos10_memm_state::namcos10_memm(machine_config &config)
{
	namcos10_base(config);

	// Can be an Intel 28F640J5 or a Sharp flash with a chip ID of 0xc0
	INTEL_28F640J5(config, m_nand);

	m_maincpu->set_addrmap(AS_PROGRAM, &namcos10_memm_state::namcos10_memm_map);
}

void namcos10_memm_state::machine_start()
{
	namcos10_state::machine_start();

	save_item(NAME(m_bank_idx));
	save_item(NAME(m_bank_access_nand_direct));
}

void namcos10_memm_state::machine_reset()
{
	namcos10_state::machine_reset();

	m_bank_idx = 0;
	m_bank_access_nand_direct = 0;
}

void namcos10_memm_state::namcos10_memm_map_inner(address_map &map)
{
	// f210000 = if this returns 0xffff then it sets 1f801008 (EXP1 delay/size) and 1f80100c (EXP3 delay/size)
	map(0xf300000, 0xf300001).w(FUNC(namcos10_memm_state::crypto_switch_w));
	map(0xf400000, 0xf5fffff).rw(FUNC(namcos10_memm_state::range_r), FUNC(namcos10_memm_state::nand_w));
	map(0xfb40000, 0xfb5ffff).rw(FUNC(namcos10_memm_state::bank_r), FUNC(namcos10_memm_state::bank_w));
}

void namcos10_memm_state::namcos10_memm_map(address_map &map)
{
	namcos10_map(map);

	map(0x10000000, 0x1fffffff).m(FUNC(namcos10_memm_state::namcos10_memm_map_inner));
	map(0x90000000, 0x9fffffff).m(FUNC(namcos10_memm_state::namcos10_memm_map_inner));
	map(0xb0000000, 0xbfffffff).m(FUNC(namcos10_memm_state::namcos10_memm_map_inner));
}

void namcos10_memm_state::decrypt_bios_region(int start, int end)
{
	uint16_t *bios = (uint16_t *)(memregion("maincpu:rom")->base() + start);

	for(int i = start / 2; i < end / 2; i++) {
		bios[i] = m_unscrambler(bios[i] ^ 0xaaaa);
	}
}

void namcos10_memm_state::memm_driver_init()
{
	memcpy(
		(uint8_t *)memregion("maincpu:rom")->base(),
		(uint8_t *)memregion("nand")->base(),
		0x62000
	);
}

void namcos10_memm_state::init_mrdrilr2()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xc, 0xd, 0xf, 0xe, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x4, 0x1, 0x2, 0x5, 0x0, 0x3); };
	memm_driver_init();
	decrypt_bios_region(0, 0x62000);
}

void namcos10_memm_state::ns10_mrdrilr2(machine_config &config)
{
	namcos10_memm(config);
	/* decrypter device (CPLD in hardware?) */
	MRDRILR2_DECRYPTER(config, m_decrypter, 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////
// MEM(N)

void namcos10_memn_state::pio_dma_write(uint32_t *p_n_psxram, uint32_t n_address, int32_t n_size)
{
	// Are DMA writes ever performed?
	logerror("%s: pio_dma_write: unhandled DMA write %08x %08x\n", machine().describe_context(), n_address, n_size);
}

void namcos10_memn_state::pio_dma_read(uint32_t *p_n_psxram, uint32_t n_address, int32_t n_size)
{
	logerror("%s: pio_dma_read: DMA read %08x %08x\n", machine().describe_context(), n_address, n_size * 4);

	if (m_is_cdrom_dma)
		m_ata->write_dmack(ASSERT_LINE);

	auto ptr = util::little_endian_cast<uint16_t>(&p_n_psxram[n_address / 4]);
	for (auto i = 0; i < n_size * 2; i++) {
		ptr[i] = m_is_cdrom_dma ? m_ata->read_dma() : nand_data_r();
	}

	if (m_is_cdrom_dma) {
		m_ata->write_dmack(CLEAR_LINE);
		m_is_cdrom_dma = false;
	}
}

uint16_t namcos10_memn_state::nand_rnb_r()
{
	return m_nand_rnb_state[m_nand_device_idx];
}

void namcos10_memn_state::crypto_switch_w(uint16_t data)
{
	logerror("%s: crypto_switch_w: %04x\n", machine().describe_context(), data);

	if (!m_decrypter.found())
		return;

	if (BIT(data, 15) != 0)
		m_decrypter->activate(data & 0xf);
	else
		m_decrypter->deactivate();
}

void namcos10_memn_state::nand_cmd_w(uint8_t data)
{
	// TODO: Sometimes the upper byte is non-zero, why?
	logerror("%s: nand_cmd_w %08x\n", machine().describe_context(), data);
	if (m_nand[m_nand_device_idx])
		m_nand[m_nand_device_idx]->command_w(data & 0xff);
}

void namcos10_memn_state::nand_address_column_w(uint8_t data)
{
	if (m_nand[m_nand_device_idx])
		m_nand[m_nand_device_idx]->address_w(data & 0xff);
	m_nand_address = (m_nand_address & 0xffff00) | data;
	logerror("%s: nand_address_column_w %02x %08x\n", machine().describe_context(), data, m_nand_address);
}

void namcos10_memn_state::nand_address_row_w(uint8_t data)
{
	if (m_nand[m_nand_device_idx])
		m_nand[m_nand_device_idx]->address_w(data & 0xff);
	m_nand_address = (m_nand_address & 0xff00ff) | (data << 8);
	logerror("%s: nand_address_row_w %02x %08x\n", machine().describe_context(), data, m_nand_address);
}

void namcos10_memn_state::nand_address_page_w(uint8_t data)
{
	if (m_nand[m_nand_device_idx])
		m_nand[m_nand_device_idx]->address_w(data & 0xff);
	m_nand_address = (m_nand_address & 0x00ffff) | (data << 16);
	logerror("%s: nand_address_page_w %02x %08x\n", machine().describe_context(), data, m_nand_address);
}

uint16_t namcos10_memn_state::nand_data_r()
{
	if (!m_nand[m_nand_device_idx])
		return 0xffff;

	uint16_t data = (m_nand[m_nand_device_idx]->data_r() << 8)
		| m_nand[m_nand_device_idx]->data_r();

	if (m_decrypter.found() && m_decrypter->is_active())
		return m_decrypter->decrypt(data);

	// Block 0 is guaranteed to always be good according to the NAND datasheet, and it seems to always contain the bad block redirect table
	// Block 1 appears to always be used by games as the EEPROM data block
	// Dumped ROMs always have block 1 as plaintext so data doesn't appear scrambled when writing to block 1
	// So treat only block 0 and block 1 as special blocks for now
	// TODO: Is there a more reliable way to detect when data should be unscrambled? (m_ctrl_reg?)
	if (m_nand_device_idx == 0 && (BIT(m_nand_address, 8, 16) >> 5) < 2)
		return data;

	return m_unscrambler(data ^ 0xaaaa);
}

void namcos10_memn_state::nand_data_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	if (!m_nand[m_nand_device_idx])
		return;

	m_nand[m_nand_device_idx]->data_w(data >> 8);
	m_nand[m_nand_device_idx]->data_w(data & 0xff);
}

void namcos10_memn_state::nand_bank_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	logerror("%s: nand_bank_w: %04x %08x\n", machine().describe_context(), data, (data * 0x3ec) << 0x0e);

	if ((m_ctrl_reg & 4) == 0)
		m_nand_device_idx = data;
}

void namcos10_memn_state::ctrl_reg_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	// Set to 4 before using 1f410000-1f450000 registers and then set to 0 after it's finished.
	// 4 is either a device lock kind of flag or maybe it controls direct NAND access?
	// 2 has also been seen
	logerror("%s ctrl_reg_w: %04x\n", machine().describe_context(), data);
	m_ctrl_reg = data;
}

uint16_t namcos10_memn_state::ctrl_reg_r(offs_t offset)
{
	return m_ctrl_reg;
}

void namcos10_memn_state::namcos10_memn_base(machine_config &config)
{
	namcos10_base(config);

	m_maincpu->subdevice<psxdma_device>("dma")->install_read_handler(5, psxdma_device::read_delegate(&namcos10_memn_state::pio_dma_read, this));
	m_maincpu->subdevice<psxdma_device>("dma")->install_write_handler(5, psxdma_device::write_delegate(&namcos10_memn_state::pio_dma_write, this));
	m_maincpu->set_addrmap(AS_PROGRAM, &namcos10_memn_state::namcos10_memn_map);
}

void namcos10_memn_state::machine_start()
{
	namcos10_state::machine_start();

	save_item(NAME(m_ctrl_reg));
	save_item(NAME(m_nand_device_idx));
	save_item(NAME(m_nand_address));
	save_item(NAME(m_nand_rnb_state));
}

void namcos10_memn_state::machine_reset()
{
	namcos10_state::machine_reset();

	m_ctrl_reg = 0;
	m_nand_device_idx = 0;
	m_nand_address = 0;

	std::fill(std::begin(m_nand_rnb_state), std::end(m_nand_rnb_state), 0);
}

void namcos10_memn_state::namcos10_memn_map_inner(address_map &map)
{
	map(0xf300000, 0xf300001).w(FUNC(namcos10_memn_state::crypto_switch_w));
	map(0xf380000, 0xf380001).w(FUNC(namcos10_memn_state::crypto_switch_w));
	map(0xf400000, 0xf400001).r(FUNC(namcos10_memn_state::nand_rnb_r));
	map(0xf410000, 0xf410000).w(FUNC(namcos10_memn_state::nand_cmd_w));
	map(0xf420000, 0xf420000).w(FUNC(namcos10_memn_state::nand_address_column_w));
	map(0xf430000, 0xf430000).w(FUNC(namcos10_memn_state::nand_address_row_w));
	map(0xf440000, 0xf440000).w(FUNC(namcos10_memn_state::nand_address_page_w));
	map(0xf450000, 0xf450001).rw(FUNC(namcos10_memn_state::nand_data_r), FUNC(namcos10_memn_state::nand_data_w));
	map(0xf460000, 0xf460001).w(FUNC(namcos10_memn_state::nand_bank_w));
	map(0xf470000, 0xf470001).rw(FUNC(namcos10_memn_state::ctrl_reg_r), FUNC(namcos10_memn_state::ctrl_reg_w));
	//map(0xf480000, 0xf480001).w(); // 0xffff is written here after a nand write/erase?
}

void namcos10_memn_state::namcos10_memn_map(address_map &map)
{
	namcos10_map(map);

	// konotako wants to access registers through 0xbf450000 so mirror the registers the way the PS1 normally
	// mirrors these ranges
	map(0x10000000, 0x1fffffff).m(FUNC(namcos10_memn_state::namcos10_memn_map_inner));
	map(0x90000000, 0x9fffffff).m(FUNC(namcos10_memn_state::namcos10_memn_map_inner));
	map(0xb0000000, 0xbfffffff).m(FUNC(namcos10_memn_state::namcos10_memn_map_inner));
}

void namcos10_memn_state::nand_copy(uint8_t *nand_base, uint16_t *dst, uint32_t start_page, int len)
{
	for (int page = start_page; page < start_page + len; page++)
	{
		int address = page * 0x210;

		for (int i = 0; i < 0x200; i += 2) {
			uint16_t data = nand_base[address + i + 1] | (nand_base[address + i] << 8);
			*dst = m_unscrambler(data ^ 0xaaaa);
			dst++;
		}
	}
}

void namcos10_memn_state::memn_driver_init()
{
	uint8_t *bios = (uint8_t *)memregion("maincpu:rom")->base();
	uint8_t *nand_base = (uint8_t *)memregion("nand0")->base();

	nand_copy(nand_base, (uint16_t *)bios, 0x40, 0xe0);
	nand_copy(nand_base, (uint16_t *)(bios + 0x0020000), 0x120, 0x1f00);
}

void namcos10_memn_state::init_ballpom()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xc, 0xe, 0xf, 0xa, 0xb, 0x8, 0x9, 0x5, 0x4, 0x6, 0x7, 0x1, 0x3, 0x0, 0x2); };
	memn_driver_init();
}

void namcos10_memn_state::init_chocovdr()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xc, 0xe, 0xf, 0x9, 0x8, 0xa, 0xb, 0x4, 0x7, 0x6, 0x5, 0x0, 0x3, 0x2, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_gahaha()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xe, 0xc, 0xf, 0xa, 0x8, 0xb, 0x9, 0x6, 0x7, 0x4, 0x5, 0x0, 0x3, 0x2, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_gahaha2()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xc, 0xf, 0xd, 0xa, 0x9, 0x8, 0xb, 0x4, 0x5, 0x6, 0x7, 0x0, 0x1, 0x3, 0x2); };
	memn_driver_init();
}

void namcos10_memn_state::init_gamshara()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xc, 0xf, 0xe, 0x8, 0x9, 0xb, 0xa, 0x5, 0x7, 0x4, 0x6, 0x0, 0x1, 0x2, 0x3); };
	memn_driver_init();
}

void namcos10_memn_state::init_gjspace()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0x8, 0xa, 0x6, 0x5, 0x7, 0xe, 0x4, 0xf, 0xd, 0x9, 0x1, 0x0, 0x2, 0xb, 0xc, 0x3); };
	memn_driver_init();
}

void namcos10_memn_state::init_gunbalina()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xc, 0xf, 0xe, 0x8, 0x9, 0xb, 0xa, 0x5, 0x7, 0x4, 0x6, 0x1, 0x0, 0x2, 0x3); };
	memn_driver_init();
}

void namcos10_memn_state::init_keroro()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xf, 0xc, 0xd, 0xa, 0x8, 0xb, 0x9, 0x4, 0x5, 0x7, 0x6, 0x2, 0x1, 0x0, 0x3); };
	memn_driver_init();
}

void namcos10_memn_state::init_knpuzzle()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xf, 0xc, 0xd, 0xa, 0x8, 0xb, 0x9, 0x4, 0x5, 0x6, 0x7, 0x1, 0x3, 0x0, 0x2); };
	memn_driver_init();
}

void namcos10_memn_state::init_konotako()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xf, 0xc, 0xd, 0x8, 0x9, 0xb, 0xa, 0x5, 0x4, 0x7, 0x6, 0x0, 0x1, 0x3, 0x2); };
	memn_driver_init();
}

void namcos10_memn_state::init_medalnt()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xf, 0xc, 0xe, 0x8, 0x9, 0xa, 0xb, 0x5, 0x4, 0x6, 0x7, 0x2, 0x3, 0x0, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_medalnt2()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xf, 0xc, 0xe, 0xa, 0x8, 0xb, 0x9, 0x4, 0x7, 0x6, 0x5, 0x1, 0x3, 0x0, 0x2); };
	memn_driver_init();
}

void namcos10_memn_state::init_mrdrilrg()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xc, 0xf, 0xd, 0xa, 0x9, 0x8, 0xb, 0x4, 0x5, 0x6, 0x7, 0x0, 0x1, 0x3, 0x2); };
	memn_driver_init();
}

void namcos10_memn_state::init_nflclsfb()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xd, 0xc, 0xf, 0x9, 0xb, 0x8, 0xa, 0x4, 0x5, 0x6, 0x7, 0x0, 0x3, 0x2, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_pacmball()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xf, 0xc, 0xe, 0x8, 0xb, 0xa, 0x9, 0x4, 0x5, 0x7, 0x6, 0x0, 0x3, 0x2, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_panikuru()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xc, 0xf, 0xd, 0x8, 0x9, 0xa, 0xb, 0x4, 0x7, 0x6, 0x5, 0x1, 0x0, 0x3, 0x2); };
	memn_driver_init();
}

void namcos10_memn_state::init_puzzball()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xc, 0xf, 0xe, 0xd, 0x8, 0x9, 0xa, 0xb, 0x4, 0x5, 0x7, 0x6, 0x1, 0x0, 0x2, 0x3); };
	memn_driver_init();
}

void namcos10_memn_state::init_sekaikh()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xc, 0xe, 0xf, 0x9, 0xb, 0x8, 0xa, 0x6, 0x5, 0x4, 0x7, 0x2, 0x3, 0x0, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_startrgn()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xd, 0xc, 0xf, 0x9, 0xb, 0x8, 0xa, 0x4, 0x5, 0x6, 0x7, 0x0, 0x3, 0x2, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_sugorotic()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xd, 0xc, 0xe, 0xf, 0x9, 0xb, 0x8, 0xa, 0x4, 0x5, 0x6, 0x7, 0x0, 0x1, 0x2, 0x3); };
	memn_driver_init();
}

void namcos10_memn_state::init_taiko2()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xc, 0xd, 0xe, 0xf, 0x9, 0x8, 0xb, 0xa, 0x6, 0x4, 0x7, 0x5, 0x2, 0x3, 0x0, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_taiko3()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xc, 0xf, 0xd, 0xa, 0x9, 0x8, 0xb, 0x4, 0x5, 0x6, 0x7, 0x0, 0x1, 0x3, 0x2); };
	memn_driver_init();
}

void namcos10_memn_state::init_taiko4()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xd, 0xc, 0xf, 0x9, 0x8, 0xb, 0xa, 0x5, 0x4, 0x6, 0x7, 0x2, 0x3, 0x0, 0x1); };
	memn_driver_init();
}

void namcos10_memn_state::init_taiko5()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xf, 0xc, 0xd, 0xa, 0x8, 0xb, 0x9, 0x4, 0x5, 0x7, 0x6, 0x2, 0x1, 0x0, 0x3); };
	memn_driver_init();
}

void namcos10_memn_state::init_taiko6()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xc, 0xf, 0xd, 0x9, 0xb, 0x8, 0xa, 0x5, 0x4, 0x7, 0x6, 0x2, 0x1, 0x0, 0x3); };
	memn_driver_init();
}

void namcos10_memn_state::namcos10_nand_k9f2808u0b(machine_config &config, int nand_count)
{
	for (int i = 0; i < nand_count; i++) {
		SAMSUNG_K9F2808U0B(config, m_nand[i], 0);
		m_nand[i]->rnb_wr_callback().set([this, i] (int state) { m_nand_rnb_state[i] = state != 1; });
	}
}

void namcos10_memn_state::namcos10_nand_k9f5608u0d(machine_config &config, int nand_count)
{
	for (int i = 0; i < nand_count; i++) {
		SAMSUNG_K9F5608U0D(config, m_nand[i], 0);
		m_nand[i]->rnb_wr_callback().set([this, i] (int state) { m_nand_rnb_state[i] = state != 1; });
	}
}

void namcos10_memn_state::ns10_ballpom(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_mgexio(config);
	namcos10_nand_k9f2808u0b(config, 2);

	/* decrypter device (CPLD in hardware?) */
	// BALLPOM_DECRYPTER(config, m_decrypter, 0);
}

// nasty, but makes code more terse
inline int gf2_reduce(uint64_t num) { return ns10_type2_decrypter_device::gf2_reduce(num); }

void namcos10_memn_state::ns10_chocovdr(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 5);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x00005239351ec1daull, 0x0000000000008090ull, 0x0000000048264808ull, 0x0000000000004820ull,
			0x0000000000000500ull, 0x0000000058ff5a54ull, 0x00000000d8220208ull, 0x00005239351e91d3ull,
			0x000000009a1dfaffull, 0x0000000090040001ull, 0x0000000000000100ull, 0x0000000000001408ull,
			0x0000000032efd3f1ull, 0x00000000000000d0ull, 0x0000000032efd2d7ull, 0x0000000000000840ull,
		}, {
			0x00002000410485daull, 0x0000000000008081ull, 0x0000000008044088ull, 0x0000000000004802ull,
			0x0000000000000500ull, 0x00000000430cda54ull, 0x0000000010000028ull, 0x00002000410491dbull,
			0x000000001100fafeull, 0x0000000018040001ull, 0x0000000000000010ull, 0x0000000000000508ull,
			0x000000006800d3f5ull, 0x0000000000000058ull, 0x000000006800d2d5ull, 0x0000000000001840ull,
		},
		0x5b22,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks >> 9) & (gf2_reduce(0x0000000010065810ull & previous_cipherwords) ^ gf2_reduce(0x0000000021005810ull & previous_plainwords)) & 1) << 10;
		}
	});
}

void namcos10_memn_state::ns10_gahaha(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exio(config);
	namcos10_nand_k9f2808u0b(config, 3);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000010a08200ull, 0x00000000001b0204ull, 0x00004ba503024016ull, 0x0000000000000004ull,
			0x0000000000000240ull, 0x0000000088080180ull, 0x000011821ce50066ull, 0x000000000a204200ull,
			0x0000000014018800ull, 0x00000000000000a0ull, 0x0000000000000412ull, 0x0000000000004002ull,
			0x000000003100c002ull, 0x0000000000002100ull, 0x00000000084000a4ull, 0x0000000031010180ull,
		}, {
			0x0000000000808000ull, 0x0000004400130200ull, 0x0000021804a54036ull, 0x0000000000000014ull,
			0x0000000000000240ull, 0x0000000085000100ull, 0x000008ca15400166ull, 0x000000009822c280ull,
			0x0000000014008008ull, 0x00000000000010a0ull, 0x0000000000000016ull, 0x0000000000004002ull,
			0x000000003120c000ull, 0x0000000000002100ull, 0x0000000018e002a6ull, 0x00000000a19121a0ull,
		},
		0xaea7,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			return 0;
		}
	});
}

void namcos10_memn_state::ns10_gahaha2(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exio(config);
	namcos10_nand_k9f2808u0b(config, 3);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000080064001ull, 0x000000000a000104ull, 0x0000018220912000ull, 0x0000000001822010ull,
			0x00000000000001a0ull, 0x00000481a4220004ull, 0x0000a11490041269ull, 0x0000000000000810ull,
			0x000000000a008200ull, 0x00000000010b0010ull, 0x0000000052108820ull, 0x0000042209a00258ull,
			0x0000000001820401ull, 0x0000000090040040ull, 0x0000000000001002ull, 0x0000209008020004ull,
		}, {
			0x0000000000020001ull, 0x000000000a000024ull, 0x0000018000830400ull, 0x0000000001802002ull,
			0x0000000000000130ull, 0x0000200110060004ull, 0x00000581080c1260ull, 0x0000000000000810ull,
			0x000000000a008040ull, 0x0000000021bf0010ull, 0x0000000040588820ull, 0x0000003000220210ull,
			0x0000000001800400ull, 0x0000000090000040ull, 0x0000000000009002ull, 0x00000403a5020004ull,
		},
		0x925a,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			u64 previous_masks = previous_cipherwords^previous_plainwords;
			return (1 & ((previous_masks >> 26) ^ (previous_masks >> 37)) & (previous_masks >> 46)) * 0x8860;
		}
	});
}

void namcos10_memn_state::ns10_gamshara(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000000028ull, 0x0000cae83f389fd9ull, 0x0000000000001000ull, 0x0000000042823402ull,
			0x0000cae8736a0592ull, 0x0000cae8736a8596ull, 0x000000008b4095b9ull, 0x0000000000002100ull,
			0x0000000004018228ull, 0x0000000000000042ull, 0x0000000000000818ull, 0x0000000000004010ull,
			0x000000008b4099f1ull, 0x00000000044bce08ull, 0x00000000000000c1ull, 0x0000000042823002ull,
		}, {
			0x0000000000000028ull, 0x00000904c2048dd9ull, 0x0000000000008000ull, 0x0000000054021002ull,
			0x00000904e0078592ull, 0x00000904e00785b2ull, 0x00000000440097f9ull, 0x0000000000002104ull,
			0x0000000029018308ull, 0x0000000000000042ull, 0x0000000000000850ull, 0x0000000000004012ull,
			0x000000004400d1f1ull, 0x000000006001ce08ull, 0x00000000000000c8ull, 0x0000000054023002ull,
		},
		0x25ab,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks >> 7) & (previous_masks >> 13) & 1) << 2;
		}
	});
}

void namcos10_memn_state::ns10_gegemdb(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_mgexio(config);
	namcos10_nand_k9f5608u0d(config, 2);

	/* decrypter device (CPLD in hardware?) */
	// GEGEMDB_DECRYPTER(config, m_decrypter, 0);
}

void namcos10_memn_state::ns10_gjspace(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 4);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000000240ull, 0x0000d617eb0f1ab1ull, 0x00000000451111c0ull, 0x00000000013b1f44ull,
			0x0000aab0b356abceull, 0x00007ca76b89602aull, 0x0000000000001800ull, 0x00000000031d1303ull,
			0x0000000000000801ull, 0x0000000030111160ull, 0x0000000001ab3978ull, 0x00000000c131b160ull,
			0x0000000000001110ull, 0x0000000000008002ull, 0x00000000e1113540ull, 0x0000d617fdce8bfcull,
		}, {
			0x0000000000008240ull, 0x000000002f301ab1ull, 0x00000000050011c0ull, 0x00000000412817c4ull,
			0x00000004c338abc6ull, 0x000000046108602aull, 0x0000000000005800ull, 0x00000000c3081347ull,
			0x0000000000000801ull, 0x0000000061001160ull, 0x0000000061183978ull, 0x00000000e520b142ull,
			0x0000000000001101ull, 0x000000000000a002ull, 0x0000000029001740ull, 0x00000000a4309bfcull,
		},
		0x2e7f,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			return 0;
		}
	});
}

void namcos10_memn_state::ns10_kd2001(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 2);

	// TODO: Also has a "pdrive" ROM? What's that for?

	// NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, logic);
}

void namcos10_memn_state::ns10_keroro(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_mgexio(config);
	namcos10_nand_k9f5608u0d(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000000024ull, 0x0000000000000884ull, 0x00000000000e0a00ull, 0x0000000000000040ull,
			0x000000000a002100ull, 0x0000000000002011ull, 0x0000000000004010ull, 0x0000050006000000ull,
			0x000000000000a004ull, 0x0000000000001082ull, 0x00000500060000c0ull, 0x0000000000000403ull,
			0x0000000020014040ull, 0x0000000000001208ull, 0x0000000000000218ull, 0x0000000020014100ull,
		}, {
			0x0000000000000024ull, 0x0000000000000888ull, 0x00000000001c1200ull, 0x0000000000000020ull,
			0x000000003a182101ull, 0x0000000000002012ull, 0x0000000000008010ull, 0x0000050000008000ull,
			0x000000000000c004ull, 0x0000000000001084ull, 0x0000050000000140ull, 0x0000000000000803ull,
			0x0000000020014080ull, 0x0000000000002208ull, 0x0000000000000228ull, 0x0000000020014200ull,
		},
		0xee91,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks>>4) & ((previous_masks>>24) ^ (previous_masks>>26)) & 1) << 6;
		},
		[] (int iv) -> uint64_t {
			constexpr uint64_t values[16]{
				0x0000, 0x0602, 0x0302, 0x0101, 0x0203, 0x0802, 0x0603, 0x0303,
				0x0001, 0x0803, 0x0501, 0x0500, 0x0202, 0x0100, 0x0401, 0x0400,
			};
			return values[iv];
		}
	});
}

void namcos10_memn_state::ns10_knpuzzle(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 3);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x00000000c0a4208cull, 0x00000000204100a8ull, 0x000000000c0306a0ull, 0x000000000819e944ull,
			0x0000000000001400ull, 0x0000000000000061ull, 0x000000000141401cull, 0x0000000000000020ull,
			0x0000000001418010ull, 0x00008d6a1eb690cfull, 0x00008d6a4d3b90ceull, 0x0000000000004201ull,
			0x00000000012c00a2ull, 0x000000000c0304a4ull, 0x0000000000000500ull, 0x0000000000000980ull,
		}, {
			0x000000002a22608cull, 0x00000000002300a8ull, 0x0000000000390ea0ull, 0x000000000100a9c4ull,
			0x0000000000001400ull, 0x0000000000000041ull, 0x0000000003014014ull, 0x0000000000000022ull,
			0x0000000003010110ull, 0x00000800031a80cfull, 0x00000800003398deull, 0x0000000000004200ull,
			0x00000000012a04a2ull, 0x00000000003984a4ull, 0x0000000000000700ull, 0x0000000000000882ull,
		},
		0x01e2,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks >> 0x13) & (gf2_reduce(0x0000000014001290ull & previous_cipherwords) ^ gf2_reduce(0x0000000000021290ull & previous_plainwords)) & 1) << 1;
		}
	});
}

void namcos10_memn_state::ns10_konotako(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x000000000000004cull, 0x00000000d39e3d3dull, 0x0000000000001110ull, 0x0000000000002200ull,
			0x000000003680c008ull, 0x0000000000000281ull, 0x0000000000005002ull, 0x00002a7371895a47ull,
			0x0000000000000003ull, 0x00002a7371897a4eull, 0x00002a73aea17a41ull, 0x00002a73fd895a4full,
			0x000000005328200aull, 0x0000000000000010ull, 0x0000000000000040ull, 0x0000000000000200ull,
		}, {
			0x000000000000008cull, 0x0000000053003d25ull, 0x0000000000001120ull, 0x0000000000002200ull,
			0x0000000037004008ull, 0x0000000000000282ull, 0x0000000000006002ull, 0x0000060035005a47ull,
			0x0000000000000003ull, 0x0000060035001a4eull, 0x0000060025007a41ull, 0x00000600b5005a2full,
			0x000000009000200bull, 0x0000000000000310ull, 0x0000000000001840ull, 0x0000000000000400ull,
		},
		0x0748,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks >> 7) & (previous_masks >> 15) & 1) << 15;
		}
	});
}

void namcos10_memn_state::ns10_medalnt(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exfinalio(config);
	namcos10_nand_k9f2808u0b(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000080601000ull, 0x0000000000006020ull, 0x0000000000004840ull, 0x0000000000000201ull,
			0x0000000000020004ull, 0x0000000000000081ull, 0x0000000000009001ull, 0x0000000001041810ull,
			0x00000000ca001806ull, 0x0000000080600500ull, 0x0000000000002022ull, 0x000000002204001cull,
			0x000000000c508044ull, 0x0000000000000808ull, 0x00000000ca001094ull, 0x0000000000000184ull,
		}, {
			0x0000000081201000ull, 0x0000000000006080ull, 0x0000000000000840ull, 0x0000000000000201ull,
			0x0000000000080005ull, 0x0000000000000081ull, 0x0000000000009004ull, 0x00000000042c4810ull,
			0x000000000a003006ull, 0x0000000081201100ull, 0x0000000000008022ull, 0x0000000028050034ull,
			0x000000004c000044ull, 0x000000000000080aull, 0x000000000a001214ull, 0x0000000000000190ull,
		},
		0x5d04,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks>>26) & (gf2_reduce(previous_cipherwords & 0x10100080) ^ gf2_reduce(previous_plainwords & 0x40100080))) << 4;
		}
	});
}

void namcos10_memn_state::ns10_medalnt2(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exfinalio(config);
	namcos10_nand_k9f5608u0d(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000000202ull, 0x0000242000120110ull, 0x0000000001624608ull, 0x0000000000001820ull,
			0x000000000000c040ull, 0x0000000000000184ull, 0x0000000007900002ull, 0x0000000000005008ull,
			0x0000000000008401ull, 0x0000000000008800ull, 0x0000000000000250ull, 0x0000000000002000ull,
			0x0000000000000024ull, 0x0000000000006080ull, 0x0000000000000042ull, 0x0000000007900006ull,
		}, {
			0x0000000000000203ull, 0x0000442000140120ull, 0x0000000002024008ull, 0x0000000000001840ull,
			0x0000000000004040ull, 0x0000000000000188ull, 0x0000000000200002ull, 0x0000000000006008ull,
			0x0000000000008801ull, 0x0000000000009000ull, 0x0000000000000290ull, 0x0000000000002003ull,
			0x0000000000000024ull, 0x000000000000a080ull, 0x0000000000000042ull, 0x000000000020001eull,
		},
		0x4c57,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (((previous_masks>>15) & (gf2_reduce(previous_cipherwords & 0x24200000) ^ gf2_reduce(previous_plainwords & 0x44200000))) & 1) << 9;
		}
	});
}

void namcos10_memn_state::ns10_mrdrilrg(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 3);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000115c68620401ull, 0x0000008102802094ull, 0x00000081a65270a8ull, 0x0000000000810010ull,
			0x00004c2c080f3059ull, 0x00000081470a9500ull, 0x000000816a123009ull, 0x00000081a2b23038ull,
			0x00002d18a06b9000ull, 0x000001186c02000cull, 0x00000081a2b2704cull, 0x0000702811016302ull,
			0x00000081470a1122ull, 0x0000000000006200ull, 0x00009170ac403013ull, 0x0000000000001044ull,
		}, {
			0x0000115c68620401ull, 0x0000008102802094ull, 0x00000081a65270a8ull, 0x0000000000810010ull,
			0x00004c2c080f3059ull, 0x00000081470a9500ull, 0x000000816a123009ull, 0x00000081a2b23038ull,
			0x00002d18a06b9000ull, 0x000001186c02000cull, 0x00000081a2b2704cull, 0x0000702811016302ull,
			0x00000081470a1122ull, 0x0000000000006200ull, 0x00009170ac403013ull, 0x0000000000001044ull,
		},
		0x0000,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & gf2_reduce(previous_masks & 0x120100400000) & gf2_reduce(previous_masks & 0x11800020000)) * 0x11;
		},
		[] (int iv) -> uint64_t {
			constexpr uint64_t values[16]{
				0x0000, 0x0000, 0x0000, 0x2000, 0x801a, 0x6000, 0x0002, 0x0000,
				0x0000, 0x0018, 0x0000, 0x0420, 0x0000, 0x8800, 0x0000, 0xc000,
			};
			return values[iv];
		}
	});
}

void namcos10_memn_state::ns10_nflclsfb(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exio(config);
	namcos10_nand_k9f2808u0b(config, 4);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x000034886e281880ull, 0x0000000012c5e7baull, 0x0000000000000200ull, 0x000000002900002aull,
			0x00000000000004c0ull, 0x0000000012c5e6baull, 0x00000000e0df8bbbull, 0x000000002011532aull,
			0x0000000000009040ull, 0x0000000000006004ull, 0x000000000000a001ull, 0x000034886e2818e1ull,
			0x0000000000004404ull, 0x0000000000004200ull, 0x0000000000009100ull, 0x0000000020115712ull,
		}, {
			0x00000e00060819c0ull, 0x000000000e08e7baull, 0x0000000000000800ull, 0x000000000100002aull,
			0x00000000000010c0ull, 0x000000000e08cebaull, 0x0000000088018bbbull, 0x000000008c005302ull,
			0x000000000000c040ull, 0x0000000000006010ull, 0x0000000000000001ull, 0x00000e00060818e3ull,
			0x0000000000000404ull, 0x0000000000004201ull, 0x0000000000001100ull, 0x000000008c0057b2ull,
		},
		0xbe32,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks >> 1) & (gf2_reduce(0x0000000040de8fb3ull & previous_cipherwords) ^ gf2_reduce(0x0000000088008fb3ull & previous_plainwords)) & 1) << 2;
		}
	});
}

void namcos10_memn_state::ns10_pacmball(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_mgexio(config);
	namcos10_nand_k9f2808u0b(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000008028ull, 0x0000000000000400ull, 0x00000000a9100004ull, 0x0000000028004200ull,
			0x0000000000001002ull, 0x0000000000001041ull, 0x0000001a70008022ull, 0x0000000081100022ull,
			0x0000000000000890ull, 0x0000000000003040ull, 0x0000e00000108411ull, 0x00000000000000a4ull,
			0x0000000000000980ull, 0x0000000000004208ull, 0x0000000000000300ull, 0x0000e00000108001ull,
		}, {
			0x0000000000008030ull, 0x0000000000000800ull, 0x0000000029100008ull, 0x0000000028008200ull,
			0x0000000000001002ull, 0x0000000000002041ull, 0x0000002e20038024ull, 0x0000000001100042ull,
			0x0000000000001090ull, 0x0000000000003080ull, 0x0000800000228421ull, 0x0000000000000124ull,
			0x0000000000000a80ull, 0x0000000000004408ull, 0x0000000000000300ull, 0x0000800000228002ull,
		},
		0x247c,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks >> 2) & (previous_masks >> 6) & 1) << 1;
		}
	});
}

void namcos10_memn_state::ns10_panikuru(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 3);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000000130ull, 0x0000000018802004ull, 0x0000b04a04781081ull, 0x0000000006802000ull,
			0x0000000000000c02ull, 0x0000000042040020ull, 0x0000000000000003ull, 0x0000b040c00c0940ull,
			0x0000000000000009ull, 0x00001020a0010400ull, 0x0000000010204104ull, 0x000000005a840004ull,
			0x0000000000000290ull, 0x0000000010204380ull, 0x0000000000001041ull, 0x0000000000006100ull,
		}, {
			0x0000000000000120ull, 0x0000000028002004ull, 0x0000010800480001ull, 0x0000000006040100ull,
			0x0000000000000c00ull, 0x0000000042000820ull, 0x0000000000001003ull, 0x0000910212040100ull,
			0x0000000000008009ull, 0x0000900021018000ull, 0x0000000090004104ull, 0x000000006a004004ull,
			0x0000000000000214ull, 0x0000000090004190ull, 0x0000000000001040ull, 0x0000000000006008ull,
		},
		0x7c29,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			return (1 & ((gf2_reduce(0x0000000088300281ull & previous_cipherwords) ^ gf2_reduce(0x0000000004600281ull & previous_plainwords))) & ((gf2_reduce(0x0000a13140090000ull & previous_cipherwords) ^ gf2_reduce(0x0000806240090000ull & previous_plainwords)))) << 2;
		},
		[] (int iv) -> uint64_t {
			constexpr uint64_t values[16]{
				0x0000, 0x020e, 0x0412, 0x0411, 0x0204, 0x001b, 0x0009, 0x0207,
				0x2040, 0x2440, 0x0012, 0x0001, 0x0614, 0x000a, 0x0214, 0x041b,
			};
			return values[iv];
		}
	});
}

void namcos10_memn_state::ns10_ptblank3(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 2);

	// NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, logic);
}

void namcos10_memn_state::ns10_puzzball(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_mgexio(config);
	namcos10_nand_k9f2808u0b(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000000288ull, 0x0000000050420100ull, 0x00000000000010a0ull, 0x0000000004020015ull,
			0x0000000020400400ull, 0x0000000004020010ull, 0x000008a880054080ull, 0x0000000000000140ull,
			0x0000000000006002ull, 0x0000000000000830ull, 0x000000000000000cull, 0x0000000000000821ull,
			0x0000000000000002ull, 0x0000008100c08204ull, 0x0000008430c20102ull, 0x000000000000c000ull,
		}, {
			0x0000000000000380ull, 0x0000000070002000ull, 0x0000000000000020ull, 0x0000000004120091ull,
			0x0000000028000410ull, 0x0000000004120200ull, 0x0000082a00244080ull, 0x0000000000000141ull,
			0x0000000000006040ull, 0x0000000000000030ull, 0x0000000000000008ull, 0x0000000000000c01ull,
			0x0000000000008402ull, 0x0000021d0040c004ull, 0x0000008c40880123ull, 0x000000000000c000ull,
		},
		0x31d6,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks>>29) & (gf2_reduce(previous_cipherwords & 0x180882002000ULL) ^ gf2_reduce(previous_plainwords & 0x80ac0002000ULL))) << 4;
		}
	});
}

void namcos10_memn_state::ns10_sekaikh(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_mgexio(config);
	namcos10_nand_k9f2808u0b(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000000510ull, 0x0000000000004000ull, 0x0000000000000406ull, 0x0000000000000400ull,
			0x00000000c0410890ull, 0x0000000041610800ull, 0x0000000000002008ull, 0x000000002051a000ull,
			0x0000000008800020ull, 0x0000000008800014ull, 0x0000000000000800ull, 0x0000000000004001ull,
			0x0000000041610884ull, 0x0000000042308018ull, 0x0000000000000888ull, 0x0000000070014820ull,
		},
		{
			0x0000000000000100ull, 0x0000000000005040ull, 0x0000000000000402ull, 0x0000000000000608ull,
			0x0000000084020892ull, 0x0000000001418a00ull, 0x0000000000000008ull, 0x0000000000d0a400ull,
			0x0000000000801020ull, 0x0000000000800004ull, 0x0000000000000020ull, 0x0000000000000001ull,
			0x0000000001410806ull, 0x00000000042c8019ull, 0x0000000000000880ull, 0x00000000b0010920ull,
		},
		0x3aa8,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks >> 0) & (previous_masks >> 3) & 1) << 10;
		}
	});
}

void namcos10_memn_state::ns10_startrgn(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_nand_k9f2808u0b(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x00003e4bfe92c6a9ull, 0x000000000000010cull, 0x00003e4b7bd6c4aaull, 0x0000b1a904b8fab8ull,
			0x0000000000000080ull, 0x0000000000008c00ull, 0x0000b1a9b2f0b4cdull, 0x000000006c100828ull,
			0x000000006c100838ull, 0x0000b1a9d3913fcdull, 0x000000006161aa00ull, 0x0000000000006040ull,
			0x0000000000000420ull, 0x0000000000001801ull, 0x00003e4b7bd6deabull, 0x0000000000000105ull,
		}, {
			0x000012021f00c6a8ull, 0x0000000000000008ull, 0x000012020b1046aaull, 0x000012001502fea8ull,
			0x0000000000002000ull, 0x0000000000008800ull, 0x000012001e02b4cdull, 0x000000002c0008aaull,
			0x000000002c00083aull, 0x000012003f027ecdull, 0x0000000021008a00ull, 0x0000000000002040ull,
			0x0000000000000428ull, 0x0000000000001001ull, 0x000012020b10ceabull, 0x0000000000000144ull,
		},
		0x8c46,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return ((previous_masks >> 12) & (previous_masks >> 14) & 1) << 4;
		}
	});
}

void namcos10_memn_state::ns10_sugorotic(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_mgexio(config);
	namcos10_nand_k9f2808u0b(config, 2);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000061402200010ull, 0x0000000000b2a150ull, 0x0000000080280021ull, 0x0000000000000880ull,
			0x0000061410010004ull, 0x0000000000000800ull, 0x0000000000000141ull, 0x0000000041002000ull,
			0x0000000000000084ull, 0x0000000000020401ull, 0x0000000041120100ull, 0x0000000000020480ull,
			0x0000000000b21110ull, 0x0000000000128800ull, 0x0000000000003000ull, 0x0000061410014020ull,
		}, {
			0x0000223011000034ull, 0x0000000040228150ull, 0x0000000000280101ull, 0x0000000000000880ull,
			0x0000223010010004ull, 0x0000000000000848ull, 0x0000000000000301ull, 0x0000000041002001ull,
			0x0000000000000084ull, 0x0000000000100408ull, 0x0000000041160800ull, 0x0000000000100000ull,
			0x0000000040228110ull, 0x000000000016c000ull, 0x0000000000003002ull, 0x0000223010010020ull,
		},
		0x9006,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks >> 25) & (previous_masks >> 22)) * 0xa00;
		}
	});
}

void namcos10_memn_state::ns10_taiko2(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exio(config);
	namcos10_nand_k9f2808u0b(config, 3);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000004011401ull, 0x000000008c208806ull, 0x000000008c200406ull, 0x0000004a08002208ull,
			0x0000000030800000ull, 0x0000000000000540ull, 0x000000002c01011eull, 0x00000000ad4805c6ull,
			0x0000000030000004ull, 0x0000000048100080ull, 0x0000003461204408ull, 0x00004510422000c0ull,
			0x0000080080210002ull, 0x00000000ae004004ull, 0x000000008c100016ull, 0x0000000082010009ull,
		}, {
			0x00000000a4019401ull, 0x000000005e600a06ull, 0x000000005e604406ull, 0x0000410308002208ull,
			0x0000000030020400ull, 0x0000000000000150ull, 0x00000000ec09010eull, 0x000000004ecc04c2ull,
			0x0000000030001004ull, 0x0000000048304180ull, 0x0000141249886488ull, 0x000000454a000081ull,
			0x0000184022210002ull, 0x000000006c00400cull, 0x000000004e140816ull, 0x0000000080090008ull,
		},
		0x0000,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks >> 25) & (previous_masks >> 31)) << 4;
		}
	});
}

void namcos10_memn_state::ns10_taiko3(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exio(config);
	namcos10_nand_k9f2808u0b(config, 3);

	// NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, logic);
}

void namcos10_memn_state::ns10_taiko4(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exio(config);
	namcos10_nand_k9f2808u0b(config, 3);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000042c82001ull, 0x0000000000000200ull, 0x000000000910405cull, 0x0000000000000c00ull,
			0x000000000f401100ull, 0x000000000910002aull, 0x00006280110a0440ull, 0x0000000040680044ull,
			0x00000000b408b000ull, 0x0000000000108860ull, 0x0000000009100008ull, 0x0000a024b0000000ull,
			0x000000000a401a40ull, 0x0000000000001400ull, 0x0000000000100162ull, 0x0000000020402081ull,
		}, {
			0x0000000082102019ull, 0x0000000000000400ull, 0x0000000005104050ull, 0x0000000000000c00ull,
			0x0000000002001d00ull, 0x000000000510001aull, 0x0000129910a10443ull, 0x0000000040040044ull,
			0x0000000034088000ull, 0x0000000000259060ull, 0x0000000005100188ull, 0x0000204480000000ull,
			0x000000000a801a80ull, 0x0000000000001400ull, 0x0000000000250262ull, 0x0000000020404081ull,
		},
		0x0000,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks >> 2) & (previous_masks>>14)) << 1;
		}
	});
}

void namcos10_memn_state::ns10_taiko5(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exio(config);
	namcos10_nand_k9f2808u0b(config, 3);

	// NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, logic);
}

void namcos10_memn_state::ns10_taiko6(machine_config &config)
{
	namcos10_memn_base(config);
	namcos10_exio(config);
	namcos10_nand_k9f2808u0b(config, 3);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x00000000000000a2ull, 0x00000000000000c8ull, 0x0000909054044a04ull, 0x00009180101a2200ull,
			0x0000000050002840ull, 0x0000000000004005ull, 0x000000000e220900ull, 0x0000000086000004ull,
			0x0000000014220d22ull, 0x0000000000000110ull, 0x0000000000000214ull, 0x0000000000001000ull,
			0x0000000031088890ull, 0x0000000000000802ull, 0x00000000f025e800ull, 0x0000000000000010ull,
		}, {
			0x00000000000000a4ull, 0x0000000000000148ull, 0x00002090a2078a04ull, 0x0000218020012218ull,
			0x0000000050c22840ull, 0x0000000000004009ull, 0x00000000164109c0ull, 0x000000008a000004ull,
			0x0000000014410d42ull, 0x0000000000000110ull, 0x0000000000000414ull, 0x0000000000001003ull,
			0x00000000320b0890ull, 0x0000000000000802ull, 0x00000000902a8800ull, 0x0000000000000020ull,
		},
		0x0000,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks>>13) & (gf2_reduce(previous_cipherwords & 0x86000000ULL) ^ gf2_reduce(previous_plainwords & 0x8a000000ULL))) << 15;
		}
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////
// MEM(P3)

void namcos10_memp3_state::firmware_write_w(uint16_t data)
{
	if (data == 1) {
		m_memp3_mcu->reset();
	}
}

void namcos10_memp3_state::ram_bank_w(uint16_t data)
{
	m_mcu_ram_bank = data;
}

uint16_t namcos10_memp3_state::unk_status1_r()
{
	// Can't upload CPU program unless this is 1
	return 1;
}

uint16_t namcos10_memp3_state::ram_r(offs_t offset)
{
	return m_mcu_ram[m_mcu_ram_bank * 0x10000 + offset];
}

void namcos10_memp3_state::ram_w(offs_t offset, uint16_t data)
{
	m_mcu_ram[m_mcu_ram_bank * 0x10000 + offset] = data;
}

uint16_t namcos10_memp3_state::unk_status2_r()
{
	// Some kind of status flag.
	// Game code loops until this is non-zero before writing to data to f30000c/320000.
	// Possibly related to MP3 decoder?
	return 1;
}

void namcos10_memp3_state::mcu_int5_w(uint16_t data)
{
	// MP3 decoder commands will only be processed when INT5 is triggered.
	// 0 gets written to this register (only?) after the MP3 decoder commands are
	// written into the MCU's memory so I believe it's responsible for making the
	// INT5 get triggered.
	m_memp3_mcu->set_input_line(TLCS900_INT5, ASSERT_LINE);
}

uint16_t namcos10_memp3_state::io_analog_r(offs_t offset)
{
	return m_p3_analog[offset].read_safe(0);
}

void namcos10_memp3_state::namcos10_memp3_map_inner(address_map &map)
{
	map(0xf300000, 0xf300001).w(FUNC(namcos10_memp3_state::firmware_write_w));
	// 1f300004 unk
	map(0xf300006, 0xf300007).rw(FUNC(namcos10_memp3_state::unk_status2_r), FUNC(namcos10_memp3_state::mcu_int5_w));
	map(0xf30000c, 0xf30000d).w(FUNC(namcos10_memp3_state::ram_bank_w));
	map(0xf30000e, 0xf30000f).r(FUNC(namcos10_memp3_state::unk_status1_r));
	map(0xf320000, 0xf33ffff).rw(FUNC(namcos10_memp3_state::ram_r), FUNC(namcos10_memp3_state::ram_w));
	map(0xf33fff0, 0xf33fff7).r(FUNC(namcos10_memp3_state::io_analog_r));
}

void namcos10_memp3_state::namcos10_memp3_map(address_map &map)
{
	namcos10_memn_map(map);

	map(0x10000000, 0x1fffffff).m(FUNC(namcos10_memp3_state::namcos10_memp3_map_inner));
	map(0x90000000, 0x9fffffff).m(FUNC(namcos10_memp3_state::namcos10_memp3_map_inner));
	map(0xb0000000, 0xbfffffff).m(FUNC(namcos10_memp3_state::namcos10_memp3_map_inner));
}

void namcos10_memp3_state::mcu_map(address_map &map)
{
	map(0x000000, 0x7fffff).ram().mirror(0x800000).share(m_mcu_ram);
}


void namcos10_memp3_state::namcos10_memp3_base(machine_config &config)
{
	namcos10_base(config);

	m_maincpu->subdevice<psxdma_device>("dma")->install_read_handler(5, psxdma_device::read_delegate(&namcos10_memp3_state::pio_dma_read, this));
	m_maincpu->subdevice<psxdma_device>("dma")->install_write_handler(5, psxdma_device::write_delegate(&namcos10_memp3_state::pio_dma_read, this));
	m_maincpu->set_addrmap(AS_PROGRAM, &namcos10_memp3_state::namcos10_memp3_map);

	TMP95C061(config, m_memp3_mcu, XTAL(16'934'400));
	m_memp3_mcu->set_addrmap(AS_PROGRAM, &namcos10_memp3_state::mcu_map);
	// Port 7 is used for communicating with the MP3 decoder chip

	// LC82310 16.9344MHz
}

void namcos10_memp3_state::machine_start()
{
	namcos10_memn_state::machine_start();

	save_item(NAME(m_mcu_ram_bank));
}

void namcos10_memp3_state::machine_reset()
{
	namcos10_memn_state::machine_reset();

	m_mcu_ram_bank = 0;

	m_memp3_mcu->suspend(SUSPEND_REASON_HALT, 1);
}

void namcos10_memp3_state::init_g13jnr()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xd, 0xc, 0xf, 0x9, 0xb, 0x8, 0xa, 0x6, 0x7, 0x4, 0x5, 0x1, 0x3, 0x0, 0x2); };
	memn_driver_init();
}

void namcos10_memp3_state::init_nicetsuk()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xc, 0xf, 0xe, 0xd, 0xa, 0x8, 0xb, 0x9, 0x5, 0x4, 0x6, 0x7, 0x2, 0x3, 0x0, 0x1); };
	memn_driver_init();
}

void namcos10_memp3_state::init_squizchs()
{
	m_unscrambler = [] (uint16_t data) { return bitswap<16>(data, 0xe, 0xc, 0xf, 0xd, 0xa, 0x8, 0xb, 0x9, 0x6, 0x4, 0x7, 0x5, 0x0, 0x1, 0x3, 0x2); };
	memn_driver_init();
}

void namcos10_memp3_state::ns10_g13jnr(machine_config &config)
{
	namcos10_memp3_base(config);
	namcos10_nand_k9f2808u0b(config, 6);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x00005600001c0582ull, 0x0000000000004024ull, 0x0000010212403000ull, 0x0000000000008404ull,
			0x0000005121060021ull, 0x0000010212402001ull, 0x0000000001024000ull, 0x0000000000000840ull,
			0x00000981c2100148ull, 0x0000000020108400ull, 0x0000000008110134ull, 0x0000000000000003ull,
			0x00004c9801080102ull, 0x0000000040860083ull, 0x0000000000000001ull, 0x0000000000000288ull,
		}, {
			0x0000441800340584ull, 0x0000000000004028ull, 0x0000010212800000ull, 0x0000000000000404ull,
			0x0000485022000041ull, 0x0000010212804001ull, 0x0000000001028000ull, 0x0000000000000841ull,
			0x00000e0104080150ull, 0x0000000023208400ull, 0x0000000040110104ull, 0x0000000000000003ull,
			0x0000470001100102ull, 0x00000000408c0083ull, 0x0000000000000002ull, 0x0000000000000308ull,
		},
		0x9546,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks >> 6) & (previous_masks >> 10)) << 14;
		}
	});
}

void namcos10_memp3_state::ns10_nicetsuk(machine_config &config)
{
	namcos10_memp3_base(config);
	namcos10_nand_k9f2808u0b(config, 8);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000000000022ull, 0x0000000000008082ull, 0x0000808400d10000ull, 0x0000000000000088ull,
			0x0000000000001040ull, 0x0000000000001600ull, 0x0000000714400404ull, 0x0000000021c40800ull,
			0x0000000000004018ull, 0x000000002c450200ull, 0x0000000000000c01ull, 0x0000000000000180ull,
			0x000000000c000414ull, 0x0000000000000110ull, 0x0000000023006000ull, 0x0000000000000068ull,
		}, {
			0x0000000000000026ull, 0x0000000000008802ull, 0x0000081100f22000ull, 0x000000000000008aull,
			0x0000000000001400ull, 0x0000000000000600ull, 0x00000012d8400404ull, 0x0000000021804800ull,
			0x0000000000004090ull, 0x0000000068012000ull, 0x0000000000004801ull, 0x0000000000000180ull,
			0x00000000c8000504ull, 0x0000000000000111ull, 0x0000000032086008ull, 0x0000000000000248ull,
		},
		0x9f6b,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks>>12) & (gf2_reduce(previous_cipherwords & 0x808400410000ULL) ^ gf2_reduce(previous_plainwords & 0x81100630000ULL))) << 4;
		}
	});
}

void namcos10_memp3_state::ns10_squizchs(machine_config &config)
{
	namcos10_memp3_base(config);
	namcos10_nand_k9f2808u0b(config, 14);

	NS10_TYPE2_DECRYPTER(config, m_decrypter, 0, ns10_type2_decrypter_device::ns10_crypto_logic{
		{
			0x0000000080203001ull, 0x000000008e001402ull, 0x0000000000000005ull, 0x0000000002030840ull,
			0x0000000000008420ull, 0x0000000000000088ull, 0x000000001c020442ull, 0x00000080c0a01041ull,
			0x00000000a1800d08ull, 0x0000000000002240ull, 0x0000020385a08010ull, 0x0000008054080024ull,
			0x0000000080209020ull, 0x00001408c4200004ull, 0x0000000000000010ull, 0x0000000000004108ull,
		}, {
			0x0000000080203001ull, 0x0000000008001404ull, 0x0000000000000005ull, 0x0000000002001040ull,
			0x0000000000000420ull, 0x0000000000000089ull, 0x0000000020020842ull, 0x00000100c1201081ull,
			0x00000000dc800b08ull, 0x0000000000004240ull, 0x0000020006a08010ull, 0x0000010054100044ull,
			0x0000000080209020ull, 0x0000141044200008ull, 0x0000000000000020ull, 0x0000000000008108ull,
		},
		0x0000,
		[] (uint64_t previous_cipherwords, uint64_t previous_plainwords) -> uint16_t {
			uint64_t previous_masks = previous_cipherwords ^ previous_plainwords;
			return (1 & (previous_masks>>12) & (gf2_reduce(previous_cipherwords & 0x140840000000ULL) ^ gf2_reduce(previous_plainwords & 0x141040000000ULL))) << 14;
		},
		[] (int iv) -> uint64_t {
			constexpr int UNKNOWN = 16;
			constexpr uint64_t values[16]{
				UNKNOWN, 0x9000,  0x9101,  UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
				UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
			};
			return values[iv];
		}
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////

static INPUT_PORTS_START( namcos10 )
	PORT_START("SYSTEM")
	PORT_BIT( 0xfb00, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPUNKNOWN_DIPLOC( 0x0001, IP_ACTIVE_LOW, "SW1:8" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0002, IP_ACTIVE_LOW, "SW1:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0004, IP_ACTIVE_LOW, "SW1:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0008, IP_ACTIVE_LOW, "SW1:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0010, IP_ACTIVE_LOW, "SW1:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0020, IP_ACTIVE_LOW, "SW1:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0040, IP_ACTIVE_LOW, "SW1:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x0080, IP_ACTIVE_LOW, "SW1:1" )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN ) // JVS sense?

	PORT_START("IN1")
	PORT_BIT( 0x0f110000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x00000040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x00000080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x00020000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT( 0x00040000, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(1)
	PORT_BIT( 0x00080000, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_PLAYER(1)

	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x00000400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x00000800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x00001000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x00002000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x00004000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x00008000, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x00200000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x00400000, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(2)
	PORT_BIT( 0x00800000, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_PLAYER(2)

	PORT_BIT( 0x10000000, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x20000000, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x40000000, IP_ACTIVE_LOW, IPT_SERVICE2 ) PORT_TOGGLE // Test SW, almost all games expect this to be a slide type and the medal games explicitly say "slide on and off test to restart"
	PORT_BIT( 0x80000000, IP_ACTIVE_LOW, IPT_SERVICE1 )

	PORT_START("EXIO_IN1")
	PORT_BIT( 0xffffffff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("EXIO_IN2")
	PORT_BIT( 0xffffffff, IP_ACTIVE_LOW, IPT_UNUSED )

INPUT_PORTS_END

static INPUT_PORTS_START( mrdrilr2 )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x00000400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x00000800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(2)

INPUT_PORTS_END

static INPUT_PORTS_START( gamshara )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0fff0000, IP_ACTIVE_LOW, IPT_UNUSED ) // Disable P1 and P2 4-6 buttons

	PORT_MODIFY("SYSTEM")
	// NOTE: this uses 7 bits, only the exact value of 0x6e gives World region, the rest give Japan
	//       currently we only allow for values 0x7f and 0x6e to be selected
	//       it is unknown if the later 'gamshara' set will work in the same way or default to English
	PORT_DIPNAME( 0x7f, 0x7f, DEF_STR( Region ) ) PORT_DIPLOCATION("SW1:8,7,6,5,4,3,2")
	PORT_DIPSETTING(0x7f, DEF_STR( Japan ) ) // JPN
	PORT_DIPSETTING(0x6e, DEF_STR( World ) ) // ETC
INPUT_PORTS_END

static INPUT_PORTS_START( startrgn )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0fffff6f, IP_ACTIVE_LOW, IPT_UNUSED ) // This game only uses 1 button and start, no P2 at all

INPUT_PORTS_END

static INPUT_PORTS_START( konotako )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x1fff4040, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SYSTEM")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Region ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(0x00, "Export (Cristaltec license)")
	PORT_DIPSETTING(0x01, DEF_STR( Japan ) )
INPUT_PORTS_END

static INPUT_PORTS_START( nflclsfb )
	PORT_INCLUDE(namcos10)

	// TODO: Trackball (EXIO, requires serial emulation in TMP95C061)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0fff1c00, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Right Side Choose L")
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Left Side Choose L")
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Select Down")
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Select Up")
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON7 ) PORT_NAME("Enter")
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_START1 ) PORT_PLAYER(1) PORT_NAME("1P Start")
	PORT_BIT( 0x00000040, IP_ACTIVE_LOW, IPT_START1 ) PORT_PLAYER(2) PORT_NAME("2P Start")
	PORT_BIT( 0x00000080, IP_ACTIVE_LOW, IPT_BUTTON8 ) PORT_NAME("Left Side Decide")
	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Right Side Choose R")
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Left Side Choose R")
	PORT_BIT( 0x00002000, IP_ACTIVE_LOW, IPT_START1 ) PORT_PLAYER(3) PORT_NAME("3P Start")
	PORT_BIT( 0x00004000, IP_ACTIVE_LOW, IPT_START1 ) PORT_PLAYER(4) PORT_NAME("4P Start")
	PORT_BIT( 0x00008000, IP_ACTIVE_LOW, IPT_BUTTON9 ) PORT_NAME("Right Side Decide")

	PORT_MODIFY("SYSTEM")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Service_Mode ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING( 0x01, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x00, DEF_STR( On ) )

	PORT_MODIFY("EXIO_IN1")
	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("1P PASS 1")
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("2P PASS 1")
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3) PORT_NAME("3P PASS 1")
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4) PORT_NAME("4P PASS 1")

	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("1P PASS 2")
	PORT_BIT( 0x00000040, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("2P PASS 2")
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3) PORT_NAME("3P PASS 2")
	PORT_BIT( 0x00000080, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4) PORT_NAME("4P PASS 2")

	PORT_MODIFY("EXIO_IN2")
	PORT_BIT( 0xffffffff, IP_ACTIVE_LOW, IPT_UNUSED )

INPUT_PORTS_END

static INPUT_PORTS_START( gahaha )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0fff7c60, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("Select Down")
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("Select Up")
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Enter")

	PORT_MODIFY("SYSTEM")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Service_Mode ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING( 0x01, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x00, DEF_STR( On ) )

	PORT_START("EXIO_ANALOG1")
	PORT_BIT( 0x3ff, 0x000, IPT_AD_STICK_X ) PORT_NAME("P1 Left X") PORT_PLAYER(1) PORT_MINMAX(0x000,0x3ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(50) PORT_CENTERDELTA(20)

	PORT_START("EXIO_ANALOG2")
	PORT_BIT( 0x3ff, 0x000, IPT_AD_STICK_Y ) PORT_NAME("P1 Left Y") PORT_PLAYER(1) PORT_MINMAX(0x000,0x3ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(50) PORT_CENTERDELTA(20) PORT_REVERSE

	PORT_START("EXIO_ANALOG3")
	PORT_BIT( 0x3ff, 0x000, IPT_AD_STICK_X ) PORT_NAME("P1 Right X") PORT_PLAYER(1) PORT_MINMAX(0x000,0x3ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(50) PORT_CENTERDELTA(20) PORT_REVERSE

	PORT_START("EXIO_ANALOG4")
	PORT_BIT( 0x3ff, 0x000, IPT_AD_STICK_Y ) PORT_NAME("P1 Right Y") PORT_PLAYER(1) PORT_MINMAX(0x000,0x3ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(50) PORT_CENTERDELTA(20)

	PORT_START("EXIO_ANALOG5")
	PORT_BIT( 0x3ff, 0x000, IPT_AD_STICK_X ) PORT_NAME("P2 Left X") PORT_PLAYER(2) PORT_MINMAX(0x000,0x3ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(50) PORT_CENTERDELTA(20)

	PORT_START("EXIO_ANALOG6")
	PORT_BIT( 0x3ff, 0x000, IPT_AD_STICK_Y ) PORT_NAME("P2 Left Y") PORT_PLAYER(2) PORT_MINMAX(0x000,0x3ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(50) PORT_CENTERDELTA(20) PORT_REVERSE

	PORT_START("EXIO_ANALOG7")
	PORT_BIT( 0x3ff, 0x000, IPT_AD_STICK_X ) PORT_NAME("P2 Right X") PORT_PLAYER(2) PORT_MINMAX(0x000,0x3ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(50) PORT_CENTERDELTA(20) PORT_REVERSE

	PORT_START("EXIO_ANALOG8")
	PORT_BIT( 0x3ff, 0x000, IPT_AD_STICK_Y ) PORT_NAME("P2 Right Y") PORT_PLAYER(2) PORT_MINMAX(0x000,0x3ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(50) PORT_CENTERDELTA(20)

INPUT_PORTS_END

static INPUT_PORTS_START( gjspace )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0fff0000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SYSTEM")
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Service_Mode ) ) PORT_DIPLOCATION("SW1:1")
	PORT_DIPSETTING( 0x80, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x00, DEF_STR( On ) )

INPUT_PORTS_END

static INPUT_PORTS_START( g13jnr )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x071f0043, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("Down Select")
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("Up Select")
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Enter")
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Trigger")

	// If the player is 10cm or closer then this will be on.
	// For playability, make it so that the player has to explicity toggle the scope
	// to be off to simulate stepping away from the scope instead of always having to
	// hold the scope sensor button
	PORT_BIT( 0x08000000, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Scope Sensor") PORT_TOGGLE

	PORT_MODIFY("SYSTEM")
	// No idea what DIPSW 1 (0x80) is doing but it causes the game to freeze
	PORT_DIPNAME( 0x01, 0x01, "Show Crosshair" ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING( 0x01, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x00, DEF_STR( On ) )

	PORT_START("P3_ANALOG1")
	PORT_BIT( 0xffff, 0x7fff, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, -1.0, 0.0, 0) PORT_MINMAX(0x0000,0xffff) PORT_SENSITIVITY(100) PORT_KEYDELTA(100) PORT_PLAYER(1) PORT_REVERSE

	PORT_START("P3_ANALOG2")
	PORT_BIT( 0xffff, 0x7fff, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_MINMAX(0x0000,0xffff) PORT_SENSITIVITY(100) PORT_KEYDELTA(100) PORT_PLAYER(1)

INPUT_PORTS_END

static INPUT_PORTS_START( mgexio_medal )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0fffffe3, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Select Down")
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Select Up")
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Enter")

	PORT_MODIFY("SYSTEM")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Service_Mode ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING( 0x01, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x00, DEF_STR( On ) )

	PORT_START("MGEXIO_SENSOR")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Check Sensor(2)")
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Check Sensor(1)")
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Check Sensor(4)")
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Check Sensor(3)")
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Check Sensor(6)")
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Check Sensor(5)")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON7 ) PORT_NAME("Check Sensor(7)")
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_TILT )

	PORT_START("MGEXIO_COIN")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_NAME("Coin Sensor(L)") PORT_CHANGED_MEMBER(DEVICE_SELF, namcos10_state, mgexio_coin_start, 0)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_NAME("Coin Sensor(R)") PORT_CHANGED_MEMBER(DEVICE_SELF, namcos10_state, mgexio_coin_start, 1)

INPUT_PORTS_END

static INPUT_PORTS_START( taiko )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0fffffe3, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Select Down")
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Select Up")
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Enter")

	// Drums use PKS1-4A1 Piezoelectric ceramic sensors for input values
	PORT_START("EXIO_ANALOG1") // P1R Fuchi
	PORT_BIT( 0x3ff, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("P1R Rim") PORT_PLAYER(1)

	PORT_START("EXIO_ANALOG2") // P1R Men
	PORT_BIT( 0x3ff, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("P1R Surface") PORT_PLAYER(1)

	PORT_START("EXIO_ANALOG3") // P1L Men
	PORT_BIT( 0x3ff, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("P1L Surface") PORT_PLAYER(1)

	PORT_START("EXIO_ANALOG4") // P1L Fuchi
	PORT_BIT( 0x3ff, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("P1L Rim") PORT_PLAYER(1)

	PORT_START("EXIO_ANALOG5") // P2R Fuchi
	PORT_BIT( 0x3ff, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("P2R Rim") PORT_PLAYER(2)

	PORT_START("EXIO_ANALOG6") // P2R Men
	PORT_BIT( 0x3ff, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("P2R Surface") PORT_PLAYER(2)

	PORT_START("EXIO_ANALOG7") // P2L Men
	PORT_BIT( 0x3ff, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("P2L Surface") PORT_PLAYER(2)

	PORT_START("EXIO_ANALOG8") // P2L Fuchi
	PORT_BIT( 0x3ff, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("P2L Rim") PORT_PLAYER(2)

INPUT_PORTS_END

static INPUT_PORTS_START( nicetsuk )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0ff9ef40, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("Up") // Used in-game
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("Down")
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Down Select") // Used in operator menu
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_BUTTON7 ) PORT_NAME("Up Select")
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Enter")
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Pedal")
	PORT_BIT( 0x00000080, IP_ACTIVE_LOW, IPT_START1 ) PORT_NAME("Choose")
	PORT_BIT( 0x00020000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Forehead/Odeko")
	PORT_BIT( 0x00040000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Back of Head/Toubu")
	PORT_BIT( 0x00001000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Chest/Mune")

INPUT_PORTS_END

static INPUT_PORTS_START( squizchs )
	PORT_INCLUDE(namcos10)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0fff7070, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_BIT( 0x00000001, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)

	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x00000400, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x00000800, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)

INPUT_PORTS_END

// MEM(M)
ROM_START( mrdrilr2 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x800000, "nand", 0 )
	ROM_LOAD( "dr22vera.1a", 0x000000, 0x800000, CRC(140eafb6) SHA1(b68f901ff18d052bc21bb548159dcbc7dd731e5c) )

	ROM_REGION( 0x2000000, "data", 0 )
	ROM_LOAD( "dr21ma1.1d", 0x0000000, 0x1000000, CRC(26dc6f55) SHA1(a9cedf547fa7a4d5850b9b3b867d46e577a035e0) )
	ROM_LOAD( "dr21ma2.2d", 0x1000000, 0x1000000, CRC(702556ff) SHA1(c95defd5fd6a9b406fc8d8f28ecfab732ef1ff42) )
ROM_END

ROM_START( mrdrilr2j )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x800000, "nand", 0 )
	ROM_LOAD( "dr21vera.1a", 0x000000, 0x800000, CRC(03e0241e) SHA1(de26054657cc129dd581e2672d5c81d26daeb5e6) )

	ROM_REGION( 0x2000000, "data", 0 )
	ROM_LOAD( "dr21ma1.1d", 0x0000000, 0x1000000, CRC(26dc6f55) SHA1(a9cedf547fa7a4d5850b9b3b867d46e577a035e0) )
	ROM_LOAD( "dr21ma2.2d", 0x1000000, 0x1000000, CRC(702556ff) SHA1(c95defd5fd6a9b406fc8d8f28ecfab732ef1ff42) )
ROM_END

ROM_START( mrdrilr2u )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x800000, "nand", 0 )
	ROM_LOAD( "dr23vera.1a", 0x000000, 0x800000, CRC(5f32b6fb) SHA1(ecb94d8317946f278a2cb0bc55abea2b89fa2e71) )

	ROM_REGION( 0x2000000, "data", 0 )
	ROM_LOAD( "dr21ma1.1d", 0x0000000, 0x1000000, CRC(26dc6f55) SHA1(a9cedf547fa7a4d5850b9b3b867d46e577a035e0) )
	ROM_LOAD( "dr21ma2.2d", 0x1000000, 0x1000000, CRC(702556ff) SHA1(c95defd5fd6a9b406fc8d8f28ecfab732ef1ff42) )
ROM_END


// MEM(N)
ROM_START( ballpom )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "k9f2808u0c.8e", 0x0000000, 0x1080000, CRC(f31cec39) SHA1(509209f7bec6384fa1e7f2d32110d1ceaed83826) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "k9f2808u0c.8d", 0x0000000, 0x1080000, CRC(66041249) SHA1(16e21f5874aa53306921664073e79b814c954cd8) )
ROM_END

ROM_START( chocovdr )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "0.8e", 0x0000000, 0x1080000, CRC(330d86d3) SHA1(0d7ea7593510531ef7350dd7ee957208681708da) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "1.8d", 0x0000000, 0x1080000, CRC(4aecd6fc) SHA1(31fe8f36e38020a92f15c44fd1a4b486636b40ce) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "2.7e", 0x0000000, 0x1080000, CRC(ac212e5a) SHA1(f2d2e65a3249992730b8b90561b9bcf5eaaafb88) )

	ROM_REGION32_LE( 0x1080000, "nand3", 0 )
	ROM_LOAD( "3.7d", 0x0000000, 0x1080000, CRC(907d3d15) SHA1(20519d1f8bd9c6bc45b65e2d7444d588e922611d) )

	ROM_REGION32_LE( 0x1080000, "nand4", 0 )
	ROM_LOAD( "4.6e", 0x0000000, 0x1080000, CRC(1ed957dd) SHA1(bc8ce9f249fe496c130c6fe67b2260c4d0734ab9) )
ROM_END

ROM_START( gahaha )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "gid2vera_0.8e", 0x0000000, 0x1080000, CRC(d7888ca3) SHA1(723f7d30a0d109c0e664bccfddb00fdf2e551835) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "gid2vera_1.8d", 0x0000000, 0x1080000, CRC(102932a7) SHA1(01133986c64d7290019706e4373006a5af04c105) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "gid2vera_2.7e", 0x0000000, 0x1080000, CRC(df8ec017) SHA1(59f9cdad77d452af25c35913e1daca6b561074c2) )
ROM_END

ROM_START( gahaha2 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "gis1vera_0.8e", 0x0000000, 0x1080000, CRC(d1e4a8b8) SHA1(59ee1b98d94dcabc48ba368fd56db13a248dcf24) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "gis1vera_1.8d", 0x0000000, 0x1080000, CRC(eeb680ac) SHA1(9737cd9db600ec10bea53a6473cfff57cb1c1aec) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "gis1vera_2.7e", 0x0000000, 0x1080000, CRC(d22c9ee1) SHA1(e0a7da2843e0a43eacb2bd8c8f088c6b7433fc1a) )
ROM_END

ROM_START( gamshara )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "10021a_e.8e", 0x0000000, 0x1080000, BAD_DUMP CRC(684ab324) SHA1(95c2e0a04c4f33039535fc451c5559d239b8fbc6) )

	// Needs the proper matching ROM from the same board
	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "10021a.8d", 0x0000000, 0x1080000, BAD_DUMP CRC(73669ff7) SHA1(eb8bbf931f1f8a049208d081d040512a3ffa9c00) )
ROM_END

ROM_START( gamsharaj )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "10021a.8e", 0x0000000, 0x1080000, CRC(6c0361fc) SHA1(7debf1f2e6bed31d59fb224a78a17a94fc573785) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "10021a.8d", 0x0000000, 0x1080000, CRC(73669ff7) SHA1(eb8bbf931f1f8a049208d081d040512a3ffa9c00) )
ROM_END

ROM_START( gegemdb )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	// gym1.8e appears to be interleaved, none of the other larger NAND chip dumps look like this. Highly suspected to be a bad dump.
	ROM_REGION32_LE( 0x2100000, "nand0", 0 )
	ROM_LOAD( "gym1.8e", 0x0000000, 0x2100000, BAD_DUMP CRC(ea740351) SHA1(4dc7ce256a2d60be512d04a992b2103602bcfaa9) ) // K9F5608U0D, double sized wrt to the other games and PCB silkscreen

	ROM_REGION32_LE( 0x2100000, "nand1", 0 )
	ROM_LOAD( "gym1.8d", 0x0000000, 0x2100000, BAD_DUMP CRC(0145a8c1) SHA1(a32dd944d022df14450bbcb01b4d1712683c0680) ) // K9F5608U0D, double sized wrt to the other games and PCB silkscreen

	ROM_REGION( 0x8000, "nvram", 0 )
	ROM_LOAD( "nvram.bin", 0x0000, 0x8000, CRC(c0c87c71) SHA1(263f7f3df772644bcf973413d3fac9ae305fda6c) )
ROM_END

ROM_START( gjspace )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "10011.8e", 0x0000000, 0x1080000, CRC(c581bca7) SHA1(3042fa8383b9f18509768feb08b400ac194831b0) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "10011.8d", 0x0000000, 0x1080000, CRC(7fb4a3fe) SHA1(677ba88af00d448c72a89af145acc9a9d85c3d76) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "10011.7e", 0x0000000, 0x1080000, CRC(31bfc843) SHA1(398017d1a4e26d63feef395fdfac56d6c9f0ed99) )

	ROM_REGION32_LE( 0x1080000, "nand3", 0 )
	ROM_LOAD( "10011.7d", 0x0000000, 0x1080000, CRC(15d83649) SHA1(8ecbf6e294182459fbe2cc098e2406767851c911) )
ROM_END

ROM_START( gunbalina )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "gnn1a.8e", 0x0000000, 0x1080000, CRC(981b03d4) SHA1(1c55458f1b2964afe2cf4e9d84548c0699808e9f) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "gnn1a.8d", 0x0000000, 0x1080000, CRC(6cd343e0) SHA1(dcec44abae1504025895f42fe574549e5010f7d5) )
ROM_END

ROM_START( kd2001 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "0.8e", 0x0000000, 0x1080000, NO_DUMP ) // broken flash ROM, couldn't be dumped

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "1.8d", 0x0000000, 0x1080000, CRC(2b0d0e8c) SHA1(d679e7044e1f93bb7bd449e6d8fcb7737d154025) )

	ROM_REGION( 0x20000, "pdrivecpu", 0 )
	ROM_LOAD( "kd11-dr0-ic10.bin", 0x00000, 0x20000, CRC(59649293) SHA1(71c3a0e73d077398e7f3d95acedc47814e99fbc6) )
ROM_END

ROM_START( keroro )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x2100000, "nand0", 0 )
	ROM_LOAD( "krg1.8e", 0x0000000, 0x2100000, CRC(12e78c66) SHA1(83573f68f27ace345a3be16f29f874f14e593233) ) // K9F5608U0D, double sized wrt to the other games and PCB silkscreen

	ROM_REGION32_LE( 0x2100000, "nand1", 0 )
	ROM_LOAD( "krg1.8d", 0x0000000, 0x2100000, CRC(879a87b7) SHA1(fcef8eb9423b4825bf27fbfe8cae6d4018cb534f) ) // K9F5608U0D, double sized wrt to the other games and PCB silkscreen
ROM_END

ROM_START( knpuzzle )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "kpm1vera_0.8e", 0x0000000, 0x1080000, CRC(4b4255da) SHA1(c8ec575e53596a167a07db97076fd69e6646d0f5) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "kpm1vera_1.8d", 0x0000000, 0x1080000, CRC(644595a6) SHA1(4b60008ca5cac894a815fe6aaa980296a83f673f) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "kpm1vera_2.7e", 0x0000000, 0x1080000, CRC(6bf164e5) SHA1(b4a2e6eb18c09220b0c8ec80159d13d0e439a559) )
ROM_END

ROM_START( konotako )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION16_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "0.8e", 0x0000000, 0x1080000, CRC(63d23a0c) SHA1(31b54119f20827ff13ecf0cd87803a5e27eaafe7) )

	ROM_REGION16_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "1.8d", 0x0000000, 0x1080000, CRC(bdbed53c) SHA1(5773069c43642e6f334cee185a6fb6908eedcf4a) )
ROM_END

ROM_START( medalnt )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "k9f2808u0c.8e", 0x0000000, 0x1080000, CRC(b8ce45c6) SHA1(cfc85e796e32f5f3cc16e12ce902f0ae088eea31) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "k9f2808u0c.8d", 0x0000000, 0x1080000, CRC(49a2a732) SHA1(1a473177827a6d0e58c289d9af064665b941519b) )
ROM_END

ROM_START( medalnt2 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x2100000, "nand0", 0 )
	ROM_LOAD( "mta1001_stmpr0a_0.8e", 0x0000000, 0x2100000, CRC(074b305a) SHA1(772799ebba96104a191db236dd45257cdef6c5f2) )

	ROM_REGION32_LE( 0x2100000, "nand1", 0 )
	ROM_LOAD( "mta1001_stmpr0a_1.8d", 0x0000000, 0x2100000, CRC(7ca5fdc0) SHA1(5920fa753b190dc87f775567fa9fb9bccb6f7a95) )
ROM_END

ROM_START( mrdrilrg )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "0.8e", 0x0000000, 0x1080000, CRC(def72bcd) SHA1(e243b7ef23b2b00612c185e01493cd01be51f154) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "1.8d", 0x0000000, 0x1080000, CRC(c87b5e86) SHA1(b034210da30e1f2f7d04f77e00bf7724437e2024) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "2.7e", 0x0000000, 0x1080000, CRC(e0a9339f) SHA1(4284e7233876cfaf8021440d78ccc8c70d00cc00) )
ROM_END

ROM_START( nflclsfb )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "0.8e", 0x0000000, 0x1080000, CRC(97ac7216) SHA1(0f3383e5df534daf899fd6d40d1932d7ace78217) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "1.8d", 0x0000000, 0x1080000, CRC(d3f519d8) SHA1(60d5f2fafd700e39245bed17e3cc6d608cc2c088) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "2.7e", 0x0000000, 0x1080000, CRC(0c65fdc2) SHA1(fa5d41a7b10ae8f8d312b61cc6d34408123bda97) )

	ROM_REGION32_LE( 0x1080000, "nand3", 0 )
	ROM_LOAD( "3.7d", 0x0000000, 0x1080000, CRC(0a4e601d) SHA1(9c302a0b5aaf7046390982e62092b867c3a534a5) )
ROM_END

ROM_START( pacmball )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "k9f2808u0c.8e", 0x0000000, 0x1080000, BAD_DUMP CRC(7b6f814d) SHA1(728167866d9350150b5fd9ebcf8fe7280efedb91) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "k9f2808u0c.8d", 0x0000000, 0x1080000, BAD_DUMP CRC(f79d7199) SHA1(4ef9b758ee778e12f7fef717e063597299fb8219) )
ROM_END

ROM_START( panikuru )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "0.8e", 0x0000000, 0x1080000, CRC(a68b0965) SHA1(037e41e80a62675ffe1c820060c8bb414fb84c30) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "1.8d", 0x0000000, 0x1080000, CRC(18e5135d) SHA1(a7b1533a1df71be5498718e301d1c9c548551fb4) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "2.7e", 0x0000000, 0x1080000, CRC(cd3b25e0) SHA1(39dfebc59e71b8f1c28e718ee71032620f11440c) )
ROM_END

ROM_START( ptblank3 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "gnn2vera_0.8e", 0x0000000, 0x1080000, CRC(3777ef6b) SHA1(44dce83f75d10f843db0feef4c2a738442434246) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "gnn2vera_1.8d", 0x0000000, 0x1080000, CRC(82d2cfb5) SHA1(4b5e713a55e74a7b32b1b9b5811892df2df86256) )
ROM_END

ROM_START( puzzball )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "k9f2808u0c.8e", 0x0000000, 0x1080000, CRC(0d9bac12) SHA1(f386fca5e5dd65995bb0bb04688222e0962fffd0) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "k9f2808u0c.8d", 0x0000000, 0x1080000, CRC(0002794e) SHA1(44b6bcea835d3dbb6b2e85ba3ea4404e1400c4f5) )
ROM_END

ROM_START( sekaikh )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "0.8e", 0x0000000, 0x1080000, CRC(b0cc4a4f) SHA1(41974931901090811e07f18c04e2af853c308f88) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "1.8d", 0x0000000, 0x1080000, CRC(65c4a8b4) SHA1(c7fefc32604bb47519a05cdb6c8b0f50034e0efd) )

	ROM_REGION16_BE( 0x8000, "exio:nvram", 0 )
	ROM_LOAD( "m48z35y.ic11", 0x0000, 0x8000, CRC(e0e52ffc) SHA1(557490e2f286773a945851f44ed0214de731cd75) )
ROM_END

ROM_START( sekaikha )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "0.8e", 0x0000000, 0x1080000, BAD_DUMP CRC(e32c36ac) SHA1(d762723b6ecf65c8cb7c85c25d9a1fbbcdcfd27a) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "1.8d", 0x0000000, 0x1080000, BAD_DUMP CRC(7cb38ece) SHA1(e21fbc9ff09ca51e1857e32318b95107ae4b3f0b) )

	ROM_REGION16_BE( 0x8000, "exio:nvram", 0 )
	ROM_LOAD( "m48z35y.ic11", 0x0000, 0x8000, CRC(e0e52ffc) SHA1(557490e2f286773a945851f44ed0214de731cd75) )
ROM_END

ROM_START( startrgn )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "stt1a_0.bin", 0x0000000, 0x1080000, CRC(1e090644) SHA1(a7a293e2bd9eea2eb64a492a47272d9d9ee2c724) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "stt1a_1.bin", 0x0000000, 0x1080000, CRC(aa527694) SHA1(a25dcbeca58a1443070848b3487a24d51d41a34b) )
ROM_END

ROM_START( sugorotc )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "stj1verc_0.8e", 0x0000000, 0x1080000, CRC(a994fc8f) SHA1(58ea3f7576e07ade0be71058705baf7ec348e55b) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "stj1verc_1.8d", 0x0000000, 0x1080000, CRC(a7a20960) SHA1(72bc89637f966fe23a84c34346be3cdc20d712e5) )
ROM_END

ROM_START( taiko2 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "tk21verc_0.8e", 0x0000000, 0x1080000, CRC(f3842791) SHA1(3108e23cb98455e5016e7fa845ab686e89ed61e8) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "tk21verc_1.8d", 0x0000000, 0x1080000, CRC(e294f460) SHA1(a22d553215d7e109c03af8bd0edb962fdcede5dc) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "tk21verc_2.7e", 0x0000000, 0x1080000, CRC(f1dbe387) SHA1(8ae5f3b659acec150d89af2a14fc2dec8c3b1643) )

	DISK_REGION( "ata:0:cdrom" )
	DISK_IMAGE_READONLY( "tk-21", 0, SHA1(cede127f3d87f894ccaa1d77c8b279f209f6a8e4) )
ROM_END

ROM_START( taiko3 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "tk31vera_0.8e", 0x0000000, 0x1080000, CRC(6ee822f1) SHA1(02817db16e20062de36c7e7c9497950636c55e7c) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "tk31vera_1.8d", 0x0000000, 0x1080000, CRC(384c9692) SHA1(f40602522de18b8b2dd14254221279905df6b38a) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "tk31vera_2.7e", 0x0000000, 0x1080000, CRC(904c09ee) SHA1(a81b70661e3f07a4e8f7cb5c2be6c2c526ce27c3) )

	DISK_REGION( "ata:0:cdrom" )
	DISK_IMAGE_READONLY( "tk-3", 0, NO_DUMP )
ROM_END

ROM_START( taiko4 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "tk41vera_0.8e", 0x0000000, 0x1080000, CRC(6dc209b1) SHA1(406a4c9e4bee9d4353d8847d7916ad459da9e089) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "tk41vera_1.8d", 0x0000000, 0x1080000, CRC(0d1731d8) SHA1(b1e30fd2a4f1034325b9745f8c371d918413d03c) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "tk41vera_2.7e", 0x0000000, 0x1080000, CRC(6fc63af0) SHA1(78bac0a11497d5cdfba17fb0ff4d6916349df527) )

	DISK_REGION( "ata:0:cdrom" )
	DISK_IMAGE_READONLY( "tk-41", 0, SHA1(6a5b960e792e4b291cdcc9e5ac4bcf84967e30e7) )
ROM_END

ROM_START( taiko5 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "tk51vera_0.8e", 0x0000000, 0x1080000, CRC(3562152e) SHA1(2372b44726bcd9bed7760bb18f7f8fde6c24af82) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "tk51vera_1.8d", 0x0000000, 0x1080000, CRC(63ffbcaa) SHA1(8cdef91605d76e1f9b8a72e0bce793f3e7da31e7) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "tk51vera_2.7e", 0x0000000, 0x1080000, CRC(1f419247) SHA1(93e624107b614bd30b18f6bac0422e64ad467742) )

	DISK_REGION( "ata:0:cdrom" )
	DISK_IMAGE_READONLY( "tk-5", 0, NO_DUMP )
ROM_END

ROM_START( taiko6 )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "tk61vera_0.8e", 0x0000000, 0x1080000, CRC(a07936c6) SHA1(a9e02f4f3d901def294a7b4758f2e7d2397aaf68) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "tk61vera_1.8d", 0x0000000, 0x1080000, CRC(288fb1aa) SHA1(0ff6b608f2540cd993a443b0d2c74567e9af5b10) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "tk61vera_2.7e", 0x0000000, 0x1080000, CRC(550bb6a1) SHA1(466ff7d5a8a06cdddbb1976ca05ccfd34c0851fd) )

	DISK_REGION( "ata:0:cdrom" )
	DISK_IMAGE_READONLY( "tk-6", 0, SHA1(ca8b8dfccc2022094c428b5e0b6391a77ec351f4) )
ROM_END


// MEM(P3)
ROM_START( g13jnr )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "glt1vera_0.2j", 0x0000000, 0x1080000, CRC(24f5bb06) SHA1(cb61b754e7c1afaeb3cff5c588d28f68a1eca092) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "glt1vera_1.1j", 0x0000000, 0x1080000, CRC(5ec552da) SHA1(d23f73ae4694c581d3827afac7f62bc6ce83eb30) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "glt1vera_2.2h", 0x0000000, 0x1080000, CRC(0835d6ed) SHA1(5dc4e734aad91e9ab722d11c57eff11e0a100657) )

	ROM_REGION32_LE( 0x1080000, "nand3", 0 )
	ROM_LOAD( "glt1vera_3.1h", 0x0000000, 0x1080000, CRC(f7486979) SHA1(a44c33ae7004e79fe66c6d2cba3d11671ce2582c) )

	ROM_REGION32_LE( 0x1080000, "nand4", 0 )
	ROM_LOAD( "glt1vera_4.2f", 0x0000000, 0x1080000, CRC(e60b526a) SHA1(3599af2ebb894833d1233cd76d35644fc95a647b) )

	ROM_REGION32_LE( 0x1080000, "nand5", 0 )
	ROM_LOAD( "glt1vera_5.1f", 0x0000000, 0x1080000, CRC(0cb2df20) SHA1(b0e10b6d00f3cc20103177faca0c14d98b10994d) )
ROM_END

ROM_START( nicetsuk )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "ntk1vera_0.2j", 0x0000000, 0x1080000, CRC(92b60f09) SHA1(2219c306e3241736d7141d59987becf2aa2a2baa) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "ntk1vera_1.1j", 0x0000000, 0x1080000, CRC(5d5a4ca2) SHA1(b14ab6ca26236f819bd7d8bd5bbd828e36a528da) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "ntk1vera_2.2h", 0x0000000, 0x1080000, CRC(df2ba95f) SHA1(8c98812bbaf4246055bdc5a171fc9f5f5c47a38b) )

	ROM_REGION32_LE( 0x1080000, "nand3", 0 )
	ROM_LOAD( "ntk1vera_3.1h", 0x0000000, 0x1080000, CRC(ee0c6a94) SHA1(7ee3f5e96635885a7f6abbd8e869205aa16f5f94) )

	ROM_REGION32_LE( 0x1080000, "nand4", 0 )
	ROM_LOAD( "ntk1vera_4.2f", 0x0000000, 0x1080000, CRC(cf9ad49e) SHA1(4aa5593bc8154bb31c5e8113e97e6384b37eec7b) )

	ROM_REGION32_LE( 0x1080000, "nand5", 0 )
	ROM_LOAD( "ntk1vera_5.1f", 0x0000000, 0x1080000, CRC(cf73d26b) SHA1(fe760793f21d2f9a833371dc9e76e4841d52a22f) )

	ROM_REGION32_LE( 0x1080000, "nand6", 0 )
	ROM_LOAD( "ntk1vera_6.2e", 0x0000000, 0x1080000, CRC(ee63d5ee) SHA1(e76d00891bb868c34a2327ccb5f968586c467b04) )

	ROM_REGION32_LE( 0x1080000, "nand7", 0 )
	ROM_LOAD( "ntk1vera_7.1e", 0x0000000, 0x1080000, CRC(5c8981e4) SHA1(5315b8a5426199c3bb08d427491d644b435bddc1) )
ROM_END

ROM_START( squizchs )
	ROM_REGION32_LE( 0x400000, "maincpu:rom", 0 )
	ROM_FILL( 0x0000000, 0x400000, 0x55 )

	ROM_REGION32_LE( 0x1080000, "nand0", 0 )
	ROM_LOAD( "chs1vera_0.2j", 0x0000000, 0x1080000, CRC(2e92071c) SHA1(51e0bf8989b8a8fdf0d915df2d2227f3e425684a) )

	ROM_REGION32_LE( 0x1080000, "nand1", 0 )
	ROM_LOAD( "chs1vera_1.1j", 0x0000000, 0x1080000, CRC(85212a85) SHA1(3c8c1d1671d65d594d30da546e8f4c2dcdfca840) )

	ROM_REGION32_LE( 0x1080000, "nand2", 0 )
	ROM_LOAD( "chs1vera_2.2h", 0x0000000, 0x1080000, CRC(6a6a1ee7) SHA1(6f42afbc58d9b40b152bb004fa06b096b1f9c29a) )

	ROM_REGION32_LE( 0x1080000, "nand3", 0 )
	ROM_LOAD( "chs1vera_3.1h", 0x0000000, 0x1080000, CRC(b7ccfaf6) SHA1(3b5ab8fc0ccd59b4566c3d26e347f550030d0a2c) )

	ROM_REGION32_LE( 0x1080000, "nand4", 0 )
	ROM_LOAD( "chs1vera_4.2f", 0x0000000, 0x1080000, CRC(6623029d) SHA1(b84e2291cd15271ce8417be71a9e2e238a7d6820) )

	ROM_REGION32_LE( 0x1080000, "nand5", 0 )
	ROM_LOAD( "chs1vera_5.1f", 0x0000000, 0x1080000, CRC(5f771c16) SHA1(7d48476283f6fd4c8fc754d7a312eea542d4b4ca) )

	ROM_REGION32_LE( 0x1080000, "nand6", 0 )
	ROM_LOAD( "chs1vera_6.2e", 0x0000000, 0x1080000, CRC(840514fd) SHA1(5afe352c4b871b71f16c8df3f53e73985df1a270) )

	ROM_REGION32_LE( 0x1080000, "nand7", 0 )
	ROM_LOAD( "chs1vera_7.1e", 0x0000000, 0x1080000, CRC(24c38637) SHA1(8b3682683fcb63c55835793e1fbfb62c8e12b61c) )

	ROM_REGION32_LE( 0x1080000, "nand8", 0 )
	ROM_LOAD( "chs1vera_8.2d", 0x0000000, 0x1080000, CRC(d59d87ec) SHA1(d08b6073481248555c03d32d9353c5fef972cc7b) )

	ROM_REGION32_LE( 0x1080000, "nand9", 0 )
	ROM_LOAD( "chs1vera_9.1d", 0x0000000, 0x1080000, CRC(822532d8) SHA1(0cb30628761ea575ac7dfc6e68b9e6745a910146) )

	ROM_REGION32_LE( 0x1080000, "nand10", 0 )
	ROM_LOAD( "chs1vera_10.2c", 0x0000000, 0x1080000, CRC(46752e59) SHA1(60b5f4a1dd83011aeed097ba8dcb05b67d55407e) )

	ROM_REGION32_LE( 0x1080000, "nand11", 0 )
	ROM_LOAD( "chs1vera_11.1c", 0x0000000, 0x1080000, CRC(fc9ed471) SHA1(7e2827e5babdbcbd5cc6f3bd4165d68f3e62bc86) )

	ROM_REGION32_LE( 0x1080000, "nand12", 0 )
	ROM_LOAD( "chs1vera_12.2b", 0x0000000, 0x1080000, CRC(ab786753) SHA1(99348e0a7d389e2e177f8907683a1bb12635e855) )

	ROM_REGION32_LE( 0x1080000, "nand13", 0 )
	ROM_LOAD( "chs1vera_13.1b", 0x0000000, 0x1080000, CRC(739e3b1e) SHA1(d7a25984d939d69de11b0fea000e51fdb4759163) )
ROM_END

} // Anonymous namespace


// MEM(M)
GAME( 2000, mrdrilr2,  0,        ns10_mrdrilr2,  mrdrilr2,     namcos10_memm_state, init_mrdrilr2,  ROT0, "Namco", "Mr. Driller 2 (World, DR22 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2000, mrdrilr2j, mrdrilr2, ns10_mrdrilr2,  mrdrilr2,     namcos10_memm_state, init_mrdrilr2,  ROT0, "Namco", "Mr. Driller 2 (Japan, DR21 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2000, mrdrilr2u, mrdrilr2, ns10_mrdrilr2,  mrdrilr2,     namcos10_memm_state, init_mrdrilr2,  ROT0, "Namco", "Mr. Driller 2 (US, DR23 Ver.A)", MACHINE_IMPERFECT_SOUND )

// MEM(N)
GAME( 2000, gahaha,    0,        ns10_gahaha,    gahaha,       namcos10_memn_state, init_gahaha,    ROT0, "Namco / Metro", "GAHAHA Ippatsudou (World, GID2 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2000, ptblank3,  0,        ns10_ptblank3,  namcos10,     namcos10_memn_state, init_gunbalina, ROT0, "Namco", "Point Blank 3 (World, GNN2 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_UNEMULATED_PROTECTION ) // needs to hookup gun IO
GAME( 2000, gunbalina, ptblank3, ns10_ptblank3,  namcos10,     namcos10_memn_state, init_gunbalina, ROT0, "Namco", "Gunbalina (Japan, GNN1 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_UNEMULATED_PROTECTION )
GAME( 2001, gahaha2,   0,        ns10_gahaha2,   gahaha,       namcos10_memn_state, init_gahaha2,   ROT0, "Namco / Metro", "GAHAHA Ippatsudou 2 (Japan, GIS1 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2001, gjspace,   0,        ns10_gjspace,   gjspace,      namcos10_memn_state, init_gjspace,   ROT0, "Namco / Metro", "Gekitoride-Jong Space (10011 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2001, kd2001,    0,        ns10_kd2001,    namcos10,     namcos10_memn_state, empty_init,     ROT0, "Namco", "Knock Down 2001 (Japan, KD11 Ver. B)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_UNEMULATED_PROTECTION )
GAME( 2001, knpuzzle,  0,        ns10_knpuzzle,  namcos10,     namcos10_memn_state, init_knpuzzle,  ROT0, "Namco", "Kotoba no Puzzle Mojipittan (Japan, KPM1 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2001, mrdrilrg,  0,        ns10_mrdrilrg,  mrdrilr2,     namcos10_memn_state, init_mrdrilrg,  ROT0, "Namco", "Mr. Driller G (Japan, DRG1 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2002, chocovdr,  0,        ns10_chocovdr,  namcos10,     namcos10_memn_state, init_chocovdr,  ROT0, "Namco", "Uchuu Daisakusen: Chocovader Contactee (Japan, CVC1 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2002, gamshara,  0,        ns10_gamshara,  gamshara,     namcos10_memn_state, init_gamshara,  ROT0, "Mitchell", "Gamshara (World, 20020912A / 10021 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_UNEMULATED_PROTECTION ) // Ver. 20020912A ETC
GAME( 2002, gamsharaj, gamshara, ns10_gamshara,  gamshara,     namcos10_memn_state, init_gamshara,  ROT0, "Mitchell", "Gamshara (Japan, 20020716A / 10021 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2002, panikuru,  0,        ns10_panikuru,  namcos10,     namcos10_memn_state, init_panikuru,  ROT0, "Namco", "Panikuru Panekuru (Japan, PPA1 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2002, puzzball,  0,        ns10_puzzball,  mgexio_medal, namcos10_memn_state, init_puzzball,  ROT0, "Namco", "Puzz Ball (Japan, PZB1 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
GAME( 2002, startrgn,  0,        ns10_startrgn,  startrgn,     namcos10_memn_state, init_startrgn,  ROT0, "Namco", "Star Trigon (Japan, STT1 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2002, sugorotc,  0,        ns10_sugorotic, mgexio_medal, namcos10_memn_state, init_sugorotic, ROT0, "Namco", "Sugorotic JAPAN (STJ1 Ver.C)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
GAME( 2003, konotako,  0,        ns10_konotako,  konotako,     namcos10_memn_state, init_konotako,  ROT0, "Mitchell", "Kono e Tako (10021 Ver.A)", MACHINE_IMPERFECT_SOUND )
GAME( 2003, nflclsfb,  0,        ns10_nflclsfb,  nflclsfb,     namcos10_memn_state, init_nflclsfb,  ROT0, "Namco / Metro", "NFL Classic Football (US, NCF3 Ver.A.)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_CONTROLS | MACHINE_IMPERFECT_SOUND )
GAME( 2003, pacmball,  0,        ns10_pacmball,  mgexio_medal, namcos10_memn_state, init_pacmball,  ROT0, "Namco", "Pacman BALL (PMB2 Ver.A.)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
GAME( 2004, sekaikh,   0,        ns10_sekaikh,   mgexio_medal, namcos10_memn_state, init_sekaikh,   ROT0, "Namco", "Sekai Kaseki Hakken (Japan, SKH1 Ver.B)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_CONTROLS | MACHINE_IMPERFECT_SOUND )
GAME( 2004, sekaikha,  sekaikh,  ns10_sekaikh,   mgexio_medal, namcos10_memn_state, init_sekaikh,   ROT0, "Namco", "Sekai Kaseki Hakken (Japan, SKH1 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_CONTROLS | MACHINE_IMPERFECT_SOUND )
GAME( 2005, ballpom,   0,        ns10_ballpom,   mgexio_medal, namcos10_memn_state, init_ballpom,   ROT0, "Namco", "Ball Pom Line", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_CONTROLS | MACHINE_IMPERFECT_SOUND ) // ROM VER. B0 FEB 09 2005 15:29:02 in test mode, boots but requires MGEXIO to proceed
GAME( 2005, medalnt,   0,        ns10_medalnt,   namcos10,     namcos10_memn_state, init_medalnt,   ROT0, "Namco", "Medal no Tatsujin Doki! Ooatari-Darake no Sugoroku Matsuri (MTL1 SPR0B)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
GAME( 2006, keroro,    0,        ns10_keroro,    mgexio_medal, namcos10_memn_state, init_keroro,    ROT0, "Namco / Banpresto", "Keroro Gunsou: Pekopon Shinryaku Shirei...De Arimasu! (KRG1 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND ) // ケロロ軍曹　地球侵略指令…であります！
GAME( 2007, gegemdb,   0,        ns10_gegemdb,   mgexio_medal, namcos10_memn_state, empty_init,     ROT0, "Namco", "Gegege no Kitaro Yokai Yokocho Matsuri de Battle Ja (GYM1 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_UNEMULATED_PROTECTION ) // ゲゲゲの鬼太郎　妖怪横丁まつりでバトルじゃ
GAME( 2007, medalnt2,  0,        ns10_medalnt2,  namcos10,     namcos10_memn_state, init_medalnt2,  ROT0, "Namco", "Medal no Tatsujin 2 Atsumare! Go! Go! Sugoroku Sentai Don Ranger Five (MTA1 STMPR0A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND ) // メダルの達人2 あつまれ!ゴー!ゴー!双六戦隊ドンレンジャーファイブ MTA100-1-ST-MPR0-A00 2007/01/30 19:51:54

GAME( 2001, taiko2,    0,        ns10_taiko2,    taiko,        namcos10_memn_state, init_taiko2,    ROT0, "Namco", "Taiko no Tatsujin 2 (Japan, TK21 Ver.C)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND  )
GAME( 2002, taiko3,    0,        ns10_taiko3,    taiko,        namcos10_memn_state, init_taiko3,    ROT0, "Namco", "Taiko no Tatsujin 3 (Japan, TK31 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_UNEMULATED_PROTECTION )
GAME( 2002, taiko4,    0,        ns10_taiko4,    taiko,        namcos10_memn_state, init_taiko4,    ROT0, "Namco", "Taiko no Tatsujin 4 (Japan, TK41 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
GAME( 2003, taiko5,    0,        ns10_taiko5,    taiko,        namcos10_memn_state, init_taiko5,    ROT0, "Namco", "Taiko no Tatsujin 5 (Japan, TK51 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_UNEMULATED_PROTECTION )
GAME( 2004, taiko6,    0,        ns10_taiko6,    taiko,        namcos10_memn_state, init_taiko6,    ROT0, "Namco", "Taiko no Tatsujin 6 (Japan, TK61 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

// MEM(P3)
GAME( 2001, g13jnr,    0,        ns10_g13jnr,    g13jnr,       namcos10_memp3_state, init_g13jnr,   ROT0, "Eighting / Raizing / Namco", "Golgo 13: Juusei no Requiem (Japan, GLT1 VER.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
GAME( 2002, nicetsuk,  0,        ns10_nicetsuk,  nicetsuk,     namcos10_memp3_state, init_nicetsuk, ROT0, "Namco / Metro", "Tsukkomi Yousei Gips Nice Tsukkomi (NTK1 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
GAME( 2003, squizchs,  0,        ns10_squizchs,  squizchs,     namcos10_memp3_state, init_squizchs, ROT0, "Namco", "Seishun-Quiz Colorful High School (CHS1 Ver.A)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
