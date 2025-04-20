// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/*****************************************************************************************

    NES Cart PCBs Emulation

****************************************************************************************/

struct nes_pcb
{
	const char              *slot_opt;
	int                     pcb_id;
};

// Here, we take the feature attribute from .xml (i.e. the PCB name) and we assign a unique ID to it
static const nes_pcb pcb_list[] =
{
	{ "nrom",             STD_NROM },
	{ "hvc_basic",        HVC_FAMBASIC },
	{ "nrom368",          STD_NROM368 },
	{ "nrom_gg",          GG_NROM },
	{ "uxrom",            STD_UXROM },
	{ "unrom_cc",         UXROM_CC },
	{ "cnrom",            STD_CNROM },
	{ "bandai_pt554",     BANDAI_PT554 },
	{ "cprom",            STD_CPROM },
	{ "axrom",            STD_AXROM },
	{ "pxrom",            STD_PXROM },
	{ "fxrom",            STD_FXROM },
	{ "bnrom",            STD_BXROM },
	{ "gxrom",            STD_GXROM },
	{ "un1rom",           STD_UN1ROM },
	{ "sxrom",            STD_SXROM },
	{ "sorom",            STD_SOROM },
	{ "szrom",            STD_SZROM },
	{ "txrom",            STD_TXROM },
	{ "hkrom",            STD_HKROM },
	{ "tqrom",            STD_TQROM },
	{ "txsrom",           STD_TXSROM },
	{ "exrom",            STD_EXROM },
	{ "disksys",          STD_DISKSYS },
	{ "pal_zz",           PAL_ZZ },
	{ "nes_qj",           NES_QJ },
	{ "nes_event",        STD_EVENT },
	{ "nes_event2",       STD_EVENT2 },
	{ "discrete_74x139",  DIS_74X139X74 },
	{ "discrete_74x377",  DIS_74X377 },
	{ "discrete_74x161",  DIS_74X161X161X32 },
	{ "bitcorp_dis",      DIS_74X161X138 },
	{ "lz93d50",          BANDAI_LZ93 },
	{ "lz93d50_ep1",      BANDAI_LZ93EX1 },
	{ "lz93d50_ep2",      BANDAI_LZ93EX2 },
	{ "fcg",              BANDAI_FCG },
	{ "fjump2",           BANDAI_FJUMP2 },
	{ "datach",           BANDAI_DATACH },
	{ "karastudio",       BANDAI_KARAOKE },
	{ "oekakids",         BANDAI_OEKAKIDS },
	{ "g101",             IREM_G101 },
	{ "lrog017",          IREM_LROG017 },
	{ "h3001",            IREM_H3001 },
	{ "holydivr",         IREM_HOLYDIVR },
	{ "tam_s1",           IREM_TAM_S1 },
	{ "jf11",             JALECO_JF11 },
	{ "jf13",             JALECO_JF13 },
	{ "jf16",             JALECO_JF16 },
	{ "jf17",             JALECO_JF17 },
	{ "jf17pcm",          JALECO_JF17_ADPCM },
	{ "jf19",             JALECO_JF19 },
	{ "jf19pcm",          JALECO_JF19_ADPCM },
	{ "ss88006",          JALECO_SS88006 },
	{ "jf23",             JALECO_JF23 },
	{ "jf24",             JALECO_JF24 },
	{ "jf29",             JALECO_JF29 },
	{ "jf33",             JALECO_JF33 },
	{ "vrc1",             KONAMI_VRC1 },
	{ "vrc2",             KONAMI_VRC2 },
	{ "vrc3",             KONAMI_VRC3 },
	{ "vrc4",             KONAMI_VRC4 },
	{ "vrc6",             KONAMI_VRC6 },
	{ "vrc7",             KONAMI_VRC7 },
	{ "namcot_163",       NAMCOT_163 },
	{ "namcot_175",       NAMCOT_175 },
	{ "namcot_340",       NAMCOT_340 },
	{ "namcot_3433",      NAMCOT_34X3 },    // DxROM is a Nintendo board for US versions of the 3433/3443 games
	{ "namcot_3425",      NAMCOT_3425 },
	{ "namcot_3446",      NAMCOT_3446 },
	{ "sunsoft1",         SUNSOFT_1 },
	{ "sunsoft2",         SUNSOFT_2 },
	{ "sunsoft3",         SUNSOFT_3 },
	{ "sunsoft4",         SUNSOFT_4 },
	{ "sunsoft_dcs",      SUNSOFT_DCS },
	{ "sunsoft_fme7",     SUNSOFT_FME7 },  // JxROM is a Nintendo board for US versions of the Sunsoft FME7 games
	{ "sunsoft5a",        SUNSOFT_FME7 },
	{ "sunsoft5b",        SUNSOFT_5 },
	{ "tc0190fmc",        TAITO_TC0190FMC },
	{ "tc0190fmcp",       TAITO_TC0190FMCP },
	{ "tc0350fmr",        TAITO_TC0190FMC },
	{ "x1_005",           TAITO_X1_005 },    // two variants exist, depending on pin17 & pin31 connections
	{ "x1_017",           TAITO_X1_017 },
	{ "nina001",          AVE_NINA01 },
	{ "nina006",          AVE_NINA06 },
	{ "maxi15",           AVE_MAXI15 },
	{ "bf9093",           CAMERICA_BF9093 },
	{ "bf9096",           CAMERICA_BF9096 },
	{ "bf9096a",          CAMERICA_BF9096_ALT },
	{ "goldenfive",       CAMERICA_GOLDENFIVE },
	{ "ade"   ,           CAMERICA_ALADDIN },
	{ "cne_decathl",      CNE_DECATHLON },
	{ "cne_fsb",          CNE_FSB },
	{ "cne_shlz",         CNE_SHLZ },
	{ "cocoma",           COCOMA_BOARD },
	{ "nanjing",          NANJING_BOARD },  // mapper 163
	{ "ntdec_asder",      NTDEC_ASDER },    // mapper 112
	{ "ntdec_fh",         NTDEC_FIGHTINGHERO },     // mapper 193
	{ "ntdec_n715021",    NTDEC_N715021 },  // mapper 81
	{ "sa009",            SACHEN_SA009 },
	{ "sa0036",           SACHEN_SA0036 },
	{ "sa0037",           SACHEN_SA0037 },
	{ "sa72007",          SACHEN_SA72007 },
	{ "sa72008",          SACHEN_SA72008 },
	{ "tca01",            SACHEN_TCA01 },
	{ "s3013",            SACHEN_3013 },
	{ "s3014",            SACHEN_3014 },
	{ "s8259a",           SACHEN_8259A },
	{ "s8259b",           SACHEN_8259B },
	{ "s8259c",           SACHEN_8259C },
	{ "s8259d",           SACHEN_8259D },
	{ "s74x374",          SACHEN_74LS374 },
	{ "s74x374a",         SACHEN_74LS374_ALT },   /* FIXME: Made up boards some different handling */
	{ "tcu01",            SACHEN_TCU01 },
	{ "tcu02",            SACHEN_TCU02 },
	{ "sa9602b",          SACHEN_SA9602B },
	{ "tengen_800032",    TENGEN_800032 },
	{ "tengen_800037",    TENGEN_800037 },
	{ "txc_22110",        TXC_22110 },
	{ "txc_22211",        TXC_22211 },
	{ "txc_dumarc",       TXC_DUMARACING },
	{ "txc_mjblock",      TXC_MJBLOCK },
	{ "txc_strikew",      TXC_STRIKEW },
	{ "txc_commandos",    TXC_COMMANDOS },
	{ "waixing_a",        WAIXING_TYPE_A },
	{ "waixing_a1",       WAIXING_TYPE_A1 },    /* FIXME: Made up boards the different CHRRAM banks (see Ji Jia Zhan Shi },  */
	{ "waixing_b",        WAIXING_TYPE_B },
	{ "waixing_c",        WAIXING_TYPE_C },
	{ "waixing_d",        WAIXING_TYPE_D },
	{ "waixing_e",        WAIXING_TYPE_E },
	{ "waixing_f",        WAIXING_TYPE_F },
	{ "waixing_f1",       WAIXING_TYPE_F1 },
	{ "waixing_g",        WAIXING_TYPE_G },
	{ "waixing_h",        WAIXING_TYPE_H },
	{ "waixing_h1",       WAIXING_TYPE_H1 },
	{ "waixing_i",        WAIXING_TYPE_I },
	{ "waixing_j",        WAIXING_TYPE_J },
	{ "waixing_sgzlz",    WAIXING_SGZLZ },
	{ "waixing_sec",      WAIXING_SECURITY },
	{ "waixing_ffv",      WAIXING_FFV },
	{ "waixing_wxzs",     WAIXING_WXZS },
	{ "waixing_wxzs2",    WAIXING_WXZS2 },
	{ "waixing_dq8",      WAIXING_DQ8 },
	{ "waixing_sh2",      WAIXING_SH2 },
	{ "fs304",            WAIXING_FS304 },   // used in Zelda 3 by Waixing
	{ "cony",             CONY_BOARD },
	{ "cony1k",           CONY1K_BOARD },
	{ "yoko",             YOKO_BOARD },
	{ "hengg_srich",      HENGG_SRICH },
	{ "hengg_xhzs",       HENGG_XHZS },
	{ "hes",              HES_BOARD },
	{ "ks106c",           KAISER_KS106C },  // mapper 352
	{ "ks202",            KAISER_KS202 },   // mapper 56
	{ "ks7010",           KAISER_KS7010 },  // used in Akumajo Dracula (FDS Conversion)
	{ "ks7012",           KAISER_KS7012 },  // used in Zanac (FDS Conversion)
	{ "ks7013b",          KAISER_KS7013B }, // used in Highway Star Kaiser bootleg
	{ "ks7016",           KAISER_KS7016 },  // used in Exciting Basketball (FDS Conversion)
	{ "ks7016b",          KAISER_KS7016B }, // used in Meikyuu Jiin Dababa alt (FDS Conversion)
	{ "ks7017",           KAISER_KS7017 },
	{ "ks7021a",          KAISER_KS7021A }, // GetsuFumaDen pirate cart
	{ "ks7022",           KAISER_KS7022 },  // mapper 175
	{ "ks7030",           KAISER_KS7030 },  // used in Doki Doki Panic alt (FDS Conversion)
	{ "ks7031",           KAISER_KS7031 },  // used in Dracula II (FDS Conversion)
	{ "ks7032",           KAISER_KS7032 },  // mapper 142
	{ "ks7037",           KAISER_KS7037 },  // Metroid (FDS Conversion)
	{ "ks7057",           KAISER_KS7057 },  // Gyruss (FDS Conversion)
	{ "ks7058",           KAISER_KS7058 },
	{ "gs2015",           RCM_GS2015 },
	{ "gs2004",           RCM_GS2004 },
	{ "gs2013",           RCM_GS2013 },
	{ "tf9in1",           RCM_TF9IN1 },
	{ "3dblock",          RCM_3DBLOCK },    // NROM + IRQ?
	{ "racermate",        UNL_RACERMATE },  // mapper 168
	{ "agci_50282",       AGCI_50282 },
	{ "dreamtech01",      DREAMTECH_BOARD },
	{ "fukutake",         FUKUTAKE_BOARD },
	{ "futuremedia",      FUTUREMEDIA_BOARD },
	{ "magicseries",      MAGICSERIES_MD },
	{ "daou_306",         OPENCORP_DAOU306 },
	{ "subor0",           SUBOR_TYPE0 },
	{ "subor1",           SUBOR_TYPE1 },
	{ "subor2",           SUBOR_TYPE2 },
	{ "unl_158b",         UNL_158B },  // Blood of Jurassic
	{ "cc21",             UNL_CC21 },
	{ "xiaozy",           UNL_XIAOZY },
	{ "edu2k",            UNL_EDU2K },
	{ "jy830623c",        UNL_JY830623C },
	{ "zemina",           ZEMINA_BOARD },
	// misc VRC clone boards
	{ "btl_2yudb",        BTL_2YUDB },
	{ "btl_900218",       BTL_900218 },  // pirate The Lord of King
	{ "ax40g",            UNL_AX40G },
	{ "ax5705",           UNL_AX5705 },
	{ "bmc_830506c",      BMC_830506C },
	{ "bmc_831128c",      BMC_831128C },
	{ "bmc_kl06",         BMC_KL06 },
	{ "unl_cfight",       UNL_CITYFIGHT },
	{ "hengg_shjy3",      HENGG_SHJY3 },    // mapper 253
	{ "sgpipe",           BTL_SHUIGUAN },
	{ "t230",             UNL_T230 },
	{ "tf1201",           UNL_TF1201 },
	{ "th21311",          UNL_TH21311 },
	{ "waixing_sgz",      WAIXING_SGZ },
	// misc bootleg boards
	{ "sc127",            UNL_SC127 },
	{ "mariobaby",        BTL_MARIOBABY },
	{ "asnicol",          BTL_AISENSHINICOL },
	{ "smb3pirate",       BTL_SMB3 },
	{ "btl_contraj",      BTL_CONTRAJ },
	{ "btl_dninja",       BTL_DRAGONNINJA },
	{ "smb2j",            UNL_SMB2J },
	{ "smb2ja",           BTL_SMB2JA },
	{ "smb2jb",           BTL_SMB2JB },
	{ "yung08",           BTL_YUNG08 },
	{ "btl_0353",         BTL_0353 },
	{ "09034a",           BTL_09034A },
	{ "l001",             BTL_L001 },
	{ "batmanfs",         BTL_BATMANFS },
	{ "palthena",         BTL_PALTHENA },
	{ "tobidase",         BTL_TOBIDASE },  // mapper 120
	{ "dbz5",             REXSOFT_DBZ5 },
	{ "sl1632",           REXSOFT_SL1632 },
	{ "somari",           SOMARI_SL12 },  // mapper 116
	{ "huang2",           SOMARI_HUANG2 },  // mapper 116 also
	{ "nitra",            NITRA_TDA },
	{ "bmw8544",          UNL_BMW8544 },
	{ "fs6",              UNL_FS6 },  // mapper 196 alt? (for Street Fighter VI / Fight Street VI },
	{ "sbros11",          BTL_SBROS11 },
	{ "family4646",       BMC_FAMILY_4646 },
	{ "pikay2k",          BTL_PIKACHUY2K },  // mapper 254
	{ "8237",             UNL_8237 },
	{ "8237a",            UNL_8237A },
	{ "sg_lionk",         SUPERGAME_LIONKING },
	{ "sg_boog",          SUPERGAME_BOOGERMAN },
	{ "kasing",           KASING_BOARD },
	{ "kay",              KAY_BOARD },
	{ "h2288",            UNL_H2288 },
	{ "unl_6035052",      UNL_603_5052 },   // mapper 238
	{ "txc_tw",           TXC_TW },
	{ "kof97",            UNL_KOF97 },
	{ "kof96",            UNL_KOF96 },
	{ "sfight3",          UNL_SF3 },
	{ "gouder",           GOUDER_37017 },
	{ "benshieng",        BMC_BENSHIENG },
	{ "action52",         ACTENT_ACT52 },
	{ "caltron6in1",      CALTRON_6IN1 },
	{ "caltron9in1",      CALTRON_9IN1 },
	{ "rumblestation",    RUMBLESTATION_BOARD },     // mapper 46
	{ "svision16",        SVISION16_BOARD },
	{ "kn42",             UNL_KN42 },
	{ "n625092",          UNL_N625092 },
	{ "a65as",            BMC_A65AS },
	{ "t262",             BMC_T262 },
	{ "studyngame",       UNL_STUDYNGAME },  // mapper 39
	{ "sgun20in1",        BMC_SUPERGUN_20IN1 },
	{ "bmc_vt5201",       BMC_VT5201 },  // mapper 59
	{ "bmc_60311c",       BMC_60311C },
	{ "bmc_80013b",       BMC_80013B },
	{ "bmc_810544c",      BMC_810544C },
	{ "bmc_82ab",         BMC_82AB },
	{ "bmc_830425c",      BMC_830425C },
	{ "bmc_830928c",      BMC_830928C },
	{ "bmc_850437c",      BMC_850437C },
	{ "bmc_891227",       BMC_891227 },
	{ "bmc_970630c",      BMC_970630C },
	{ "bmc_jy012005",     BMC_JY012005 },
	{ "bmc_jy820845c",    BMC_JY820845C },
	{ "srpg_5in1",        BMC_SRPG_5IN1 },
	{ "n32_4in1",         BMC_N32_4IN1 },
	{ "ntd03",            BMC_NTD_03 },
	{ "bmc_ctc09",        BMC_CTC09 },
	{ "bmc_ctc12in1",     BMC_CTC_12IN1 },
	{ "bmc_ds927",        BMC_DS927 },
	{ "bmc_fam250",       BMC_FAM250 },
	{ "bmc_gka",          BMC_GKA },
	{ "bmc_gkb",          BMC_GKB },
	{ "bmc_gkcxin1",      BMC_GKCXIN1 },
	{ "bmc_gn91b",        BMC_GN91B },
	{ "bmc_hp898f",       BMC_HP898F },
	{ "bmc_k1029",        BMC_K1029 },
	{ "bmc_k3006",        BMC_K3006 },
	{ "bmc_k3033",        BMC_K3033 },
	{ "bmc_k3036",        BMC_K3036 },
	{ "bmc_k3046",        BMC_K3046 },
	{ "bmc_k3071",        BMC_K3071 },
	{ "bmc_s009",         BMC_S009 },
	{ "bmc_sa005a",       BMC_SA005A },
	{ "bmc_tf2740",       BMC_TF2740 },
	{ "bmc_th2348",       BMC_TH2348 },
	{ "bmc_tj03",         BMC_TJ03 },
	{ "bmc_ws",           BMC_WS },
	{ "bmc_hik300",       BMC_SUPERHIK_300IN1 },
	{ "bmc_s700",         BMC_SUPER_700IN1 },
	{ "bmc_ball11",       BMC_BALLGAMES_11IN1 },
	{ "bmc_22games",      BMC_22GAMES },
	{ "bmc_64y2k",        BMC_64IN1NR },
	{ "bmc_420y2k",       BMC_TELETUBBIES },
	{ "bmc_12in1",        BMC_12IN1 },
	{ "bmc_20in1",        BMC_20IN1 },
	{ "bmc_21in1",        BMC_21IN1 },
	{ "bmc_31in1",        BMC_31IN1 },
	{ "bmc_35in1",        BMC_35IN1 },
	{ "bmc_36in1",        BMC_36IN1 },
	{ "bmc_64in1",        BMC_64IN1 },
	{ "bmc_70in1",        BMC_70IN1 },
	{ "bmc_72in1",        BMC_72IN1 },
	{ "bmc_76in1",        BMC_76IN1 },
	{ "bmc_150in1",       BMC_150IN1 },
	{ "bmc_190in1",       BMC_190IN1 },
	{ "bmc_500in1",       BMC_500IN1 },
	{ "bmc_800in1",       BMC_800IN1 },
	{ "bmc_1200in1",      BMC_1200IN1 },
	{ "bmc_2751",         BMC_2751 },
	{ "bmc_8157",         BMC_8157 },
	{ "bmc_g146",         BMC_G146 },
	{ "bmc_11160",        BMC_11160 },
	{ "a88s1",            BMC_A88S1 },
	{ "bmc_el86xc",       BMC_EL860947C },
	{ "fk23c",            BMC_FK23C },
	{ "fk23ca",           BMC_FK23CA },
	{ "nt639",            BMC_NT639 },
	{ "resetsxrom",       BMC_RESETSXROM },
	{ "resettxrom",       BMC_RESETTXROM },
	{ "s24in1c03",        BMC_S24IN1SC03 },
	{ "smd133",           SMD133_BOARD },
	{ "tech9in1",         BMC_TECHLINE9IN1 },
	{ "bmc_5in1",         BMC_5IN1_1993 },
	{ "bmc_8in1",         BMC_8IN1 },
	{ "bmc_15in1",        BMC_15IN1 },
	{ "bmc_sbig7in1",     BMC_SUPERBIG_7IN1 },
	{ "bmc_hik8in1",      BMC_HIK8IN1 },
	{ "bmc_jy208",        BMC_JY208 },
	{ "bmc_jy302",        BMC_JY302 },
	{ "bmc_kc885",        BMC_KC885 },
	{ "bmc_sfc12",        BMC_SFC12 },
	{ "bmc_hik4in1",      BMC_SUPERHIK_4IN1 },
	{ "bmc_f15",          BMC_F15 },
	{ "bmc_f600",         BMC_F600 },
	{ "bmc_gn45",         BMC_GN45 },
	{ "bmc_gold7in1",     BMC_GOLD_7IN1 },
	{ "bmc_l6in1",        BMC_L6IN1 },
	{ "bmc_00202650",     BMC_00202650 },
	{ "bmc_411120c",      BMC_411120C },
	{ "bmc_810305c",      BMC_810305C },
	{ "bmc_820720c",      BMC_820720C },
	{ "bmc_830118c",      BMC_830118C },
	{ "bmc_830832c",      BMC_830832C },
	{ "bmc_yy841101c",    BMC_YY841101C },
	{ "bmc_yy841155c",    BMC_YY841155C },
	{ "pjoy84",           BMC_PJOY84 },
	{ "bmc_gold260",      BMC_GOLD260 },
	{ "bmc_th22913",      BMC_TH22913 },
	{ "bmc_reset4",       BMC_4IN1RESET },
	{ "bmc_reset42",      BMC_42IN1RESET },
	{ "bmc_nc20mb",       BMC_NC20MB },
	{ "bmc_lc160",        BMC_LITTLECOM160 },
	{ "jncota_kt1001",    JNCOTA_KT1001 },
	{ "jyc_a",            JYCOMPANY_A },
	{ "jyc_b",            JYCOMPANY_B },
	{ "jyc_c",            JYCOMPANY_C },
	{ "tek90",            JYCOMPANY_A },
	{ "sa9602b",          SACHEN_SA9602B },
	{ "sachen_shero",     SACHEN_SHERO },
	{ "sachen_zgdh",      SACHEN_ZGDH },
	{ "a9746",            UNL_A9746 },
	{ "mmalee2",          UNL_MMALEE },
	{ "unl_2708",         UNL_2708 },
	{ "unl_dh08",         UNL_DH08 },
	{ "unl_le05",         UNL_LE05 },
	{ "unl_lg25",         UNL_LG25 },
	{ "unl_lh10",         UNL_LH10 },
	{ "unl_lh28_lh54",    UNL_LH28_LH54 },
	{ "unl_lh31",         UNL_LH31 },
	{ "unl_lh32",         UNL_LH32 },
	{ "unl_lh42",         UNL_LH42 },
	{ "unl_lh51",         UNL_LH51 },
	{ "unl_lh53",         UNL_LH53 },
	{ "unl_ac08",         UNL_AC08 },
	{ "unl_malisb",       UNL_MALISB },
	{ "rt01",             UNL_RT01 },   // Russian Test Cart
	{ "unl_43272",        UNL_43272 },
	{ "ninjaryu",         UNL_NINJARYU },
	{ "unl_eh8813a",      UNL_EH8813A },  // Dr. Mario II
	{ "nocash_nochr",     NOCASH_NOCHR },
	{ "action53",         UNL_ACTION53 },
	{ "batmap_000",       BATMAP_000 },
	{ "batmap_srrx",      BATMAP_SRRX },
	{ "8bitxmas",         SEALIE_8BITXMAS },
	{ "cufrom",           SEALIE_CUFROM },
	{ "dpcmcart",         SEALIE_DPCMCART },
	{ "unrom512",         SEALIE_UNROM512 },
	{ "2a03pur",          UNL_2A03PURITANS },
	{ "farid_slrom8in1",  FARID_SLROM8IN1 },
	{ "farid_unrom8in1",  FARID_UNROM8IN1 },
	{ "ffe3",             FFE3_BOARD },
	{ "ffe4",             FFE4_BOARD },
	{ "ffe8",             FFE8_BOARD },
	{ "unl_dance",        UNSUPPORTED_BOARD },
	{ "onebus",           UNSUPPORTED_BOARD },
	{ "pec586",           UNSUPPORTED_BOARD },
	{ "test",             TEST_BOARD },
	{ "unknown",          UNKNOWN_BOARD }  //  a few pirate dumps uses the wrong mapper...
};

static const nes_pcb *nes_pcb_lookup( const char *slot )
{
	for (auto & elem : pcb_list)
	{
		if (!strcmp(elem.slot_opt, slot))
			return &elem;
	}
	return nullptr;
}

static const nes_pcb *nes_id_lookup( int id )
{
	for (auto & elem : pcb_list)
	{
		if (elem.pcb_id == id)
			return &elem;
	}
	return nullptr;
}

int nes_cart_slot_device::nes_get_pcb_id( const char *slot )
{
	const nes_pcb *pcb = nes_pcb_lookup(slot);

	if (pcb == nullptr)
		fatalerror("Unimplemented PCB type %s\n", slot);

	return pcb->pcb_id;
}


const char * nes_cart_slot_device::nes_get_slot( int pcb_id )
{
	const nes_pcb *pcb = nes_id_lookup(pcb_id);

	if (pcb == nullptr)
		fatalerror("Unimplemented PCB ID %d\n", pcb_id);

	return pcb->slot_opt;
}


/*************************************************************

 nes_pcb_reset

 Resets the mmc bankswitch areas to their defaults.
 It returns a value "err" that indicates if it was
 successful. Possible values for err are:

 0 = success
 1 = no pcb found
 2 = pcb not supported

 *************************************************************/

struct nes_cart_lines
{
	const char *tag;
	int line;
};

static const struct nes_cart_lines nes_cart_lines_table[] =
{
	{ "PRG A0",    0 },
	{ "PRG A1",    1 },
	{ "PRG A2",    2 },
	{ "PRG A3",    3 },
	{ "PRG A4",    4 },
	{ "PRG A5",    5 },
	{ "PRG A6",    6 },
	{ "PRG A7",    7 },
	{ "CHR A10",  10 },
	{ "CHR A11",  11 },
	{ "CHR A12",  12 },
	{ "CHR A13",  13 },
	{ "CHR A14",  14 },
	{ "CHR A15",  15 },
	{ "CHR A16",  16 },
	{ "CHR A17",  17 },
	{ "NC",      127 },
	{ nullptr }
};

static int nes_cart_get_line( const char *feature )
{
	const struct nes_cart_lines *nes_line = &nes_cart_lines_table[0];

	if (feature == nullptr)
		return 128;

	while (nes_line->tag)
	{
		if (strcmp(nes_line->tag, feature) == 0)
			break;

		nes_line++;
	}

	return nes_line->line;
}

struct n163_vol_lines
{
	const char *tag;
	int line;
};

static const struct n163_vol_lines n163_vol_table[] =
{
	{ "SUBMAPPER 0",    0 },
	{ "SUBMAPPER 1",    1 },
	{ "SUBMAPPER 2",    2 },
	{ "SUBMAPPER 3",    3 },
	{ "SUBMAPPER 4",    4 },
	{ "SUBMAPPER 5",    5 },
	{ nullptr }
};

static int n163_get_submapper_num( const char *feature )
{
	const struct n163_vol_lines *n163_line = &n163_vol_table[0];

	if (feature == nullptr)
		return 128;

	while (n163_line->tag)
	{
		if (strcmp(n163_line->tag, feature) == 0)
			break;

		n163_line++;
	}

	return n163_line->line;
}

void nes_cart_slot_device::call_load_pcb()
{
	uint32_t vram_size = 0, prgram_size = 0, battery_size = 0, mapper_sram_size = 0;
	// SETUP step 1: getting PRG, VROM, VRAM sizes
	uint32_t prg_size = get_software_region_length("prg");
	uint32_t vrom_size = get_software_region_length("chr");
	vram_size = get_software_region_length("vram");
	vram_size += get_software_region_length("vram2");

	// validate the xml fields
	if (!prg_size)
		fatalerror("No PRG entry for this software! Please check if the xml list got corrupted\n");
	if (prg_size < 0x8000)
		fatalerror("PRG entry is too small! Please check if the xml list got corrupted\n");

	// SETUP step 2: getting PCB and other settings
	if (get_feature("slot"))
		m_pcb_id = nes_get_pcb_id(get_feature("slot"));
	else
		m_pcb_id = NO_BOARD;

	// SETUP step 3: storing the info needed for emulation
	if (get_software_region("bwram") != nullptr)
		battery_size = get_software_region_length("bwram");

	if (get_software_region("wram") != nullptr)
		prgram_size = get_software_region_length("wram");

	if (get_feature("mirroring"))
	{
		const char *mirroring = get_feature("mirroring");
		if (!strcmp(mirroring, "horizontal"))
			m_cart->set_mirroring(PPU_MIRROR_HORZ);
		if (!strcmp(mirroring, "vertical"))
			m_cart->set_mirroring(PPU_MIRROR_VERT);
		if (!strcmp(mirroring, "high"))
			m_cart->set_mirroring(PPU_MIRROR_HIGH);
		if (!strcmp(mirroring, "low"))
			m_cart->set_mirroring(PPU_MIRROR_LOW);
		if (!strcmp(mirroring, "4screen"))
		{
			// A few boards uses 4-screen mirroring: Gauntlet (DDROM or TRR1ROM or Tengen 800004),
			// Rad Racer II (TVROM), and Napoleon Senki (IREM LROG017 with 74*161/161/21/138)
			m_cart->set_four_screen_vram(true);
			m_cart->set_mirroring(PPU_MIRROR_4SCREEN);
		}
		if (!strcmp(mirroring, "pcb_controlled"))
		{
			// A few boards have variants with hardcoded mirroring and variants with mapper
			// controlled mirroring. We use a variable to avoid the need of dupe devices.
			// See e.g. HES 6-in-1 vs other HES games, Irem Major League vs other G-101 games,
			// Sunsoft-2 Shanghai vs Mito Koumon, Camerica BF9093 games vs BF9097 games, etc.
			// Boards where all games control mirroring do not make real use of this.
			m_cart->set_pcb_ctrl_mirror(true);
		}
	}

	/* Check for pins in specific boards which require them */
	if (m_pcb_id == STD_CNROM)
	{
		int mask = 0, state = 0;

		if (get_feature("chr-pin26") != nullptr)
		{
			mask |= 0x01;
			state |= !strcmp(get_feature("chr-pin26"), "CE") ? 0x01 : 0;
		}
		if (get_feature("chr-pin27") != nullptr)
		{
			mask |= 0x02;
			state |= !strcmp(get_feature("chr-pin27"), "CE") ? 0x02 : 0;
		}

		m_cart->set_ce(mask, state);
	}

	if (m_pcb_id == TAITO_X1_005 && get_feature("x1-pin17") != nullptr && get_feature("x1-pin31") != nullptr)
	{
		if (!strcmp(get_feature("x1-pin17"), "CIRAM A10") && !strcmp(get_feature("x1-pin31"), "NC"))
			m_cart->set_x1_005_alt(true);
	}

	if (m_pcb_id == KONAMI_VRC2)
	{
		m_cart->set_vrc_lines(nes_cart_get_line(get_feature("vrc2-pin3")),
						nes_cart_get_line(get_feature("vrc2-pin4")),
						(nes_cart_get_line(get_feature("vrc2-pin21")) != 10) ? 1 : 0);
//      osd_printf_error("VRC-2, pin3: A%d, pin4: A%d, pin21: %d\n", nes_cart_get_line(get_feature("vrc2-pin3")), nes_cart_get_line(get_feature("vrc2-pin4")),
//                              nes_cart_get_line(get_feature("vrc2-pin21")));
	}

	if (m_pcb_id == KONAMI_VRC4)
	{
		m_cart->set_vrc_lines(nes_cart_get_line(get_feature("vrc4-pin3")),
						nes_cart_get_line(get_feature("vrc4-pin4")),
						0);
//      osd_printf_error("VRC-4, pin3: A%d, pin4: A%d\n", nes_cart_get_line(get_feature("vrc4-pin3"), nes_cart_get_line(get_feature("vrc4-pin4"));
	}

	if (m_pcb_id == KONAMI_VRC6)
	{
		m_cart->set_vrc_lines(nes_cart_get_line(get_feature("vrc6-pin9")),
						nes_cart_get_line(get_feature("vrc6-pin10")),
						0);
//      osd_printf_error("VRC-6, pin9: A%d, pin10: A%d\n", nes_cart_get_line(get_feature("vrc6-pin9"), nes_cart_get_line(get_feature("vrc6-pin10"));
	}

	if (m_pcb_id == KONAMI_VRC7)
		m_cart->set_vrc_lines(nes_cart_get_line(get_feature("vrc7-pin19")), 0, 0);

	if (m_pcb_id == STD_SXROM || m_pcb_id == STD_SOROM)
	{
		if (get_feature("mmc1_type") != nullptr)
		{
			using mmc1_type = device_nes_cart_interface::mmc1_type;

			const char *type = get_feature("mmc1_type");
			if (!strcmp(type, "MMC1"))
				m_cart->set_mmc1_type(mmc1_type::MMC1);
			else if (!strcmp(type, "MMC1A"))
				m_cart->set_mmc1_type(mmc1_type::MMC1A);
			else if (!strncmp(type, "MMC1B", 5)) // common prefix of several variants
				m_cart->set_mmc1_type(mmc1_type::MMC1B);
		}
	}

	if (m_pcb_id == STD_HKROM || m_pcb_id == TAITO_X1_017)
		mapper_sram_size = m_cart->get_mapper_sram_size();

	if (m_pcb_id == TAITO_X1_005 || m_pcb_id == NAMCOT_163)
	{
		if (get_feature("batt"))
			mapper_sram_size = m_cart->get_mapper_sram_size();
	}

	if (m_pcb_id == NAMCOT_163)
	{
		if (get_feature("n163-vol"))
			m_cart->set_n163_vol(n163_get_submapper_num(get_feature("n163-vol")));
	}

	// get address of SMD133 chip on boards that use it
	if (m_pcb_id == SMD133_BOARD)
	{
		const char *addr = get_feature("smd133-addr");
		int val = 6;

		if (addr == nullptr || sscanf(addr, "%u%*c", &val) != 1 || val < 5 || val > 6)
			logerror("Unexpected smd133-addr: %s\n", addr ? addr : "(nullptr)");
		m_cart->set_smd133_addr(val * 0x1000);
	}

	// get outer PRG bank size for multicart boards that use this feature
	if (m_pcb_id == BMC_EL860947C || m_pcb_id == BMC_RESETTXROM)
	{
		const char *size = get_feature("outer-prg-size");
		int kbyte = 128;

		if (size == nullptr || sscanf(size, "%u%*c", &kbyte) != 1 || kbyte & (kbyte - 1) || kbyte <= 0)
			logerror("Unexpected outer-prg-size: %s\n", size ? size : "(nullptr)");
		m_cart->set_outer_prg_size(kbyte);
	}

	// get outer CHR bank size for multicart boards that use this feature
	if (m_pcb_id == BMC_RESETTXROM)
	{
		const char *size = get_feature("outer-chr-size");
		int kbyte = 128;

		if (size == nullptr || sscanf(size, "%u%*c", &kbyte) != 1 || kbyte & (kbyte - 1) || kbyte <= 0)
			logerror("Unexpected outer-chr-size: %s\n", size ? size : "(nullptr)");
		m_cart->set_outer_chr_size(kbyte);
	}


	// pirate variants of boards with bus conflict are often not suffering from it
	// and actually games glitch if bus conflict is emulated...
	if (get_feature("bus_conflict") && !strcmp(get_feature("bus_conflict"), "no"))
		m_cart->set_bus_conflict(false);


	// SETUP step 4: logging what we have found
	logerror("Loaded game from softlist:\n");
	if (get_feature("pcb"))
	{
		logerror("-- PCB: %s", get_feature("pcb"));
		if (m_pcb_id == UNSUPPORTED_BOARD)
			logerror(" (currently not supported by MAME)");
		logerror("\n");
	}
	logerror("-- PRG 0x%x (%d x 16k chunks)\n", prg_size, prg_size / 0x4000);
	logerror("-- VROM 0x%x (%d x 8k chunks)\n", vrom_size, vrom_size / 0x2000);
	logerror("-- VRAM 0x%x (%d x 8k chunks)\n", vram_size, vram_size / 0x2000);
	logerror("-- PRG NVWRAM: %d\n", battery_size + mapper_sram_size);
	logerror("-- PRG WRAM: %d\n",  prgram_size);

	// SETUP steps 5/6: allocate pointers for PRG/VROM and load the data!
	m_cart->prg_alloc(prg_size);
	memcpy(m_cart->get_prg_base(), get_software_region("prg"), prg_size);
	if (vrom_size)
	{
		m_cart->vrom_alloc(vrom_size);
		memcpy(m_cart->get_vrom_base(), get_software_region("chr"), vrom_size);
	}

	// SETUP steps 7: allocate pointers for any extra chip data, when needed
	if (m_pcb_id == BATMAP_SRRX)
	{
		uint32_t dpcm_size = get_software_region_length("dpcm");
		if (dpcm_size)
		{
			m_cart->misc_rom_alloc(dpcm_size);
			memcpy(m_cart->get_misc_rom_base(), get_software_region("dpcm"), dpcm_size);
		}
	}

	// SETUP steps 8: allocate the remaining pointers, when needed
	if (vram_size)
		m_cart->vram_alloc(vram_size);
	if (prgram_size)
		m_cart->prgram_alloc(prgram_size);

	// Attempt to load a battery file for this ROM
	// A few boards have internal RAM with a battery (MMC6, Taito X1-005 & X1-017, etc.)
	if (battery_size || mapper_sram_size)
	{
		uint32_t tot_size = battery_size + mapper_sram_size;
		std::vector<uint8_t> temp_nvram(tot_size);

		// some games relies on specific battery patterns to work
		// (e.g. Silva Saga does not work with SRAM fully initialized to 0x00)
		// and we use the info from xml here to prepare a default NVRAM
		std::vector<uint8_t> default_nvram(tot_size);
		if (battery_size)
			memcpy(&default_nvram[0], get_software_region("bwram"), battery_size);
		if (mapper_sram_size)
			memset(&default_nvram[battery_size], 0, mapper_sram_size);

		// load battery (using default if no battery exists)
		battery_load(&temp_nvram[0], tot_size, &default_nvram[0]);

		// copy battery into PCB arrays
		if (battery_size)
		{
			m_cart->battery_alloc(battery_size);
			memcpy(m_cart->get_battery_base(), &temp_nvram[0], battery_size);
		}
		if (mapper_sram_size)
			memcpy(m_cart->get_mapper_sram_base(), &temp_nvram[battery_size], mapper_sram_size);
	}
}
