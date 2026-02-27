#include "global.h"
#include "mom_savings.h"
#include "main.h"
#include "item.h"
#include "random.h"
#include "constants/items.h"
#include "constants/decorations.h"
#include "constants/vars.h"
#include "event_data.h"
#include "string_util.h"
#include "strings.h"
#include "script.h"
#include "menu_helpers.h"
#include "money.h"
#include "menu.h"
#include "task.h"
#include "sound.h"
#include "constants/songs.h"
#include "text_window.h"
#include "palette.h"
#include "window.h"
#include "bg.h"
#include "fieldmap.h"
#include "constants/map_types.h"
#include "decoration_inventory.h"

extern const u8 EventScript_MomGiftCall_Item[];
extern const u8 EventScript_MomGiftCall_Berry[];
extern const u8 EventScript_MomGiftCall_Decoration[];

// Player wallet maximum (must match MAX_MONEY in money.c)
#define MAX_PLAYER_MONEY 9999999

// Special marker for dynamic gifts
#define MOM_GIFT_STARTER_DOLL 0xFFFF  // Placeholder for starter-based doll selection

// Sequential gift table (purchased in order, one-time only)
// These items are purchased when the player's savings reach specific thresholds
static const struct MomGiftSequential sMomGifts_Sequential[] = {
    {ITEM_SUPER_POTION,      900,    600,   FALSE},
    {ITEM_REPEL,             4000,   270,   FALSE},
    {ITEM_SILK_SCARF,        7000,   600,   FALSE},
    {MOM_GIFT_STARTER_DOLL,  10000,  1000,   TRUE},   // Dynamic: gives doll matching player's starter
    {ITEM_MOON_STONE,        15000,  2100,  FALSE},
    {ITEM_HYPER_POTION,      19000,  1200,  FALSE},
    {ITEM_LEFTOVERS,         30000,  1500,  FALSE},
    {ITEM_CHOICE_BAND,       40000,  100,   FALSE},
    {DECOR_SNORLAX_DOLL,     50000,  22800, TRUE},
    {ITEM_MASTER_BALL,       100000, 50000, FALSE},
    // Add more items as desired (max 16 due to u16 bitflags)
};
/* 
HGSS ITEM LIST (for reference)
Trigger	Cost	Item
$900	$600	Super Potion
$4000	$270	Repel
$7000	$600	Super Potion
$10000	$100	Silk Scarf
$15000	$3000	Moon Stone
$19000	$900	Hyper Potion
$30000	$200	Choice Scarf
$40000	$200	Muscle Band
$50000	$200	Focus Sash
*/

#define MOM_ITEMS_SEQUENTIAL_COUNT ARRAY_COUNT(sMomGifts_Sequential)

// Berry gift list (purchased randomly at multiples of threshold)
// Mom will randomly select one berry from this list every $2300
// This list consists of berries that are used by Kurt in Azalea Town
static const u16 sMomGifts_Berries[] = {
    ITEM_PECHA_BERRY,
    ITEM_RAWST_BERRY,
    ITEM_CHERI_BERRY,
    ITEM_ORAN_BERRY,
    ITEM_CHESTO_BERRY,
    ITEM_ASPEAR_BERRY,
    ITEM_PERSIM_BERRY,
};

#define MOM_BERRIES_COUNT ARRAY_COUNT(sMomGifts_Berries)

static bool8 Mom_CheckSequentialGifts(u32 balance, u16 *purchasedItem);
static bool8 Mom_CheckRandomBerries(u32 newBalance, u32 oldBalance, u16 *purchasedItem);
static void Mom_AddItemToPC(u16 itemId, u16 quantity, bool8 isDecoration);

// Initialize mom's savings data (called on new game)
void InitMomSavings(void)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    mom->momsMoney = 0;
    mom->normalGiftFlags = 0;
    mom->isSavingMoney = FALSE;
    mom->isInitialized = TRUE;
}

void Mom_EnsureInitialized(void)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;

    if (mom->isInitialized == TRUE)
        return;

    mom->momsMoney = 0;
    mom->normalGiftFlags = 0;
    mom->isSavingMoney = FALSE;
    mom->isInitialized = TRUE;
}

// Enable or disable mom's automatic saving feature
void Mom_EnableSaving(bool8 enable)
{
    gSaveBlock1Ptr->momSavings.isSavingMoney = enable;
}

// Check if mom's saving feature is currently enabled
bool8 Mom_IsSavingEnabled(void)
{
    return gSaveBlock1Ptr->momSavings.isSavingMoney;
}

// Get the current balance of mom's savings
u32 Mom_GetBalance(void)
{
    return gSaveBlock1Ptr->momSavings.momsMoney;
}

// Attempt to deposit money into mom's savings
// isAutomatic: TRUE if from battle winnings, FALSE if manual deposit
// Returns TRUE if successful
bool8 Mom_TryDepositMoney(u32 amount)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Store old balance for tier comparison
    u32 oldBalance = mom->momsMoney;
    
    // Add money to savings
    mom->momsMoney += amount;
    
    // Check if balance would overflow (max is 999999)
    if (mom->momsMoney > MOM_MAX_MONEY)
        mom->momsMoney = MOM_MAX_MONEY;
    
    // Check if any gifts should be purchased
    // Note: Berries are only purchased from automatic battle savings, not manual deposits
    Mom_CheckForGiftPurchase(mom->momsMoney, oldBalance, FALSE);
    
    return TRUE;
}

// Deposit money from automatic battle savings
// This is called after winning a battle when savings is enabled
bool8 Mom_AutoDepositFromBattle(u32 amount)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Store old balance for tier comparison
    u32 oldBalance = mom->momsMoney;
    
    // Add money to savings
    mom->momsMoney += amount;
    
    // Check if balance would overflow (max is 999999)
    if (mom->momsMoney > MOM_MAX_MONEY)
        mom->momsMoney = MOM_MAX_MONEY;
    
    // Check if any gifts should be purchased
    // Berries can be purchased from automatic deposits
    Mom_CheckForGiftPurchase(mom->momsMoney, oldBalance, TRUE);
    
    return TRUE;
}

// Attempt to withdraw money from mom's savings
// Returns TRUE if successful, FALSE if insufficient funds
bool8 Mom_TryWithdrawMoney(u32 amount)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Check if enough funds
    if (mom->momsMoney < amount)
        return FALSE;
    
    // Withdraw money
    mom->momsMoney -= amount;
    
    return TRUE;
}

// Check if mom should purchase a gift based on the new balance
// This is called after depositing money
// isAutomatic: TRUE if from automatic battle savings, FALSE if manual deposit
// Returns TRUE if a gift was purchased
bool8 Mom_CheckForGiftPurchase(u32 newBalance, u32 oldBalance, bool8 isAutomatic)
{
    u16 purchasedItem = ITEM_NONE;
    u16 quantity = 1;
    bool8 isDecoration = FALSE;
    
    // Don't purchase new gifts if there's already a pending gift waiting
    // This prevents the new gift from overwriting the pending one
    if (FlagGet(FLAG_MOM_HAS_GIFT))
        return FALSE;
    
    // Try sequential gifts first
    // Sequential gifts are one-time purchases at specific thresholds
    if (Mom_CheckSequentialGifts(newBalance, &purchasedItem))
    {
        // Check if this is the dynamic starter doll gift
        if (purchasedItem == MOM_GIFT_STARTER_DOLL)
        {
            // Select the appropriate doll based on the player's starter
            u16 starterChoice = VarGet(VAR_STARTER_MON);
            switch (starterChoice)
            {
                case 0:  // Chikorita
                    purchasedItem = DECOR_CHIKORITA_DOLL;
                    break;
                case 1:  // Cyndaquil
                    purchasedItem = DECOR_CYNDAQUIL_DOLL;
                    break;
                case 2:  // Totodile
                    purchasedItem = DECOR_TOTODILE_DOLL;
                    break;
                default: // Fallback to Togepi if unknown
                    purchasedItem = DECOR_TOGEPI_DOLL;
                    break;
            }
            isDecoration = TRUE;
        }
        else
        {
            // Check if this gift is a decoration
            for (u32 i = 0; i < MOM_ITEMS_SEQUENTIAL_COUNT; i++)
            {
                if (sMomGifts_Sequential[i].itemId == purchasedItem)
                {
                    isDecoration = sMomGifts_Sequential[i].isDecoration;
                    break;
                }
            }
        }
        
        quantity = 1;
        Mom_AddItemToPC(purchasedItem, quantity, isDecoration);
        
        // Set flag/var to trigger mom's phone call
        VarSet(VAR_MOM_GIFT_ITEM, purchasedItem);
        // Use quantity 0 to signal decoration gift (decorations don't have quantities)
        VarSet(VAR_MOM_GIFT_QUANTITY, isDecoration ? 0 : quantity);
        FlagSet(FLAG_MOM_HAS_GIFT);
        
        return TRUE;
    }
    
    // If no sequential items triggered, try random berries
    // Berry gifts are recurring purchases at multiples of the threshold
    // BUT only from automatic battle savings, not manual deposits
    if (isAutomatic && Mom_CheckRandomBerries(newBalance, oldBalance, &purchasedItem))
    {
        quantity = MOM_BERRY_QUANTITY;
        Mom_AddItemToPC(purchasedItem, quantity, FALSE);
        
        // Set flag/var to trigger mom's phone call
        VarSet(VAR_MOM_GIFT_ITEM, purchasedItem);
        VarSet(VAR_MOM_GIFT_QUANTITY, quantity);
        FlagSet(FLAG_MOM_HAS_GIFT);
        
        return TRUE;
    }
    
    return FALSE;
}

// ===== Internal Functions =====

// Check if any sequential gifts should be purchased
// Sequential gifts are purchased in order, one at a time, when thresholds are met
// Returns TRUE if an item was purchased
static bool8 Mom_CheckSequentialGifts(u32 balance, u16 *purchasedItem)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Loop through all sequential items
    for (u32 i = 0; i < MOM_ITEMS_SEQUENTIAL_COUNT; i++)
    {
        // Skip if already purchased (check bit flag)
        if (mom->normalGiftFlags & (1 << i))
            continue;
        
        // Skip if can't afford yet
        if (balance < sMomGifts_Sequential[i].threshold)
            continue;
        
        // Purchase this item!
        // Set the bit flag to mark this item as purchased
        mom->normalGiftFlags |= (1 << i);
        
        // Deduct the cost from savings
        mom->momsMoney -= sMomGifts_Sequential[i].cost;
        
        // Return the item that was purchased
        *purchasedItem = sMomGifts_Sequential[i].itemId;
        
        return TRUE;  // Only one purchase per deposit
    }
    
    return FALSE;  // No items were affordable or all already purchased
}

// Check if a random berry should be purchased
// Berries are purchased when the player crosses into a new "tier" (multiple of threshold)
// Returns TRUE if a berry was purchased
static bool8 Mom_CheckRandomBerries(u32 newBalance, u32 oldBalance, u16 *purchasedItem)
{
    struct MomSavingsData *mom = &gSaveBlock1Ptr->momSavings;
    
    // Calculate which "tier" we're in
    // Tier 0 = $0-2299, Tier 1 = $2300-4599, Tier 2 = $4600-6899, etc.
    u32 newTier = newBalance / MOM_RANDOM_THRESHOLD;
    u32 oldTier = oldBalance / MOM_RANDOM_THRESHOLD;
    
    // Only purchase if we crossed into a new tier
    // This prevents re-purchasing when withdrawing money
    if (newTier <= oldTier)
        return FALSE;
    
    // Select random berry from list
    u32 randomIndex = Random() % MOM_BERRIES_COUNT;
    u16 berryId = sMomGifts_Berries[randomIndex];
    
    // Deduct cost
    mom->momsMoney -= MOM_BERRY_COST;
    
    // Return the berry ID
    *purchasedItem = berryId;
    
    return TRUE;
}

static bool8 Mom_GiftIsBerry(void)
{
    u16 item = VarGet(VAR_MOM_GIFT_ITEM);
    return (item >= ITEM_CHERI_BERRY && item <= ITEM_ENIGMA_BERRY);
}

static bool8 Mom_GiftIsDecoration(void)
{
    // Decorations are signaled by quantity == 0
    return (VarGet(VAR_MOM_GIFT_QUANTITY) == 0);
}

bool8 Mom_TryTriggerGiftCall(void)
{
    u8 mapType = gMapHeader.mapType;
    
    // No gift waiting
    if (!FlagGet(FLAG_MOM_HAS_GIFT))
        return FALSE;
    
    // Only call when player is outdoors on a route, city, or town
    // Don't interrupt when indoors or underground
    if (mapType != MAP_TYPE_ROUTE && mapType != MAP_TYPE_OCEAN_ROUTE && mapType != MAP_TYPE_CITY && mapType != MAP_TYPE_TOWN)
        return FALSE;
    
    // All conditions met - trigger the appropriate call!
    if (Mom_GiftIsDecoration())
        ScriptContext_SetupScript(EventScript_MomGiftCall_Decoration);
    else if (Mom_GiftIsBerry())
        ScriptContext_SetupScript(EventScript_MomGiftCall_Berry);
    else
        ScriptContext_SetupScript(EventScript_MomGiftCall_Item);

    return TRUE;
}

// ===== Money Input UI =====

#define MOM_MAX_INPUT        999999

// Window IDs for mom savings UI
enum {
    WIN_MOM_MONEY,
    WIN_MOM_INPUT,
    WIN_MOM_MESSAGE,
};

// Window templates matching shop UI layout
static const struct WindowTemplate sMomInputWindowTemplates[] = {
    [WIN_MOM_MONEY] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 10,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x001E,
    },
    [WIN_MOM_INPUT] = {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 11,
        .width = 6,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x018E,
    },
    [WIN_MOM_MESSAGE] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 27,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x01A2,
    },
    DUMMY_WIN_TEMPLATE
};

// Static variables for large money amounts (can't fit in s16 task data)
static u32 sMomInputAmount = 0;
static u32 sMomInputMaxAmount = 0;

// Task data aliases (tAmount and tMaxAmount replaced by static variables above)
#define tIsDeposit      data[0]
#define tWindowMoney    data[1]
#define tWindowInput    data[2]
#define tWindowMessage  data[3]

static void Task_MomInput_ShowMessage(u8 taskId);
static void Task_MomInput_InitAmountDialogue(u8 taskId);
static void Task_MomInput_HandleInput(u8 taskId);
static void MomInput_CleanupWindows(u8 taskId);

static void MomInput_DisplayMessage(u8 taskId, const u8 *text, TaskFunc callback)
{
    s16 *data = gTasks[taskId].data;
    DisplayMessageAndContinueTask(taskId, tWindowMessage, 10, 14, FONT_NORMAL, GetPlayerTextSpeedDelay(), text, callback);
    ScheduleBgCopyTilemapToVram(0);
}

static void MomInput_PrintAmount(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    FillWindowPixelBuffer(tWindowInput, PIXEL_FILL(1));
    PrintMoneyAmount(tWindowInput, 0, 1, sMomInputAmount, 0);
}

static void Task_MomInput_ShowMessage(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    const u8 *text = tIsDeposit ? gText_MomHowMuchDeposit : gText_MomHowMuchWithdraw;
    MomInput_DisplayMessage(taskId, text, Task_MomInput_InitAmountDialogue);
}

static void Task_MomInput_InitAmountDialogue(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    // Create input window with frame
    DrawStdFrameWithCustomTileAndPalette(tWindowInput, FALSE, 1, 13);
    MomInput_PrintAmount(taskId);
    ScheduleBgCopyTilemapToVram(0);

    gTasks[taskId].func = Task_MomInput_HandleInput;
}

static void Task_MomInput_HandleInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (AdjustQuantityAccordingToDPadInput_MomVersion(&sMomInputAmount, sMomInputMaxAmount) == TRUE)
    {
        MomInput_PrintAmount(taskId);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_0x8000 = sMomInputAmount;

        // Execute deposit or withdraw
        if (tIsDeposit)
        {
            RemoveMoney(&gSaveBlock1Ptr->money, sMomInputAmount);
            Mom_TryDepositMoney(sMomInputAmount);
        }
        else
        {
            Mom_TryWithdrawMoney(sMomInputAmount);
            AddMoney(&gSaveBlock1Ptr->money, sMomInputAmount);
        }

        MomInput_CleanupWindows(taskId);
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_0x8000 = 0;

        MomInput_CleanupWindows(taskId);
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

static void MomInput_CleanupWindows(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    // Remove money sprite label
    RemoveMoneyLabelObject();

    // Clear and remove money window
    ClearStdWindowAndFrameToTransparent(tWindowMoney, FALSE);
    ClearWindowTilemap(tWindowMoney);
    RemoveWindow(tWindowMoney);

    // Clear and remove input window
    ClearStdWindowAndFrameToTransparent(tWindowInput, FALSE);
    ClearWindowTilemap(tWindowInput);
    RemoveWindow(tWindowInput);

    // Clear and remove message window
    ClearDialogWindowAndFrameToTransparent(tWindowMessage, FALSE);
    ClearWindowTilemap(tWindowMessage);
    RemoveWindow(tWindowMessage);

    ScheduleBgCopyTilemapToVram(0);
}

static void MomInput_Open(bool8 isDeposit)
{
    u8 taskId = CreateTask(Task_MomInput_ShowMessage, 8);
    s16 *data = gTasks[taskId].data;

    // Create windows individually using AddWindow to avoid conflicts with script system
    tWindowMoney = AddWindow(&sMomInputWindowTemplates[WIN_MOM_MONEY]);
    tWindowInput = AddWindow(&sMomInputWindowTemplates[WIN_MOM_INPUT]);
    tWindowMessage = AddWindow(&sMomInputWindowTemplates[WIN_MOM_MESSAGE]);

    DeactivateAllTextPrinters();
    LoadUserWindowBorderGfx(tWindowMoney, 1, BG_PLTT_ID(13));
    LoadMessageBoxGfx(tWindowMoney, 0xA, BG_PLTT_ID(14));
    PutWindowTilemap(tWindowMoney);
    PutWindowTilemap(tWindowMessage);

    tIsDeposit = isDeposit;
    sMomInputAmount = 1;

    // Show money box with border and sprite label
    // For deposits: show player's wallet
    // For withdrawals: show mom's balance
    if (isDeposit)
    {
        PrintMoneyAmountInMoneyBoxWithBorder(tWindowMoney, 1, 13, GetMoney(&gSaveBlock1Ptr->money));
    }
    else
    {
        PrintMoneyAmountInMoneyBoxWithBorder(tWindowMoney, 1, 13, Mom_GetBalance());
    }
    AddMoneyLabelObject(19, 11);  // Sprite label at (8*0)+19, (8*0)+11

    // Cap deposit at min(999999, player's wallet, mom's remaining capacity)
    // Cap withdraw at min(9999999, mom's balance, player's remaining wallet capacity)
    if (isDeposit)
    {
        u32 momsRemainingCapacity = MOM_MAX_MONEY - Mom_GetBalance();
        u32 playersAvailableMoney = GetMoney(&gSaveBlock1Ptr->money);
        sMomInputMaxAmount = min(MOM_MAX_INPUT, min(playersAvailableMoney, momsRemainingCapacity));
    }
    else
    {
        u32 playersWalletCapacity = MAX_PLAYER_MONEY - GetMoney(&gSaveBlock1Ptr->money);
        u32 momsAvailableMoney = Mom_GetBalance();
        sMomInputMaxAmount = min(MOM_MAX_INPUT, min(momsAvailableMoney, playersWalletCapacity));
    }

    ScriptContext_Stop();
}

/* Special script functions related to mom's savings (called from event scripts) */

static void Mom_AddItemToPC(u16 itemId, u16 quantity, bool8 isDecoration)
{
    if (isDecoration)
    {
        // It's a decoration - add to decoration inventory
        DecorationAdd(itemId);
    }
    else
    {
        // It's an item - add to PC
        AddPCItem(itemId, quantity);
    }
}

void Special_MomEnableSaving(void)
{
    Mom_EnableSaving(TRUE);
}

void Special_MomDisableSaving(void)
{
    Mom_EnableSaving(FALSE);
}

void Special_MomGetBalance(void)
{
    u32 balance = Mom_GetBalance();
    ConvertIntToDecimalStringN(gStringVar1, balance, STR_CONV_MODE_LEFT_ALIGN, 6);
    StringExpandPlaceholders(gStringVar2, gText_PokedollarVar1);
    gSpecialVar_Result = balance;
}

void Special_MomDeposit(void)
{
    u32 amount = gSpecialVar_0x8000;
    bool8 success = Mom_TryDepositMoney(amount);
    gSpecialVar_Result = success;
}

void Special_MomWithdraw(void)
{
    u32 amount = gSpecialVar_0x8000;
    bool8 success = Mom_TryWithdrawMoney(amount);
    gSpecialVar_Result = success;
}

void Special_MomIsSavingEnabled(void)
{
    gSpecialVar_Result = Mom_IsSavingEnabled();
}

void Special_MomEnsureInitialized(void)
{
    Mom_EnsureInitialized();
}

void Special_MomOpenDepositInput(void)
{
    MomInput_Open(TRUE);
}

void Special_MomOpenWithdrawInput(void)
{
    MomInput_Open(FALSE);
}

#undef tIsDeposit
#undef tWindowMoney
#undef tWindowInput
#undef tWindowMessage
