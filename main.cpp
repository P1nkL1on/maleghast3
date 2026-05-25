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
    TRIGGER_ROUND_START, // round is when every unit takes a turn
    TRIGGER_TURN_START,
    TRIGGER_TURN_END,
    TRIGGER_BEFORE_MOVE,
    TRIGGER_AFTER_MOVE,
    TRIGGER_BEFORE_ACT,
    TRIGGER_AFTER_ACT,
    TRIGGER_BEFORE_ATTACK,
    TRIGGER_SLAINED,
    TRIGGER_AFTER_DAMAGED,
    TRIGGER_AFTER_HP_CHANGED,
    TRIGGER_AFTER_POS_CHANGED, // should be called at EVERY map_pos travelled during move/step
    TRIGGER_AFTER_HAZARD_CHANGED,
    TRIGGER_AFTER_WALL_CHANGED,

    TRIGGER_SOUL_OWN_TURN,
    TRIGGER_SOUL_ALLIED_TURN,
    TRIGGER_SOUL_FOE_TURN,
    TRIGGER_SOUL_OWN_OR_ALLIED_TURN = TRIGGER_SOUL_OWN_TURN | TRIGGER_SOUL_ALLIED_TURN,
    TRIGGER_SOUL_ANY_TURN = TRIGGER_SOUL_OWN_TURN | TRIGGER_SOUL_ALLIED_TURN | TRIGGER_SOUL_FOE_TURN,

    // ask a player, will it do it when it may?
    TAKE_ACTION_SCAVENGE_AMMO,
    TAKE_ACTION_RAPID_MOVE,
    TAKE_ACTION_ANCILLARY_LIMBS,
    TAKE_ACTION_AUTOPHAGIA,
    TAKE_ACTION_CONSECRATE,
    TAKE_ACTION_PROPAGATE_SWARM,
    TAKE_ACTION_LEAP,
    TAKE_ACTION_STEP,

    // what bonus types can be applied to a roll?
    ROLL_TAG_NONE,
    ROLL_TAG_ATTACK,
    ROLL_TAG_IGNORE_COVER,

    // which tokens to select?
    SELECT_TOKEN_ANY,
    SELECT_TOKEN_ONLY_NEGATIVE,
    SELECT_TOKEN_ONLY_POSITIVE,
    SELECT_TOKEN_ONLY_PLAGUE,
    SELECT_TOKEN_ONLY_REMOVABLE,

    // can it be negated by armor?
    DAMAGE_NORMAL,
    DAMAGE_TOXIC,
    DAMAGE_GRAZE,
    DAMAGE_FIRE,
    DAMAGE_HOLY,
    DAMAGE_CURSE,
    DAMAGE_DEVIL,
    DAMAGE_PIERCING,
    DAMAGE_CANT_BE_INCREASED,
    DAMAGE_CANT_BE_DECREASED,
    DAMAGE_CANT_SLAY,

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

    COUNTER_CURSEPROOF,
    COUNTER_HAS_COVER_FROM_ALL_DIRECTIONS,
    COUNTER_UNABLE_TO_MOVE,
    COUNTER_UNABLE_TO_STEP,
    COUNTER_IMMUNE_TO_PUSH,
    COUNTER_IMMUNE_TO_PULL,
    COUNTER_IMMUNE_TO_HAZARDS,
    COUNTER_MOVEMENT_FREE,
    COUNTER_MOVEMENT_THROUGH_WALLS,
    COUNTER_MOVEMENT_DESTROY_WALLS,
    COUNTER_MOVEMENT_ABSORB_CORPSES,
    COUNTER_LAST_MOVEMENT_WALLS_DESTROYED,
    COUNTER_LAST_MOVEMENT_CORPSES_ABSORBED,
    COUNTER_SUPER_ARMOR,
    COUNTER_PHYSICAL_ARMOR,
    COUNTER_ALTERED_MV,
    COUNTER_ALTERED_DF,
    COUNTER_FLIGHT,
    COUNTER_IS_2X2,
    COUNTER_COST_HALF_UNIT_SLOT,
    COUNTER_ACTIVATED_TWO_AT_A_TIME,
    // When slain, does not remove Doom, and (5+) Dooms slayer.
    COUNTER_INVERTED_CRUCIFIX,

    COUNTER_FORMATION,
    COUNTER_RELOAD,
    COUNTER_RELOAD_2,
    COUNTER_SCAVENGE_AMMO_AVAILABLE,
    COUNTER_BONE_SHARDS,
    COUNTER_TRANSFORM_TO_GUN,

    COUNTER_BLOOD_RAGE,
    // After any ability resolves that damages this unit, this unit deals 1 damage back to the ability's owner, even if this unit was slain.
    COUNTER_RETALIATION,
    COUNTER_STEAMING_RAGE,
    COUNTER_RETALIATION_DECREASE_ON_TURN_START,
    COUNTER_RETALIATION_DECREASE_ON_TURN_END,
    COUNTER_MACHINEHEART,

    COUNTER_SMOG_SHROUD,
    // Gains death burst: splash (self): 1 toxic damage and 1 plague. This effect cannot stack with itself but stacks with other death burst effects.
    COUNTER_SUPPURATE,
    // Gains death burst: Create a hazard instead of a corpse.
    COUNTER_ACID_BLOOD,

    COUNTER_SLITHER,

    COUNTER_MIRACLE,
    COUNTER_DELAY_JUDGEMENT,
    COUNTER_SMITE,
    COUNTER_WINCH,
    COUNTER_ABLUTIONS,
    COUNTER_BLOOD_OF_THE_COVENANT,
    COUNTER_CANT_GET_VITALITY,

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
    SELECT_UNIT_WITH_PLAGUE_TOKENS,
    SELECT_UNIT_WITH_VITALITY_TOKENS,
    SELECT_UNIT_WITHOUT_CURSEPROOF,
    SELECT_UNIT_WITH_DEATHBURST,

    // +1 max range if unit has cover against a target
    SELECT_UNIT_MODIFY_BRACE,
    SELECT_UNIT_IGNORE_LINE_OF_SIGHT,
    // if selects more than 1 unit, all of them must be adjacent to each other
    SELECT_UNIT_ADJACENT_TARGETS,
    // no maximum range against isolated units
    SELECT_UNIT_MODIFY_DEAD_GRASP,
};


enum select_space_filter
{
    SELECT_SPACE_EXCLUDE_NONE,
    SELECT_SPACE_EXCLUDE_OCCUPIED,
    SELECT_SPACE_EXCLUDE_WALLS,
    SELECT_SPACE_EXCLUDE_CORPSELESS,
};


enum token_type
{
    TOKEN_STRENGTH,
    TOKEN_WEAK,
    TOKEN_SPEED,
    TOKEN_SLOW,
    TOKEN_VITALITY,
    TOKEN_VULNERABLE,
    TOKEN_BERSERK,
    TOKEN_PLAGUE,
    TOKEN_DOOM,
    TOKEN_MUTATION,
};


enum movement_tags
{
    MOVEMENT_DEFAULT,
    MOVEMENT_FREE,
    MOVEMENT_IGNORE_HAZARDS,
    MOVEMENT_DESTROY_WALLS,
    MOVEMENT_ABSORB_CORPSES,
    // If pulling a unit into a wall, +2 distance and the kidnapped unit can pass through walls and units during this movement
    MOVEMENT_KIDNAP,
    // automatically added on push/pull
    MOVEMENT_FORCED,
};


enum upgrade
{
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
    virtual bool remove_token(token_type type, int count = 1) = 0;
    virtual void gain_token(token_type type, int count = 1) = 0;

    virtual void push(tunit &from, int distance = 1) = 0;
    virtual void pull(tunit &to, int distance = 1, movement_tags extra_tags = MOVEMENT_DEFAULT) = 0;

    virtual int inc_counter(int counter, int x, int def_value = 0) = 0;
    virtual void set_counter(int counter, int x) = 0;
    virtual int counter(int counter, int def_value = 0) const = 0;

    virtual list<tunit *> units_in_range(int, select_unit_filter exclude = SELECT_UNIT_EXCLUDE_NONE) const = 0;
    virtual list<tunit *> units_in_range(int, int, select_unit_filter exclude = SELECT_UNIT_EXCLUDE_NONE) const = 0;
    virtual int corpses_in_range(int) const = 0;
    virtual int corpses_in_range(int, int) const = 0;
    virtual list<map_pos> spaces_in_range(int, int, select_space_filter = SELECT_SPACE_EXCLUDE_NONE) const = 0;
    virtual bool is_ally(tunit &) const = 0;
    virtual void take_damage(int x, int type, tunit *from, bool *slayed = nullptr) = 0;
    virtual map_pos pos() const = 0;
    virtual bool has_upgrade(upgrade) const = 0;
    virtual int unit_type() const = 0;
    virtual int faction() const = 0;
    virtual bool is_slain() const = 0;
    virtual int n_moves() const = 0;
    virtual int n_acts() const = 0;
    virtual int hp() const = 0;
    virtual void set_hp(int hp = 1) = 0;
    virtual int inc_moves(int inc) = 0;
    virtual bool has_cover(tunit &from) const = 0;
    virtual bool is_in_formation() const = 0;
    virtual bool is_isolated() const { return !is_in_formation(); }
    // walls can't trigger effects TODO: check every ability w/ a wall in tests
    virtual bool can_trigger_effects() const = 0;

    virtual void may_treat_token_a_as_b(token_type a, token_type b) = 0;

    virtual int size() const { return counter(COUNTER_IS_2X2) ? 2 : 1; }
    virtual bool is_curseproof() const { return counter(COUNTER_CURSEPROOF) > 0; }
};


struct ttoken
{
    virtual ~ttoken() = default;
    virtual int count() const = 0;
    virtual bool is_positive() const = 0;
    virtual bool is_negative() const = 0;
    virtual bool is_removable() const = 0;
    virtual token_type type() const = 0;
};


struct taction
{
    virtual ~taction() = default;

    virtual int trigger() const = 0;

    virtual tunit &self() = 0;
    virtual tunit &activated() = 0;
    // splits the turn to the atomic actions, that can't be interrupted
    virtual bool then() = 0;
    // prevent action, because no target
    virtual void no_target() = 0;
    // prevent action, because no resources
    virtual void no_resources() = 0;

    #ifndef NDEBUG
    virtual void unimplemented() = 0;
    #endif

    virtual optional<int> player_must_select_roll(const list<int> &dice_rolls) = 0;
    virtual optional<int> player_must_select_token_count(int up_to_x) = 0;
    virtual optional<int> player_must_select_corpse_count(int up_to_x) = 0;
    virtual optional<token_type> player_may_select_token_type(const list<token_type> &token_types) = 0;
    virtual optional<token_type> player_must_select_token_type(const list<token_type> &token_types) = 0;
    virtual ttoken *player_may_select_token(const list<ttoken *> &tokens, int filter = SELECT_TOKEN_ANY) = 0;
    virtual ttoken *player_must_select_token(const list<ttoken *> &tokens, int filter = SELECT_TOKEN_ANY) = 0;
    virtual list<ttoken *> player_must_select_tokens(const list<ttoken *> &tokens, int up_tp, int filter = SELECT_TOKEN_ANY) = 0;
    virtual tunit *player_may_select_unit(const list<tunit *> &units, const list<tunit *> &exclude = {}) = 0;
    virtual tunit *player_must_select_unit(const list<tunit *> &units, const list<tunit *> &exclude = {}) = 0;
    virtual list<tunit *> player_must_select_units(const list<tunit *> &units, int min, int max) = 0;
    virtual list<tunit *> player_must_select_infect(tunit &from) = 0;
    virtual list<tunit *> player_must_select_line(int, list<map_pos> *poses = nullptr) = 0;
    virtual optional<map_pos> player_must_select_space(const map_pos &, int range, select_space_filter filter = SELECT_SPACE_EXCLUDE_NONE) = 0;
    virtual optional<map_pos> player_must_select_space(const map_pos &, int min, int max, select_space_filter filter = SELECT_SPACE_EXCLUDE_NONE) = 0;
    virtual list<map_pos> player_must_select_spaces(const map_pos &, int up_to, int min, int max, select_space_filter filter = SELECT_SPACE_EXCLUDE_NONE) = 0;
    virtual bool player_may_take_action(int) = 0;
    virtual bool player_may_spend_soul(int x) = 0;
    virtual int player_roll_d6(tunit &who, int tags = ROLL_TAG_NONE, int extra_mod = +0) = 0;
    virtual int d6_gradations(int d6, const map<int, int> &treshold_to_result = {}) const = 0;
    virtual bool is_headshot(int d6) const = 0;

    virtual list<tunit *> units_in_range(const map_pos &, int min, int max, select_unit_filter f = SELECT_UNIT_EXCLUDE_NONE) const = 0;

    virtual bool is_hit(tunit &target, int d6) const = 0;
    virtual void unit_move(tunit &, movement_tags extra_tags = MOVEMENT_DEFAULT) = 0;
    // TODO: may unit trigger something on step and die? then it should be [[no_discard]] bool unit_step
    virtual void unit_step(tunit &, int range = 1, movement_tags tags = MOVEMENT_DEFAULT) = 0;
    virtual void slay(tunit &) = 0;
    virtual void obliterate(tunit &) = 0;
    virtual int inc_corpse(const map_pos &, int x = 0) = 0;
    virtual tunit &copy_unit(tunit &, const map_pos &new_pos) = 0;
    virtual void swap_unit_pos(tunit &, tunit &) = 0;
    virtual void set_wall(const map_pos &) = 0;
    virtual bool is_wall(const map_pos &) const = 0;
    virtual void destroy_wall(const map_pos &) = 0;
    virtual void set_hazard(const map_pos &) = 0;
    virtual bool is_hazard(const map_pos &) const = 0;
    virtual void set_adverse_terrain(const map_pos &) = 0;
    virtual bool is_adverse_terrain(const map_pos &) const = 0;

    virtual void reload(tunit &t) { t.set_counter(COUNTER_RELOAD, 0); }

    virtual void trigger_deathburst(tunit &t) = 0;

    virtual int round() const = 0;
    virtual bool round(int x) const { return round() >= x; }
    virtual bool round_even() const { return !(round() % 2); }

    virtual void mutate(tunit &unit) { unit.gain_token(TOKEN_MUTATION, +1); }
    virtual int player_may_spare_parts(tunit &) = 0;
};


using action_foo = void(*)(taction &);


struct tcard
{
    virtual ~tcard() = default;
    virtual void set_faction_type(int faction, int type) = 0;
    virtual void set_stats(int mv, int hp, int df, int arm) = 0;

    virtual void add_upgrade(upgrade) = 0;

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
    virtual void add_unit(unit_card);
    virtual void add_necromancer(unit_card);
};


// *** ABILITIES AND TRAITS ***

// While adjacent to an ally, gain +1D on attacks.
void formation(taction &c)
{
    c.self().set_counter(COUNTER_FORMATION, 1);
}


// Once used, a unit cannot use any ability with this tag until it reloads. To reload, sacrifice a MOVE. Other abilities may allow a reload for free.
void reload(taction &c, int counter = COUNTER_RELOAD)
{
    int moves_left = c.self().inc_moves(0);
    if (!moves_left)
        return c.no_resources();

    if (!c.self().counter(COUNTER_RELOAD))
        return c.no_target();

    c.self().inc_moves(-1);
    c.reload(c.self());
}


// May be used as a cover object by allies.
void siege_shield(taction &c)
{
    return c.unimplemented();
}


// If this unit ends its turn with no foes adjacent, this unit becomes curseproof and all attacks against it gain -1D. Both effects last until the start of its next turn.
void active_camo(taction &c)
{
    return c.unimplemented();
}


// May consume one adjacent corpse before or after ACTing to reload and gain 1 strength.
void scavenge_ammo(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_SCAVENGE_AMMO))
        return;

    if (c.trigger() == TRIGGER_BEFORE_ACT)
        c.self().set_counter(COUNTER_SCAVENGE_AMMO_AVAILABLE, 1);
    bool use = c.self().counter(COUNTER_SCAVENGE_AMMO_AVAILABLE);
    if (c.trigger() == TRIGGER_AFTER_ACT)
        c.self().set_counter(COUNTER_SCAVENGE_AMMO_AVAILABLE, 0);

    if (!use)
        return c.no_resources();

    if (!c.self().corpses_in_range(1, 1))
        return c.no_target();

    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, SELECT_SPACE_EXCLUDE_CORPSELESS);
    if (!p)
        return;

    c.inc_corpse(*p, -1);
    c.reload(c.self());
    c.self().gain_token(TOKEN_STRENGTH);
    c.self().set_counter(COUNTER_SCAVENGE_AMMO_AVAILABLE, 0);
}


// Attack, Range 2-3, reload. On hit: 1 piercing damage. Headshot: inflict 1 vulnerable.
void ol45(taction &c)
{
    if (c.self().counter(COUNTER_RELOAD))
        return reload(c);

    bool brace = c.self().has_upgrade(UPGRADE_BRACE);
    select_unit_filter f = brace ? SELECT_UNIT_MODIFY_BRACE : SELECT_UNIT_EXCLUDE_NONE;
    list<tunit *> us = c.self().units_in_range(2, 3, f);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, DAMAGE_GRAZE, &c.self());
        c.self().set_counter(COUNTER_RELOAD, 1);
        return;
    }
    u->take_damage(1, DAMAGE_PIERCING, &c.self());
    if (c.is_headshot(d6))
        u->gain_token(TOKEN_VULNERABLE);
    if (brace)
        u->push(c.self(), 1);
    c.self().set_counter(COUNTER_RELOAD, 1);

    if (c.self().has_upgrade(UPGRADE_TACTICAL_RELOAD) && c.is_headshot(d6)) {
        c.reload(c.self());
        c.self().gain_token(TOKEN_STRENGTH);
    }
}


// Push, melee. Effect: Push 1 and (3+) inflict 1 vulnerable.
void baton(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int d6 = c.player_roll_d6(c.self());
    if (!c.is_hit(*u, d6))
        return;

    u->push(c.self(), 1);
    if (d6 >= 3)
        u->gain_token(TOKEN_VULNERABLE);
}


// Attack, melee, push. On hit: 2 damage and push 1. Headshot: push 2.
void skull_crack(taction &c)
{
    return c.unimplemented();
}


// Range 2-3, reload, splash. Effect: 1 fire damage and 1 vulnerable, then Splash (target): push 1 away from target.
void flashbang(taction &c)
{
    if (c.self().counter(COUNTER_RELOAD))
        return reload(c);
}


// Self. Effect: Until start of this unit's next turn, this unit cannot MOVE, step, or be pushed or pulled, gains cover, and grants cover to adjacent allies from all directions.
void shieldwall(taction &c)
{
    return c.unimplemented();
}


// Range 3. Effect: A unit in range reloads and gains one, (6+) or two strength.
void regurgitate_ammo(taction &c)
{
    if (c.self().has_upgrade(UPGRADE_HOT_CHAMBER)) {
        list<tunit *> us = c.self().units_in_range(3, SELECT_UNIT_ADJACENT_TARGETS);
        if (us.empty())
            return c.no_target();

        us = c.player_must_select_units(us, 1, 2);
        for (tunit *u : us)
            c.reload(*u);
        return;
    }

    list<tunit *> us = c.self().units_in_range(3);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int d6 = c.player_roll_d6(c.self());
    int x = c.d6_gradations(d6, {{0, 1}, {6, 2}});

    c.reload(*u);
    u->gain_token(TOKEN_STRENGTH, x);
}


// Curse, Range 1-3. Effect: Unit takes 1 damage after any ACT ability resolves that pushes or pulls them. Lasts until end of this unit's next turn or until this unit has taken 3 damage this way.
void bone_shards(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_WITHOUT_CURSEPROOF);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->set_counter(COUNTER_BONE_SHARDS, 3);

    if (c.self().has_upgrade(UPGRADE_VOMIT_BULLETS)) {
        int d6 = c.player_roll_d6(c.self());
        int n = c.d6_gradations(d6, {{0, 0}, {4, 1}, {6, 1}});
        while (n--)
            u->push(c.self(), 1);
    }
}


// Self. Effect: Step 1, then Splash (self): 1 fire damage, push 1, and inflict vulnerable, then self is obliterated.
void destructive_glee(taction &c)
{
    int d6 = 1;
    if (c.self().has_upgrade(UPGRADE_NAPALM_INJECTOR))
        d6 = c.player_roll_d6(c.self());

    c.unit_step(c.self(), 1);
    for (tunit *u : c.self().units_in_range(1, 1)) {
        int dmg = d6 >= 4 ? 2 : 1;
        u->take_damage(dmg, DAMAGE_FIRE, &c.self());
        u->push(c.self(), 1);
        int x = d6 >= 6 ? 2 : 1;
        u->gain_token(TOKEN_VULNERABLE, +x);
    }
    c.obliterate(c.self());
}


// Attack, Range 2-4, reload. On hit: 2 damage. Headshot: and inflict 1 vulnerable.
void snipe(taction &c)
{
    if (c.self().counter(COUNTER_RELOAD))
        return reload(c);

    bool gun = c.self().counter(COUNTER_TRANSFORM_TO_GUN);
    int max_range = gun ? 6 : 4;
    list<tunit *> us = c.self().units_in_range(2, max_range);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int mod = 0;
    if (c.self().has_upgrade(UPGRADE_EXTENDED_BARREL))
        mod = c.self().has_cover(*u) ? +1 : -1;

    int tags = gun ? enum_or(ROLL_TAG_ATTACK, ROLL_TAG_IGNORE_COVER) : ROLL_TAG_ATTACK;
    int d6 = c.player_roll_d6(c.self(), tags, mod);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, DAMAGE_GRAZE, &c.self());
        c.self().set_counter(COUNTER_RELOAD, 1);
        return;
    }

    int type = gun ? DAMAGE_PIERCING : DAMAGE_NORMAL;
    u->take_damage(2, type, &c.self());
    if (c.is_headshot(d6))
        u->gain_token(TOKEN_VULNERABLE);
    if (c.self().has_upgrade(UPGRADE_CALIBER_UP)) {
        int x = c.is_headshot(d6) ? 2 : 1;
        u->push(c.self(), x);
    }
    c.self().set_counter(COUNTER_RELOAD, 1);
}


// Self. Effect: Unit becomes unable to MOVE or step, but ranged abilities gain +2 maximum range and ignore armor and cover. It can end this effect by sacrificing a MOVE, but if it does, it loses all associated effects.
void transform_to_gun(taction &c)
{
    if (c.self().counter(COUNTER_TRANSFORM_TO_GUN)) {
        int moves_left = c.self().inc_moves(0);
        if (!moves_left)
            return c.no_resources();

        c.self().inc_moves(-1);
        c.self().set_counter(COUNTER_TRANSFORM_TO_GUN, 0);
        c.self().inc_counter(COUNTER_UNABLE_TO_MOVE, -1);
        c.self().inc_counter(COUNTER_UNABLE_TO_STEP, -1);

        if (c.self().has_upgrade(UPGRADE_CLAW_PITONS)) {
            c.self().inc_counter(COUNTER_IMMUNE_TO_PUSH, -1);
            c.self().inc_counter(COUNTER_IMMUNE_TO_PULL, -1);
        }
        return;
    }

    c.self().set_counter(COUNTER_TRANSFORM_TO_GUN, 1);
    c.self().inc_counter(COUNTER_UNABLE_TO_MOVE, +1);
    c.self().inc_counter(COUNTER_UNABLE_TO_STEP, +1);

    if (c.self().has_upgrade(UPGRADE_CLAW_PITONS)) {
        c.self().inc_counter(COUNTER_IMMUNE_TO_PUSH, +1);
        c.self().inc_counter(COUNTER_IMMUNE_TO_PULL, +1);
    }
}


// Curse, Range 2-4. Effect: Inflict 2 vulnerable, (4+): 3 vulnerable, (6+): Remove any vitality first.
void deathmark(taction &c)
{
    bool gun = c.self().counter(COUNTER_TRANSFORM_TO_GUN);
    int max_range = gun ? 6 : 4;
    list<tunit *> us = c.self().units_in_range(2, max_range, SELECT_UNIT_WITHOUT_CURSEPROOF);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int tags = gun ? ROLL_TAG_IGNORE_COVER : ROLL_TAG_NONE;
    int d6 = c.player_roll_d6(c.self(), tags);
    int x = d6 >= 4 ? 3 : 2;
    ttoken *t = u->find_token(TOKEN_VITALITY);
    if (d6 >= 6 && t)
        u->remove_token(t->type(), t->count());

    u->gain_token(TOKEN_VULNERABLE, x);
}


// Self, push. Effect: Step 1, then splash (self) push foes 1, (4+) push foes 2. May move into and destroy walls with this step.
void juggernaut(taction &c)
{
    if (c.self().has_upgrade(UPGRADE_FORTIFY)) {
        for (ttoken *t : c.self().tokens()) {
            if (t->is_negative())
                c.self().remove_token(t->type(), t->count());
        }
    }

    c.unit_step(c.self(), MOVEMENT_DESTROY_WALLS);

    list<tunit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_EXCLUDE_ALLY);
    if (us.empty())
        return;

    int d6 = c.player_roll_d6(c.self());
    int push = c.d6_gradations(d6, {{0, 1}, {5, 2}});
    for (tunit *u : us)
        u->push(c.self(), push);
}


// Attack, Range 3-4, reload, charge. On hit: 1 damage then Splash: 1 fire damage. Headshot: +1 damage on main target.
void mortar(taction &c)
{
    if (c.self().counter(COUNTER_RELOAD))
        return reload(c);

    list<tunit *> us = c.self().units_in_range(3, 4);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    bool gunner = c.self().has_upgrade(UPGRADE_GUNNER_PIVOT) && c.self().is_in_formation();
    int tags = gunner ? enum_or(ROLL_TAG_ATTACK, ROLL_TAG_IGNORE_COVER) : ROLL_TAG_ATTACK;
    int d6 = c.player_roll_d6(c.self(), tags);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, DAMAGE_GRAZE, &c.self());
        c.self().set_counter(COUNTER_RELOAD, 1);
        return;
    }

    int dmg = c.is_headshot(d6) ? 2 : 1;
    u->take_damage(1, DAMAGE_NORMAL, &c.self());

    list<tunit *> splash = u->units_in_range(1, 1);
    for (tunit *u : splash)
        u->take_damage(1, DAMAGE_FIRE, &c.self());

    c.self().set_counter(COUNTER_RELOAD, 1);
}


// Line, reload. Effect: Line 5, 1 fire damage, and inflicts 1 vulnerable on the first unit in the line. Pierces through walls and can target through walls.
void catechism_devil_cannon(taction &c)
{
    if (c.self().counter(COUNTER_RELOAD_2))
        return reload(c);

    list<map_pos> line;
    list<tunit *> us = c.player_must_select_line(5, &line);

    bool heavy = c.self().has_upgrade(UPGRADE_HEAVY_CALIBER_CANNON);
    if (heavy) {
        for (const map_pos &p : line)
            c.destroy_wall(p);
    }

    bool first = true;
    for (tunit *u : us) {
        u->take_damage(1, DAMAGE_FIRE, &c.self());
        if (heavy)
            u->push(c.self(), 1);
        if (first) {
            first = false;
            u->gain_token(TOKEN_VULNERABLE);
        }
    }
    c.self().set_counter(COUNTER_RELOAD_2, 1);
}


// Line 4, reload. Effect: First unit in the line takes 1 piercing damage. If catching an allied unit in the line, also inflict 2 vulnerable.
void cross_fire(taction &c)
{
    return c.unimplemented();
}


// Step. Effect: Step 1, then push an adjacent foe 1 or (5+) 2 spaces. That foe must spend a MOVE before next attacking.
void slide_kick(taction &c)
{
    return c.unimplemented();
}


// Self. Effect: Reload and gain 1 strength (4+) then regain this unit’s ACT. This ability can’t be used again this turn.
void trick_reload(taction &c)
{
    return c.unimplemented();
}


// Ignores adverse terrain and elevation attack and movement penalties
void flight(taction &c)
{
    c.self().set_counter(COUNTER_FLIGHT, 1);
}


// Starts combat with 4 mutation tokens
void warpflesh(taction &c)
{
    c.self().gain_token(TOKEN_MUTATION, +4);
}


// 2x2 unit
void large(taction &c)
{
    c.self().set_counter(COUNTER_IS_2X2, 1);
}


// If a unit with this trait is reduced to 0 HP, they are not slain, but instead gain 1 strength and remain at 0 HP, no matter how much damage they took. If they take any further damage at 0 HP, they are slain as normal.
void blood_rage(taction &c)
{
    // already activated
    if (c.self().counter(COUNTER_BLOOD_RAGE))
        return;

    if (c.self().hp() > 0)
        return;

    c.self().set_counter(COUNTER_BLOOD_RAGE, 1);
    c.self().set_hp(0);
    c.self().gain_token(TOKEN_STRENGTH);
}


// After a berserk unit ACTs, it splashes (self) 1 damage, then discards a berserk token. This damage can't be increased in any way. If this effect does not damage any other characters, it deals 1 damage to itself instead, ignoring armor. Berserk counts as a positive and a negative token.
void berserk(taction &c)
{
    ttoken *t = c.self().find_token(TOKEN_BERSERK);
    if (!t)
        return;

    c.self().remove_token(t->type(), 1);
    list<tunit *> splash = c.self().units_in_range(1, 1);
    if (splash.empty()) {
        c.self().take_damage(1, DAMAGE_PIERCING, &c.self());
        return;
    }
    for (tunit *u : splash)
        u->take_damage(1, DAMAGE_CANT_BE_INCREASED, &c.self());
}


// This unit may spend speed tokens as strength. May pass through units but not end their turn in their spaces.
void hellwheel(taction &c)
{
    c.self().may_treat_token_a_as_b(TOKEN_SPEED, TOKEN_STRENGTH);
    return c.unimplemented();
}


// When taking damage, gain 1 berserk and 1 strength. This effect can't trigger more than once a round.
void steaming_rage(taction &c)
{
    if (c.trigger() == TRIGGER_ROUND_START) {
        c.self().set_counter(COUNTER_STEAMING_RAGE, 1);
        return;
    }
    if (c.trigger() == TRIGGER_AFTER_DAMAGED) {
        if (!c.self().counter(COUNTER_STEAMING_RAGE))
            return c.no_resources();

        c.self().set_counter(COUNTER_STEAMING_RAGE, 0);
        c.self().gain_token(TOKEN_BERSERK);
        c.self().gain_token(TOKEN_STRENGTH);
        return;
    }
}


// Self. Effect: MOVE as far as possible in a straight line, passing through units. Then, units passed through during this movement take 1 damage.
void charge(taction &c)
{
    return c.unimplemented();
}


// Attack, melee. On hit: 1 damage and 1 damage to another adjacent unit.
void cleave(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_NORMAL, &c.self());

    us.remove(u);
    if (us.empty())
        return;

    u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->take_damage(1, DAMAGE_NORMAL, &c.self());
}


// Self. Effect: Gain 1 strength and speed (4+) and retaliation until start of this unit's next turn.
void rev(taction &c)
{
    return c.unimplemented();
}


// Attack, melee. On hit: 1 damage, Effect: (4+): 1 damage again, (6+): 1 damage again.
void chainsaw(taction &c)
{
    bool swing = c.self().has_upgrade(UPGRADE_HEAVY_SWING);
    if (swing && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 1);

    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty()) {
        if (swing)
            return;
        return c.no_target();
    }

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        u->take_damage(1, DAMAGE_GRAZE, &c.self());
    else {
        bool bloody = c.self().has_upgrade(UPGRADE_BLOODY_TEETH) && c.self().hp() == 0;
        int type = bloody ? DAMAGE_PIERCING : DAMAGE_NORMAL;
        int d6 = c.player_roll_d6(c.self());
        int times = c.d6_gradations(d6, {{0, 1}, {4, 2}, {6, 3}});
        while (times--) {
            u->take_damage(1, type, &c.self());
            if (!c.then())
                return;
        }
    }

    if (swing && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 1);
}


// Self. Effect: MOVE as far as possible in a straight line. Then, all units adjacent to any space of this MOVE take 1 fire damage.
void wild_slashes(taction &c)
{

}


// Curse, Range 1-3. Effect: Unit gains 2 berserk or 2 speed.
void pain_frenzy(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_WITHOUT_CURSEPROOF);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    optional<token_type> tt = c.player_must_select_token_type({TOKEN_BERSERK, TOKEN_SPEED});
    if (!tt)
        return;

    u->gain_token(*tt, 2);
}


// Curse, Range 1-3. Effect: Clear all negative tokens on self or a unit, then deal 1 piercing fire damage to them. This damage can't slay a unit.
void cauterize(taction &c)
{
    list<tunit *> us = c.self().units_in_range(3, enum_or(SELECT_UNIT_WITHOUT_CURSEPROOF, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int removed = 0;
    for (ttoken *t : u->tokens()) {
        if (t->is_negative()) {
            removed += t->count();
            u->remove_token(t->type(), t->count());
        }
    }

    u->take_damage(removed, enum_or(DAMAGE_FIRE, DAMAGE_CANT_SLAY), &c.self());

    if (c.self().has_upgrade(UPGRADE_ADRENALIZE) && u->is_ally(c.self()))
        u->gain_token(TOKEN_SPEED, removed);
}


// Pull, Range 2-3. Effect: Pull unit 3.
void meat_hook(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.no_target();

    bool is3 = c.self().has_upgrade(UPGRADE_FRENZY_HOOK) && c.self().hp() <= 1;
    int max = is3 ? 3 : 1;
    us = c.player_must_select_units(us, 1, max);

    for (tunit *u : us)
        u->pull(c.self(), 3);
}

// Attack, Melee. On hit: 1 damage. Effect: (4+) 1 damage again. (6+) 1 damage again.
void exfoliate(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    int d6 = c.player_roll_d6(c.self());
    int times = c.d6_gradations(d6, {{0, 1}, {4, 2}, {6, 3}});
    while (times--) {
        u->take_damage(1, DAMAGE_NORMAL, &c.self());
        if (!c.then())
            return;
    }
}


// Self. Unit MOVEs as far as possible in a straight line. The first unit passed through during this MOVE take 1 fire damage. Then this unit gains 2 speed.
void spin_out(taction &c)
{
    return c.unimplemented();
}


// At 1 hp or lower, gains super armor.
void machineheart(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_MACHINEHEART))
        return;

    bool was = c.self().counter(COUNTER_MACHINEHEART);
    bool will = c.self().hp() <= 1;
    if (was == will)
        return;

    if (will) {
        c.self().inc_counter(COUNTER_MACHINEHEART, +1);
        c.self().inc_counter(COUNTER_SUPER_ARMOR, +1);
    } else {
        c.self().inc_counter(COUNTER_MACHINEHEART, -1);
        c.self().inc_counter(COUNTER_SUPER_ARMOR, -1);
    }
}


// Self. Effect: Pull all units in range 2 one space toward this unit. Then splash (self): 1 fire damage and destroy all adjacent walls.
void pulverize(taction &c)
{
    if (c.self().has_upgrade(UPGRADE_FUEL_INJECTORS) && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 2);

    list<tunit *> us = c.self().units_in_range(1, 2);
    tunit *u = c.player_must_select_unit(us);
    if (u)
        u->pull(c.self(), 2);

    us = c.self().units_in_range(1, 1);
    for (tunit *u : us)
        u->take_damage(1, DAMAGE_FIRE, &c.self());

    list<map_pos> ps = c.self().spaces_in_range(1, 1);
    for (const map_pos &p : ps)
        c.destroy_wall(p);
}


// Self, retaliation. Effect: Gain 1 strength and 1 berserk, (4+) then gain retaliation until the end of this unit's next turn.
void building_rage(taction &c)
{
    c.self().gain_token(TOKEN_STRENGTH);
    c.self().gain_token(TOKEN_BERSERK);

    int d6 = c.player_roll_d6(c.self());
    if (d6 >= 4) {
        c.self().inc_counter(COUNTER_RETALIATION, +1);
        c.self().inc_counter(COUNTER_RETALIATION_DECREASE_ON_TURN_END, +2);
    }
}


// Attack, melee. On hit: 2 damage. Deal 1 damage again if this unit has 3 or less HP, then deal 1 damage again if it is at 1 HP or less.
void rip_and_tear(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    int d6 = c.player_roll_d6(c.self());
    bool slayed = false;
    u->take_damage(2, DAMAGE_NORMAL, &c.self(), &slayed);

    bool o = c.self().has_upgrade(UPGRADE_BIFURCATE);
    if (o && slayed)
        return c.obliterate(*u);

    int extra = c.self().hp() <= 1 ? 2 : c.self().hp() <= 3 ? 1 : 0;
    while (extra--) {
        if (!c.then())
            return;
        u->take_damage(1, DAMAGE_NORMAL, &c.self(), &slayed);
        if (o && slayed)
            return c.obliterate(*u);
    }
}


// Melee. Effect: Tinker with an adjacent unit. Unit gains 1 berserk, then roll the effect die (1-3): Increase unit’s MV stat by +1 this battle, (4-5): increase a unit’s DF stat by +1 this battle, to a max of 6+, (6+): Increase unit’s current and max HP by 1 this battle.
void tinker(taction &c)
{
    return c.unimplemented();
}


// Range 1-3. Effect: Self or a unit in range gains 2 speed. If target is at 1 HP or lower, splash (target) 1 fire damage.
void overlock(taction &c)
{
    return c.unimplemented();
}


// Ally, melee. Effect: Self or an adjacent ally has their HP set to 1.
void restart_engine(taction &c)
{
    return c.unimplemented();
}


// Units affected by plague take 1 toxic damage at the end of their turn, then that unit discards a plague token. This damage cannot be reduced or ignored in any way (by tokens or armor) but cannot slay a unit. Gargamox units don't take damage from plague and don't remove plague tokens, but can still be affected by plague.
void plague(taction &c)
{
    ttoken *t = c.self().find_token(TOKEN_PLAGUE);
    if (!t)
        return;

    if (c.self().faction() == FACTION_GARGAMOX)
        return;

    c.self().take_damage(1, enum_or(DAMAGE_TOXIC, DAMAGE_CANT_BE_DECREASED, DAMAGE_CANT_SLAY), nullptr);
    c.self().remove_token(t->type(), 1);
}


// Has Deathburst: Splash (self): 1 plague and 1 toxic damage to already already plagued foes.
void toxic_revenge(taction &c)
{
    bool push = c.self().has_upgrade(UPGRADE_BLOAT) && c.self().find_token(TOKEN_PLAGUE);
    list<tunit *> us = c.self().units_in_range(1, 1);
    for (tunit *u : us) {
        bool dmg = u->find_token(TOKEN_PLAGUE) && !u->is_ally(c.self());
        u->gain_token(TOKEN_PLAGUE);
        if (dmg)
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
        if (push)
            u->push(c.self(), 1);
    }
    if (c.self().has_upgrade(UPGRADE_AFTERMATH)) {
        c.inc_corpse(c.self().pos(), -1);
        c.set_hazard(c.self().pos());
    }
}

// Immune to hazards. May treat plague tokens as strength.
void plaguebearer(taction &c)
{
    c.self().set_counter(COUNTER_IMMUNE_TO_HAZARDS, 1);
    c.self().may_treat_token_a_as_b(TOKEN_PLAGUE, TOKEN_STRENGTH);
}

// Has Deathburst: Splash (self): Remove any 1 token from all units in the area.
void deathwash(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    for (tunit *u : us) {
        if (!u->n_tokens())
            continue;
        ttoken *t = c.player_must_select_token(u->tokens());
        if (!t)
            return;
        u->remove_token(t->type(), 1);
    }
}

// Has Deathburst: create a hazard under a number of units in range 2 equal to the number of plague tokens in this unit.
void swarm_release(taction &c)
{
    int n = c.self().n_tokens(SELECT_TOKEN_ONLY_PLAGUE);
    if (!n)
        return c.no_resources();

    list<tunit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.no_target();

    us = c.player_must_select_units(us, min(n, (int)us.size()), n);
    for (tunit *u : us)
        c.set_hazard(u->pos());
}


// Immune to hazards.
void toxic_avenger_immune_to_hazards(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_TOXIC_AVENGER))
        return;

    c.self().inc_counter(COUNTER_IMMUNE_TO_HAZARDS, +1);
}


// While standing in a hazard, gain 1 plague at turn start.
void toxic_avenger(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_TOXIC_AVENGER))
        return;

    if (c.is_hazard(c.self().pos()))
        c.self().gain_token(TOKEN_PLAGUE);
}


// Has Deathburst: Remove up to three plague tokens on this unit, then splash (self): 1 toxic damage, once, per plague token removed.
void vile_rupture(taction &c)
{
    int n = c.self().n_tokens(TOKEN_PLAGUE);
    optional<int> removed = c.player_must_select_token_count(min(3, n));
    if (!removed)
        return;

    int times = *removed;
    while (times--) {
        c.self().remove_token(TOKEN_PLAGUE, 1);
        for (tunit *u : c.self().units_in_range(1, 1))
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
    }
}

// Has deathburst: summon a slimelet in a free adjacent space. Unlike other summons, the slimelet can be freely activated this round.
void burst_guts(taction &c)
{
    return c.unimplemented();
}

// Follows all summon rules, but can act this round.
void spit_up(taction &c)
{
    return c.unimplemented();
}

// Can be activated two at a time.
void summoned_thrall(taction &c)
{
    return c.unimplemented();
}

// Range 2-3. Effect: Pull unit one. This gains +1 range and pull for each plague token on this unit.
void pseudopod(taction &c)
{
    int n = c.self().n_tokens(SELECT_TOKEN_ONLY_PLAGUE);
    list<tunit *> us = c.self().units_in_range(2, 3 + n);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->pull(c.self(), 1 + n);

    if (c.self().has_upgrade(UPGRADE_TENTACLE_WHIP) && u->find_token(TOKEN_PLAGUE))
        u->gain_token(TOKEN_SLOW);
}

// Attack, melee. On hit: 1 damage and 1 plague.
void shamble(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_NORMAL, &c.self());
    u->gain_token(TOKEN_PLAGUE);
}


// splash (self): 1 plague, and allies gain 1 strength.
void invigorating_viscera(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_INVIGORATING_VISCERA))
        return;

    list<tunit *> us = c.self().units_in_range(1, 1);
    for (tunit *u : us) {
        u->gain_token(TOKEN_PLAGUE);
        if (u->is_ally(c.self()))
            u->gain_token(TOKEN_STRENGTH);
    }
}


// Curseproof and cover from all directions while standing in a hazard.
void smog_shroud(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_SMOG_SHROUD))
        return;

    bool was = c.self().counter(COUNTER_SMOG_SHROUD);
    bool will = c.is_hazard(c.self().pos());
    if (was == will)
        return;

    if (will) {
        c.self().inc_counter(COUNTER_SMOG_SHROUD, +1);
        c.self().inc_counter(COUNTER_CURSEPROOF, +1);
        c.self().inc_counter(COUNTER_HAS_COVER_FROM_ALL_DIRECTIONS, +1);
    } else {
        c.self().inc_counter(COUNTER_SMOG_SHROUD, -1);
        c.self().inc_counter(COUNTER_CURSEPROOF, -1);
        c.self().inc_counter(COUNTER_HAS_COVER_FROM_ALL_DIRECTIONS, -1);
    }
}


// Range 1-3. Effect: Create a hazard in a free space in range 3 and inflict plague on an adjacent target (4+) all adjacent targets.
void pustulate(taction &c)
{
    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 3, enum_or(SELECT_SPACE_EXCLUDE_OCCUPIED, SELECT_SPACE_EXCLUDE_WALLS));
    if (!p)
        return;

    c.set_hazard(*p);

    int d6 = c.player_roll_d6(c.self());
    list<tunit *> us = c.units_in_range(*p, 1, 1);
    if (us.empty())
        return;

    if (d6 < 4) {
        tunit *u = c.player_must_select_unit(us);
        if (!u)
            return;
        us = {u};
    }

    for (tunit *u : us)
        u->gain_token(TOKEN_PLAGUE);
}

// Line 4. Line: 1 plague. Already plagued foes gain 1 slow.
void vomitous_mass(taction &c)
{
    list<tunit *> us = c.player_must_select_line(4);
    bool dmg = c.self().has_upgrade(UPGRADE_CATALYZE);

    for (tunit *u : us) {
        bool slow = u->find_token(TOKEN_PLAGUE);
        u->gain_token(TOKEN_PLAGUE);
        if (slow)
            u->gain_token(TOKEN_SLOW);
        bool in_hazard = c.is_hazard(u->pos());
        if (dmg && in_hazard)
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
    }
}

// Attack, melee. On hit: 1 toxic damage and infect: 1 toxic damage.
void rotblade(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_TOXIC, &c.self());

    us = c.player_must_select_infect(*u);
    for (tunit *u : us)
        u->take_damage(1, DAMAGE_TOXIC, &c.self());
}

// Curse, Range 1-3. Effect: Unit permanently gains death burst: splash (self): 1 toxic damage and 1 plague. This effect cannot stack with itself but stacks with other death burst effects.
void suppurate(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_WITHOUT_CURSEPROOF);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->inc_counter(COUNTER_SUPPURATE, +1);

    if (c.self().has_upgrade(UPGRADE_ACID_BLOOD))
        u->inc_counter(COUNTER_ACID_BLOOD, +1);
}

// Curse, Range 1-3. Effect: Strip all plague tokens from a unit in range. Foes take 1 toxic damage. Then, they gain 1 slow per token removed. Allies gain 1 strength per token removed.
void evolve_strain(taction &c)
{
    list<tunit *> us;
    bool all = false;
    if (c.self().has_upgrade(UPGRADE_PANDEMIC)) {

        all = false;
    } else {
        us = c.self().units_in_range(1, 3, enum_or(SELECT_UNIT_WITHOUT_CURSEPROOF, SELECT_UNIT_WITH_MUTATION_TOKENS));
        if (us.empty())
            return c.no_target();

        tunit *u = c.player_must_select_unit(us);
        if (!u)
            return;
        us = {u};
        all = true;
    }

    for (tunit *u : us) {
        if (!u->is_ally(c.self()))
            u->take_damage(1, DAMAGE_TOXIC, &c.self());

        ttoken *t = u->find_token(TOKEN_PLAGUE);
        if (!t)
            continue;

        int removed = all ? t->count() : 1;
        u->remove_token(t->type(), removed);
        u->gain_token(u->is_ally(c.self()) ? TOKEN_STRENGTH : TOKEN_SLOW, removed);
    }
}

// Range 1-3. Effect: Unit triggers deathburst without being slain.
void swell_with_corruption(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_WITH_DEATHBURST);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    bool twice = c.self().has_upgrade(UPGRADE_MASSIVE_SWELL) && c.player_roll_d6(c.self()) >= 4;
    int times = twice ? 2 : 1;
    while (times--)
        c.trigger_deathburst(*u);
}

// Range 2-4. Effect: Create one, (3+) two, or (5+) three hazards in range. May spend a plague token on self to re-roll the effect die once.
void propagate_swarm(taction &c)
{
    int d6 = c.player_roll_d6(c.self());
    if (c.self().find_token(TOKEN_PLAGUE) && c.player_may_take_action(TAKE_ACTION_PROPAGATE_SWARM)) {
        d6 = max(d6, c.player_roll_d6(c.self()));
        c.self().remove_token(TOKEN_PLAGUE, 1);
    }

    int n = c.d6_gradations(d6, {{0, 1}, {3, 2}, {5, 3}});
    while (n--) {
        optional<map_pos> p = c.player_must_select_space(c.self().pos(), 3);
        if (!p)
            return;
        c.set_hazard(*p);
    }
}

// Attack, Range 2-4. On hit: 2 damage and pull 1. Infect: 1 damage and pull 1. Effect: Allies take no damage.
void driving_vermin(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (c.self().has_upgrade(UPGRADE_DEFILER))
        c.set_hazard(u->pos());

    optional<int> removed = 0;
    if (c.self().has_upgrade(UPGRADE_SWARM_FEED)) {
        int max = c.self().n_tokens(TOKEN_PLAGUE);
        removed = c.player_must_select_token_count(max);
        if (!removed)
            return;
        c.self().remove_token(TOKEN_PLAGUE, *removed);
    }

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK, *removed)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    if (!u->is_ally(c.self()))
        u->take_damage(2, DAMAGE_NORMAL, &c.self());
    u->pull(c.self(), 1);

    us = c.player_must_select_infect(*u);
    for (tunit *u : us) {
        if (!u->is_ally(c.self()))
            u->take_damage(1, DAMAGE_NORMAL, &c.self());
        u->pull(c.self(), 1);
    }
}

// Self. Effect: Inflict two, (5+) or three plague tokens on self, then may inflict 1 slow on an adjacent unit per plague token gained.
void percolate(taction &c)
{
    int d6 = c.player_roll_d6(c.self());
    int n = c.d6_gradations(d6, {{0, 2}, {5, 3}});
    c.self().gain_token(TOKEN_PLAGUE, n);

    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return;

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->gain_token(TOKEN_SLOW, n);
}

// Self. Effect: MOVE with free movement in a straight line. During this move, is immune to hazards and may move through walls and units. Any foe this unit passes through takes 1 toxic damage and this unit may pass off any plague tokens on this unit to affected units.
void surge(taction &c)
{
    return c.unimplemented();
}

// Attack, Range 2-3. On hit: 1 toxic damage (3+) and create a hazard under target. Ignores armor against units already in hazards.
void toxic_spit(taction &c)
{
    return c.unimplemented();
}

// Range 1-2. Effect: Push target unit 1 space, then create one hazard in a space they vacated. If this unit is plagued, push 2 instead.
void projectile_vomit(taction &c)
{
    return c.unimplemented();
}


// Any unit affected by at least one Doom token takes 4 devil damage at the end of round 4. A unit can only clear a Doom token by slaying another unit and they cannot be cleared in any other way.
void doom(taction &c)
{
    ttoken *t = c.self().find_token(TOKEN_DOOM);
    if (t)
        return;

    if (c.round() == 4)
        c.self().take_damage(4, DAMAGE_DEVIL, nullptr);
}


// When slain, does not remove Doom, and (5+) Dooms slayer.
void inverted_crucifix(taction &c)
{
    c.self().set_counter(COUNTER_INVERTED_CRUCIFIX, 1);
}


// Has free movement while adjacent to a wall.
void slither(taction &c)
{
    list<map_pos> ps = c.self().spaces_in_range(1, 1);
    bool will = false;
    for (const map_pos &p : ps)
        will |= c.is_wall(p);

    bool was = c.self().counter(COUNTER_SLITHER);
    if (was == will)
        return;

    if (will) {
        c.self().inc_counter(COUNTER_SLITHER, +1);
        c.self().inc_counter(COUNTER_MOVEMENT_FREE, +1);
    } else {
        c.self().inc_counter(COUNTER_SLITHER, -1);
        c.self().inc_counter(COUNTER_MOVEMENT_FREE, -1);
    }
}


// When MOVEing a second time or more in a turn, can remove this unit from the battlefield and place it any free space in range 4, then clear a token.
void teleport(taction &c)
{
    return c.unimplemented();
}

// Abilities ignore line of sight.
void soul_sight(taction &c)
{
    return c.unimplemented();
}

// Before MOVEing, may remove and place any adjacent walls in any other free adjacent spaces.
void labyrinth_master(taction &c)
{
    return c.unimplemented();
}

// The Geist is bound to the stone of Anzenmezzeron. It cannot MOVE or step. At the start of its turn, teleport it to any space adjacent to a wall. At the start of combat, place a wall in a free adjacent space to it.
void buried_alive(taction &c)
{
    return c.unimplemented();
}

// If not adjacent to a wall, the geist loses all armor.
void tomb_bound(taction &c)
{
    return c.unimplemented();
}


// May move through walls.
void squirm(taction &c)
{
    if (c.self().has_upgrade(UPGRADE_SQUIRM))
        return;

    c.self().inc_counter(COUNTER_MOVEMENT_THROUGH_WALLS, +1);
}


// Range 2-4. Effect: Pull 1. Ignores line of sight.
void beckon(taction &c)
{
    select_unit_filter f = c.self().has_upgrade(UPGRADE_DEAD_GRASP) ? enum_or(SELECT_UNIT_IGNORE_LINE_OF_SIGHT, SELECT_UNIT_MODIFY_DEAD_GRASP) : SELECT_UNIT_IGNORE_LINE_OF_SIGHT;
    list<tunit *> us = c.self().units_in_range(2, 4, f);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->pull(c.self(), 1);

    if (c.self().has_upgrade(UPGRADE_IMPENDING_DEATH) && u->is_isolated())
        u->gain_token(TOKEN_WEAK);
}

// Attack, melee. On hit: 1 damage and (4+) Dooms unit.
void shudder(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_NORMAL, &c.self());

    int d6 = c.player_roll_d6(c.self());
    if (d6 >= 4)
        u->gain_token(TOKEN_DOOM);
}


// May step 1 before ACTing, or step 2 and clear a token if adjacent to a wall.
void leap(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_LEAP))
        return;

    list<map_pos> ps = c.self().spaces_in_range(1, 1);
    bool wall = false;
    for (const map_pos &p : ps)
        wall |= c.is_wall(p);

    if (!c.player_may_take_action(TAKE_ACTION_LEAP))
        return;

    c.unit_step(c.self(), wall ? 2 : 1);

    if (!c.self().n_tokens(SELECT_TOKEN_ONLY_REMOVABLE))
        return;

    ttoken *t = c.player_may_select_token(c.self().tokens(), SELECT_TOKEN_ONLY_REMOVABLE);
    if (!t)
        return;

    c.self().remove_token(t->type(), 1);
}


// Melee, Range 1-2. Effect: Create a wall, (3+) then create adverse terrain (5+) then create adverse terrain again.
void tombraiser(taction &c)
{
    int range = c.self().has_upgrade(UPGRADE_FOUL_MONUMENTS) ? 4 : 2;
    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, range, SELECT_SPACE_EXCLUDE_OCCUPIED);
    if (!p)
        return;
    c.set_wall(*p);

    int d6 = c.player_roll_d6(c.self());
    int n = c.d6_gradations(d6, {{0, 0}, {3, 1}, {5, 2}});
    while (n--) {
        p = c.player_must_select_space(c.self().pos(), 1, 2);
        if (p)
            c.set_adverse_terrain(*p);
    }
}

// Range 2-4. Effect: Pull 1, ignoring line of sight. If pulling a unit into a wall, pull 3 instead and the kidnapped unit can pass through walls and units during this pull.
void kidnap(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, 4, enum_or(SELECT_UNIT_IGNORE_LINE_OF_SIGHT, SELECT_UNIT_WITHOUT_CURSEPROOF));
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->pull(c.self(), 1, MOVEMENT_KIDNAP);
}

// Attack, melee. On hit: 2 damage, then against isolated units inflict 1 weak (4+) and Doom them.
void serpents_kiss(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(2, DAMAGE_NORMAL, &c.self());

    bool kiss = u->is_isolated();
    if (c.self().has_upgrade(UPGRADE_IVORY_SERPENT))
        kiss |= c.is_adverse_terrain(u->pos());

    if (kiss) {
        u->gain_token(TOKEN_WEAK);
        int d6 = c.player_roll_d6(c.self());
        if (d6 >= 4)
            u->gain_token(TOKEN_DOOM);
    }
}

// Curse, Self. Curse: Splash (self): foes gain 1 weak, (5+) and are Doomed. Doomed units take 1 curse damage.
void horrendous_shriek(taction &c)
{
    return c.unimplemented();
}

// Attack, melee. On hit: 2 curse damage. Effect: Against isolated units, gain 1 strength and may then MOVE again.
void urgal_blade(taction &c)
{
    return c.unimplemented();
}

// Line 4. Effect: Line: 1 curse damage, Isolated units take 1 curse damage again. May penetrate walls and does not damage walls. Effect: If this line passes through a wall, this unit gains 1 strength.
void bale_scream(taction &c)
{
    return c.unimplemented();
}

// Range 2-4. Effect: Create a wall in range and (5+) adjacent foes to the wall gain 1 weak.
void tombstone(taction &c)
{
    return c.unimplemented();
}

// Curse, Range 1-4, requires isolated unit. Effect: Choose an isolated unit. That unit gains 2 weak and may no longer MOVE or step until the end of its next turn or unit it's no longer isolated.
void hells_grasp(taction &c)
{
    return c.unimplemented();
}

// Curse, Range 2-4. Effect: Unit gains 1 weak and is pulled 3.
void beckon_lamb(taction &c)
{
    return c.unimplemented();
}

// Attack, melee. On hit: Deals 1 damage, +1 for each of the following that is true about the target: Doomed, Isolated, Weak, In adverse terrain.
void horrendous_end(taction &c)
{
    return c.unimplemented();
}

// Attack, Range 1-2. Effect: Pull 1 before making attack. On hit: 2 damage. Against isolated units, inflicts 1 weak.
void strangle(taction &c)
{
    return c.unimplemented();
}

// Range 2-3, Curse. Effect: Target unit gains 1 or (5+) 2 weak. At the end of their turn, create a wall in a free adjacent space to them.
void grave_bind(taction &c)
{
    return c.unimplemented();
}

// A unit affected by a Miracle has a 5+ effect chance to instantly return to life at 1 HP at the end of any turn it is slain, clearing all tokens.
void miracle(taction &c)
{
    if (c.trigger() == TRIGGER_SLAINED) {
        c.self().set_counter(COUNTER_MIRACLE, 1);
        return;
    }
    if (c.trigger() == TRIGGER_TURN_END && c.self().counter(COUNTER_MIRACLE)) {
        c.self().counter(COUNTER_MIRACLE, 0);
        int dc = c.self().counter(COUNTER_DELAY_JUDGEMENT) ? 2 : 5;
        if (!c.self().is_slain() || c.player_roll_d6(c.self()) < dc)
            return;
        for (ttoken *t : c.self().tokens())
            c.self().remove_token(t->type(), t->count());
        c.self().set_hp();
    }
}


// When MOVEing for the second time or more in a turn, gain 1 vitality.
void zealotry(taction &c)
{
    bool already_moved_once = c.self().n_moves() >= 1;
    if (already_moved_once)
        c.self().gain_token(TOKEN_VITALITY);
}


// Lacks miracle, but starts combat with 4 vitality tokens.
void miraculous_flesh(taction &c)
{
    c.self().gain_token(TOKEN_VITALITY, 4);
}


// Self. Splash (self): Remove one negative token from all allies in range and place them on this unit. Then remove one, (4+) two, or (6+) all negative tokens from this unit, and gain 1 strength for each negative token removed this way.
void mea_culpa(taction &c)
{
    bool may_step = c.self().has_upgrade(UPGRADE_CAGER);
    if (may_step && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 2);

    list<tunit *> us = c.self().units_in_range(1, 1, enum_or(SELECT_UNIT_EXCLUDE_FOE, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
    if (us.empty()) {
        if (!may_step)
            return c.no_target();
        return;
    }

    set<token_type> ts;
    for (tunit *u : us) {
        for (ttoken *t : u->tokens())
            ts.insert(t->type());
    }

    list<token_type> ts_list(ts.begin(), ts.end());
    ts_list.sort();

    optional<token_type> tt = c.player_must_select_token_type(ts_list);
    if (!tt)
        return;

    for (tunit *u : us) {
        ttoken *t = u->find_token(*tt);
        if (!t)
            continue;
        u->remove_token(t->type(), 1);
        c.self().gain_token(t->type());
    }
    if (!c.then())
        return;

    int d6 = c.player_roll_d6(c.self());
    int x = c.d6_gradations(d6, {{0, 1}, {4, 2}, {6, -1}});
    int removed = 0;
    if (x == -1) {
        for (ttoken *t : c.self().tokens()) {
            if (t->is_negative()) {
                removed += t->count();
                c.self().remove_token(t->type(), t->count());
            }
        }
    } else {
        list<ttoken *> ts = c.player_must_select_tokens(c.self().tokens(), x, SELECT_TOKEN_ONLY_NEGATIVE);
        for (ttoken *t : ts) {
            removed++;
            c.self().remove_token(t->type(), 1);
        }
    }
    c.self().gain_token(TOKEN_STRENGTH, removed);

    if (c.self().has_upgrade(UPGRADE_HOLY_BLOOD)) {
        int x = d6 >= 4 ? 2 : 1;
        list<map_pos> ps = c.player_must_select_spaces(c.self().pos(), x, 1, 1, enum_or(SELECT_SPACE_EXCLUDE_OCCUPIED, SELECT_SPACE_EXCLUDE_WALLS));
        for (const map_pos &p : ps)
            c.set_hazard(p);
    }
}


// Range 2-3. Effect: 1 holy damage and (3+) create a hazard under target.
void holy_water(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->take_damage(1, DAMAGE_HOLY, &c.self());
    if (c.player_roll_d6(c.self()) >= 3)
        c.set_hazard(u->pos());
}


// Attack, melee. On hit: 1 damage, and Stance (Odd): Splash (self): 1 holy damage. (Even): gain 1 vitality.
void excoriate(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_NORMAL, &c.self());

    bool even = c.round_even();
    bool both = c.self().has_upgrade(UPGRADE_TASTE_THE_LASH) && c.round(4);
    if (!even || both) {
        for (tunit *u : c.self().units_in_range(1, 1))
            u->take_damage(1, DAMAGE_HOLY, &c.self());
    }
    if (even || both)
        c.self().gain_token(TOKEN_VITALITY);
}


// Self. Stance: The next use of Whirling Chain deals +damage equal to half the round number, rounded up and this unit may also step that many spaces before using it.
void smite(taction &c)
{
    c.self().set_counter(COUNTER_SMITE, 1);
}


// Attack, Range 1-2. On hit: 2 damage. Stance (Odd): and MOVE again, Stance (Even): and push units 2.
void whirling_chain(taction &c)
{
    int dmg = 2;
    if (c.self().counter(COUNTER_SMITE)) {
        c.self().set_counter(COUNTER_SMITE, 0);
        int x = (c.round() + !c.round_even() * 1) / 2;
        dmg += x;
        if (c.player_may_take_action(TAKE_ACTION_STEP))
            c.unit_step(c.self(), x);
    }

    list<tunit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (c.self().has_upgrade(UPGRADE_SUFFUSE) && d6 >= 5) {
        c.self().gain_token(TOKEN_VITALITY);
        if (c.self().n_tokens(SELECT_TOKEN_ONLY_NEGATIVE)) {
            ttoken *t = c.player_must_select_token(c.self().tokens(), SELECT_TOKEN_ONLY_NEGATIVE);
            if (!t)
                return;
            c.self().remove_token(t->type());
        }
    }

    if (!c.is_hit(*u, d6))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    bool slayed = false;
    u->take_damage(dmg, DAMAGE_NORMAL, &c.self(), &slayed);
    if (c.self().has_upgrade(UPGRADE_PUNISHER) && c.round(3) && slayed)
        c.obliterate(*u);

    if (!c.then())
        return;

    if (c.round_even())
        u->push(c.self(), 2);
    else
        c.unit_move(c.self());

    if (!c.then())
        return;

    if (c.self().has_upgrade(UPGRADE_FIERY_CHAIN)) {
        optional<map_pos> p = c.player_must_select_space(u->pos(), 1, 1, SELECT_SPACE_EXCLUDE_WALLS);
        if (!p)
            return;
        c.set_hazard(*p);
    }
}


// Self. Effect: Become unable to MOVE or step until start of this unit’s next turn. At the start of that turn, allied units adjacent to this unit gain 1 vitality.
void gentleness(taction &c)
{
    return c.unimplemented();
}


// Range 1-3. Effect: An allied unit in range gains miracle. If it already had miracle, miracle triggers on a 2+ until the end of their next turn. At the end of that unit's turn, they lose miracle.
void delay_judgement(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 3);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!u->counter(COUNTER_MIRACLE))
        return u->set_counter(COUNTER_MIRACLE, 1);

    u->set_counter(COUNTER_DELAY_JUDGEMENT, 1);
}


// Self. Effect: Splash (self): Stance (Even): Allies gain 1 vitality. (Odd): Foes take 1 holy damage.
void blessed_censer(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 1);
    bool can_affect_units = !us.empty();
    bool can_clean_corpses = c.self().has_upgrade(UPGRADE_CONSECRATE) && c.self().corpses_in_range(1, 1);

    if (!can_affect_units && !can_clean_corpses)
        return c.no_target();

    if (can_clean_corpses && c.player_may_take_action(TAKE_ACTION_CONSECRATE)) {
        for (const map_pos &p : c.self().spaces_in_range(1, 1))
            c.inc_corpse(p, -c.inc_corpse(p, 0));
        if (!c.then())
            return;
    }

    int times = c.self().has_upgrade(UPGRADE_BOILING_CENSER) ? 2 : 1;
    while (times--) {
        for (tunit *u : us) {
            if (c.round_even() && u->is_ally(c.self()))
                u->gain_token(TOKEN_VITALITY, 1);
            if (!c.round_even() && !u->is_ally(c.self()))
                u->take_damage(1, DAMAGE_HOLY, &c.self());
        }
    }
}


// If this unit does not move during its turn, it may remove a negative token from itself.
void focus(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_FOCUS))
        return;

    if (c.self().n_moves())
        return c.no_resources();

    if (!c.self().n_tokens(SELECT_TOKEN_ONLY_NEGATIVE))
        return c.no_target();

    ttoken *t = c.player_may_select_token(c.self().tokens(), SELECT_TOKEN_ONLY_NEGATIVE);
    if (!t)
        return;
    c.self().remove_token(t->type());
}


// Self. Effect: Step 1. Next attack from this unit gains +1D and +2 maximum range. This effect stacks.
void winch(taction &c)
{
    c.unit_step(c.self(), 1);
    c.self().inc_counter(COUNTER_WINCH, +1);
}


// Attack, Range 2-3. On hit: 2 damage. Stance (Round 3+): with piercing. (Round 5+): 4 damage with piercing.
void requiesce_en_pace(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    int dmg = c.round(5) ? 4 : 2;
    int type = c.round(3) ? DAMAGE_PIERCING : DAMAGE_NORMAL;

    bool slayed = false;
    u->take_damage(dmg, type, &c.self(), &slayed);

    if (c.self().has_upgrade(UPGRADE_EXPLOSIVE_BOLTS))
        u->push(c.self(), round(5) ? 4 : round(3) ? 2 : 1);

    if (c.self().has_upgrade(UPGRADE_HEART_DESTROYER) && slayed)
        c.obliterate(*u);
}

// Range 2-6. Effect: Create one, (3+) two, (5+) or three hazards in range. May destroy a wall instead of creating a hazard.
void bolides(taction &c)
{
    int d6 = c.player_roll_d6(c.self());
    int n = c.d6_gradations(d6, {{0, 1}, {3, 2}, {5, 3}});

    while (n--) {
        select_space_filter f = c.self().has_upgrade(UPGRADE_SCATHE) ? SELECT_SPACE_EXCLUDE_NONE : SELECT_SPACE_EXCLUDE_OCCUPIED;
        optional<map_pos> p = c.player_must_select_space(c.self().pos(), 2, 6, f);
        if (!p)
            return;
        if (c.is_wall(*p))
            c.destroy_wall(*p);
        else
            c.set_hazard(*p);
    }
}


// Self, Range 3-6. Effect: Remove up to two vitality tokens on this unit, then deal 1 holy damage to that many units in range, ignoring line of sight, and push those units 1.
void indignation(taction &c)
{
    int n = min(2, c.self().n_tokens(TOKEN_VITALITY));
    if (!n)
        return c.no_resources();

    list<tunit *> us = c.self().units_in_range(3, 6, SELECT_UNIT_IGNORE_LINE_OF_SIGHT);
    if (us.empty())
        return c.no_target();

    optional<int> removed = c.player_must_select_token_count(n);
    if (!removed)
        return;

    us = c.player_must_select_units(us, 0, n);
    if (us.empty())
        return;

    int dmg = c.self().has_upgrade(UPGRADE_WITNESS) && c.round(4) ? 2 : 1;
    for (tunit *u : us) {
        u->take_damage(dmg, DAMAGE_HOLY, &c.self());
        u->push(c.self(), 1);
    }
}


// Self. Effect: Gain two vitality. Until the start of this unit's next turn, adjacent allies may spend vitality tokens on this unit as if they were their own.
void ablutions(taction &c)
{
    int x = 2;
    if (c.self().has_upgrade(UPGRADE_HOLY_FONT)) {
        if (c.round(3))
            x = 3;
        if (c.round(5))
            x = 4;
    }
    c.self().gain_token(TOKEN_VITALITY, x);
    c.self().set_counter(COUNTER_ABLUTIONS, 1);
}


// Melee, Attack. On hit: Self and an adjacent ally gain 1 vitality (Stance: R4+): 2 vitality. Then deal 1 damage and push 1. Cannot miss allies and pushes them +1.
void bible_thump(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, SELECT_UNIT_EXCLUDE_SELF);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    bool hit = u->is_ally(c.self()) ? true : c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK));
    if (!hit)
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    int x = c.round(4) ? 2 : 1;
    c.self().gain_token(TOKEN_VITALITY, x);

    list<tunit *> allies = c.self().units_in_range(1, enum_or(SELECT_UNIT_EXCLUDE_SELF, SELECT_UNIT_EXCLUDE_FOE));
    tunit *ally = c.player_must_select_unit(allies);
    if (ally)
        ally->gain_token(TOKEN_VITALITY, x);

    if (!c.then())
        return;

    u->take_damage(1, DAMAGE_NORMAL, &c.self());
    u->push(c.self(), u->is_ally(c.self()) ? 2 : 1);
}


// Self, Ally, Melee. Effect: Choose self or an adjacent ally. At the end of that unit’s turn, slay them. They splash (self) 1 holy damage to foes, 1 vitality to allies. Increase damage and vitality by +1 if the unit had 3 or more vitality.
void blood_of_the_covenant(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, SELECT_UNIT_EXCLUDE_FOE);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    u->set_counter(COUNTER_BLOOD_OF_THE_COVENANT, 1);
}


// Range 1-3, Self, Ally. Effect: Target unit converts all vitality into strength, and then becomes unable to gain vitality tokens for the rest of combat.
void wrath(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 3, enum_or(SELECT_UNIT_EXCLUDE_FOE, SELECT_UNIT_WITH_VITALITY_TOKENS));
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    ttoken *t = u->find_token(TOKEN_VITALITY);
    if (t) {
        u->remove_token(t->type(), t->count());
        u->gain_token(TOKEN_STRENGTH, t->count());
    }
    u->set_counter(COUNTER_CANT_GET_VITALITY, 1);
}


// 2 of these units are worth 1 unit slot. Can be activated two at a time.
void thrall(taction &c)
{
    c.self().set_counter(COUNTER_COST_HALF_UNIT_SLOT, 1);
    c.self().set_counter(COUNTER_ACTIVATED_TWO_AT_A_TIME, 1);
}


// When slain, leaves an extra corpse token in an adjacent space.
void fall_to_shambles(taction &c)
{
    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, 1);
    if (!p)
        return;

    c.inc_corpse(*p, +1);
}

// Self.Effect: Gain 1 strength, (4+) 1 speed, (5+) and 1 vitality, (6+) and explode for splash (self): 1 damage. Spare parts: Roll +1D per corpse and pick the highest result.
void unstable_mutation(taction &c)
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

    c.self().gain_token(TOKEN_STRENGTH, 1);
    if (best >= 4)
        c.self().gain_token(TOKEN_SPEED, 1);
    if (best >= 5)
        c.self().gain_token(TOKEN_VITALITY, 1);
    if (best >= 6) {
        for (tunit *u : c.self().units_in_range(1, 1))
            u->take_damage(1, DAMAGE_NORMAL, &c.self());
    }
}


// Attack, Melee. Effect: Mutate. On hit: 1 damage.
void twisting_strike(taction &c)
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
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_NORMAL, &c.self());

    if (c.self().has_upgrade(UPGRADE_GENESTEALER) && d6 >= 5 && u->n_tokens(SELECT_TOKEN_ONLY_POSITIVE)) {
        list<ttoken *> ts = u->tokens();
        ttoken *t = c.player_may_select_token(ts);
        if (!t)
            return;

        u->remove_token(t->type());
        c.self().gain_token(t->type());
    }
}


// After MOVEing, drop a corpse in a free adjacent space.
void leftovers(taction &c)
{
    if (c.player_roll_d6(c.self()) < 4)
        return;

    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, enum_or(SELECT_SPACE_EXCLUDE_OCCUPIED, SELECT_SPACE_EXCLUDE_WALLS));
    if (!p)
        return;

    c.inc_corpse(*p, +1);
}


// Range 3. Effect: One or two units in range mutate. Spare parts: and also dole out 1 strength per corpse consumed.
void inject_mutagen(taction &c)
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
            u->gain_token(TOKEN_STRENGTH, tokens);
    }
}


// Curse, Range 1-3. Effect: A unit in range removes one token of this unit's choice (3+): two (5+): all.
void purge(taction &c)
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
        u->remove_token(t->type(), 1);
        stolen.push_back(t);
    }

    if (c.self().has_upgrade(UPGRADE_SCOUR_FLESH) && !u->is_ally(c.self()))
        u->take_damage(1, enum_or(DAMAGE_TOXIC, DAMAGE_PIERCING), &c.self());

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

        another->gain_token(t->type());
    }
}


// Marriage: Range 1-2. Effect: Instantly slay self or an allied unit in range. Another allied unit in range gains 2 speed, vitality, and strength, or just 1 if the slain unit was a thrall.
void marriage(taction &c)
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
            dst->remove_token(t->type(), t->count());
    }

    dst->gain_token(TOKEN_SPEED, x);
    dst->gain_token(TOKEN_VITALITY, x);
    dst->gain_token(TOKEN_STRENGTH, x);
}


void the_hunger(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_THE_HUNGER))
        return;

    if (c.self().corpses_in_range(2))
        c.unit_step(c.self(), 2);
}


void autophagia(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_AUTOPHAGIA))
        return;

    if (!c.player_may_take_action(TAKE_ACTION_AUTOPHAGIA))
        return;

    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, 1);
    if (!p)
        return;

    c.self().take_damage(1, enum_or(DAMAGE_NORMAL, DAMAGE_PIERCING), &c.self());
    c.inc_corpse(*p, +1);
}


// Self Effect: Mutate, then gain 1 strength. Spare Parts: Then gain (3+) 1 speed, (5+) and 1 vitality, (6+) then mutate again. Roll 1D per corpse consumed for the effect.
void bloodgorger(taction &c)
{
    c.mutate(c.self());
    c.self().gain_token(TOKEN_STRENGTH);

    int times = c.player_may_spare_parts(c.self());
    while (times--) {
        int d6 = c.player_roll_d6(c.self());
        if (d6 >= 3)
            c.self().gain_token(TOKEN_SPEED);
        if (d6 >= 5)
            c.self().gain_token(TOKEN_VITALITY);
        if (d6 >= 6) {
            if (!c.then())
                return;
            c.mutate(c.self());
        }
    }
}

// Melee, Attack On hit: 1 damage. Effect: splash (self): 1 damage.
void bloody_slashes(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_NORMAL, &c.self());
    list<tunit *> splash = c.self().units_in_range(1, 1);
    for (tunit *u : splash)
        u->take_damage(1, DAMAGE_NORMAL, &c.self());
}


// Range 2-4 Effect: Splash (target): Create a corpse in the area for every unit in the area, up to three times, then mutate.
void regurgitate(taction &c)
{
    const auto regurgitate_drown_in_viscera = [](taction &c, tunit *target)
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
                u->remove_token(t->type(), 1);
        }
    };

    const auto regurgitate_cleansing_wash = [](taction &c, tunit *target)
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

        u->remove_token(t->type());
    };

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
void sin_eater(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_NORMAL, &c.self());

    if (u->n_tokens(SELECT_TOKEN_ONLY_POSITIVE)) {
        c.self().gain_token(TOKEN_STRENGTH);
        return;
    }

    int times = 1 + c.player_may_spare_parts(c.self());
    while (times--) {
        ttoken *t = c.player_may_select_token(u->tokens());
        if (!t)
            return;
        u->remove_token(t->type(), 1);
        c.self().gain_token(t->type(), 1);
    }
}


// Self Effect: Gain 1 strength, OR 1 speed, OR 1 vitality. Spare parts: Repeat this effect once for each corpse consumed.
void sculpt_flesh(taction &c)
{
    int times = max(1, c.player_may_spare_parts(c.self()));

    while (times--) {
        optional<token_type> t = c.player_must_select_token_type({TOKEN_STRENGTH, TOKEN_SPEED, TOKEN_VITALITY});
        if (!t)
            return;

        c.self().gain_token(*t);
    }
}


// Melee Effect: Deal 1 devil damage to an adjacent unit. If reduce to 0 hp, obliterates unit and the homunculus gains any tokens the absorbed unit had.
void absorb(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    bool dead = false;
    u->take_damage(1, DAMAGE_DEVIL, &c.self(), &dead);
    if (!dead)
        return;

    if (c.self().has_upgrade(UPGRADE_MELD))
        c.mutate(c.self());
    else
        c.obliterate(*u);

    for (ttoken *t : u->tokens())
        c.self().gain_token(t->type(), t->count());

    if (c.self().has_upgrade(UPGRADE_FORM_CARAPACE))
        c.self().inc_counter(COUNTER_PHYSICAL_ARMOR, +1);
}


// Flesh Whip: Attack, Range 1-2 On hit: 1 damage and splash (target): 1 damage and (4+) create one (6+) or two corpse tokens in an adjacent space to target.
void flesh_whip(taction &c)
{
    list<tunit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.no_target();

    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    // self + splash
    for (tunit *near : u->units_in_range(1))
        u->take_damage(1, DAMAGE_NORMAL, &c.self());

    int n = c.d6_gradations(d6, {{1, 0}, {4, 1}, {6, 2}});
    if (!n)
        return;

    optional<map_pos> p = c.player_must_select_space(u->pos(), 1);
    if (!p)
        return;

    c.inc_corpse(*p, n);
}


void ball_of_limbs(taction &c)
{
    if (!c.self().has_upgrade(UPGRADE_BALL_OF_LIMBS))
        return;

    bool start_second_move = c.trigger() == TRIGGER_BEFORE_MOVE && c.self().n_moves() >= 1;
    bool end_second_move = c.trigger() == TRIGGER_AFTER_MOVE && c.self().n_moves() >= 2;
    if (start_second_move) {
        c.self().inc_counter(COUNTER_MOVEMENT_DESTROY_WALLS, +1);
        c.self().inc_counter(COUNTER_MOVEMENT_ABSORB_CORPSES, +1);
        return;
    }

    if (end_second_move) {
        c.self().inc_counter(COUNTER_MOVEMENT_DESTROY_WALLS, -1);
        c.self().inc_counter(COUNTER_MOVEMENT_ABSORB_CORPSES, -1);
        int walls = c.self().counter(COUNTER_LAST_MOVEMENT_WALLS_DESTROYED);
        int corpses = c.self().counter(COUNTER_LAST_MOVEMENT_CORPSES_ABSORBED);
        int times = walls + corpses;
        while (times--)
            c.mutate(c.self());
        return;
    }
}


// At turn start, may convert one of this unit’s mutation tokens into strength, speed, or vitality. Then, mutate.
void polyglot(taction &c)
{
    ttoken *t = c.self().find_token(TOKEN_MUTATION);
    if (!t)
        return c.no_resources();
    optional<token_type> tt = c.player_may_select_token_type({TOKEN_STRENGTH, TOKEN_SPEED, TOKEN_VITALITY});
    if (tt) {
        c.self().remove_token(t->type());
        c.self().gain_token(*tt);
    }
    if (!c.then())
        return;
    c.mutate(c.self());
}


// At turn end, may remove one token from this unit and grant to a unit in range 2.
void accelerate_evolution(taction &c)
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

    c.self().remove_token(t->type(), 1);
    u->gain_token(t->type(), 1);
}


// May remove a mutation token at start or end of own turn to step 2.
void rapid_move(taction &c)
{
    if (c.trigger() == TRIGGER_TURN_START) {
        c.self().inc_counter(COUNTER_RAPID_MOVE_AVAILABLE, +1);
        ttoken *t = c.self().find_token(TOKEN_MUTATION);
        if (!t)
            return c.no_resources();
        if (!c.player_may_take_action(TAKE_ACTION_RAPID_MOVE))
            return;

        c.self().remove_token(t->type());
        c.unit_step(c.self(), 2);
        c.self().inc_counter(COUNTER_RAPID_MOVE_AVAILABLE, -1);
        return;
    }

    if (c.trigger() == TRIGGER_TURN_END) {
        bool rm_available = c.self().counter(COUNTER_RAPID_MOVE_AVAILABLE);
        ttoken *t = c.self().find_token(TOKEN_MUTATION);
        if (!rm_available || !t)
            return c.no_resources();

        if (c.player_may_take_action(TAKE_ACTION_RAPID_MOVE)) {
            c.self().remove_token(t->type());
            c.unit_step(c.self(), 2);
        }
        c.self().set_counter(COUNTER_RAPID_MOVE_AVAILABLE, 0);
        return;
    }
}


// May remove a mutation token on self to grant +1D on any attack and allow it to ignore cover.
void ancillary_limbs(taction &c)
{
    ttoken *t = c.self().find_token(TOKEN_MUTATION);
    if (!t)
        return c.no_resources();
    if (!c.player_may_take_action(TAKE_ACTION_ANCILLARY_LIMBS))
        return;
    c.self().inc_counter(COUNTER_ANCILLARY_LIMBS, +1);
}


// Line 4. Line: 1 damage. Effect: Allies in the line mutate instead of taking damage.
void experimental_surgery(taction &c)
{
    list<tunit *> us = c.player_must_select_line(4);
    for (tunit *u : us) {
        if (c.self().is_ally(*u))
            c.mutate(*u);
        else
            u->take_damage(1, DAMAGE_NORMAL, &c.self());
        if (!c.then())
            return;
    }
}


// Self. Effect: Create one, (3+) two, (5+) three corpses in free adjacent spaces, then mutate once for each corpse created.
void new_material(taction &c)
{
    int d6 = c.player_roll_d6(c.self());
    int cs = c.d6_gradations(d6, {{1, 1}, {3, 2}, {5, 3}});
    int n = 0;
    while (cs--) {
        optional<map_pos> p = c.player_must_select_space(c.self().pos(), 1, enum_or(SELECT_SPACE_EXCLUDE_OCCUPIED, SELECT_SPACE_EXCLUDE_WALLS));
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
void clone(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, enum_or(SELECT_UNIT_EXCLUDE_SELF, SELECT_UNIT_EXCLUDE_FOE));
    if (us.empty())
        return c.no_target();
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;
    optional<map_pos> p = c.player_must_select_space(c.self().pos(), 2, enum_or(SELECT_SPACE_EXCLUDE_OCCUPIED, SELECT_SPACE_EXCLUDE_WALLS));
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
void stitch_fix(taction &c)
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
void inject_stimulant(taction &c)
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
void biotoxin_injector(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.no_target();
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;
    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    u->take_damage(1, DAMAGE_TOXIC, &c.self());
    u->inc_counter(COUNTER_BIOTOXIN_INJECTOR, +1);
}


// Attack, Range 2-5. On hit: May remove one positive token from target, then deal 1 toxin damage. Spare Parts: Repeat this effect once.
void mutagen_injector(taction &c)
{
    list<tunit *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.no_target();
    tunit *u = c.player_must_select_unit(us);
    if (!u)
        return;
    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE, &c.self());

    int effect = c.player_may_spare_parts(c.self()) ? 2 : 1;
    while (effect--) {
        list<ttoken *> ts = u->tokens();
        ttoken *t = c.player_may_select_token(ts, SELECT_TOKEN_ONLY_POSITIVE);
        if (!t)
            break;
        u->remove_token(t->type());
        u->take_damage(1, DAMAGE_TOXIC, &c.self());
    }
}

// Line 3. Effect: Line: 1 fire damage and inflict 1 vulnerable, (3-4) OR 1 toxic damage and inflict 1 slow, (5-6) OR 1 curse damage and inflict 1 weak. Spare Parts: Roll 1D for effect per corpse consumed and choose any result.
void chaos_beam(taction &c)
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
            u->take_damage(1, DAMAGE_FIRE, &c.self());
            u->gain_token(TOKEN_VULNERABLE);
            break;
        case 3:
        case 4:
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
            u->gain_token(TOKEN_SLOW);
            break;
        case 5:
        case 6:
            u->take_damage(1, DAMAGE_CURSE, &c.self());
            u->gain_token(TOKEN_WEAK);
            break;
        }
        if (!c.then())
            return;
    }
}


// (1 SOUL): Own or Allied Turn, Range 4. Trigger: Turn start. Effect: Unit gains 1 strength, (3-4) OR 1 speed, (5-6) OR 1 vitality. Spare Parts: May choose one token per corpse consumed instead of rolling.
void wild_mutation(taction &c)
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
        u->gain_token(t);
}


// (3 SOUL): Own or allied turn. Range 4. Copy all positive tokens on target unit, then grant them to another unit in range.
void sample_genome(taction &c)
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
            dst->gain_token(t->type(), t->count());
    }
}


// (1 SOUL): Own or allied turn. Range 3. Effect: Swap places with an allied unit, then both of you mutate.
void flesh_jump(taction &c)
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
void grow_bonus_legs(taction &c)
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

    int removed = u->remove_token(t->type(), *remove);
    if (!c.then())
        return;
    c.unit_step(*u, removed * 2, enum_or(MOVEMENT_FREE, MOVEMENT_IGNORE_HAZARDS));
}


// (2 SOUL): Own or allied turn. Range 3. Trigger: Turn start. Effect: Remove any number of mutation tokens from target, then target gains +1D on attacks this turn per token removed, and their damage ignores armor.
void grow_bonus_limbs(taction &c)
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

    int removed = u->remove_token(t->type(), *remove);
    if (!c.then())
        return;
    u->inc_counter(COUNTER_GROW_BONUS_LIMBS, removed);
}


// (3 SOUL): Curse, Any turn. Trigger: Turn end. Effect: Deal 1 toxic damage to unit. If this reduces them to 0 hp, they are obliterated. Create up to 3 corpse tokens under their space or in free adjacent spaces.
void recycle(taction &c)
{
    tunit &u = c.activated();
    if (u.is_curseproof())
        return c.no_target();

    if (!c.player_may_spend_soul(3))
        return c.no_resources();

    bool slayed = false;
    u.take_damage(1, DAMAGE_TOXIC, &c.self(), &slayed);
    if (!slayed)
        return;
    c.obliterate(u);
    optional<map_pos> p = c.player_must_select_space(u.pos(), 1, enum_or(SELECT_SPACE_EXCLUDE_OCCUPIED, SELECT_SPACE_EXCLUDE_WALLS));
    if (!p)
        return;
    optional<int> n = c.player_must_select_corpse_count(3);
    if (!n)
        return;
    c.inc_corpse(*p, *n);
}


// (4 SOUL): Curse, Foe turn. Range 3. Trigger: Turn start. Effect: Inflict 1 slow, weak, and vulnerable on a foe (5+) twice.
void devolve(taction &c)
{
    tunit &u = c.activated();
    if (u.is_curseproof())
        return c.no_target();

    if (!c.player_may_spend_soul(4))
        return c.no_resources();

    int d6 = c.player_roll_d6(c.self());
    int times = c.d6_gradations(d6, {{1, 1}, {5, 2}});
    while (times--) {
        u.gain_token(TOKEN_SLOW);
        u.gain_token(TOKEN_WEAK);
        u.gain_token(TOKEN_VULNERABLE);
    }
}


// (6 SOUL): Own turn. Trigger: Turn start. Effect: Increase MV to 6, DF to 6+, gain 6 strength, become curseproof, gain free movement, and gain super armor. Keep any damage taken. At the end of your next turn, your form destabilizes and you are obliterated.
void final_form(taction &c)
{
    if (!c.player_may_spend_soul(6))
        return c.no_resources();

    c.self().inc_counter(COUNTER_CURSEPROOF, +1);
    c.self().inc_counter(COUNTER_MOVEMENT_FREE, +1);
    c.self().inc_counter(COUNTER_SUPER_ARMOR, +1);

    c.self().set_counter(COUNTER_ALTERED_MV, 6);
    c.self().set_counter(COUNTER_ALTERED_DF, 6);
    c.self().gain_token(TOKEN_STRENGTH, 6);

    c.self().set_counter(COUNTER_FINAL_FORM, 2);
}

// *** UNITS ***
void gunwight(tcard &c)
{
    c.set_faction_type(FACTION_CARCASS, UNIT_THRALL);
    c.set_stats(2, 2, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, formation);
    c.add_trait(TRIGGER_COMBAT_START, thrall);
    c.add_trait(enum_or(TRIGGER_BEFORE_ACT, TRIGGER_AFTER_ACT), scavenge_ammo);

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

    c.add_trait(TRIGGER_AFTER_DAMAGED, blood_rage);
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

    c.add_trait(TRIGGER_AFTER_DAMAGED, blood_rage);

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

    c.add_trait(TRIGGER_AFTER_DAMAGED, blood_rage);

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

    c.add_trait(TRIGGER_AFTER_DAMAGED, blood_rage);
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

    c.add_trait(TRIGGER_AFTER_DAMAGED, blood_rage);
    c.add_trait(enum_or(TRIGGER_ROUND_START, TRIGGER_AFTER_DAMAGED), steaming_rage);
    c.add_trait(TRIGGER_COMBAT_START, large);
    c.add_trait(TRIGGER_AFTER_HP_CHANGED, machineheart);

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

    c.add_trait(TRIGGER_AFTER_DAMAGED, blood_rage);

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
    c.add_trait(TRIGGER_SLAINED, invigorating_viscera);
    c.add_trait(enum_or(TRIGGER_AFTER_HAZARD_CHANGED, TRIGGER_AFTER_POS_CHANGED), smog_shroud);

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
    c.add_trait(TRIGGER_COMBAT_START, toxic_avenger_immune_to_hazards);
    c.add_trait(TRIGGER_TURN_START, toxic_avenger);

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

    c.add_trait(TRIGGER_COMBAT_START, inverted_crucifix);
    c.add_trait(TRIGGER_COMBAT_START, thrall);
    c.add_trait(TRIGGER_COMBAT_START, squirm);

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

    c.add_trait(enum_or(TRIGGER_AFTER_HAZARD_CHANGED, TRIGGER_AFTER_POS_CHANGED), slither);
    c.add_trait(TRIGGER_BEFORE_ACT, leap)

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

    c.add_trait(TRIGGER_BEFORE_MOVE, teleport);

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

    c.add_trait(TRIGGER_BEFORE_MOVE, labyrinth_master);
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
    c.add_trait(TRIGGER_BEFORE_MOVE, zealotry);

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
    c.add_trait(TRIGGER_TURN_END, focus);

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

    c.add_trait(TRIGGER_COMBAT_START, flight);
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


// TODO: update this and other IGORRI according to the latest balance changes
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
    c.add_trait(enum_or(TRIGGER_BEFORE_MOVE, TRIGGER_AFTER_MOVE), ball_of_limbs);

    c.add_act_ability(sculpt_flesh);
    c.add_act_ability(absorb);
    c.add_act_ability(flesh_whip);

    c.add_upgrade(UPGRADE_BALL_OF_LIMBS);
    c.add_upgrade(UPGRADE_MELD);
    c.add_upgrade(UPGRADE_FORM_CARAPACE);
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
    c.add_unit(gunwight);
    c.add_unit(enforcer);
    c.add_unit(ammo_goblin);
    c.add_unit(barrelform);
    c.add_unit(egis_weapon);
    // c.add_card(operator);
}


void goregrinders(tfaction &c)
{
    c.add_unit(warhead);
    c.add_unit(carnifex);
    c.add_unit(pain_ghoul);
    c.add_unit(painwheel);
    c.add_unit(berserker);
}


void gargamox(tfaction &c)
{
    c.add_unit(scum);
    c.add_unit(rotten);
    c.add_unit(leech);
    c.add_unit(host);
    c.add_unit(slime);
}


void deadsouls(tfaction &c)
{
    c.add_unit(sacrifice);
    c.add_unit(chosen);
    c.add_unit(visigheist);
    c.add_unit(banshee);
    c.add_unit(bound_devil);
}


void abhorrer(tfaction &c)
{
    c.add_unit(penitent);
    c.add_unit(zealot);
    c.add_unit(antipriest);
    c.add_unit(inquisitor);
    c.add_unit(holy_body);
}


void igorri(tfaction &c)
{
    c.add_unit(stitch);
    c.add_unit(chop_doc);
    c.add_unit(lycan);
    c.add_unit(strigoi);
    c.add_unit(homonculus);
    c.add_necromancer(chirurgeon);
}


int main()
{
    cout << "hi!\n";
    return 0;
}
