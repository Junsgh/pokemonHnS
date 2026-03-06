# Pokémon Heart & Soul – Flag Analysis

Deep analysis of flags that control **zones**, **acquisitions**, and **what’s done** in the game. Based on `include/constants/flags.h` and map scripts.

---

## 1. Zone access (gates, guards, blockages)

### 1.1 Reception Gate (Indigo Junction) – Route 26 North ↔ Kanto / Mt. Silver

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_INDIGOJUNCTION_HIDE_KANTO_GUARD** | **Set** when Snorlax in Vermilion is defeated (wake + battle/catch). | Kanto guard **removed** → can go **east** to Route 22 → Viridian, Pewter, League. |
| **FLAG_INDIGOJUNCTION_HIDE_SILVER_GUARD** | **Set** when you talk to Oak in Pallet Town Lab with **16 badges**. | Silver guard **removed** → can go **west** to Route 28 → Mt. Silver. |

- **Reception Gate script:** Needs 8 Johto badges to let you through at all; then object visibility uses the two flags above for east/west exits.
- **Route 27:** Script clears both guards when `VAR_ROUTE27_STATE` / story allows (Route27 scripts clear both flags in one path).
- **Pallet Town Lab:** Sets `FLAG_INDIGOJUNCTION_HIDE_SILVER_GUARD` when Oak is talked to with 16 badges.

**Your state:** Snorlax done → Kanto guard cleared. Oak with 16 badges → Silver guard cleared. **Both paths from Reception Gate are open.**

---

### 1.2 Elite Four / Indigo Plateau

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_HIDE_LEAGUE_BLACKBELT** | **Set** when Blue is defeated (Viridian Gym). **Cleared** on first Hall of Fame (so you can rechallenge E4). | When **set**: Black Belt in front of E4 **removed** → can enter League. When **cleared**: Black Belt **back** (after first clear). |

- **Viridian Gym (Blue):** On win → `setflag FLAG_HIDE_LEAGUE_BLACKBELT`, `setflag FLAG_HIDE_CERULEANCAVE_GUARD`, `setflag FLAG_HIDE_SEAFOAM_BLAINE`.
- **Hall of Fame (first time):** `clearflag FLAG_HIDE_LEAGUE_BLACKBELT` so the guard returns for rechallenge.

**Your state:** Blue defeated → Black Belt removed → **E4 is enterable.** After first Hall of Fame clear, guard returns but you can still rechallenge.

---

### 1.3 Cerulean Cave (Mewtwo)

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_HIDE_CERULEANCAVE_GUARD** | **Set** when Blue is defeated (Viridian Gym). **Cleared** when you take SS Aqua to Kanto (trip resets Kanto progress). | When **set**: guard at Cerulean Cave **removed** → can enter. |

- **Cerulean City map:** Guard object uses `FLAG_HIDE_CERULEANCAVE_GUARD` (when set, guard is hidden).

**Your state:** Blue defeated → **Cerulean Cave is open** (unless you later take SS Aqua, which clears this and other Kanto flags).

---

### 1.4 Route 19 (south to Fuchsia) – Kingler block

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_DEFEATED_CINNABAR_ISLAND_GYM** | **Set** when Blaine is defeated at **Seafoam Islands Gym**. | When **set**: Kingler line on Route 19 **hidden** → can swim south to Fuchsia. |

- **Route 19 map:** Multiple Kingler objects use this flag; when set they are hidden.
- **Seafoam Islands Gym:** On Blaine defeat → `setflag FLAG_DEFEATED_CINNABAR_ISLAND_GYM`, `setflag FLAG_BADGE15_GET`.

**Your state:** Blaine (Seafoam) defeated → **Route 19 south to Fuchsia is open.**

---

### 1.5 Tin Tower (Ho-Oh) and Whirl Islands (Lugia)

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_HIDE_TINTOWER_GUARD** | **Set** when you get Rainbow Wing from Pewter (after Ho-Oh event at Ecruteak Theater). **Cleared** in Ecruteak Theater story and in Ecruteak City OnTransition in some states. | When **set**: guard in front of Tin Tower **removed**. |
| **FLAG_HIDE_WHIRL_ISLANDS_TENTACRUEL** | **Set** in Pewter City (same scene as Rainbow Wing?). **Cleared** in Ecruteak (city + theater) in some story states. | When **set**: Tentacruel blocking Whirl Islands **removed**. |
| **FLAG_HIDE_WHIRL_ISLANDS_GUARD** | Used for Whirl Islands entrance. | Controls visibility of guard at Whirl Islands. |
| **FLAG_HIDE_TIN_TOWER_KIMONO_GIRLS** / **FLAG_HIDE_WHIRL_ISLANDS_KIMONO_GIRLS** | Theater / story. | Control Kimono Girls for Ho-Oh/Lugia events. |

- **Pewter City:** Gives Rainbow Wing and sets `FLAG_HIDE_TINTOWER_GUARD`, `FLAG_HIDE_WHIRL_ISLANDS_TENTACRUEL`.
- **Ecruteak City / Theater:** Various clears/sets for guard, Tentacruel, and Kimono Girls depending on story (e.g. clear Tin Tower guard and Kimono Girls after Theater event).

**Your state:** Depends on whether you did Pewter Rainbow Wing and Ecruteak Theater; those control Tin Tower and Whirl Islands access.

---

### 1.6 Game clear / post-League (HnS + Hoenn)

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_SYS_GAME_CLEAR** | Set by the **GameClear** special (C/asm), typically on first Hall of Fame completion. | Unlocks: Trainer Hill, Slateport ferry (SS Tidal), Lilycove ferry, Desert Underpass, Trick House puzzle 8, postgame NPCs (mints, Jirachi, etc.). |
| **FLAG_IS_CHAMPION** | **Set** on first Hall of Fame; also in EverGrande Hall of Fame script. | Enables E4 rematches (Will, Koga, Bruno, Karen, Lance), Clair post-League dialogue, and Kanto champ cutscene on later Hall of Fame visits. |
| **FLAG_IS_KANTO_CHAMPION** | **Set** in Hall of Fame `SetGameClearFlags` (rematch clear). | Used for Kanto champion–related state. |

**Your state:** If you’ve cleared the League once, `FLAG_SYS_GAME_CLEAR` and `FLAG_IS_CHAMPION` are set; postgame areas and rematches are available.

---

## 2. Key NPCs and story (Kanto)

### 2.1 Blue (Viridian vs Cinnabar)

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_HIDE_VIRIDIAN_BLUE** | **Cleared** when you talk to Blue on **Cinnabar Island** (SS Aqua arrival). **Set** when you defeat Blue at Viridian Gym. | When **clear**: Blue is **at** Viridian Gym. When **set**: Blue not at gym (e.g. still on Cinnabar). |
| **FLAG_HIDE_CINNABAR_BLUE** | **Set** when you talk to Blue on Cinnabar (he leaves). **Cleared** when you take SS Aqua to Kanto. | When **set**: Blue not on Cinnabar (moved to Viridian). |
| **FLAG_HIDE_DOJO_BLUE** | **Set** after defeating Blue at Viridian Gym (he goes to dojo). **Cleared** by SS Aqua. | When **set**: Blue at dojo (post-Viridian). |

- **SS Aqua 1F:** On Cinnabar arrival: `setflag FLAG_HIDE_VIRIDIAN_BLUE`, `clearflag FLAG_HIDE_CINNABAR_BLUE`, and clears Kanto badges + League/Cerulean Cave flags. So: **talk to Blue on Cinnabar → he appears at Viridian Gym.**

**Your state:** You talked to Blue on Cinnabar and beat him at Viridian → Blue is in dojo; Viridian Gym is “done.”

---

### 2.2 Misty (Route 25 vs Cerulean Gym)

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_HIDE_ROUTE25_MISTY** | **Cleared** when Machine Part is returned to Power Plant. | When **cleared**: Misty **appears** on Route 25; after scene she goes to Cerulean Gym. |

- **Power Plant / Kanto Rocket:** Returning Machine Part sets `FLAG_RETURNED_MACHINE_PART` and clears `FLAG_HIDE_ROUTE25_MISTY`.

**Your state:** Machine Part returned → Misty on Route 25 done → **Misty at Cerulean Gym.**

---

### 2.3 Blaine (Seafoam)

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_HIDE_SEAFOAM_BLAINE** | **Set** when Blue is defeated (Viridian Gym). | Hides “Blaine” duplicate/extra; Blaine himself is at Seafoam Gym. |

**Your state:** Blue defeated → flag set; Blaine at Seafoam is the only one relevant; you’ve beaten him → Cinnabar “gym” done.

---

### 2.4 Snorlax (Vermilion)

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_HIDE_VERMILION_SNORLAX** | Set when Snorlax is defeated/caught (after waking with Kanto Radio). Cleared on Hall of Fame rematch (SetGameClearFlags) to respawn. | When **set**: Snorlax **gone**. |

- **Vermilion City:** Defeating Snorlax sets `FLAG_INDIGOJUNCTION_HIDE_KANTO_GUARD` (Reception Gate east open).

**Your state:** Snorlax caught → Snorlax gone, Kanto guard cleared.

---

## 3. Key items and story (what can be acquired)

### 3.1 Machine Part / Power Plant / Magnet Train / Radio

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_RETURNED_MACHINE_PART** | Set when you return Machine Part to Power Plant. | Magnet Train (Saffron) runs; Copycat’s Clefairy Doll available; Lavender Radio Station can give Kanto Radio; Misty appears on Route 25. |
| **FLAG_HIDDEN_ITEM_MACHINE_PART** | Set when you pick up the hidden Machine Part (in the building where you get it). | One-time pickup. |
| **FLAG_KANTO_RADIO_GOT** | Set when you receive Kanto Radio at Lavender Town Radio Tower. | Needed to wake Snorlax in Vermilion (Poké Flute channel). |
| **FLAG_HIDE_POWER_PLANT_ENGINEER** | Set when Machine Part returned. | Engineer no longer blocking / different dialogue. |

- **Vermilion Port:** Sailor won’t sail until `FLAG_RETURNED_MACHINE_PART`.
- **Saffron Train Station:** Train not running until `FLAG_RETURNED_MACHINE_PART`.

**Your state:** Machine Part returned, Kanto Radio obtained → Snorlax woken and caught; train and port logic “done.”

---

### 3.2 One-time TMs / key items (examples)

- **FLAG_GOT_VIRIDIAN_TM_SLUDGE_BOMB** – From Blue (Viridian Gym).
- **FLAG_GOT_TM_PSYCHIC** – Saffron Gym (Sabrina).
- **FLAG_GOT_TM_STEEL_WING** – SS Aqua / Pewter; cleared when taking SS Aqua to Kanto.
- **FLAG_GOT_ROUTE_14_AERIAL_ACE** – Route 14.
- **FLAG_GOT_TM_REST** – (One-time TM.)
- **FLAG_ROUTE25_GOT_NUGGET** – Route 25 nugget.
- **FLAG_GOT_ROUTE2_NUGGET** – Route 2 nugget.

Many more in `flags.h` as `FLAG_GOT_*`, `FLAG_RECEIVED_*`, `FLAG_ITEM_*` for TMs, HMs, and key items.

---

### 3.3 Cerulean / Rocket

| Flag | When set/cleared | Effect |
|------|-------------------|--------|
| **FLAG_HIDE_CERULEAN_GYM_ROCKET** | Cleared when Kanto Rocket story starts at Power Plant (VAR_KANTO_ROCKET_STORY_STATE). | Rocket in Cerulean Gym. |
| **FLAG_HIDE_CERULEAN_CAPE_ROCKET** | Story. | Rocket on cape. |
| **FLAG_HIDE_COPYCAT_CLEFAIRY_DOLL** | Cleared when you return the doll (after Machine Part / Copycat). | Copycat has doll. |

---

## 4. Badges and gyms

- **FLAG_BADGE01_GET … FLAG_BADGE16_GET** – Set when the corresponding gym leader is defeated (Violet, Falkner → … → Viridian, Blue).
- **FLAG_DEFEATED_*_GYM** – One per gym; set on leader defeat (e.g. `FLAG_DEFEATED_VERMILION_GYM`, `FLAG_DEFEATED_CINNABAR_ISLAND_GYM`).

**Your state:** 16 badges → all 16 badge flags set; all gym defeat flags you’ve passed are set.

---

## 5. Legendaries / event Pokémon (visibility)

Many `FLAG_HIDE_*` and `FLAG_DEFEATED_*` / `FLAG_CAUGHT_*` control legendaries. Examples:

- **FLAG_HIDE_MEWTWO** – Cleared in Hall of Fame SetGameClearFlags (rematch) so Mewtwo can be encountered again.
- **FLAG_HIDE_VERMILION_SNORLAX** – Cleared on rematch clear to respawn Snorlax.
- **FLAG_HIDE_ARTICUNO / ZAPDOS / MOLTRES** – Cleared on rematch clear.
- **FLAG_HIDE_LUGIA / FLAG_HIDE_HO_OH** – Cleared in SetGameClearFlags; Lugia/Ho-Oh also tied to `VAR_COMPLETED_LUGIA`, `VAR_COMPLETED_HO_OH`.
- **FLAG_UNLOCK_BIRDS / FLAG_UNLOCK_MEWTWO** – Unlock encounter (e.g. after League).

---

## 6. Summary: “What’s done” for your run

From your described progress (16 badges, Blue beaten, Snorlax caught, Machine Part returned, Kanto Radio, Misty and Blaine done):

| Category | Status |
|----------|--------|
| **Reception Gate east (Kanto)** | ✅ Open (Snorlax done). |
| **Reception Gate west (Mt. Silver)** | ✅ Open (Oak with 16 badges). |
| **Elite Four** | ✅ Unlocked (Blue defeated). |
| **Cerulean Cave** | ✅ Unlocked (Blue defeated). |
| **Route 19 → Fuchsia** | ✅ Open (Blaine defeated). |
| **Viridian Gym (Blue)** | ✅ Done. |
| **Misty (Route 25 + Cerulean)** | ✅ Done (Machine Part returned). |
| **Blaine (Seafoam)** | ✅ Done. |
| **Snorlax** | ✅ Caught; Kanto guard cleared. |
| **Machine Part / Magnet Train / Radio** | ✅ Returned; Kanto Radio obtained. |

**Still gated / optional:**

- **Tin Tower / Whirl Islands:** Depend on Pewter Rainbow Wing and Ecruteak Theater story (Tin Tower guard, Tentacruel, Kimono Girls).
- **Post-League (FLAG_SYS_GAME_CLEAR):** Trainer Hill, Slateport/Lilycove ferries, Desert Underpass, Trick House 8, mints, Jirachi, etc. – all require clearing the Hall of Fame once (GameClear sets FLAG_SYS_GAME_CLEAR).
- **E4 rematches:** Require FLAG_IS_CHAMPION (set on first Hall of Fame).
- **Cerulean Cave / Mewtwo:** Already open for you; rematch clear can reset Mewtwo and other legendaries via SetGameClearFlags.

This file can be updated as you discover more flag dependencies in the scripts or map objects.
