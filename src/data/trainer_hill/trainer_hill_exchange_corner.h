// ============================================================
// Trainer Hill Courtyard exchange corner data
// Item lists and descriptions for the scrollable multichoice
// shop system used by TrainerHill_Courtyard NPCs.
// ============================================================

// --- Item lists (used for icon display + purchase logic) ---

static const u16 sTrainerHillExchangeCorner_Decor1[] =
{
    DECOR_KISS_POSTER,
    DECOR_KISS_CUSHION,
    DECOR_SMOOCHUM_DOLL,
    DECOR_TOGEPI_DOLL,
    DECOR_MEOWTH_DOLL,
    DECOR_CLEFAIRY_DOLL,
    DECOR_DITTO_DOLL,
    DECOR_CYNDAQUIL_DOLL,
    DECOR_CHIKORITA_DOLL,
    DECOR_TOTODILE_DOLL,
    ITEM_LIST_END
};

static const u16 sTrainerHillExchangeCorner_Decor2[] =
{
    DECOR_LAPRAS_DOLL,
    DECOR_SNORLAX_DOLL,
    DECOR_VENUSAUR_DOLL,
    DECOR_CHARIZARD_DOLL,
    DECOR_BLASTOISE_DOLL,
    ITEM_LIST_END
};

static const u16 sTrainerHillExchangeCorner_Vitamins[] =
{
    ITEM_PROTEIN,
    ITEM_CALCIUM,
    ITEM_IRON,
    ITEM_ZINC,
    ITEM_CARBOS,
    ITEM_HP_UP,
    ITEM_LIST_END
};

static const u16 sTrainerHillExchangeCorner_HoldItems[] =
{
    ITEM_LEFTOVERS,
    ITEM_WHITE_HERB,
    ITEM_QUICK_CLAW,
    ITEM_MENTAL_HERB,
    ITEM_BRIGHT_POWDER,
    ITEM_CHOICE_BAND,
    ITEM_KINGS_ROCK,
    ITEM_FOCUS_BAND,
    ITEM_SCOPE_LENS,
    ITEM_LIST_END
};

// --- Description text pointers (indexed parallel to item lists) ---

extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_KissPosterDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_KissCushionDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_SmoochumDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_TogepiDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_MeowthDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_ClefairyDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_DittoDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_CyndaquilDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_ChikoritaDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_TotodileDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_LargeDollDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_ProteinDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_CalciumDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_IronDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_ZincDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_CarbosDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_HPUpDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_LeftoversDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_WhiteHerbDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_QuickClawDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_MentalHerbDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_BrightpowderDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_ChoiceBandDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_KingsRockDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_FocusBandDesc[];
extern const u8 TrainerHill_Courtyard_ExchangeServiceCorner_Text_ScopeLensDesc[];

static const u8 *const sTrainerHillExchangeCorner_Decor1Descriptions[] =
{
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_KissPosterDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_KissCushionDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_SmoochumDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_TogepiDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_MeowthDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_ClefairyDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_DittoDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_CyndaquilDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_ChikoritaDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_TotodileDollDesc,
    gText_Exit,
};

static const u8 *const sTrainerHillExchangeCorner_Decor2Descriptions[] =
{
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_LargeDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_LargeDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_LargeDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_LargeDollDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_LargeDollDesc,
    gText_Exit,
};

static const u8 *const sTrainerHillExchangeCorner_VitaminsDescriptions[] =
{
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_ProteinDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_CalciumDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_IronDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_ZincDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_CarbosDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_HPUpDesc,
    gText_Exit,
};

static const u8 *const sTrainerHillExchangeCorner_HoldItemsDescriptions[] =
{
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_LeftoversDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_WhiteHerbDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_QuickClawDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_MentalHerbDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_BrightpowderDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_ChoiceBandDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_KingsRockDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_FocusBandDesc,
    TrainerHill_Courtyard_ExchangeServiceCorner_Text_ScopeLensDesc,
    gText_Exit,
};
