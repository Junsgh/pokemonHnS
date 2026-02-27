#ifndef GUARD_MOM_SAVINGS_H
#define GUARD_MOM_SAVINGS_H

#include "global.h"

// Constants
#define MOM_RANDOM_THRESHOLD 2300   // Every $2300 triggers random berry purchase
#define MOM_BERRY_QUANTITY 5        // Mom buys 5 berries at a time
#define MOM_BERRY_COST 100          // Cost per berry purchase
#define MOM_MAX_MONEY 999999        // Maximum money mom can hold

// SaveBlock structure for Mom's savings data
struct MomSavingsData
{
    u32 momsMoney;           // Mom's current savings balance
    u16 normalGiftFlags;     // Bitflags for which sequential items purchased (up to 16)
    u8 isSavingMoney;        // Boolean: is the feature active?
    u8 isInitialized;        // Boolean: is the savings feature initialized?
};

// Item table structure for sequential gifts
struct MomGiftSequential
{
    u16 itemId;         // Item or decoration ID to purchase
    u32 threshold;      // When balance reaches this amount
    u16 cost;           // Cost to deduct from savings
    bool8 isDecoration; // TRUE if this is a decoration, FALSE if item
};

// Public function declarations
void Mom_EnableSaving(bool8 enable);
bool8 Mom_IsSavingEnabled(void);
u32 Mom_GetBalance(void);
bool8 Mom_TryDepositMoney(u32 amount);
bool8 Mom_AutoDepositFromBattle(u32 amount);
bool8 Mom_TryWithdrawMoney(u32 amount);
bool8 Mom_CheckForGiftPurchase(u32 newBalance, u32 oldBalance, bool8 isAutomatic);
void Mom_EnsureInitialized(void);
void InitMomSavings(void);
bool8 Mom_TryTriggerGiftCall(void);
void Special_MomOpenDepositInput(void);
void Special_MomOpenWithdrawInput(void);

#endif // GUARD_MOM_SAVINGS_H