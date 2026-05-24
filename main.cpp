#include <cmath>
#include <optional>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>


using namespace std;

enum stuff
{
    TRIGGER_COMBAT_START,
    TRIGGER_ACTION_MANUAL,
    TRIGGER_TURN_START,
    TRIGGER_TURN_END,
    TRIGGER_BEFORE_ATTACK,
    TRIGGER_MOVE,
    TRIGGER_MOVE_FIRST, // is also a move
    TRIGGER_MOVE_SECOND, // is also a move
    TRIGGER_AFTER_MOVE,
    TRIGGER_SLAINED,
    TRIGGER_DAMAGED,

    TRIGGER_SOUL_OWN_TURN,
    TRIGGER_SOUL_ALLIED_TURN,
    TRIGGER_SOUL_FOE_TURN,
    TRIGGER_SOUL_OWN_OR_ALLIED_TURN = TRIGGER_SOUL_OWN_TURN | TRIGGER_SOUL_ALLIED_TURN,
    TRIGGER_SOUL_ANY_TURN = TRIGGER_SOUL_OWN_TURN | TRIGGER_SOUL_ALLIED_TURN | TRIGGER_SOUL_FOE_TURN,

    // unit allowed to make a step on start/end of it's turn
    COUNTER_RAPID_MOVE_AVAILABLE,
    // +1D on any attack and ignore cover. removed after attack
    COUNTER_ANCILLARY_LIMBS,
    // at the end of it's turn explode(self) effect for 1 toxin damage. allies (of one, who set this effect) mutate instead of taking damage
    COUNTER_BIOTOXIN_INJECTOR,
    // +1D to attacks per stack, and damage ignores armor. til turn end
    COUNTER_GROW_BONUS_LIMBS,
    // tick down at the of your turn, obliterated when reaches zero
    COUNTER_FINAL_FORM,
    COUNTER_CURSEPROOF,
    COUNTER_FREE_MOVEMENT,
    COUNTER_SUPER_ARMOR,
    COUNTER_PHYSICAL_ARMOR,
    COUNTER_ALTERED_MV,
    COUNTER_ALTERED_DF,
    COUNTER_FLIGHT,
    COUNTER_IS_2X2,
    COUNTER_COST_HALF_UNIT_SLOT,
    COUNTER_ACTIVATED_TWO_AT_A_TIME,
    COUNTER_MIRACLE,

    // While in cover, attacks gain +1 max range and push 1 on hit.
    UPGRADE_BRACE,
    // Gain Headshot: reload and gain 1 strength.
    UPGRADE_TACTICAL_RELOAD,
    // May consume one adjacent corpse before or after ACTing to reload and gain 1 strength.
    UPGRADE_SCAVENGE_AMMO,
    // Unit may step 1 before using Skull Crack and it gains splash (self) push foes 1.
    UPGRADE_SHIELD_CHARGE,
    // Shieldwall also clears up to two negative tokens on self or adjacent allies.
    UPGRADE_BULWARK,
    // Flashbang deals 1 splash fire damage but gains charge and has no effect on its main target.
    UPGRADE_BONE_DUST_NAPALM,
    // Bone Shards gains effect: (4+) push 1, (6+) push 1 again.
    UPGRADE_VOMIT_BULLETS,
    // Destructive Glee's splash effect gains effect (4+): +1 fire damage, (6+): and +1 vulnerable.
    UPGRADE_NAPALM_INJECTOR,
    // Regurgitate Ammo now targets two different adjacent units but no longer grants strength.
    UPGRADE_HOT_CHAMBER,
    // Transform to Gun makes this unit immune to push and pull.
    UPGRADE_CLAW_PITONS,
    // Snipe gains +1D when this unit is in cover and -1D when it is not.
    UPGRADE_EXTENDED_BARREL,
    // Snipe gains push 1 and Headshot: push 2.
    UPGRADE_CALIBER_UP,
    // Juggernaut also clears all negative tokens.
    UPGRADE_FORTIFY,
    // Catechism pushes all units in the line 1 space and destroys walls.
    UPGRADE_HEAVY_CALIBER_CANNON,
    // When at least one ally is adjacent to this unit, Mortar ignores cover.
    UPGRADE_GUNNER_PIVOT,
    // May step 1 before and after activating Cross Fire, but it becomes line 3.
    UPGRADE_BURST_FIRE,
    // Cross Fire’s extra effect also triggers if it catches a wall in the line.
    UPGRADE_RICOCHET,
    // Slide Kick also inflicts 1 vulnerable.
    UPGRADE_KNIFE_SLASH,
    // Charge has a (5+) chance to grant retaliation until the start of its next turn.
    UPGRADE_BLADED,
    // At 1 HP or lower, gain free movement.
    UPGRADE_OVERCLOCKED,
    // Gain Curseproof.
    UPGRADE_LOBOTOMIZED,
    // May step 1 before using Chainsaw and after using chainsaw.
    UPGRADE_HEAVY_SWING,
    // Rev grants one (5+) or two berserk.
    UPGRADE_RILE,
    // At 0 HP, all damage gains pierce.
    UPGRADE_BLOODY_TEETH,
    // At 1 HP or lower, self and adjacent allies become curseproof.
    UPGRADE_STIM_HAZE,
    // At 1 HP or lower, Meat Hook can pull up to three units.
    UPGRADE_FRENZY_HOOK,
    // Allies gain 1 speed per negative token removed from Cauterize.
    UPGRADE_ADRENALIZE,
    // Spin Out may pull a unit in range 3 one space before its MOVE.
    UPGRADE_BARBED_WHEEL,
    // Spin Out grants 1 berserk.
    UPGRADE_ENDLESS_SCREAMING,
    // At 1 HP, may MOVE again for free at any point during turn.
    UPGRADE_HURTLE,
    // At 1 HP or lower, gains SUPER.
    UPGRADE_MACHINEHEART,
    // May step 2 before using Pulverize.
    UPGRADE_FUEL_INJECTORS,
    // Rip and Tear obliterates units that it reduces to 0 HP.
    UPGRADE_BIFURCATE,
    // Overlock has a 5+ effect chance to grant retaliation until the start of this units next turn.
    UPGRADE_RAGE_OVERCLOCK,
    // Restart engine allows allows ally to step 2.
    UPGRADE_ROLLING_START,
    // At 1 HP or lower, all abilities target self and an adjacent ally.
    UPGRADE_FRENZIED_TINKERING,
    // Toxic Revenge's splash pushes 1 if this unit is plagued.
    UPGRADE_BLOAT,
    // Pseudopod inflicts 1 slow on plagued units.
    UPGRADE_TENTACLE_WHIP,
    // Toxic Revenge leaves a hazard instead of a corpse.
    UPGRADE_AFTERMATH,
    // Vomitous Mass deals 1 toxic damage to units standing in hazards.
    UPGRADE_CATALYZE,
    // Gain death burst: splash (self): 1 plague, and allies gain 1 strength.
    UPGRADE_INVIGORATING_VISCERA,
    // Curseproof and cover from all directions while standing in a hazard.
    UPGRADE_SMOG_SHROUD,
    // Swell with corruption has a 4+ chance to trigger twice.
    UPGRADE_MASSIVE_SWELL,
    // Evolve Strain removes 1 plague from all adjacent characters instead, with the same effects.
    UPGRADE_PANDEMIC,
    // Suppurate also grants deathburst: Create a hazard instead of a corpse.
    UPGRADE_ACID_BLOOD,
    // May spend plague tokens on self to grant Driving Vermin +1D per token spent.
    UPGRADE_SWARM_FEED,
    // Immune to hazards. While standing in a hazard, gain 1 plague at turn start.
    UPGRADE_TOXIC_AVENGER,
    // Driving Vermin gains effect (5+) create a hazard under target before the attack.
    UPGRADE_DEFILER,
    // After Surge, inflict 1 plague on self for each unit passed through.
    UPGRADE_ROTTEN_SURGE,
    // During Surge, may create a hazard in one space passed through.
    UPGRADE_STICKY_TRAIL,
    // Surge inflicts 2 slow on the first unit affected.
    UPGRADE_THE_GUNK,
    // Slimelets have MAG.
    UPGRADE_CONGEALED,
    // For both units, toxic spit also inflicts 1 plague against units already in hazards.
    UPGRADE_PUTRIDITY,
    // When Burst Guts activates, it has a 5+ effect chance to summon two slimelets instead.
    UPGRADE_STUFFED,
    // May move through walls.
    UPGRADE_SQUIRM,
    // Beckon has no maximum range against isolated units.
    UPGRADE_DEAD_GRASP,
    // Beckon inflicts 1 weak on isolated units.
    UPGRADE_IMPENDING_DEATH,
    // Serpent's Kiss also triggers its effects against units standing in adverse terrain.
    UPGRADE_IVORY_SERPENT,
    // May step 1 before ACTing, or step 2 and clear a token if adjacent to a wall.
    UPGRADE_LEAP,
    // Tombraiser gains +2 max range.
    UPGRADE_FOUL_MONUMENTS,
    // Gain 2 strength if Horrendous Shriek hits an isolated foe.
    UPGRADE_TERRORIZE,
    // When teleporting, create a space of adverse terrain in the first space vacated.
    UPGRADE_SOUL_FROST,
    // Shriek obliterates one unit in its area at 1 HP or lower.
    UPGRADE_CONDEMN,
    // Tombstone gains effect (6+): adjacent foes are Doomed.
    UPGRADE_DOOM_BELL,
    // If Bale Scream slays a unit, it creates a space of adverse terrain instead of a corpse.
    UPGRADE_FREEZE_SOUL,
    // Bale Scream pulls all units in its area 1.
    UPGRADE_SIREN,
    // Beckon Lamb has no maximum range against Doomed units.
    UPGRADE_TO_THE_SLAUGHTER,
    // Attacks against other Tyrants cannot have their damage reduced and obliterate their targets instead of slaying them.
    UPGRADE_SUPREMACY,
    // This unit is curseproof.
    UPGRADE_STRONG_PACT,
    // At the start of round 3, create a new wall in free space anywhere for each Lithogeist still active.
    UPGRADE_TOMB_BURST,
    // Replace all instances of the word wall in Buried Alive and Tomb Bound with adverse terrain instead.
    UPGRADE_RUIN_GEIST,
    // When Strangle slays a unit, create a wall in their space instead of a corpse.
    UPGRADE_PULL_UNDER,
    // Mea Culpa creates one or (4+) two hazards in free adjacent spaces.
    UPGRADE_HOLY_BLOOD,
    // May step 2 before using Mea Culpa.
    UPGRADE_CAGER,
    // Stance (4+): Excoriate triggers both effects.
    UPGRADE_TASTE_THE_LASH,
    // Stance: Whirling Chain Obliterates foes it slays at round 3+.
    UPGRADE_PUNISHER,
    // Smite removes a negative token from this unit.
    UPGRADE_SUFFUSE,
    // Whirling Chain creates one or (5+) two hazards adjacent to its target after it resolves.
    UPGRADE_FIERY_CHAIN,
    // Self and adjacent units are curseproof during gentleness.
    UPGRADE_CLEANSING_PRAYER,
    // Blessed Censer may remove up to three corpses or hazards in range.
    UPGRADE_CONSECRATE,
    // Stance (Round 4+): Blessed Censer's effects trigger twice.
    UPGRADE_BOILING_CENSER,
    // If this unit does not move during its turn, it may remove a negative token from itself.
    UPGRADE_FOCUS,
    // Stance: Requiesce en Pace obliterates units it slays at round 3+.
    UPGRADE_HEART_DESTROYER,
    // Stance: Requiesce en Pace pushes units one, (Round 3+) two, or (Round 5+) four spaces.
    UPGRADE_EXPLOSIVE_BOLTS,
    // When Bolides destroys a wall, it can deal 1 damage to an adjacent foe.
    UPGRADE_SCATHE,
    // Ablutions gains Stance (Round 3+): gain 3 vitality instead, Round (5+): gain 4 vitality instead.
    UPGRADE_HOLY_FONT,
    // Indignation gains Stance (Round 4+): deal 2 holy damage instead.
    UPGRADE_WITNESS,
    // Stance: At round 6+, Bible Thump deals 4 damage on hit.
    UPGRADE_THE_GOOD_WORD,
    // Bible Thump removes a negative token before granting vitality.
    UPGRADE_CLEANSING_THE_HOSTS,
    // Blood of the Covenant causes miracle to trigger on a 3+.
    UPGRADE_SANCTIFIED_SACRIFICE,
    // Twisting Strike gains Effect (5+): steal a positive token from target.
    UPGRADE_GENESTEALER,
    // Twisting Strike may cause an adjacent ally to mutate.
    UPGRADE_SPREAD_MUTATE,
    // May step 1 (4+) or 3 spaced before Unstable Mutation.
    UPGRADE_WARPING_MUTATE,
    // May grant one of the tokens stolen by Purge to another unit in range.
    UPGRADE_ABSORB,
    // Purge deals 1 piercing toxic damage to foes.
    UPGRADE_SCOUR_FLESH,
    // If targeting self, Marriage also clears all negative tokens on target first.
    UPGRADE_CONJOIN,
    // If starting turn in range 2 of a corpse, may step 2
    UPGRADE_THE_HUNGER,
    // At turn start, may deal 1 damage to self, ignoring armor, to generate     a corpse in an adjacent space
    UPGRADE_AUTOPHAGIA,
    // May use corpses as cover spaces
    UPGRADE_HUNCH,
    // Regurgitate has a 5+ effect chance to strip a token of your choice from each foe in the area.
    UPGRADE_DROWN_IN_VISCERA,
    // May mutate once for each corpse created with Regurgitate instead.
    UPGRADE_RAPID_ADAPTATION,
    // Regurgitate has a 5+ effect chance to remove a negative token from an ally caught in the area
    UPGRADE_CLEANSING_WASH,
    // When choosing to MOVE again, destroys walls and absorbs corpses it moves over, mutating each time
    UPGRADE_BALL_OF_LIMBS,
    // When absorbing allies, slays instead of obliterating them, and this unit mutates.
    UPGRADE_MELD,
    // If absorb destroys a unit, gains physical armor for the rest of combat
    UPGRADE_FORM_CARAPACE,

    // ask a player, will it do it when it may?
    TAKE_ACTION_RAPID_MOVE,
    TAKE_ACTION_ANCILLARY_LIMBS,
    TAKE_ACTION_AUTOPHAGIA,
    TAKE_ACTION_STEP,

    // what bonus types can be applied to a roll?
    ROLL_TAG_NONE,
    ROLL_TAG_CURSE,
    ROLL_TAG_ATTACK,

    // which tokens to select?
    SELECT_TOKEN_ANY,
    SELECT_TOKEN_ONLY_NEGATIVE,
    SELECT_TOKEN_ONLY_POSITIVE,

    // can it be negated by armor?
    DAMAGE_TOXIN,
    DAMAGE_NORMAL,
    DAMAGE_GRAZE,
    DAMAGE_FIRE,
    DAMAGE_CURSE,
    DAMAGE_DEVIL,
    DAMAGE_PIERCING,

    // what dmg types can be decreased?
    ARMOR_NONE,
    ARMOR_PHYS,
    ARMOR_MAG,
    ARMOR_SUPER,

    // factions
    FACTION_CARCASS,
    FACTION_GOREGRINDERS,
    FACTION_GARGAMOX,
    FACTION_DEADSOULS,
    FACTION_ABHORRER,
    FACTION_IGORRI,

    // unit type
    UNIT_THRALL,
    UNIT_SCION,
    UNIT_FREAK,
    UNIT_HORROR,
    UNIT_HUNTER,
    UNIT_TYRANT,
    UNIT_NECROMANCER,
};


enum select_unit_filter
{
    SELECT_UNIT_EXCLUDE_NONE,
    SELECT_UNIT_EXCLUDE_SELF,
    SELECT_UNIT_EXCLUDE_ALLY,
    SELECT_UNIT_EXCLUDE_FOE,

    SELECT_UNIT_WITH_TOKENS,
    SELECT_UNIT_WITH_POSITIVE_TOKENS,
    SELECT_UNIT_WITH_NEGATIVE_TOKENS,
    SELECT_UNIT_WITH_MUTATION_TOKENS,
    SELECT_UNIT_WITHOUT_CURSEPROOF,
};


enum select_space_filter
{
    SELECT_SPACE_ANY,
    SELECT_SPACE_FREE,
};


enum token_type
{
    TOKEN_MUTATION,
    TOKEN_STRENGTH,
    TOKEN_WEAK,
    TOKEN_SPEED,
    TOKEN_SLOW,
    TOKEN_VITALITY,
    TOKEN_VULNERABLE,
};


enum movement_tags
{
    MOVEMENT_DEFAULT,
    MOVEMENT_FREE,
    MOVEMENT_IGNORE_HAZARDS,
    MOVEMENT_DESTROY_WALLS,
    MOVEMENT_ABSORB_CORPSES,
};


template <typename T> T enum_or(T a, T b) { return T((int)a | (int)b); }
template <typename T> T enum_or(T a, T b, T c) { return enum_or(a, enum_or(b, c)); }


struct map_pos
{
    int x, y;
};


struct ttoken;


struct tunit
{
    virtual ~tunit() = default;
    virtual list<ttoken *> tokens() = 0;
    virtual int n_tokens(int filter = SELECT_TOKEN_ANY) const = 0;
    virtual ttoken *find_token(token_type type) = 0;
    virtual bool remove_token(ttoken &, int count = 1) = 0;
    virtual void add_token(token_type type, int count = 1) = 0;

    virtual int inc_counter(int counter, int x, int def_value = 0) = 0;
    virtual void set_counter(int counter, int x) = 0;
    virtual int counter(int counter, int def_value = 0) const = 0;

    virtual list<tunit *> units_in_range(int, select_unit_filter exclude = SELECT_UNIT_EXCLUDE_NONE) const = 0;
    virtual list<tunit *> units_in_range(int, int, select_unit_filter exclude = SELECT_UNIT_EXCLUDE_NONE) const = 0;
    virtual int corpses_in_range(int) const = 0;
    virtual bool is_ally(tunit &) const = 0;
    virtual void take_damage(int x, int type, bool *dead = nullptr) = 0;
    virtual void revive(int hp = 1) = 0;
    virtual map_pos pos() const = 0;
    virtual bool has_upgrade(int) const = 0;
    virtual int unit_type() const = 0;
    virtual bool is_slain() const = 0;

    virtual int size() const { return counter(COUNTER_IS_2X2) ? 2 : 1; }
    virtual bool is_curseproof() const { return counter(COUNTER_CURSEPROOF) > 0; }
};


struct ttoken
{
    virtual ~ttoken() = default;
    virtual int count() const = 0;
    virtual bool is_positive() const = 0;
    virtual token_type type() const = 0;
};


struct unit_action_context
{
    virtual ~unit_action_context() = default;

    virtual int trigger() const = 0;

    virtual tunit &self() = 0;
    virtual tunit &activated() = 0;
    // splits the turn to the atomic actions, that can't be interrupted
    virtual bool then() = 0;
    // prevent action, because no target
    virtual void no_target() = 0;
    // prevent action, because no resources
    virtual void no_resources() = 0;

    virtual optional<int> player_must_select_roll(const list<int> &dice_rolls) = 0;
    virtual optional<int> player_must_select_token_count(int up_to_x) = 0;
    virtual optional<int> player_must_select_corpse_count(int up_to_x) = 0;
    virtual optional<token_type> player_may_select_token_type(const list<token_type> &token_types) = 0;
    virtual optional<token_type> player_must_select_token_type(const list<token_type> &token_types) = 0;
    virtual ttoken *player_may_select_token(const list<ttoken *> &tokens, int filter = SELECT_TOKEN_ANY) = 0;
    virtual ttoken *player_must_select_token(const list<ttoken *> &tokens, int filter = SELECT_TOKEN_ANY) = 0;
    virtual tunit *player_may_select_unit(const list<tunit *> &units, const list<tunit *> &exclude = {}) = 0;
    virtual tunit *player_must_select_unit(const list<tunit *> &units, const list<tunit *> &exclude = {}) = 0;
    virtual list<tunit *> player_must_select_units(const list<tunit *> &units, int min, int max) = 0;
    virtual list<tunit *> player_must_select_line(int) = 0;
    virtual optional<map_pos> player_must_select_space(const map_pos &, int range, select_space_filter filter = SELECT_SPACE_ANY) = 0;
    virtual optional<map_pos> player_must_select_space(const map_pos &, int min, int max, select_space_filter filter = SELECT_SPACE_ANY) = 0;
    virtual bool player_may_take_action(int) = 0;
    virtual bool player_may_spend_soul(int x) = 0;
    virtual int player_roll_d6(tunit &who, int tags = ROLL_TAG_NONE) = 0;
    virtual int d6_gradations(int d6, const map<int, int> &treshold_to_result = {}) const = 0;

    virtual bool is_hit(tunit &target, int d6) const = 0;
    virtual void unit_move(tunit &, movement_tags extra_tags = MOVEMENT_DEFAULT, int *walls_destroyed = nullptr, int *corpses_absorved = nullptr) = 0;
    // TODO: may unit trigger something on step and die? then it should be [[no_discard]] bool unit_step
    virtual void unit_step(tunit &, int range = 1, movement_tags tags = MOVEMENT_DEFAULT) = 0;
    virtual void slay(tunit &) = 0;
    virtual void obliterate(tunit &) = 0;
    virtual int inc_corpse(const map_pos &, int x = 0) = 0;
    virtual tunit &copy_unit(tunit &, const map_pos &new_pos) = 0;
    virtual void swap_unit_pos(tunit &, tunit &) = 0;

    virtual void mutate(tunit &unit) { unit.add_token(TOKEN_MUTATION, +1); }
    virtual int player_may_spare_parts(tunit &) = 0;
};


using action_foo = void(*)(unit_action_context &);


struct tcard
{
    virtual ~tcard() = default;
    virtual void set_faction_type(int faction, int type) = 0;
    virtual void set_stats(int mv, int hp, int df, int arm) = 0;

    virtual void set_move_override(action_foo) = 0;
    virtual void add_upgrade(int) = 0;

    virtual void add_trait(int trigger, action_foo) = 0;
    virtual void add_bonus_trait(int trigger, action_foo) = 0;

    virtual void add_act_ability(action_foo) = 0;
    virtual void add_upgrade_act_ability(action_foo) = 0;

    virtual void add_soul_ability(int trigger, action_foo) = 0;
    virtual void add_upgrade_soul_ability(int trigger, action_foo) = 0;
};


using unit_card = void(*)(tcard &);


struct tfaction
{
    virtual ~tfaction() = default;
    virtual void add_card(unit_card);
};


// *** ABILITIES AND TRAITS ***

// While adjacent to an ally, gain +1D on attacks.
void formation(unit_action_context &c)
{
}


// Once used, a unit cannot use any ability with this tag until it reloads. To reload, sacrifice a MOVE. Other abilities may allow a reload for free.
void reload(unit_action_context &c)
{
}


// Effect that triggers when rolling a 6 on the final attack roll.
void headshot(unit_action_context &c)
{
}


// May be used as a cover object by allies.
void siege_shield(unit_action_context &c)
{
}


// If this unit ends its turn with no foes adjacent, this unit becomes curseproof and all attacks against it gain -1D. Both effects last until the start of its next turn.
void active_camo(unit_action_context &c)
{
}


// Attack, Range 2-3, reload. On hit: 1 piercing damage. Headshot: inflict 1 vulnerable.
void ol45(unit_action_context &c)
{
}


// Push, melee. Effect: Push 1 and (3+) inflict 1 vulnerable.
void baton(unit_action_context &c)
{
}


// Attack, melee, push. On hit: 2 damage and push 1. Headshot: push 2.
void skull_crack(unit_action_context &c)
{
}


// Range 2-3, reload, splash. Effect: 1 fire damage and 1 vulnerable, then Splash (target): push 1 away from target.
void flashbang(unit_action_context &c)
{
}


// Self. Effect: Until start of this unit's next turn, this unit cannot MOVE, step, or be pushed or pulled, gains cover, and grants cover to adjacent allies from all directions.
void shieldwall(unit_action_context &c)
{
}


// Range 1-2. Effect: A unit in range reloads and gains one, (6+) or two strength.
void regurgitate_ammo(unit_action_context &c)
{
}


// Curse, Range 1-3. Effect: Unit takes 1 damage after any ACT ability resolves that pushes or pulls them. Lasts until end of this unit's next turn or until this unit has taken 3 damage this way.
void bone_shards(unit_action_context &c)
{
}


// Self. Effect: Step 1, then Splash (self): 1 fire damage, push 1, and inflict vulnerable, then self is obliterated.
void destructive_glee(unit_action_context &c)
{
}


// Attack, Range 2-4, reload. On hit: 2 damage. Headshot: and inflict 1 vulnerable.
void snipe(unit_action_context &c)
{
}


// Self. Effect: Unit becomes unable to MOVE or step, but ranged abilities gain +2 maximum range and ignore armor and cover. It can end this effect by sacrificing a MOVE, but if it does, it loses all associated effects.
void transform_to_gun(unit_action_context &c)
{
}


// Curse, Range 2-4. Effect: Inflict 2 vulnerable, (4+): 3 vulnerable, (6+): Remove any vitality first.
void deathmark(unit_action_context &c)
{
}


// Self, push. Effect: Step 1, then splash (self) push foes 1, (4+) push foes 2. May move into and destroy walls with this step.
void juggernaut(unit_action_context &c)
{
}


// Attack, Range 3-4, reload, charge. On hit: 1 damage then Splash: 1 fire damage. Headshot: +1 damage on main target.
void mortar(unit_action_context &c)
{
}


// Line, reload. Effect: Line 5, 1 fire damage, and inflicts 1 vulnerable on the first unit in the line. Pierces through walls and can target through walls.
void catechism_devil_cannon(unit_action_context &c)
{
}


// Line 4, reload. Effect: First unit in the line takes 1 piercing damage. If catching an allied unit in the line, also inflict 2 vulnerable.
void cross_fire(unit_action_context &c)
{
}


// Step. Effect: Step 1, then push an adjacent foe 1 or (5+) 2 spaces. That foe must spend a MOVE before next attacking.
void slide_kick(unit_action_context &c)
{
}


// Self. Effect: Reload and gain 1 strength (4+) then regain this unit’s ACT. This ability can’t be used again this turn.
void trick_reload(unit_action_context &c)
{
}


// Ignores adverse terrain and elevation attack and movement penalties
void flight(unit_action_context &c)
{
    c.self().set_counter(COUNTER_FLIGHT, 1);
}


// Starts combat with 4 mutation tokens
void warpflesh(unit_action_context &c)
{
    c.self().add_token(TOKEN_MUTATION, +4);
}


// 2x2 unit
void large(unit_action_context &c)
{
    c.self().set_counter(COUNTER_IS_2X2, 1);
}


// If a unit with this trait is reduced to 0 HP, they are not slain, but instead gain 1 strength and remain at 0 HP, no matter how much damage they took. If they take any further damage at 0 HP, they are slain as normal.
void blood_rage(unit_action_context &c)
{
}


// After a berserk unit ACTs, it splashes (self) 1 damage, then discards a berserk token. This damage can't be increased in any way. If this effect does not damage any other characters, it deals 1 damage to itself instead, ignoring armor. Berserk counts as a positive and a negative token.
void berserk(unit_action_context &c)
{
}


// After any ability resolves that damages this unit, this unit deals 1 damage back to the ability's owner, even if this unit was slain.
void retaliation(unit_action_context &c)
{
}


// This unit may spend speed tokens as strength. May pass through units but not end their turn in their spaces.
void hellwheel(unit_action_context &c)
{
}


// When taking damage, gain 1 berserk and 1 strength. This effect can't trigger more than once a round.
void steaming_rage(unit_action_context &c)
{
}


// Self. Effect: MOVE as far as possible in a straight line, passing through units. Then, units passed through during this movement take 1 damage.
void charge(unit_action_context &c)
{
}


// Attack, melee. On hit: 1 damage and 1 damage to another adjacent unit.
void cleave(unit_action_context &c)
{
}


// Self. Effect: Gain 1 strength and speed (4+) and retaliation until start of this unit's next turn.
void rev(unit_action_context &c)
{
}


// Attack, melee. On hit: 1 damage, Effect: (4+): 1 damage again, (6+): 1 damage again.
void chainsaw(unit_action_context &c)
{
}


// Self. Effect: MOVE as far as possible in a straight line. Then, all units adjacent to any space of this MOVE take 1 fire damage.
void wild_slashes(unit_action_context &c)
{
}


// Curse, Range 1-3. Effect: Unit gains 2 berserk or 2 speed.
void pain_frenzy(unit_action_context &c)
{
}


// Curse, Range 1-3. Effect: Clear all negative tokens on self or a unit, then deal 1 piercing fire damage to them. This damage can't slay a unit.
void cauterize(unit_action_context &c)
{
}


// Pull, Range 2-3. Effect: Pull unit 3.
void meat_hook(unit_action_context &c)
{
}


// Attack, Melee. On hit: 1 damage. Effect: (4+) 1 damage again. (6+) 1 damage again.
void exfoliate(unit_action_context &c)
{
}


// Self. Unit MOVEs as far as possible in a straight line. The first unit passed through during this MOVE take 1 fire damage. Then this unit gains 2 speed.
void spin_out(unit_action_context &c)
{
}


// Self. Effect: Pull all units in range 2 one space toward this unit. Then splash (self): 1 fire damage and destroy all adjacent walls.
void pulverize(unit_action_context &c)
{
}


// Self, retaliation. Effect: Gain 1 strength and 1 berserk, (4+) then gain retaliation until the end of this unit's next turn.
void building_rage(unit_action_context &c)
{
}


// Attack, melee. On hit: 2 damage. Deal 1 damage again if this unit has 3 or less HP, then deal 1 damage again if it is at 1 HP or less.
void rip_and_tear(unit_action_context &c)
{
}


// Melee. Effect: Tinker with an adjacent unit. Unit gains 1 berserk, then roll the effect die (1-3): Increase unit’s MV stat by +1 this battle, (4-5): increase a unit’s DF stat by +1 this battle, to a max of 6+, (6+): Increase unit’s current and max HP by 1 this battle.
void tinker(unit_action_context &c)
{
}


// Range 1-3. Effect: Self or a unit in range gains 2 speed. If target is at 1 HP or lower, splash (target) 1 fire damage.
void overlock(unit_action_context &c)
{
}


// Ally, melee. Effect: Self or an adjacent ally has their HP set to 1.
void restart_engine(unit_action_context &c)
{
}


// Units affected by plague take 1 toxic damage at the end of their turn, then that unit discards a plague token. This damage cannot be reduced or ignored in any way (by tokens or armor) but cannot slay a unit. Gargamox units don't take damage from plague and don't remove plague tokens, but can still be affected by plague.
void plague(unit_action_context &c)
{
}

// Jumps from the original target up to three times to up to three different characters, as long as each character is adjacent to the previous character.
void infect(unit_action_context &c)
{
}

// Effect that triggers when this unit is slain.
void death_burst(unit_action_context &c)
{
}

// Has Deathburst: Splash (self): 1 plague and 1 toxic damage to already already plagued foes.
void toxic_revenge(unit_action_context &c)
{
}

// Immune to hazards. May treat plague tokens as strength.
void plaguebearer(unit_action_context &c)
{
}

// Has Deathburst: Splash (self): Remove any 1 token from all units in the area.
void deathwash(unit_action_context &c)
{
}

// Has Deathburst: create a hazard under a number of units in range 2 equal to the number of plague tokens in this unit.
void swarm_release(unit_action_context &c)
{
}

// Has Deathburst: Remove up to three plague tokens on this unit, then splash (self): 1 toxic damage, once, per plague token removed.
void vile_rupture(unit_action_context &c)
{
}

// Has deathburst: summon a slimelet in a free adjacent space. Unlike other summons, the slimelet can be freely activated this round.
void burst_guts(unit_action_context &c)
{
}

// Follows all summon rules, but can act this round.
void spit_up(unit_action_context &c)
{
}

// Can be activated two at a time.
void summoned_thrall(unit_action_context &c)
{
}

// Range 2-3. Effect: Pull unit one. This gains +1 range and pull for each plague token on this unit.
void pseudopod(unit_action_context &c)
{
}

// Attack, melee. On hit: 1 damage and 1 plague.
void shamble(unit_action_context &c)
{
}

// Range 1-3. Effect: Create a hazard in a free space in range 3 and inflict plague on an adjacent target (4+) all adjacent targets.
void pustulate(unit_action_context &c)
{
}

// Line 4. Line: 1 plague. Already plagued foes gain 1 slow.
void vomitous_mass(unit_action_context &c)
{
}

// Attack, melee. On hit: 1 toxic damage and infect: 1 toxic damage.
void rotblade(unit_action_context &c)
{
}

// Curse, Range 1-3. Effect: Unit permanently gains death burst: splash (self): 1 toxic damage and 1 plague. This effect cannot stack with itself but stacks with other death burst effects.
void suppurate(unit_action_context &c)
{
}

// Curse, Range 1-3. Effect: Strip all plague tokens from a unit in range. Foes take 1 toxic damage. Then, they gain 1 slow per token removed. Allies gain 1 strength per token removed.
void evolve_strain(unit_action_context &c)
{
}

// Range 1-3. Effect: Unit triggers deathburst without being slain.
void swell_with_corruption(unit_action_context &c)
{
}

// Range 2-4. Effect: Create one, (3+) two, or (5+) three hazards in range. May spend a plague token on self to re-roll the effect die once.
void propagate_swarm(unit_action_context &c)
{
}

// Attack, Range 2-4. On hit: 2 damage and pull 1. Infect: 1 damage and pull 1. Effect: Allies take no damage.
void driving_vermin(unit_action_context &c)
{
}

// Self. Effect: Inflict two, (5+) or three plague tokens on self, then may inflict 1 slow on an adjacent unit per plague token gained.
void percolate(unit_action_context &c)
{
}

// Self. Effect: MOVE with free movement in a straight line. During this move, is immune to hazards and may move through walls and units. Any foe this unit passes through takes 1 toxic damage and this unit may pass off any plague tokens on this unit to affected units.
void surge(unit_action_context &c)
{
}

// Attack, Range 2-3. On hit: 1 toxic damage (3+) and create a hazard under target. Ignores armor against units already in hazards.
void toxic_spit(unit_action_context &c)
{
}

// Range 1-2. Effect: Push target unit 1 space, then create one hazard in a space they vacated. If this unit is plagued, push 2 instead.
void projectile_vomit(unit_action_context &c)
{
}


// Any unit affected by at least one Doom token takes 4 devil damage at the end of round 4. A unit can only clear a Doom token by slaying another unit and they cannot be cleared in any other way.
void doom(unit_action_context &c)
{
}

// Gains extra effects against units with no allies adjacent.
void isolation(unit_action_context &c)
{
}

// When slain, does not remove Doom, and (5+) Dooms slayer.
void inverted_crucifix(unit_action_context &c)
{
}

// Has free movement while adjacent to a wall.
void slither(unit_action_context &c)
{
}

// When MOVEing a second time or more in a turn, can remove this unit from the battlefield and place it any free space in range 4, then clear a token.
void teleport(unit_action_context &c)
{
}

// Abilities ignore line of sight.
void soul_sight(unit_action_context &c)
{
}

// Before MOVEing, may remove and place any adjacent walls in any other free adjacent spaces.
void labyrinth_master(unit_action_context &c)
{
}

// The Geist is bound to the stone of Anzenmezzeron. It cannot MOVE or step. At the start of its turn, teleport it to any space adjacent to a wall. At the start of combat, place a wall in a free adjacent space to it.
void buried_alive(unit_action_context &c)
{
}

// If not adjacent to a wall, the geist loses all armor.
void tomb_bound(unit_action_context &c)
{
}

// Range 2-4. Effect: Pull 1. Ignores line of sight.
void beckon(unit_action_context &c)
{
}

// Attack, melee. On hit: 1 damage and (4+) Dooms unit.
void shudder(unit_action_context &c)
{
}

// Melee, Range 1-2. Effect: Create a wall, (3+) then create adverse terrain (5+) then create adverse terrain again.
void tombraiser(unit_action_context &c)
{
}

// Range 2-4. Effect: Pull 1, ignoring line of sight. If pulling a unit into a wall, pull 3 instead and the kidnapped unit can pass through walls and units during this pull.
void kidnap(unit_action_context &c)
{
}

// Attack, melee. On hit: 2 damage, then against isolated units inflict 1 weak (4+) and Doom them.
void serpents_kiss(unit_action_context &c)
{
}

// Curse, Self. Curse: Splash (self): foes gain 1 weak, (5+) and are Doomed. Doomed units take 1 curse damage.
void horrendous_shriek(unit_action_context &c)
{
}

// Attack, melee. On hit: 2 curse damage. Effect: Against isolated units, gain 1 strength and may then MOVE again.
void urgal_blade(unit_action_context &c)
{
}

// Line 4. Effect: Line: 1 curse damage, Isolated units take 1 curse damage again. May penetrate walls and does not damage walls. Effect: If this line passes through a wall, this unit gains 1 strength.
void bale_scream(unit_action_context &c)
{
}

// Range 2-4. Effect: Create a wall in range and (5+) adjacent foes to the wall gain 1 weak.
void tombstone(unit_action_context &c)
{
}

// Curse, Range 1-4, requires isolated unit. Effect: Choose an isolated unit. That unit gains 2 weak and may no longer MOVE or step until the end of its next turn or unit it's no longer isolated.
void hells_grasp(unit_action_context &c)
{
}

// Curse, Range 2-4. Effect: Unit gains 1 weak and is pulled 3.
void beckon_lamb(unit_action_context &c)
{
}

// Attack, melee. On hit: Deals 1 damage, +1 for each of the following that is true about the target: Doomed, Isolated, Weak, In adverse terrain.
void horrendous_end(unit_action_context &c)
{
}

// Attack, Range 1-2. Effect: Pull 1 before making attack. On hit: 2 damage. Against isolated units, inflicts 1 weak.
void strangle(unit_action_context &c)
{
}

// Range 2-3, Curse. Effect: Target unit gains 1 or (5+) 2 weak. At the end of their turn, create a wall in a free adjacent space to them.
void grave_bind(unit_action_context &c)
{
}

// A unit affected by a Miracle has a 5+ effect chance to instantly return to life at 1 HP at the end of any turn it is slain, clearing all tokens.
void miracle(unit_action_context &c)
{
    if (c.trigger() == TRIGGER_SLAINED) {
        c.self().set_counter(COUNTER_MIRACLE, 1);
        return;
    }
    if (c.trigger() == TRIGGER_TURN_END && c.self().counter(COUNTER_MIRACLE)) {
        c.self().counter(COUNTER_MIRACLE, 0);
        if (!c.self().is_slain() || c.player_roll_d6(c.self()) < 5)
            return;
        for (ttoken *t : c.self().tokens())
            c.self().remove_token(*t, t->count());
        c.self().revive();
    }
}


// When MOVEing for the second time or more in a turn, gain 1 vitality.
void zealotry(unit_action_context &c)
{

}


// Lacks miracle, but starts combat with 4 vitality tokens.
void miraculous_flesh(unit_action_context &c)
{

}


// Self. Splash (self): Remove one negative token from all allies in range and place them on this unit. Then remove one, (4+) two, or (6+) all negative tokens from this unit, and gain 1 strength for each negative token removed this way.
void mea_culpa(unit_action_context &c)
{
}


// Range 2-3. Effect: 1 holy damage and (3+) create a hazard under target.
void holy_water(unit_action_context &c)
{
}


// Attack, melee. On hit: 1 damage, and Stance (Odd): Splash (self): 1 holy damage. (Even): gain 1 vitality.
void excoriate(unit_action_context &c)
{
}


// Self. Stance: MOVE again. The next use of Whirling Chain deals +damage equal to half the round number, rounded up.
void smite(unit_action_context &c)
{
}


// Attack, Range 1-2. On hit: 2 damage. Stance (Odd): and MOVE again, Stance (Even): and push units 2.
void whirling_chain(unit_action_context &c)
{
}


// Self. Effect: Become unable to MOVE or step until start of this unit’s next turn. At the start of that turn, allied units adjacent to this unit gain 1 vitality.
void gentleness(unit_action_context &c)
{
}


// Range 1-3. Effect: An allied unit in range gains miracle. If it already had miracle, miracle triggers on a 2+ until the end of their next turn. At the end of that unit's turn, they lose miracle.
void delay_judgement(unit_action_context &c)
{
}


// Self. Effect: Splash (self): Stance (Even): Allies gain 1 vitality. (Odd): Foes take 1 holy damage.
void blessed_censer(unit_action_context &c)
{
}


// Self. Effect: Step 1. Next attack from this unit gains +1D and +2 maximum range. This effect stacks.
void winch(unit_action_context &c)
{
}


// Attack, Range 2-3. On hit: 2 damage. Stance (Round 3+): with piercing. (Round 5+): 4 damage with piercing.
void requiesce_en_pace(unit_action_context &c)
{
}

// Range 2-6. Effect: Create one, (3+) two, (5+) or three hazards in range. May destroy a wall instead of creating a hazard.
void bolides(unit_action_context &c)
{
}


// Self, Range 3-6. Effect: Remove up to two vitality tokens on this unit, then deal 1 holy damage to that many units in range, ignoring line of sight, and push those units 1.
void indignation(unit_action_context &c)
{
}


// Self. Effect: Gain two vitality. Until the start of this unit's next turn, adjacent allies may spend vitality tokens on this unit as if they were their own.
void ablutions(unit_action_context &c)
{
}


// Melee, Attack. On hit: Self and an adjacent ally gain 1 vitality (Stance: R4+): 2 vitality. Then deal 1 damage and push 1. Cannot miss allies and pushes them +1.
void bible_thump(unit_action_context &c)
{
}


// Self, Ally, Melee. Effect: Choose self or an adjacent ally. At the end of that unit’s turn, slay them. They splash (self) 1 holy damage to foes, 1 vitality to allies. Increase damage and vitality by +1 if the unit had 3 or more vitality.
void blood_of_the_covenant(unit_action_context &c)
{
}


// Range 1-3, Self, Ally. Effect: Target unit converts all vitality into strength, and then becomes unable to gain vitality tokens for the rest of combat.
void wrath(unit_action_context &c)
{
}


// 2 of these units are worth 1 unit slot. Can be activated two at a time.
void thrall(unit_action_context &c)
{
    c.self().set_counter(COUNTER_COST_HALF_UNIT_SLOT, 1);
    c.self().set_counter(COUNTER_ACTIVATED_TWO_AT_A_TIME, 1);
}


void regurgitate_drown_in_viscera(unit_action_context &c, tunit *target)
{
    if (c.player_roll_d6(c.self()) < 5)
        return;

    list<tunit *> us = target->units_in_range(1, 1, enum_or(SELECT_UNIT_EXCLUDE_ALLY, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
    if (us.empty())
        return;

    set<token_type> ts;
    for (tunit *u : us) {
        for (ttoken *t : u->tokens())
            ts.insert(t->type());
    }

    list<token_type> ts_list(ts.begin(), ts.end());
    ts_list.sort();

    optional<token_type> tt = c.player_may_select_token_type(ts_list);
    if (!tt)
        return;

    for (tunit *u : us) {
        ttoken *t = u->find_token(*tt);
        if (t)
            u->remove_token(*t, 1);
    }
}


void regurgitate_cleansing_wash(unit_action_context &c, tunit *target)
{
    if (c.player_roll_d6(c.self()) < 5)
        return;

    list<tunit *> us = target->units_in_range(1, 1, enum_or(SELECT_UNIT_EXCLUDE_FOE, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
    if (us.empty())
        return;

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    ttoken *t = c.player_must_select_token(u->tokens(), SELECT_TOKEN_ONLY_NEGATIVE);
    if (!t)
        return;

    u->remove_token(*t);
}


// When slain, leaves an extra corpse token in an adjacent space.
void fall_to_shambles(unit_action_context &c)
{
    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, 1);
    if (!p)
        return;

    c.inc_corpse(*p, +1);
}

// Self.Effect: Gain 1 strength, (4+) 1 speed, (5+) and 1 vitality, (6+) and explode for splash (self): 1 damage. Spare parts: Roll +1D per corpse and pick the highest result.
void unstable_mutation(unit_action_context &c)
{
    if (c.self().has_upgrade(UPGRADE_WARPING_MUTATE) && c.player_may_take_action(TAKE_ACTION_STEP)) {
        int d6 = c.player_roll_d6(c.self());
        int x = c.d6_gradations(d6, {{0, 1}, {4, 3}});
        c.unit_step(c.self(), x);
    }

    int corpses = c.player_may_spare_parts(c.self());
    int rolls = 1 + corpses;
    int best = 0;
    while (rolls--)
        best = max(best, c.player_roll_d6(c.self()));

    c.self().add_token(TOKEN_STRENGTH, 1);
    if (best >= 4)
        c.self().add_token(TOKEN_SPEED, 1);
    if (best >= 5)
        c.self().add_token(TOKEN_VITALITY, 1);
    if (best >= 6) {
        for (tunit *u : c.self().units_in_range(1, 1))
            u->take_damage(1, DAMAGE_NORMAL);
    }
}


// Attack, Melee. Effect: Mutate. On hit: 1 damage.
void twisting_strike(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    tunit *m = &c.self();
    if (c.self().has_upgrade(UPGRADE_SPREAD_MUTATE)) {
        m = c.player_must_select_unit(c.self().units_in_range(1, SELECT_UNIT_EXCLUDE_FOE));
        if (!m)
            return;
    }
    c.mutate(*m);

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6))
        return u->take_damage(1, DAMAGE_GRAZE);

    u->take_damage(1, DAMAGE_NORMAL);

    if (c.self().has_upgrade(UPGRADE_GENESTEALER) && d6 >= 5 && u->n_tokens(SELECT_TOKEN_ONLY_POSITIVE)) {
        list<ttoken *> ts = u->tokens();
        ttoken *t = c.player_may_select_token(ts);
        if (!t)
            return;

        u->remove_token(*t);
        c.self().add_token(t->type());
    }
}


// After MOVEing, drop a corpse in a free adjacent space.
void leftovers(unit_action_context &c)
{
    if (c.player_roll_d6(c.self()) < 4)
        return;

    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, SELECT_SPACE_FREE);
    if (!p)
        return;

    c.inc_corpse(*p, +1);
}


// Range 3. Effect: One or two units in range mutate. Spare parts: and also dole out 1 strength per corpse consumed.
void inject_mutagen(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(3);
    if (us.empty())
        return c.no_target();

    us = c.player_must_select_units(us, 1, 2);
    if (us.empty())
        return;

    int tokens = c.player_may_spare_parts(c.self());
    for (tunit *u : us) {
        c.mutate(*u);
        if (tokens)
            u->add_token(TOKEN_STRENGTH, tokens);
    }
}


// Curse, Range 1-3. Effect: A unit in range removes one token of this unit's choice (3+): two (5+): all.
void purge(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_TOKENS);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int d6 = c.player_roll_d6(c.self());
    int tokens = c.d6_gradations(d6, {{0, 1}, {3, 2}, {5, -1}});
    list<ttoken *> stolen;
    while (tokens--) {
        ttoken *t = c.player_may_select_token(u->tokens());
        if (!t)
            break;
        u->remove_token(*t, 1);
        stolen.push_back(t);
    }

    if (c.self().has_upgrade(UPGRADE_SCOUR_FLESH) && !u->is_ally(c.self()))
        u->take_damage(1, enum_or(DAMAGE_TOXIN, DAMAGE_PIERCING));

    if (c.self().has_upgrade(UPGRADE_ABSORB)) {
        list<tunit *> us = c.self().units_in_range(3);
        if (us.empty())
            return;

        tunit *another = c.player_may_select_unit(us, {u});
        if (!another)
            return;

        ttoken *t = c.player_may_select_token(stolen);
        if (t)
            return;

        another->add_token(t->type());
    }
}


// Marriage: Range 1-2. Effect: Instantly slay self or an allied unit in range. Another allied unit in range gains 2 speed, vitality, and strength, or just 1 if the slain unit was a thrall.
void marriage(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(1, 2);
    if (us.size() < 2)
        return c.no_target();

    list<tunit *> srcs = us;
    srcs.push_back(&c.self());
    tunit *src = c.player_must_select_unit(srcs);
    if (!src)
        return;

    tunit *dst = c.player_must_select_unit(us);
    if (!dst)
        return;

    int x = src->unit_type() == UNIT_THRALL ? 1 : 2;
    c.slay(*src);

    if (c.self().has_upgrade(UPGRADE_CONJOIN) && src == &c.self()) {
        for (ttoken *t : dst->tokens())
            dst->remove_token(*t, t->count());
    }

    dst->add_token(TOKEN_SPEED, x);
    dst->add_token(TOKEN_VITALITY, x);
    dst->add_token(TOKEN_STRENGTH, x);
}


void the_hunger(unit_action_context &c)
{
    if (!c.self().has_upgrade(UPGRADE_THE_HUNGER))
        return;

    if (c.self().corpses_in_range(2))
        c.unit_step(c.self(), 2);
}


void autophagia(unit_action_context &c)
{
    if (!c.self().has_upgrade(UPGRADE_AUTOPHAGIA))
        return;

    if (!c.player_may_take_action(TAKE_ACTION_AUTOPHAGIA))
        return;

    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, 1);
    if (!p)
        return;

    c.self().take_damage(1, enum_or(DAMAGE_NORMAL, DAMAGE_PIERCING));
    c.inc_corpse(*p, +1);
}


// Self Effect: Mutate, then gain 1 strength. Spare Parts: Then gain (3+) 1 speed, (5+) and 1 vitality, (6+) then mutate again. Roll 1D per corpse consumed for the effect.
void bloodgorger(unit_action_context &c)
{
    c.mutate(c.self());
    c.self().add_token(TOKEN_STRENGTH);

    int times = c.player_may_spare_parts(c.self());
    while (times--) {
        int d6 = c.player_roll_d6(c.self());
        if (d6 >= 3)
            c.self().add_token(TOKEN_SPEED);
        if (d6 >= 5)
            c.self().add_token(TOKEN_VITALITY);
        if (d6 >= 6)
            c.mutate(c.self());
    }
}

// Melee, Attack On hit: 1 damage. Effect: splash (self): 1 damage.
void bloody_slashes(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE);

    u->take_damage(1, DAMAGE_NORMAL);
    list<tunit *> splash = c.self().units_in_range(1, 1);
    for (tunit *u : splash)
        u->take_damage(1, DAMAGE_NORMAL);
}


// Range 2-4 Effect: Splash (target): Create a corpse in the area for every unit in the area, up to three times, then mutate.
void regurgitate(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    list<tunit *> splash = u->units_in_range(1, SELECT_UNIT_EXCLUDE_SELF);
    int corpses = min(3, (int)splash.size());
    int mutations = c.self().has_upgrade(UPGRADE_RAPID_ADAPTATION) ? corpses : 1;

    while (corpses--) {
        optional<map_pos> p = c.player_must_select_space(u->pos(), 1, 1);
        if (!p)
            return;
        c.inc_corpse(*p, +1);
    }

    if (c.self().has_upgrade(UPGRADE_DROWN_IN_VISCERA))
        regurgitate_drown_in_viscera(c, u);

    if (c.self().has_upgrade(UPGRADE_CLEANSING_WASH))
        regurgitate_cleansing_wash(c, u);

    if (!c.then())
        return;
    while (mutations--)
        c.mutate(c.self());
}


// Range 2-4, Attack On hit: 1 damage. Effect: Steal a positive token from target. Spare Parts: plus one more token per corpse consumed. If target has no positive tokens, gain 1 strength instead of stealing a token
void sin_eater(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE);

    u->take_damage(1, DAMAGE_NORMAL);

    if (u->n_tokens(SELECT_TOKEN_ONLY_POSITIVE)) {
        c.self().add_token(TOKEN_STRENGTH);
        return;
    }

    int times = 1 + c.player_may_spare_parts(c.self());
    while (times--) {
        ttoken *t = c.player_may_select_token(u->tokens());
        if (!t)
            return;
        u->remove_token(*t, 1);
        c.self().add_token(t->type(), 1);
    }
}


// Self Effect: Gain 1 strength, OR 1 speed, OR 1 vitality. Spare parts: Repeat this effect once for each corpse consumed.
void sculpt_flesh(unit_action_context &c)
{
    int times = max(1, c.player_may_spare_parts(c.self()));

    while (times--) {
        optional<token_type> t = c.player_must_select_token_type({TOKEN_STRENGTH, TOKEN_SPEED, TOKEN_VITALITY});
        if (!t)
            return;

        c.self().add_token(*t);
    }
}


// Melee Effect: Deal 1 devil damage to an adjacent unit. If reduce to 0 hp, obliterates unit and the homunculus gains any tokens the absorbed unit had.
void absorb(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE);

    bool dead = false;
    u->take_damage(1, DAMAGE_DEVIL, &dead);
    if (!dead)
        return;

    if (c.self().has_upgrade(UPGRADE_MELD))
        c.mutate(c.self());
    else
        c.obliterate(*u);

    for (ttoken *t : u->tokens())
        c.self().add_token(t->type(), t->count());

    if (c.self().has_upgrade(UPGRADE_FORM_CARAPACE))
        c.self().inc_counter(COUNTER_PHYSICAL_ARMOR, +1);
}


// Flesh Whip: Attack, Range 1-2 On hit: 1 damage and splash (target): 1 damage and (4+) create one (6+) or two corpse tokens in an adjacent space to target.
void flesh_whip(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6))
        return u->take_damage(1, DAMAGE_GRAZE);

    // self + splash
    for (tunit *near : u->units_in_range(1))
        u->take_damage(1, DAMAGE_NORMAL);

    int n = c.d6_gradations(d6, {{1, 0}, {4, 1}, {6, 2}});
    if (!n)
        return;

    optional<map_pos> p = c.player_must_select_space(u->pos(), 1);
    if (!p)
        return;

    c.inc_corpse(*p, n);
}


void ball_of_limbs(unit_action_context &c)
{
    if (!c.self().has_upgrade(UPGRADE_BALL_OF_LIMBS) || c.trigger() != TRIGGER_MOVE_SECOND)
        return;

    int walls = -1;
    int corpses = -1;
    c.unit_move(c.self(), enum_or(MOVEMENT_DESTROY_WALLS, MOVEMENT_ABSORB_CORPSES), &walls, &corpses);

    int times = walls + corpses;
    while (times--)
        c.mutate(c.self());
}


// At turn start, may convert one of this unit’s mutation tokens into strength, speed, or vitality. Then, mutate.
void polyglot(unit_action_context &c)
{
    ttoken *t = c.self().find_token(TOKEN_MUTATION);
    if (!t)
        return c.no_resources();
    optional<token_type> tt = c.player_may_select_token_type({TOKEN_STRENGTH, TOKEN_SPEED, TOKEN_VITALITY});
    if (tt) {
        c.self().remove_token(*t);
        c.self().add_token(*tt);
    }
    if (!c.then())
        return;
    c.mutate(c.self());
}


// At turn end, may remove one token from this unit and grant to a unit in range 2.
void accelerate_evolution(unit_action_context &c)
{
    list<ttoken *> ts = c.self().tokens();
    if (ts.empty())
        return c.no_resources();
    list<tunit *> us = c.self().units_in_range(2);
    if (us.empty())
        return c.no_target();

    ttoken *t = c.player_may_select_token(ts);
    if (!t)
        return;
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    c.self().remove_token(*t, 1);
    u->add_token(t->type(), 1);
}


// May remove a mutation token at start or end of own turn to step 2.
void rapid_move(unit_action_context &c)
{
    if (c.trigger() & TRIGGER_TURN_START) {
        c.self().inc_counter(COUNTER_RAPID_MOVE_AVAILABLE, +1);
        ttoken *t = c.self().find_token(TOKEN_MUTATION);
        if (!t)
            return c.no_resources();
        if (!c.player_may_take_action(TAKE_ACTION_RAPID_MOVE))
            return;

        c.self().remove_token(*t);
        c.unit_step(c.self(), 2);
        c.self().inc_counter(COUNTER_RAPID_MOVE_AVAILABLE, -1);
        return;
    }

    if (c.trigger() & TRIGGER_TURN_END) {
        bool rm_available = c.self().counter(COUNTER_RAPID_MOVE_AVAILABLE);
        ttoken *t = c.self().find_token(TOKEN_MUTATION);
        if (!rm_available || !t)
            return c.no_resources();

        if (c.player_may_take_action(TAKE_ACTION_RAPID_MOVE)) {
            c.self().remove_token(*t);
            c.unit_step(c.self(), 2);
        }
        c.self().set_counter(COUNTER_RAPID_MOVE_AVAILABLE, 0);
        return;
    }
}


// May remove a mutation token on self to grant +1D on any attack and allow it to ignore cover.
void ancillary_limbs(unit_action_context &c)
{
    ttoken *t = c.self().find_token(TOKEN_MUTATION);
    if (!t)
        return c.no_resources();
    if (!c.player_may_take_action(TAKE_ACTION_ANCILLARY_LIMBS))
        return;
    c.self().inc_counter(COUNTER_ANCILLARY_LIMBS, +1);
}


// Line 4. Line: 1 damage. Effect: Allies in the line mutate instead of taking damage.
void experimental_surgery(unit_action_context &c)
{
    list<tunit *> us = c.player_must_select_line(4);
    for (tunit *u : us) {
        if (c.self().is_ally(*u))
            c.mutate(*u);
        else
            u->take_damage(1, DAMAGE_NORMAL);
        if (!c.then())
            return;
    }
}


// Self. Effect: Create one, (3+) two, (5+) three corpses in free adjacent spaces, then mutate once for each corpse created.
void new_material(unit_action_context &c)
{
    int d6 = c.player_roll_d6(c.self());
    int cs = c.d6_gradations(d6, {{1, 1}, {3, 2}, {5, 3}});
    int n = 0;
    while (cs--) {
        optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, SELECT_SPACE_FREE);
        if (p) {
            c.inc_corpse(*p, +1);
            ++n;
        }
        if (!c.then())
            return;
    }
    while (n--) {
        c.mutate(c.self());
        if (!c.then())
            return;
    }
}


// Range 2. Effect: Create a perfect copy of an allied unit in range in any other free space in range. Then obliterate the original as it collapses into flesh and replace it with a corpse token.
void clone(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(2, enum_or(SELECT_UNIT_EXCLUDE_SELF, SELECT_UNIT_EXCLUDE_FOE));
    if (us.empty())
        return c.no_target();
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;
    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 2, SELECT_SPACE_FREE);
    if (!p)
        return c.no_target();
    c.copy_unit(*u, *p);
    if (!c.then())
        return;

    map_pos up = u->pos();
    c.obliterate(*u);
    c.inc_corpse(up, +1);
}


// Range 3. Effect: Remove up to three negative tokens from a unit, then it may step 1 and it mutates. Spare Parts: The unit mutates and steps once for each negative token removed instead.
void stitch_fix(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_NEGATIVE_TOKENS);
    if (us.empty())
        return c.no_target();
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int removed = 0;
    for (int i = 0; i < 3; ++i) {
        list<ttoken *> ts = u->tokens();
        ttoken *t = c.player_may_select_token(ts, SELECT_TOKEN_ONLY_NEGATIVE);
        if (!t)
            break;
        removed++;
    }
    if (!c.then())
        return;
    if (c.player_may_spare_parts(*u)) {
        if (c.player_may_take_action(TAKE_ACTION_STEP))
            c.unit_step(*u, removed);
        while (removed--)
            c.mutate(*u);
        return;
    }
    // no spare parts
    if (c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(*u, 1);
    c.mutate(*u);
}


// Range 3. Effect: Choose a unit in range. That unit may step 2. If it ends its turn in the space of a corpse, it mutates, removes the corpse, then may repeat this effect.
void inject_stimulant(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(3);
    if (us.empty())
        return c.no_target();
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    while (true) {
        if (!c.player_may_take_action(TAKE_ACTION_STEP))
            break;

        c.unit_step(*u, 2);
        bool on_corpse = c.inc_corpse(u->pos());
        if (!on_corpse)
            break;

        c.inc_corpse(u->pos(), -1);
        c.mutate(*u);
        if (!c.then())
            break;
    }
}


// Attack, range 2-5. On hit: 1 toxin damage. Effect: At end of target’s next turn, they explode for a splash (target) effect for 1 toxin damage. Your allies in the area mutate instead of taking damage.
void biotoxin_injector(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.no_target();
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;
    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE);

    u->take_damage(1, DAMAGE_TOXIN);
    u->inc_counter(COUNTER_BIOTOXIN_INJECTOR, +1);
}


// Attack, Range 2-5. On hit: May remove one positive token from target, then deal 1 toxin damage. Spare Parts: Repeat this effect once.
void mutagen_injector(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.no_target();
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;
    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE);

    int effect = c.player_may_spare_parts(c.self()) ? 2 : 1;
    while (effect--) {
        list<ttoken *> ts = u->tokens();
        ttoken *t = c.player_may_select_token(ts, SELECT_TOKEN_ONLY_POSITIVE);
        if (!t)
            break;
        u->remove_token(*t);
        u->take_damage(1, DAMAGE_TOXIN);
    }
}

// Line 3. Effect: Line: 1 fire damage and inflict 1 vulnerable, (3-4) OR 1 toxic damage and inflict 1 slow, (5-6) OR 1 curse damage and inflict 1 weak. Spare Parts: Roll 1D for effect per corpse consumed and choose any result.
void chaos_beam(unit_action_context &c)
{
    list<tunit *> us = c.player_must_select_line(3);

    optional<int> effect;
    {
        int n = max(1, c.player_may_spare_parts(c.self()));
        list<int> rolls;
        while (n--)
            rolls.push_back(c.player_roll_d6(c.self()));
        effect = c.player_must_select_roll(rolls);
    }
    if (!effect)
        return;

    for (tunit *u : us) {
        switch (*effect) {
        case 1:
        case 2:
            u->take_damage(1, DAMAGE_FIRE);
            u->add_token(TOKEN_VULNERABLE);
            break;
        case 3:
        case 4:
            u->take_damage(1, DAMAGE_TOXIN);
            u->add_token(TOKEN_SLOW);
            break;
        case 5:
        case 6:
            u->take_damage(1, DAMAGE_CURSE);
            u->add_token(TOKEN_WEAK);
            break;
        }
        if (!c.then())
            return;
    }
}


// (1 SOUL): Own or Allied Turn, Range 4. Trigger: Turn start. Effect: Unit gains 1 strength, (3-4) OR 1 speed, (5-6) OR 1 vitality. Spare Parts: May choose one token per corpse consumed instead of rolling.
void wild_mutation(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(4);
    if (us.empty())
        return c.no_target();

    if (c.player_may_spend_soul(1))
        return c.no_resources();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int spare_parts = c.player_may_spare_parts(c.self());
    list<token_type> ts;

    if (spare_parts) {
        while (spare_parts--) {
            optional<token_type> t = c.player_must_select_token_type({TOKEN_STRENGTH, TOKEN_SPEED, TOKEN_VITALITY});
            if (!t)
                break;
            ts.push_back(*t);
        }
    } else {
        switch (c.player_roll_d6(c.self())) {
            case 1:
            case 2:
                ts.push_back(TOKEN_STRENGTH);
                break;
            case 3:
            case 4:
                ts.push_back(TOKEN_SPEED);
                break;
            case 5:
            case 6:
                ts.push_back(TOKEN_VITALITY);
                break;
        }
    }
    for (token_type t : ts)
        u->add_token(t);
}


// (3 SOUL): Own or allied turn. Range 4. Copy all positive tokens on target unit, then grant them to another unit in range.
void sample_genome(unit_action_context &c)
{
    list<tunit *> srcs = c.self().units_in_range(4, SELECT_UNIT_WITH_POSITIVE_TOKENS);
    if (srcs.empty())
        return c.no_target();

    list<tunit *> dsts = c.self().units_in_range(4);
    if (dsts.size() < 2)
        return c.no_target();

    if (c.player_may_spend_soul(3))
        return c.no_resources();

    tunit *src = c.player_must_select_unit(srcs);
    if (!src)
        return;

    dsts.remove(src);
    tunit *dst = c.player_must_select_unit(dsts);
    if (!dst)
        return;

    for (ttoken *t : src->tokens()) {
        if (t->is_positive())
            dst->add_token(t->type(), t->count());
    }
}


// (1 SOUL): Own or allied turn. Range 3. Effect: Swap places with an allied unit, then both of you mutate.
void flesh_jump(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(3, enum_or(SELECT_UNIT_EXCLUDE_SELF, SELECT_UNIT_EXCLUDE_FOE));
    if (us.empty())
        return c.no_target();

    if (!c.player_may_spend_soul(1))
        return c.no_resources();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    c.swap_unit_pos(c.self(), *u);
    if (!c.then())
        return;
    c.mutate(c.self());
    c.mutate(*u);
}


// (2 SOUL): Own or allied turn. Range 3. Effect: Remove any number of mutation tokens on self or target unit, then target may step 2 per token removed with free movement, ignoring hazards.
void grow_bonus_legs(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_MUTATION_TOKENS);
    if (us.empty())
        return c.no_target();

    if (!c.player_may_spend_soul(2))
        return c.no_resources();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    ttoken *t = u->find_token(TOKEN_MUTATION);
    optional<int> remove = c.player_must_select_token_count(t->count());
    if (!remove)
        return;

    int removed = u->remove_token(*t, *remove);
    if (!c.then())
        return;
    c.unit_step(*u, removed * 2, enum_or(MOVEMENT_FREE, MOVEMENT_IGNORE_HAZARDS));
}


// (2 SOUL): Own or allied turn. Range 3. Trigger: Turn start. Effect: Remove any number of mutation tokens from target, then target gains +1D on attacks this turn per token removed, and their damage ignores armor.
void grow_bonus_limbs(unit_action_context &c)
{
    list<tunit *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_MUTATION_TOKENS);
    if (us.empty())
        return c.no_target();

    if (!c.player_may_spend_soul(2))
        return c.no_resources();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    ttoken *t = u->find_token(TOKEN_MUTATION);
    optional<int> remove = c.player_must_select_token_count(t->count());
    if (!remove)
        return;

    int removed = u->remove_token(*t, *remove);
    if (!c.then())
        return;
    u->inc_counter(COUNTER_GROW_BONUS_LIMBS, removed);
}


// (3 SOUL): Curse, Any turn. Trigger: Turn end. Effect: Deal 1 toxic damage to unit. If this reduces them to 0 hp, they are obliterated. Create up to 3 corpse tokens under their space or in free adjacent spaces.
void recycle(unit_action_context &c)
{
    tunit &u = c.activated();
    if (u.is_curseproof())
        return c.no_target();

    if (!c.player_may_spend_soul(3))
        return c.no_resources();

    bool dead = false;
    u.take_damage(1, DAMAGE_TOXIN, &dead);
    if (!dead)
        return;
    c.obliterate(u);
    optional<map_pos> p = c.player_must_select_space(u.pos(), 1, SELECT_SPACE_FREE);
    if (!p)
        return;
    optional<int> n = c.player_must_select_corpse_count(3);
    if (!n)
        return;
    c.inc_corpse(*p, *n);
}


// (4 SOUL): Curse, Foe turn. Range 3. Trigger: Turn start. Effect: Inflict 1 slow, weak, and vulnerable on a foe (5+) twice.
void devolve(unit_action_context &c)
{
    tunit &u = c.activated();
    if (u.is_curseproof())
        return c.no_target();

    if (!c.player_may_spend_soul(4))
        return c.no_resources();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_CURSE);
    int times = c.d6_gradations(d6, {{1, 1}, {5, 2}});
    while (times--) {
        u.add_token(TOKEN_SLOW);
        u.add_token(TOKEN_WEAK);
        u.add_token(TOKEN_VULNERABLE);
    }
}


// (6 SOUL): Own turn. Trigger: Turn start. Effect: Increase MV to 6, DF to 6+, gain 6 strength, become curseproof, gain free movement, and gain super armor. Keep any damage taken. At the end of your next turn, your form destabilizes and you are obliterated.
void final_form(unit_action_context &c)
{
    if (!c.player_may_spend_soul(6))
        return c.no_resources();

    c.self().inc_counter(COUNTER_CURSEPROOF, +1);
    c.self().inc_counter(COUNTER_CURSEPROOF, +1);
    c.self().inc_counter(COUNTER_FREE_MOVEMENT, +1);

    c.self().set_counter(COUNTER_ALTERED_MV, 6);
    c.self().set_counter(COUNTER_ALTERED_DF, 6);
    c.self().add_token(TOKEN_STRENGTH, 6);

    c.self().set_counter(COUNTER_FINAL_FORM, 2);
}

// *** UNITS ***
void gunwight(tcard &c)
{
    c.set_faction_type(FACTION_CARCASS, UNIT_THRALL);
    c.set_stats(2, 2, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, formation);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(ol45);
    c.add_act_ability(baton);

    c.add_upgrade(UPGRADE_BRACE);
    c.add_upgrade(UPGRADE_TACTICAL_RELOAD);
    c.add_upgrade(UPGRADE_SCAVENGE_AMMO);
}


void enforcer(tcard &c)
{
    c.set_faction_type(FACTION_CARCASS, UNIT_SCION);
    c.set_stats(3, 4, 3, ARMOR_PHYS);

    c.add_trait(TRIGGER_COMBAT_START, formation);

    c.add_act_ability(skull_crack);
    c.add_act_ability(flashbang);
    c.add_act_ability(shieldwall);

    c.add_upgrade(UPGRADE_SHIELD_CHARGE);
    c.add_upgrade(UPGRADE_BULWARK);
    c.add_upgrade(UPGRADE_BONE_DUST_NAPALM);
}


void ammo_goblin(tcard &c)
{
    c.set_faction_type(FACTION_CARCASS, UNIT_FREAK);
    c.set_stats(3, 4, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, formation);

    c.add_act_ability(regurgitate_ammo);
    c.add_act_ability(bone_shards);
    c.add_act_ability(destructive_glee);

    c.add_upgrade(UPGRADE_VOMIT_BULLETS);
    c.add_upgrade(UPGRADE_NAPALM_INJECTOR);
    c.add_upgrade(UPGRADE_HOT_CHAMBER);
}


void barrelform(tcard &c)
{
    c.set_faction_type(FACTION_CARCASS, UNIT_HUNTER);
    c.set_stats(2, 4, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, formation);

    c.add_act_ability(snipe);
    c.add_act_ability(transform_to_gun);
    c.add_act_ability(deathmark);

    c.add_upgrade(UPGRADE_CLAW_PITONS);
    c.add_upgrade(UPGRADE_EXTENDED_BARREL);
    c.add_upgrade(UPGRADE_CALIBER_UP);
}


void egis_weapon(tcard &c)
{
    c.set_faction_type(FACTION_CARCASS, UNIT_TYRANT);
    c.set_stats(3, 6, 2, ARMOR_PHYS);

    c.add_trait(TRIGGER_COMBAT_START, formation);
    c.add_trait(TRIGGER_COMBAT_START, siege_shield);
    c.add_trait(TRIGGER_COMBAT_START, large);

    c.add_act_ability(juggernaut);
    c.add_act_ability(mortar);
    c.add_act_ability(catechism_devil_cannon);

    c.add_upgrade(UPGRADE_FORTIFY);
    c.add_upgrade(UPGRADE_HEAVY_CALIBER_CANNON);
    c.add_upgrade(UPGRADE_GUNNER_PIVOT);
}


void infiltrator(tcard &c)
{
    c.set_faction_type(FACTION_CARCASS, UNIT_HUNTER);
    c.set_stats(3, 3, 3, ARMOR_MAG);

    c.add_trait(TRIGGER_COMBAT_START, formation);
    c.add_trait(TRIGGER_TURN_END, active_camo);

    c.add_act_ability(cross_fire);
    c.add_act_ability(slide_kick);
    c.add_act_ability(trick_reload);

    c.add_upgrade(UPGRADE_BURST_FIRE);
    c.add_upgrade(UPGRADE_RICOCHET);
    c.add_upgrade(UPGRADE_KNIFE_SLASH);
}


void warhead(tcard &c)
{
    c.set_faction_type(FACTION_GOREGRINDERS, UNIT_THRALL);
    c.set_stats(4, 1, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, blood_rage);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(charge);
    c.add_act_ability(cleave);

    c.add_upgrade(UPGRADE_BLADED);
    c.add_upgrade(UPGRADE_OVERCLOCKED);
    c.add_upgrade(UPGRADE_LOBOTOMIZED);
}


void carnifex(tcard &c)
{
    c.set_faction_type(FACTION_GOREGRINDERS, UNIT_SCION);
    c.set_stats(4, 4, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, blood_rage);

    c.add_act_ability(rev);
    c.add_act_ability(chainsaw);
    c.add_act_ability(wild_slashes);

    c.add_upgrade(UPGRADE_HEAVY_SWING);
    c.add_upgrade(UPGRADE_RILE);
    c.add_upgrade(UPGRADE_BLOODY_TEETH);
}


void pain_ghoul(tcard &c)
{
    c.set_faction_type(FACTION_GOREGRINDERS, UNIT_FREAK);
    c.set_stats(4, 4, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, blood_rage);

    c.add_act_ability(pain_frenzy);
    c.add_act_ability(cauterize);
    c.add_act_ability(meat_hook);

    c.add_upgrade(UPGRADE_STIM_HAZE);
    c.add_upgrade(UPGRADE_FRENZY_HOOK);
    c.add_upgrade(UPGRADE_ADRENALIZE);
}


void painwheel(tcard &c)
{
    c.set_faction_type(FACTION_GOREGRINDERS, UNIT_HORROR);
    c.set_stats(5, 4, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, blood_rage);
    c.add_trait(TRIGGER_COMBAT_START, hellwheel);

    c.add_act_ability(exfoliate);
    c.add_act_ability(spin_out);

    c.add_upgrade(UPGRADE_BARBED_WHEEL);
    c.add_upgrade(UPGRADE_ENDLESS_SCREAMING);
    c.add_upgrade(UPGRADE_HURTLE);
}


void berserker(tcard &c)
{
    c.set_faction_type(FACTION_GOREGRINDERS, UNIT_TYRANT);
    c.set_stats(4, 6, 2, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, blood_rage);
    c.add_trait(TRIGGER_DAMAGED, steaming_rage);
    c.add_trait(TRIGGER_COMBAT_START, large);

    c.add_act_ability(pulverize);
    c.add_act_ability(building_rage);
    c.add_act_ability(rip_and_tear);

    c.add_upgrade(UPGRADE_MACHINEHEART);
    c.add_upgrade(UPGRADE_FUEL_INJECTORS);
    c.add_upgrade(UPGRADE_BIFURCATE);
}


void gearhead(tcard &c)
{
    c.set_faction_type(FACTION_GOREGRINDERS, UNIT_FREAK);
    c.set_stats(4, 4, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, blood_rage);

    c.add_act_ability(tinker);
    c.add_act_ability(overlock);
    c.add_act_ability(restart_engine);

    c.add_upgrade(UPGRADE_RAGE_OVERCLOCK);
    c.add_upgrade(UPGRADE_ROLLING_START);
    c.add_upgrade(UPGRADE_FRENZIED_TINKERING);
}


void scum(tcard &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_THRALL);
    c.set_stats(3, 1, 3, ARMOR_SUPER);

    c.add_trait(TRIGGER_SLAINED, toxic_revenge);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(pseudopod);
    c.add_act_ability(shamble);

    c.add_upgrade(UPGRADE_BLOAT);
    c.add_upgrade(UPGRADE_TENTACLE_WHIP);
    c.add_upgrade(UPGRADE_AFTERMATH);
}


void rotten(tcard &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_SCION);
    c.set_stats(4, 4, 3, ARMOR_PHYS);

    c.add_trait(TRIGGER_COMBAT_START, plaguebearer);

    c.add_act_ability(pustulate);
    c.add_act_ability(vomitous_mass);
    c.add_act_ability(rotblade);

    c.add_upgrade(UPGRADE_CATALYZE);
    c.add_upgrade(UPGRADE_INVIGORATING_VISCERA);
    c.add_upgrade(UPGRADE_SMOG_SHROUD);
}


void leech(tcard &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_FREAK);
    c.set_stats(4, 4, 4, ARMOR_MAG);

    c.add_trait(TRIGGER_SLAINED, deathwash);

    c.add_act_ability(suppurate);
    c.add_act_ability(evolve_strain);
    c.add_act_ability(swell_with_corruption);

    c.add_upgrade(UPGRADE_MASSIVE_SWELL);
    c.add_upgrade(UPGRADE_PANDEMIC);
    c.add_upgrade(UPGRADE_ACID_BLOOD);
}


void host(tcard &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_HUNTER);
    c.set_stats(3, 4, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_SLAINED, swarm_release);

    c.add_act_ability(propagate_swarm);
    c.add_act_ability(driving_vermin);

    c.add_upgrade(UPGRADE_SWARM_FEED);
    c.add_upgrade(UPGRADE_TOXIC_AVENGER);
    c.add_upgrade(UPGRADE_DEFILER);
}


void slime(tcard &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_HORROR);
    c.set_stats(4, 4, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_SLAINED, vile_rupture);

    c.add_act_ability(percolate);
    c.add_act_ability(surge);

    c.add_upgrade(UPGRADE_ROTTEN_SURGE);
    c.add_upgrade(UPGRADE_STICKY_TRAIL);
    c.add_upgrade(UPGRADE_THE_GUNK);
}


void belcher(tcard &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_THRALL);
    c.set_stats(3, 1, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_SLAINED, burst_guts);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(toxic_spit);
    c.add_act_ability(projectile_vomit);

    c.add_upgrade(UPGRADE_CONGEALED);
    c.add_upgrade(UPGRADE_PUTRIDITY);
    c.add_upgrade(UPGRADE_STUFFED);
}


void slimelet(tcard &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_THRALL);
    c.set_stats(3, 1, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, spit_up);
    c.add_trait(TRIGGER_COMBAT_START, summoned_thrall);

    c.add_act_ability(toxic_spit);
}


void sacrifice(tcard &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_THRALL);
    c.set_stats(4, 2, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_SLAINED, inverted_crucifix);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(beckon);
    c.add_act_ability(shudder);

    c.add_upgrade(UPGRADE_SQUIRM);
    c.add_upgrade(UPGRADE_DEAD_GRASP);
    c.add_upgrade(UPGRADE_IMPENDING_DEATH);
}


void chosen(tcard &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_SCION);
    c.set_stats(4, 3, 5, ARMOR_MAG);

    c.add_trait(TRIGGER_MOVE, slither);

    c.add_act_ability(tombraiser);
    c.add_act_ability(kidnap);
    c.add_act_ability(serpents_kiss);

    c.add_upgrade(UPGRADE_IVORY_SERPENT);
    c.add_upgrade(UPGRADE_LEAP);
    c.add_upgrade(UPGRADE_FOUL_MONUMENTS);
}


void visigheist(tcard &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_HORROR);
    c.set_stats(4, 3, 5, ARMOR_MAG);

    c.add_trait(TRIGGER_MOVE, teleport);

    c.add_act_ability(horrendous_shriek);
    c.add_act_ability(urgal_blade);

    c.add_upgrade(UPGRADE_TERRORIZE);
    c.add_upgrade(UPGRADE_SOUL_FROST);
    c.add_upgrade(UPGRADE_CONDEMN);
}


void banshee(tcard &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_HUNTER);
    c.set_stats(3, 3, 5, ARMOR_MAG);

    c.add_trait(TRIGGER_COMBAT_START, soul_sight);

    c.add_act_ability(bale_scream);
    c.add_act_ability(tombstone);

    c.add_upgrade(UPGRADE_DOOM_BELL);
    c.add_upgrade(UPGRADE_FREEZE_SOUL);
    c.add_upgrade(UPGRADE_SIREN);
}


void bound_devil(tcard &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_TYRANT);
    c.set_stats(3, 6, 2, ARMOR_MAG);

    c.add_trait(TRIGGER_MOVE, labyrinth_master);
    c.add_trait(TRIGGER_COMBAT_START, large);

    c.add_act_ability(hells_grasp);
    c.add_act_ability(beckon_lamb);
    c.add_act_ability(horrendous_end);

    c.add_upgrade(UPGRADE_TO_THE_SLAUGHTER);
    c.add_upgrade(UPGRADE_SUPREMACY);
    c.add_upgrade(UPGRADE_STRONG_PACT);
}


void lithogeist(tcard &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_HORROR);
    c.set_stats(0, 3, 3, ARMOR_SUPER);

    c.add_trait(TRIGGER_COMBAT_START, buried_alive);
    // TODO: trigger is unknown
    // c.add_trait(TRIGGER_ANY, tomb_bound);

    c.add_act_ability(strangle);
    c.add_act_ability(grave_bind);

    c.add_upgrade(UPGRADE_TOMB_BURST);
    c.add_upgrade(UPGRADE_RUIN_GEIST);
    c.add_upgrade(UPGRADE_PULL_UNDER);
}


void penitent(tcard &c)
{
    c.set_faction_type(FACTION_ABHORRER, UNIT_SCION);
    c.set_stats(4, 3, 2, ARMOR_SUPER);

    c.add_trait(TRIGGER_TURN_END, miracle);

    c.add_act_ability(mea_culpa);
    c.add_act_ability(holy_water);
    c.add_act_ability(excoriate);

    c.add_upgrade(UPGRADE_HOLY_BLOOD);
    c.add_upgrade(UPGRADE_CAGER);
    c.add_upgrade(UPGRADE_TASTE_THE_LASH);
}


void zealot(tcard &c)
{
    c.set_faction_type(FACTION_ABHORRER, UNIT_HORROR);
    c.set_stats(4, 4, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_TURN_END, miracle);
    c.add_trait(TRIGGER_MOVE, zealotry);

    c.add_act_ability(smite);
    c.add_act_ability(whirling_chain);

    c.add_upgrade(UPGRADE_PUNISHER);
    c.add_upgrade(UPGRADE_SUFFUSE);
    c.add_upgrade(UPGRADE_FIERY_CHAIN);
}


void antipriest(tcard &c)
{
    c.set_faction_type(FACTION_ABHORRER, UNIT_FREAK);
    c.set_stats(3, 4, 3, ARMOR_MAG);

    c.add_trait(TRIGGER_TURN_END, miracle);

    c.add_act_ability(gentleness);
    c.add_act_ability(delay_judgement);
    c.add_act_ability(blessed_censer);

    c.add_upgrade(UPGRADE_CLEANSING_PRAYER);
    c.add_upgrade(UPGRADE_CONSECRATE);
    c.add_upgrade(UPGRADE_BOILING_CENSER);
}


void inquisitor(tcard &c)
{
    c.set_faction_type(FACTION_ABHORRER, UNIT_HUNTER);
    c.set_stats(3, 4, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_TURN_END, miracle);

    c.add_act_ability(winch);
    c.add_act_ability(requiesce_en_pace);

    c.add_upgrade(UPGRADE_FOCUS);
    c.add_upgrade(UPGRADE_HEART_DESTROYER);
    c.add_upgrade(UPGRADE_EXPLOSIVE_BOLTS);
}


void holy_body(tcard &c)
{
    c.set_faction_type(FACTION_ABHORRER, UNIT_TYRANT);
    c.set_stats(3, 3, 2, ARMOR_SUPER);

    c.add_trait(TRIGGER_COMBAT_START, miraculous_flesh);
    c.add_trait(TRIGGER_COMBAT_START, large);

    c.add_act_ability(bolides);
    c.add_act_ability(indignation);
    c.add_act_ability(ablutions);

    c.add_upgrade(UPGRADE_SCATHE);
    c.add_upgrade(UPGRADE_HOLY_FONT);
    c.add_upgrade(UPGRADE_WITNESS);
}


void abbot(tcard &c)
{
    c.set_faction_type(FACTION_ABHORRER, UNIT_FREAK);
    c.set_stats(4, 3, 3, ARMOR_PHYS);

    c.add_trait(TRIGGER_TURN_END, miracle);

    c.add_act_ability(bible_thump);
    c.add_act_ability(blood_of_the_covenant);
    c.add_act_ability(wrath);

    c.add_upgrade(UPGRADE_THE_GOOD_WORD);
    c.add_upgrade(UPGRADE_CLEANSING_THE_HOSTS);
    c.add_upgrade(UPGRADE_SANCTIFIED_SACRIFICE);
}


void exorcist(tcard &c)
{
    c.set_faction_type(FACTION_ABHORRER, UNIT_NECROMANCER);
    c.set_stats(4, 10, 4, ARMOR_NONE);
}


void stitch(tcard &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_THRALL);
    c.set_stats(3, 2, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_SLAINED, fall_to_shambles);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(unstable_mutation);
    c.add_act_ability(twisting_strike);

    c.add_upgrade(UPGRADE_GENESTEALER);
    c.add_upgrade(UPGRADE_SPREAD_MUTATE);
    c.add_upgrade(UPGRADE_WARPING_MUTATE);
}


void chop_doc(tcard &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_FREAK);
    c.set_stats(4, 4, 4, ARMOR_MAG);

    c.add_trait(TRIGGER_AFTER_MOVE, leftovers);

    c.add_act_ability(inject_mutagen);
    c.add_act_ability(purge);
    c.add_act_ability(marriage);

    c.add_upgrade(UPGRADE_ABSORB);
    c.add_upgrade(UPGRADE_SCOUR_FLESH);
    c.add_upgrade(UPGRADE_CONJOIN);
}


void lycan(tcard &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_HORROR);
    c.set_stats(4, 4, 4, ARMOR_NONE);

    // TODO: how to implement lope?
    // c.add_trait(TRIGGER_COMBAT_START, lope);
    c.add_trait(TRIGGER_TURN_START, the_hunger);
    c.add_trait(TRIGGER_TURN_START, autophagia);

    c.add_act_ability(bloodgorger);
    c.add_act_ability(bloody_slashes);

    c.add_upgrade(UPGRADE_THE_HUNGER);
    c.add_upgrade(UPGRADE_AUTOPHAGIA);
    // TODO: how to implement it?
    c.add_upgrade(UPGRADE_HUNCH);
}


void strigoi(tcard &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_HUNTER);
    c.set_stats(3, 4, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, flight);

    c.add_act_ability(regurgitate);
    c.add_act_ability(sin_eater);

    c.add_upgrade(UPGRADE_DROWN_IN_VISCERA);
    c.add_upgrade(UPGRADE_RAPID_ADAPTATION);
    c.add_upgrade(UPGRADE_CLEANSING_WASH);
}


void homonculus(tcard &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_TYRANT);
    c.set_stats(4, 6, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, warpflesh);
    c.add_trait(TRIGGER_COMBAT_START, large);

    c.add_act_ability(sculpt_flesh);
    c.add_act_ability(absorb);
    c.add_act_ability(flesh_whip);

    c.add_upgrade(UPGRADE_BALL_OF_LIMBS);
    c.add_upgrade(UPGRADE_MELD);
    c.add_upgrade(UPGRADE_FORM_CARAPACE);
    c.set_move_override(ball_of_limbs);
}


void chirurgeon(tcard &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_NECROMANCER);
    c.set_stats(4, 10, 3, ARMOR_MAG);

    c.add_trait(TRIGGER_TURN_START, polyglot);
    c.add_bonus_trait(TRIGGER_TURN_END, accelerate_evolution);
    c.add_bonus_trait(enum_or(TRIGGER_TURN_START, TRIGGER_TURN_END), rapid_move);
    c.add_bonus_trait(TRIGGER_BEFORE_ATTACK, ancillary_limbs);

    c.add_act_ability(experimental_surgery);
    c.add_upgrade_act_ability(new_material);
    c.add_upgrade_act_ability(clone);
    c.add_upgrade_act_ability(stitch_fix);
    c.add_upgrade_act_ability(inject_stimulant);
    c.add_upgrade_act_ability(biotoxin_injector);
    c.add_upgrade_act_ability(mutagen_injector);
    c.add_upgrade_act_ability(chaos_beam);

    c.add_soul_ability(enum_or(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, TRIGGER_TURN_START), wild_mutation);
    c.add_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, sample_genome);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, flesh_jump);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, grow_bonus_legs);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, grow_bonus_limbs);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_TURN_END), recycle);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_FOE_TURN, TRIGGER_TURN_START), devolve);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_OWN_TURN, TRIGGER_TURN_START), final_form);
}


void carcass(tfaction &c)
{
    c.add_card(gunwight);
    c.add_card(enforcer);
    c.add_card(ammo_goblin);
    c.add_card(barrelform);
    c.add_card(egis_weapon);
    // c.add_card(operator);
}


void goregrinders(tfaction &c)
{
    c.add_card(warhead);
    c.add_card(carnifex);
    c.add_card(pain_ghoul);
    c.add_card(painwheel);
    c.add_card(berserker);
}


void gargamox(tfaction &c)
{
    c.add_card(scum);
    c.add_card(rotten);
    c.add_card(leech);
    c.add_card(host);
    c.add_card(slime);
}


void deadsouls(tfaction &c)
{
    c.add_card(sacrifice);
    c.add_card(chosen);
    c.add_card(visigheist);
    c.add_card(banshee);
    c.add_card(bound_devil);
}


void abhorrer(tfaction &c)
{
    c.add_card(penitent);
    c.add_card(zealot);
    c.add_card(antipriest);
    c.add_card(inquisitor);
    c.add_card(holy_body);
}


void igorri(tfaction &c)
{
    c.add_card(stitch);
    c.add_card(chop_doc);
    c.add_card(lycan);
    c.add_card(strigoi);
    c.add_card(homonculus);
    c.add_card(chirurgeon);
}


int main()
{
    cout << "hi!\n";
    return 0;
}
