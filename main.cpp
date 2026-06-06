#include <cmath>
#include <optional>
#include <list>
#include <map>
#include <algorithm>
#include <iostream>
#include <set>


using namespace std;

enum trigger_type
{
    TRIGGER_COMBAT_START,
    TRIGGER_ACTION_MANUAL,
    TRIGGER_ROUND_START, // round is when every unit takes a turn
    TRIGGER_ROUND_END,
    TRIGGER_TURN_START,
    TRIGGER_TURN_END,
    TRIGGER_BEFORE_MOVE,
    TRIGGER_AFTER_MOVE,
    TRIGGER_BEFORE_ACT,
    TRIGGER_AFTER_ACT,
    TRIGGER_BEFORE_ATTACK,
    TRIGGER_BEFORE_SLAINED,
    TRIGGER_AFTER_DAMAGED,
    TRIGGER_AFTER_HP_CHANGED,
    TRIGGER_AFTER_POS_CHANGED, // should be called at EVERY map_pos travelled during move/step
    TRIGGER_AFTER_HAZARD_CHANGED,
    TRIGGER_AFTER_WALL_CHANGED,
    TRIGGER_AFTER_ATTACK_DAMAGED,
    TRIGGER_AFTER_ATTACK_HITED,
    TRIGGER_AFTER_UNIT_MOVED,
    TRIGGER_AFTER_UNIT_DAMAGED,
    TRIGGER_AFTER_UNIT_ACTED,
    TRIGGER_AFTER_HEADSHOT,
    TRIGGER_AFTER_UNIT_SLAINED,
    TRIGGER_AFTER_SLAIN, // self killed other
    TRIGGER_AFTER_UNIT_TURN_END, // after any unit's turn ends
    TRIGGER_BEFORE_ATTACKED,
    TRIGGER_AFTER_ATTACKED,

    TRIGGER_SOUL_OWN_TURN,
    TRIGGER_SOUL_ALLIED_TURN,
    TRIGGER_SOUL_FOE_TURN,
    TRIGGER_SOUL_OWN_OR_ALLIED_TURN = TRIGGER_SOUL_OWN_TURN | TRIGGER_SOUL_ALLIED_TURN,
    TRIGGER_SOUL_ANY_TURN = TRIGGER_SOUL_OWN_TURN | TRIGGER_SOUL_ALLIED_TURN | TRIGGER_SOUL_FOE_TURN,
};


enum take_action
{
    TAKE_ACTION_SCAVENGE_AMMO,
    TAKE_ACTION_RAPID_MOVE,
    TAKE_ACTION_ANCILLARY_LIMBS,
    TAKE_ACTION_AUTOPHAGIA,
    TAKE_ACTION_CONSECRATE,
    TAKE_ACTION_PROPAGATE_SWARM,
    TAKE_ACTION_LEAP,
    TAKE_ACTION_TELEPORT,
    TAKE_ACTION_LABYRINTH_MASTER,
    TAKE_ACTION_STEP,
    TAKE_ACTION_MOVE_AGAIN,
    TAKE_ACTION_VIRULENCE,
    TAKE_ACTION_STANCE_2,
    TAKE_ACTION_STANCE_5,
    TAKE_ACTION_REMOVE_NEGATIVE_TOKEN,
    TAKE_ACTION_SUMMON_FLOCK,
    TAKE_ACTION_DIVE_BOMB,
    TAKE_ACTION_FERAL_DODGE,
};


enum roll_tag
{
    ROLL_TAG_NONE,
    ROLL_TAG_ATTACK,
    ROLL_TAG_IGNORE_COVER,
};


enum select_token_filter
{
    SELECT_TOKEN_ANY       = 0,
    SELECT_TOKEN_REMOVABLE = 1 << 0,
    SELECT_TOKEN_POSITIVE  = 1 << 2,
    SELECT_TOKEN_NEGATIVE  = 1 << 3,
    SELECT_TOKEN_PLAGUE    = 1 << 4,
    SELECT_TOKEN_VITALITY  = 1 << 5,
};


enum damage_type
{
    DAMAGE_PHYSICAL,

    DAMAGE_FIRE,
    DAMAGE_TOXIC,
    DAMAGE_CURSE,
    DAMAGE_HOLY,

    // can't be increased, reduced or ignored, obliterates on slay
    DAMAGE_DEVIL,

    // damage source: for immunities tracking
    // TODO: go and set this damage types across the implemented abilities
    DAMAGE_FROM_LINE,
    DAMAGE_FROM_SPLASH,

    // inherits damage type, but no other properties
    DAMAGE_GRAZE,
    // ignores armor
    DAMAGE_PIERCING,
    DAMAGE_CANT_BE_INCREASED,
    DAMAGE_CANT_BE_REDUCED,
    DAMAGE_CANT_SLAY,
    DAMAGE_OBLITERATE_ON_SLAY,
    DAMAGE_CANT_BENEFIT_FROM_STRENGTH,
    DAMAGE_IGNORE_VITALITY,

    // for a special case: devil damage of homonculus:absorb with mold upgrade
    DAMAGE_SLAY_ON_OBLITERATE,
};


enum armor
{
    ARMOR_NONE,
    // -1 physical
    ARMOR_PHYS,
    // -1 fire, toxic, curse, and holy damage
    ARMOR_MAG,
    // as both PHYS and MAG
    ARMOR_SUPER,

};

enum unit_faction
{
    FACTION_CARCASS,
    FACTION_GOREGRINDERS,
    FACTION_GARGAMOX,
    FACTION_DEADSOULS,
    FACTION_ABHORRER,
    FACTION_IGORRI,
    FACTION_STEEPLEWRACK,
};


enum unit_type
{
    UNIT_THRALL,
    UNIT_SCION,
    UNIT_FREAK,
    UNIT_HORROR,
    UNIT_HUNTER,
    UNIT_TYRANT,
    UNIT_NECROMANCER,

};


enum trait_id
{
    TRAIT_CURSEPROOF,
    TRAIT_MINUS_1D_FROM_ALL_ATTACKS,
    TRAIT_CAN_BE_USED_AS_COVER_BY_ALLIES,
    TRAIT_HAS_COVER_FROM_ALL_DIRECTIONS,
    TRAIT_UNABLE_TO_MOVE,
    TRAIT_UNABLE_TO_STEP,
    TRAIT_IMMUNE_TO_PUSH,
    TRAIT_IMMUNE_TO_PULL,
    TRAIT_IMMUNE_TO_HAZARDS,
    TRAIT_IMMUNE_TO_SPLASH_DAMAGE,
    TRAIT_IMMUNE_TO_LINE_DAMAGE,
    TRAIT_IMMUNE_TO_GRAZE_DAMAGE,
    TRAIT_IMMUNE_TO_ALL_DAMAGE,
    TRAIT_RANGED_ATTACKS_AUTO_MISS,
    TRAIT_MOVEMENT_FREE,
    TRAIT_MOVEMENT_THROUGH_WALLS,
    TRAIT_MOVEMENT_DESTROY_WALLS,
    TRAIT_MOVEMENT_ABSORB_CORPSES,
    TRAIT_LAST_MOVEMENT_WALLS_DESTROYED,
    TRAIT_LAST_MOVEMENT_CORPSES_ABSORBED,
    TRAIT_SUPER_ARMOR,
    TRAIT_PHYSICAL_ARMOR,
    TRAIT_MAGICAL_ARMOR,
    TRAIT_ALTERED_MV,
    TRAIT_ALTERED_DF,
    TRAIT_FLIGHT,
    TRAIT_IS_LARGE,
    TRAIT_COST_HALF_UNIT_SLOT,
    TRAIT_ACTIVATED_TWO_AT_A_TIME,
    TRAIT_LEAVE_HAZARD_INSTEAD_OF_CORPSE,
    TRAIT_LEAVE_ADVERSE_TERRAIN_INSTEAD_OF_CORPSE,
    TRAIT_LEAVE_WALL_INSTEAD_OF_CORPSE,
    // Foes count as isolated when adjacent to this unit unless they have two or more allies adjacent
    TRAIT_ADJACENT_FOES_COUNT_ISOLATED_UNLESS_2_ALLIES,
    // If your necromancer is the primary target of a foe’s ability and you’re adjacent to one of minions, you can redirect the ability to target your minion instead, regardless of the it’s range. This only works if you are the primary target of an ability. For example, if you are caught in a splash effect from an explosion, you wouldn’t be able to redirect it unless you were directly targeted.
    TRAIT_BODY_BLOCK, // 1 or 2 for available body-block ally range

    TRAIT_FORMATION,
    // player may choose manually which ability to reload
    TRAIT_RELOAD_CHOOSE,
    TRAIT_RELOAD,
    TRAIT_RELOAD_2,
    TRAIT_RELOAD_3,
    TRAIT_RELOAD_4,
    TRAIT_SCAVENGE_AMMO_AVAILABLE,
    TRAIT_BONE_SHARDS,
    TRAIT_TRANSFORM_TO_GUN,
    // Once a round, reloading grants you 1 strength
    TRAIT_HOT_CLIP,
    // You can use your units as cover. While in cover your abilities gain +2 range.
    TRAIT_BONE_WALL,
    // All ranged attacks gain a minimum range of 1 and push units 1 on hit if they are adjacent
    TRAIT_GUN_KATA,
    // Your ranged attacks gain headshot: reload
    TRAIT_CLUTCH_RELOAD,
    TRAIT_DEVIL_BULLET,
    TRAIT_UNHOLY_SIXGUN_HEADSHOTS,
    TRAIT_OVERWATCH,

    TRAIT_BLOOD_RAGE,
    // After any ability resolves that damages this unit, this unit deals 1 damage back to the ability's owner, even if this unit was slain.
    TRAIT_RETALIATION,
    TRAIT_STEAMING_RAGE,
    TRAIT_MACHINEHEART,
    TRAIT_NERVE_TWITCH,
    TRAIT_BLOOD_RUSH,

    TRAIT_SMOG_SHROUD,
    // Gains death burst: splash (self): 1 toxic damage and 1 plague. This effect cannot stack with itself but stacks with other death burst effects.
    TRAIT_SUPPURATE,
    TRAIT_POLLUTION_SHROUD,
    // Infect abilities that you use or that target you may jump an extra time.
    TRAIT_INFECT_JUMP_EXTRA,

    // When slain, does not remove Doom, and (5+) Dooms slayer.
    TRAIT_INVERTED_CRUCIFIX,
    TRAIT_SLITHER,
    // may no longer MOVE or step until the end of its next turn or unit it's no longer isolated.
    TRAIT_HELLS_GRASP,
    TRAIT_MAGGOT_DESTROYER,

    TRAIT_MIRACLE,
    TRAIT_HOLY_VESSEL,
    TRAIT_HOLY_VESSEL_READY,
    // Attacks cannot miss (all misses turn into hits).
    TRAIT_WINTER_ROSE_STANCE,
    TRAIT_DELAY_JUDGEMENT,
    TRAIT_SMITE,
    TRAIT_WINCH,
    TRAIT_ABLUTIONS,
    TRAIT_BLOOD_OF_THE_COVENANT,
    TRAIT_CANT_GET_VITALITY,
    // self and adjacent allies may spend this unit's HP as if it were vitality tokens.
    TRAIT_COMMUNION,
    TRAIT_DECREE_OF_FORBIDDANCE, // handled
    // may treat the current round number as either 2
    TRAIT_STANCE_2,
    // may treat the current round number as either 5
    TRAIT_STANCE_5,

    // unit allowed to make a step on start/end of it's turn
    TRAIT_RAPID_MOVE_AVAILABLE,
    // +1D on any attack and ignore cover. removed after attack
    TRAIT_ANCILLARY_LIMBS,
    // at the end of it's turn explode(self) effect for 1 toxin damage. allies (of one, who set this effect) mutate instead of taking damage
    TRAIT_BIOTOXIN_INJECTOR,
    // +1D to attacks per stack, and damage ignores armor. til turn end
    TRAIT_GROW_BONUS_LIMBS,
    // tick down at the of your turn, obliterated when reaches zero
    TRAIT_FINAL_FORM,

    TRAIT_BLOOD_FRENZY,
    // number of times in round, that dive bomb can be used
    TRAIT_DIVE_BOMB_CHARGE,
    TRAIT_DIVE_BOMB_RABID_CHARGE,
    TRAIT_ENDLESS_CHARGE,
    TRAIT_SINEW,
    // counts as having +2 for related unit rip_apart
    TRAIT_PARANOIA,
    TRAIT_AURA_MINUS_1D_FOR_ALL_FOE_ATTACKS,
    TRAIT_AURA_IMMUNE_TO_GRAZE_DAMAGE,
    TRAIT_CLINGING,
    // counts as two allied units for the purposes of Rip Apart, but not to itself
    TRAIT_FLESH_SACRIFICE,
};


enum select_unit_filter
{
    SELECT_UNIT_ANY = 0,
    SELECT_UNIT_NO_SELF = 1,
    SELECT_UNIT_ALLY = 1 << 1,
    SELECT_UNIT_NO_ALLY = 1 << 2,
    SELECT_UNIT_FOE = 1 << 3,
    SELECT_UNIT_NO_FOE = 1 << 4,
    SELECT_UNIT_NO_WALL = 1 << 5, // TODO: change it to include walls. by default excluded

    SELECT_UNIT_WITH_TOKENS = 1 << 6,
    SELECT_UNIT_WITH_POSITIVE_TOKENS = 1 << 7,
    SELECT_UNIT_WITH_NEGATIVE_TOKENS = 1 << 8,
    SELECT_UNIT_WITH_MUTATION_TOKENS = 1 << 9,
    SELECT_UNIT_WITH_PLAGUE_TOKENS = 1 << 10,
    SELECT_UNIT_WITH_VITALITY_TOKENS = 1 << 11,
    SELECT_UNIT_FOR_CURSE = 1 << 12,
    SELECT_UNIT_WITH_DEATHBURST = 1 << 13,
    SELECT_UNIT_ISOLATED = 1 << 14,
    SELECT_UNIT_WITH_HP_1_OR_LOWER = 1 << 15,

    SELECT_UNIT_IGNORE_LINE_OF_SIGHT = 1 << 16,
    // +X max range if unit has cover against a target
    SELECT_UNIT_IF_COVER_MAX_RANGE_1 = 1 << 17,
    SELECT_UNIT_IF_COVER_MAX_RANGE_2 = 1 << 18,
    // no maximum range against isolated units
    SELECT_UNIT_IF_ISOLATED_MAX_RANGE_INF = 1 << 19,
    // if selects more than 1 unit, all of them must be adjacent to each other
    SELECT_UNIT_MULTIPLE_ADJACENT_TARGETS = 1 << 20,
};


enum select_space_filter
{
    SELECT_SPACE_ANY = 0,
    SELECT_SPACE_UNIT = 1 << 0,
    SELECT_SPACE_NO_UNIT = 1 << 1,
    SELECT_SPACE_WALLS = 1 << 2,
    SELECT_SPACE_NO_WALLS = 1 << 3,
    SELECT_SPACE_CORPSES = 1 << 4,
    SELECT_SPACE_HAZARD = 1 << 5,
    SELECT_SPACE_NO_ADVERSE_TERRAIN = 1 << 6,
    SELECT_SPACE_IGNORE_LINE_OF_SIGHT = 1 << 7,

    SELECT_SPACE_FREE = SELECT_SPACE_NO_UNIT | SELECT_SPACE_NO_WALLS,
};


enum direction
{
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_UP,
    DIRECTION_DOWN,
};


enum token_type
{
    // Strength and weak cancel each other.
    // Strength: +1 damage, discard one after dealing damage.
    TOKEN_STRENGTH,
    // Weak: -1 damage, discard one after dealing damage.
    TOKEN_WEAK,
    // Vitality and Vulnerability cancel each other.
    // Vulnerability: Take +1 more damage, discard one after taking damage.
    TOKEN_VULNERABLE,
    // Vitality: Take -1 damage, discard one after taking damage.
    TOKEN_VITALITY,
    // Slow and speed cancel each other.
    // Slow: MOVE a max of 1 space, discard one after MOVEing.
    TOKEN_SLOW,
    // Speed: MOVE +2 more spaces, discard one after MOVEing.
    TOKEN_SPEED,

    // After a berserk unit ACTs, it splashes (self) 1 damage, then discards a berserk token. This damage can't be increased in any way. If this effect does not damage any other characters, it deals 1 damage to itself instead, ignoring armor. Berserk counts as a positive and a negative token.
    TOKEN_BERSERK,
    // Units affected by plague take 1 toxic damage at the end of their turn, then that unit discards a plague token. This damage cannot be reduced or ignored in any way (by tokens or armor) but cannot slay a unit. Gargamox units don't take damage from plague and don't remove plague tokens, but can still be affected by plague.
    TOKEN_PLAGUE,
    // Any unit affected by at least one Doom token takes 4 devil damage at the end of round 4. A unit can only clear a Doom token by slaying another unit and they cannot be cleared in any other way.
    TOKEN_DOOM,
    // Mutation tokens can be discarded instead of discarding any other token. (Mutation tokens can be a stand-in and act as a ‘copy’ for any other token, allowing you to keep the original)
    TOKEN_MUTATION,
};


enum movement_tags
{
    MOVEMENT_DEFAULT,
    MOVEMENT_FREE,
    MOVEMENT_STRAIGHT_LINE,
    MOVEMENT_IGNORE_HAZARDS,
    MOVEMENT_DESTROY_WALLS,
    MOVEMENT_ABSORB_CORPSES,
    MOVEMENT_THROUGH_FOES,
    MOVEMENT_THROUGH_WALLS,
    // If pulling a unit into a wall, +2 distance and the kidnapped unit can pass through walls and units during this movement
    MOVEMENT_KIDNAP,
    // automatically added on push/pull
    MOVEMENT_FORCED,
    // remove unit from the battlefield and place it any free space in range
    MOVEMENT_TELEPORT,
    MOVEMENT_STEP_CAN_SPEND_SPEED,
};


enum upgrade_id
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

    // Bloodletting gains Rip Apart (5+): deal 1 curse damage again.
    UPGRADE_THE_RED_WET_HUE,
    // At round 5+, become immune to graze, line, and splash damage.
    UPGRADE_BLOOD_FRENZY,
    // Dive Bomb may be used for free on one Flock each round.
    UPGRADE_RABID,
    // At 1 HP or lower, become immune to damage from grazes, lines, and splash effects.
    UPGRADE_SINEW,
    // Cannibalize may target an enemy at 1 HP or lower but grants no tokens when used this way.
    UPGRADE_VORACIOUS,
    // Raise Roost gains +1 range and may first pull a unit in range 2 by one space.
    UPGRADE_THE_NOOSE,
    // Sonic Screech also grants one or two allies in the area 1 speed or strength.
    UPGRADE_EAR_SPLITTER,
    // Paranoia deals 1 piercing curse damage but only counts as +1 adjacent unit.
    UPGRADE_PAINFUL_WHISPERS,
    // When Siren's Song expires, adjacent allies gain 1 speed.
    UPGRADE_LULLABY,
    // If starting turn adjacent to a wall, gain 1 speed and clear a negative token.
    UPGRADE_PERCHING,
    // While adjacent to a wall, damage ignores vitality and armor.
    UPGRADE_WRITHING,
    // While adjacent to a wall, become curseproof.
    UPGRADE_CLINGING,
    // When arriving, may slay all adjacent allied units and gain 1 speed or strength for each.
    UPGRADE_MESSY_ARRIVAL,
    // Lose Large and gain free movement.
    UPGRADE_SLEEK_OWL,
    // Concussive Shriek also slays allied units in the area, counting each as a wall.
    UPGRADE_BONE_RESONANCE,
    // If Feeding Call triggers two or more times before expiring, gain 2 strength.
    UPGRADE_ENTRAIL_SCATTER,
    // Horrendous Pecking does not end when this unit moves outside its turn.
    UPGRADE_JITTER,
    // Flense ignores armor and vitality against targets at 1 HP or lower.
    UPGRADE_WOLF_DOWN,
};


template <typename T> T enum_or(T a, T b) { return T((int)a | (int)b); }
template <typename T> T enum_or(T a, T b, T c) { return enum_or(a, enum_or(b, c)); }


struct map_space;
struct unit;
struct token;
struct combat;
struct player;
struct unit_card;


// bound to combat place (even if it was taken from a unit, that later moved)
struct map_space
{
    virtual ~map_space() = default;
    virtual bool is_adjacent(const map_space &) const = 0;
    virtual bool has_line_of_sight(const map_space &) const = 0;

    virtual unit *unit_standing() const = 0;
    virtual bool is_wall() const = 0;
    virtual bool is_adverse_terrain() const = 0;
    virtual bool is_hazard() const = 0;
    virtual bool is_stairs() const = 0;
    virtual int n_corpses() const = 0;

    virtual void set_wall(bool) = 0;
    virtual void set_adverse_terrain(bool) = 0;
    virtual void set_hazard(bool) = 0;
    virtual void inc_corpses(int) = 0;

    virtual map_space *adjacent(direction) const = 0;
    virtual list<map_space *> spaces_in_range(int, int, select_space_filter = SELECT_SPACE_ANY) const = 0;

    bool is_free() const { return !is_wall() && !unit_standing(); }
    bool passes_filter(select_space_filter) const;
};


// the action has been resolved, either successfully or not. and should not be rerolled.
enum class action_resolved
{
    COMPLETELY,
    PREMATURELY,
    PLAYER_CHOSE_NOT_TO,
    MISSED_A_HIT,
};


// if action can't be done further (or even started). rollback if possible. it's not possible, if RNG or some undoable changes and choices were made.
enum class action_prevented
{
    NO_TARGET, // TODO: split into no units, no tokens, etc...
    NO_SOUL,
    NO_MOVE,
    NO_TOKENS,
    NO_UPGRADE,
    NO_HAZARD,
    RELOAD,
    ALREADY_USED,
    CHARGE,
    CONDITION_UNSATISFIED,
};


// action failed, because timeout or something. rollback if possible
enum class action_failed
{
    PLAYER_IGNORED_MUST_SELECT,
};


struct action_result {};


struct unit
{
    virtual ~unit() = default;

    virtual list<token *> tokens() const = 0;
    int n_tokens(select_token_filter filter = SELECT_TOKEN_ANY) const;
    token *find_token(token_type type) const;

    virtual void inc_token(token_type t, int c) = 0;
    void remove_token(token_type type, int count = 1) { return inc_token(type, -count); }
    void gain_token(token_type type, int count = 1) { return inc_token(type, count); }

    virtual void deploy(const map_space &) = 0;
    virtual void teleport(int distance) = 0;
    // deduct the player, doing movement from the (unit &from host)
    virtual void push(unit &from, int distance = 1) = 0;
    virtual void push(map_space &from, int distance = 1) = 0;
    // deduct the player, doing movement from the (unit &from host)
    virtual void pull(unit &to, int distance = 1, movement_tags extra_tags = MOVEMENT_DEFAULT) = 0;

    // TODO: make void
    virtual int inc_trait(trait_id c, int x) = 0;
    virtual void set_trait(trait_id c, int x) = 0;
    virtual int trait(trait_id c) const = 0;

    virtual void add_trait_related_unit(trait_id, unit *) = 0;
    virtual void remove_trait_related_unit(trait_id, unit *) = 0;
    virtual bool has_trait_related_unit(trait_id, unit *) const = 0;

    virtual void inc_trait_after(trait_id c, int x, trigger_type by_trigger, int after_n_triggers) = 0;
    virtual void set_trait_after(trait_id c, int x, trigger_type by_trigger, int after_n_triggers) = 0;

    virtual void do_after(action_result(*foo)(combat &), trigger_type by_trigger, int after_n_triggers) = 0;

    virtual list<unit *> units_in_range(int, int, select_unit_filter exclude = SELECT_UNIT_ANY) const = 0;
    virtual list<map_space *> spaces_in_range(int, int, select_space_filter = SELECT_SPACE_ANY) const = 0;
    int corpses_in_range(int, int) const;

    virtual map_space *space() const = 0;
    bool is_adjacent(const unit &other) const { return space()->is_adjacent(*other.space()); }

    virtual void take_damage(int x, damage_type type, unit *from) = 0;
    virtual bool has_upgrade(upgrade_id) const = 0;
    virtual bool is_ally(unit &) const = 0;
    virtual unit_type type() const = 0;
    virtual unit_faction faction() const = 0;
    virtual bool is_slain() const = 0;
    virtual void set_slain(bool) = 0;
    virtual int n_moves() const = 0;
    virtual int n_acts() const = 0;
    virtual int max_hp() const = 0;
    virtual int hp() const = 0;
    virtual void set_hp(int hp) = 0;
    virtual int inc_moves(int inc) = 0;
    virtual bool has_cover(unit &from) const = 0;
    virtual bool is_adjacent_to_ally() const = 0;
    bool is_isolated() const { return !is_adjacent_to_ally(); }
    // if to_who is nullptr, use the self as the target
    virtual int rip_apart_for(const unit *to_who = nullptr) const = 0;
    // return if rip_apart is x or more
    virtual bool rip_apart_for(int x, const unit *to_who = nullptr) const = 0;
    // most effects doesn't affect walls, but some abilities require to know what did they hit, wall or not
    virtual bool is_wall() const = 0;
    virtual void may_treat_token_a_as_b(token_type a, token_type b) = 0;

    bool is_curseproof() const { return trait(TRAIT_CURSEPROOF) > 0; }
};


struct token
{
    virtual ~token() = default;
    virtual int count() const = 0;
    virtual token_type type() const = 0;
    virtual bool is_positive() const = 0;
    virtual bool is_negative() const = 0;
    virtual bool is_removable() const = 0;
    virtual unit &host() const = 0;

    bool passes_filter(select_token_filter) const;
};


struct combat
{
    virtual ~combat() = default;

    virtual trigger_type trigger() const = 0;

    virtual unit &self() = 0;
    virtual unit &activated() = 0;
    // splits the turn to the atomic actions, that can't be interrupted
    virtual bool then() = 0;

    virtual action_result action_resolved(action_resolved st = action_resolved::COMPLETELY) = 0;
    virtual action_result action_prevented(action_prevented why) = 0;
    virtual action_result action_failed(action_failed why = action_failed::PLAYER_IGNORED_MUST_SELECT) = 0;
    #ifndef NDEBUG
    virtual action_result action_unimplemented() = 0;
    virtual action_result action_unreachable() = 0;
    #endif

    virtual optional<int> player_must_select_roll(const list<int> &dice_rolls) = 0;
    virtual optional<armor> player_must_select_armor(const list<armor> &armors) = 0;
    virtual optional<take_action> player_must_select_action(const list<take_action> &actions) = 0;
    virtual optional<int> player_must_select_token_count(int up_to_x) = 0;
    virtual optional<int> player_must_select_corpse_count(int up_to_x) = 0;
    virtual optional<token_type> player_may_select_token_type(const list<token_type> &token_types) = 0;
    virtual optional<token_type> player_must_select_token_type(const list<token_type> &token_types) = 0;
    virtual token *player_may_select_token(const list<token *> &tokens, int filter = SELECT_TOKEN_ANY) = 0;
    virtual token *player_must_select_token(const list<token *> &tokens, int filter = SELECT_TOKEN_ANY) = 0;
    virtual list<token *> player_must_select_tokens(const list<token *> &tokens, int up_tp, int filter = SELECT_TOKEN_ANY) = 0;
    virtual unit *player_may_select_unit(const list<unit *> &units, const list<unit *> &exclude = {}) = 0;
    virtual unit *player_must_select_unit(const list<unit *> &units, const list<unit *> &exclude = {}) = 0;
    virtual list<unit *> player_must_select_units(const list<unit *> &units, int min, int max) = 0;
    virtual list<unit *> player_must_select_infect(unit &from) = 0;
    virtual list<unit *> player_must_select_line(int, list<map_space *>*poses = nullptr) = 0;
    virtual map_space *player_must_select_space(const map_space *, int range, select_space_filter filter = SELECT_SPACE_ANY) = 0;
    virtual map_space *player_must_select_space(const map_space *, int min, int max, select_space_filter filter = SELECT_SPACE_ANY) = 0;
    virtual map_space *player_must_select_space(const list<map_space *> &spaces, select_space_filter filter = SELECT_SPACE_ANY) = 0;
    virtual map_space *player_must_select_any_space(select_space_filter filter = SELECT_SPACE_ANY) = 0;
    virtual list<map_space *> player_must_select_spaces(const map_space *, int up_to, int min, int max, select_space_filter filter = SELECT_SPACE_ANY) = 0;
    virtual optional<direction> player_must_select_direction() = 0;
    virtual bool player_may_take_action(take_action) = 0;
    virtual bool player_may_spend_soul(int x) = 0;
    virtual int player_roll_d6(unit &who, roll_tag tags = ROLL_TAG_NONE, int extra_mod = 0) = 0;

    virtual int effect_gradations(int d6, const map<int, int> &treshold_to_result = {}) const = 0;
    virtual bool is_headshot(int d6) const = 0;
    // how much unit slots were spent on pre-combat buying phase on the exact unit
    virtual int player_unit_slots_spent_on(unit_faction, unit_type) const = 0;

    virtual list<unit *> units_in_range(const map_space &, int min, int max, select_unit_filter f = SELECT_UNIT_ANY) const = 0;

    virtual bool is_hit(unit &target, int d6) const = 0;
    virtual list<map_space *> unit_move_again(unit &, movement_tags extra_tags = MOVEMENT_DEFAULT) = 0;
    // TODO: may unit trigger something on step and die? then it should be [[no_discard]] bool unit_step
    virtual void unit_step(unit &, int range = 1, movement_tags tags = MOVEMENT_DEFAULT) = 0;
    virtual void slay(unit &) = 0;
    virtual void obliterate(unit &) = 0;
    virtual int inc_corpse(const map_space &, int x = 0) = 0;
    virtual unit &copy_unit(unit &, const map_space &new_pos) = 0;
    virtual void swap_unit_pos(unit &, unit &, movement_tags tags = MOVEMENT_TELEPORT) = 0;
    virtual void swap_unit_pos(unit &, map_space &, movement_tags tags = MOVEMENT_TELEPORT) = 0;
    virtual void summon(const map_space &, void(*unit)(unit_card &)) = 0;
    virtual int inc_soul(int x) = 0;

    // TODO: check how ammo goblin choose what to reload? or does it reload everything?
    // TODO: add TRAIT_HOT_CLIP support
    // must inc_counter by -1 (because exists abilities w/ 2+ reloads needed)
    virtual void reload(unit &t, trait_id c = TRAIT_RELOAD_CHOOSE);

    virtual void trigger_deathburst(unit &t) = 0;

    virtual int round() const = 0;
    virtual bool round(int x) const { return round() >= x; }
    virtual bool round_even() const { return !(round() % 2); }

    virtual void mutate(unit &unit) { unit.gain_token(TOKEN_MUTATION, +1); }
    virtual int player_may_spare_parts(unit &) = 0;
};


using action = action_result(*)(combat &);


struct unit_card
{
    virtual ~unit_card() = default;
    virtual void set_faction_type(unit_faction faction, unit_type type) = 0;
    virtual void set_stats(int mv, int hp, int df, armor arm) = 0;

    virtual void add_upgrade(upgrade_id) = 0;

    virtual void add_trait(int trigger, action) = 0;
    virtual void add_bonus_trait(int trigger, action) = 0;

    virtual void add_act_ability(action) = 0;
    virtual void add_upgrade_act_ability(action) = 0;

    virtual void add_soul_ability(int trigger, action) = 0;
    virtual void add_upgrade_soul_ability(int trigger, action) = 0;
};


using set_unit_card = void(*)(unit_card &);


struct faction
{
    virtual ~faction() = default;
    virtual void add_unit(set_unit_card);
};


using set_faction = void(*)(faction &);


// *** ABILITIES AND TRAITS ***

// While adjacent to an ally, gain +1D on attacks.
action_result formation(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Once used, a unit cannot use any ability with this tag until it reloads. To reload, sacrifice a MOVE. Other abilities may allow a reload for free.
action_result reload(combat &c, trait_id counter)
{
    int moves_left = c.self().inc_moves(0);
    if (!moves_left)
        return c.action_prevented(action_prevented::RELOAD);

    if (!c.self().trait(counter))
        return c.action_prevented(action_prevented::NO_TARGET);

    c.self().inc_moves(-1);
    c.reload(c.self(), counter);
    return c.action_resolved();
}


// May be used as a cover object by allies.
action_result siege_shield(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// If this unit ends its turn with no foes adjacent, this unit becomes curseproof and all attacks against it gain -1D. Both effects last until the start of its next turn.
action_result active_camo(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// May consume one adjacent corpse before or after ACTing to reload and gain 1 strength.
action_result scavenge_ammo(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_SCAVENGE_AMMO))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    if (c.trigger() == TRIGGER_BEFORE_ACT)
        c.self().set_trait(TRAIT_SCAVENGE_AMMO_AVAILABLE, 1);
    bool use = c.self().trait(TRAIT_SCAVENGE_AMMO_AVAILABLE);
    if (c.trigger() == TRIGGER_AFTER_ACT)
        c.self().set_trait(TRAIT_SCAVENGE_AMMO_AVAILABLE, 0);

    if (!use)
        return c.action_prevented(action_prevented::ALREADY_USED);

    if (!c.self().corpses_in_range(0, 1))
        return c.action_prevented(action_prevented::NO_TARGET);

    map_space *p = c.player_must_select_space(c.self().space(), 1, SELECT_SPACE_CORPSES);
    if (!p)
        return c.action_failed();

    c.inc_corpse(*p, -1);
    c.reload(c.self());
    c.self().gain_token(TOKEN_STRENGTH, +1);
    c.self().set_trait(TRAIT_SCAVENGE_AMMO_AVAILABLE, 0);
    return c.action_resolved();
}


// Attack, Range 2-3, reload. On hit: 1 piercing damage. Headshot: inflict 1 vulnerable.
action_result ol45(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD))
        return reload(c, TRAIT_RELOAD);

    bool brace = c.self().has_upgrade(UPGRADE_BRACE);
    select_unit_filter f = brace ? SELECT_UNIT_IF_COVER_MAX_RANGE_1 : SELECT_UNIT_ANY;
    list<unit *> us = c.self().units_in_range(2, 3, f);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().set_trait(TRAIT_RELOAD, 1);

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, enum_or(DAMAGE_PIERCING, DAMAGE_PHYSICAL), &c.self());
    if (c.is_headshot(d6))
        u->gain_token(TOKEN_VULNERABLE, +1);
    if (brace)
        u->push(c.self(), 1);

    if (c.self().has_upgrade(UPGRADE_TACTICAL_RELOAD) && c.is_headshot(d6)) {
        c.reload(c.self());
        c.self().gain_token(TOKEN_STRENGTH, +1);
    }
    return c.action_resolved();
    return c.action_resolved();
}


// Push, melee. Effect: Push 1 and (3+) inflict 1 vulnerable.
action_result baton(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->push(c.self(), 1);

    int d6 = c.player_roll_d6(c.self());
    if (d6 >= 3)
        u->gain_token(TOKEN_VULNERABLE, +1);
    return c.action_resolved();
}


// Attack, melee, push. On hit: 2 damage and push 1. Headshot: push 2.
action_result skull_crack(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Range 2-3, reload, splash. Effect: 1 fire damage and 1 vulnerable, then Splash (target): push 1 away from target.
action_result flashbang(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD))
        return reload(c, TRAIT_RELOAD);
    return c.action_resolved();
}


// Self. Effect: Until start of this unit's next turn, this unit cannot MOVE, step, or be pushed or pulled, gains cover, and grants cover to adjacent allies from all directions.
action_result shieldwall(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Range 3. Effect: A unit in range reloads and gains one, (6+) or two strength.
action_result regurgitate_ammo(combat &c)
{
    if (c.self().has_upgrade(UPGRADE_HOT_CHAMBER)) {
        list<unit *> us = c.self().units_in_range(3, enum_or(SELECT_UNIT_MULTIPLE_ADJACENT_TARGETS, SELECT_UNIT_NO_WALL));
        if (us.empty())
            return c.action_prevented(action_prevented::NO_TARGET);

        us = c.player_must_select_units(us, 1, 2);
        for (unit *u : us)
            c.reload(*u);
        return c.action_resolved();
    }

    list<unit *> us = c.self().units_in_range(1, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self());
    int x = c.effect_gradations(d6, {{0, 1}, {6, 2}});

    c.reload(*u);
    u->gain_token(TOKEN_STRENGTH, x);
    return c.action_resolved();
}


// Curse, Range 1-3. Effect: Unit takes 1 damage after any ACT ability resolves that pushes or pulls them. Lasts until end of this unit's next turn or until this unit has taken 3 damage this way.
action_result bone_shards(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, enum_or(SELECT_UNIT_NO_WALL, SELECT_UNIT_FOR_CURSE));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->set_trait(TRAIT_BONE_SHARDS, 3);
    u->set_trait_after(TRAIT_BONE_SHARDS, 0, TRIGGER_TURN_END, 1);

    if (c.self().has_upgrade(UPGRADE_VOMIT_BULLETS)) {
        int d6 = c.player_roll_d6(c.self());
        int n = c.effect_gradations(d6, {{0, 0}, {4, 1}, {6, 1}});
        while (n--)
            u->push(c.self(), 1);
    }
    return c.action_resolved();
}


// Self. Effect: Step 1, then Splash (self): 1 fire damage, push 1, and inflict vulnerable, then self is obliterated.
action_result destructive_glee(combat &c)
{
    int d6 = 1;
    if (c.self().has_upgrade(UPGRADE_NAPALM_INJECTOR))
        d6 = c.player_roll_d6(c.self());

    c.unit_step(c.self(), 1);
    for (unit *u : c.self().units_in_range(1, 1)) {
        int dmg = d6 >= 4 ? 2 : 1;
        u->take_damage(dmg, DAMAGE_FIRE, &c.self());
        u->push(c.self(), 1);
        int x = d6 >= 6 ? 2 : 1;
        u->gain_token(TOKEN_VULNERABLE, +x);
    }
    c.obliterate(c.self());
    return c.action_resolved();
}


// Attack, Range 2-4, reload. On hit: 2 damage. Headshot: and inflict 1 vulnerable.
action_result snipe(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD))
        return reload(c, TRAIT_RELOAD);

    bool gun = c.self().trait(TRAIT_TRANSFORM_TO_GUN);
    int max_range = gun ? 6 : 4;
    list<unit *> us = c.self().units_in_range(2, max_range);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().set_trait(TRAIT_RELOAD, 1);

    int mod = 0;
    if (c.self().has_upgrade(UPGRADE_EXTENDED_BARREL))
        mod = c.self().has_cover(*u) ? +1 : -1;

    roll_tag tags = gun ? enum_or(ROLL_TAG_ATTACK, ROLL_TAG_IGNORE_COVER) : ROLL_TAG_ATTACK;
    int d6 = c.player_roll_d6(c.self(), tags, mod);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    damage_type type = gun ? enum_or(DAMAGE_PIERCING, DAMAGE_PHYSICAL) : DAMAGE_PHYSICAL;
    u->take_damage(2, type, &c.self());
    if (c.is_headshot(d6))
        u->gain_token(TOKEN_VULNERABLE, +1);
    if (c.self().has_upgrade(UPGRADE_CALIBER_UP)) {
        int x = c.is_headshot(d6) ? 2 : 1;
        u->push(c.self(), x);
    }
    return c.action_resolved();
    return c.action_resolved();
}


// Self. Effect: Unit becomes unable to MOVE or step, but ranged abilities gain +2 maximum range and ignore armor and cover. It can end this effect by sacrificing a MOVE, but if it does, it loses all associated effects.
action_result transform_to_gun(combat &c)
{
    if (c.self().trait(TRAIT_TRANSFORM_TO_GUN)) {
        int moves_left = c.self().inc_moves(0);
        if (!moves_left)
            return c.action_prevented(action_prevented::NO_MOVE);

        c.self().inc_moves(-1);
        c.self().set_trait(TRAIT_TRANSFORM_TO_GUN, 0);
        c.self().inc_trait(TRAIT_UNABLE_TO_MOVE, -1);
        c.self().inc_trait(TRAIT_UNABLE_TO_STEP, -1);

        if (c.self().has_upgrade(UPGRADE_CLAW_PITONS)) {
            c.self().inc_trait(TRAIT_IMMUNE_TO_PUSH, -1);
            c.self().inc_trait(TRAIT_IMMUNE_TO_PULL, -1);
        }
        return c.action_resolved();
    }

    c.self().set_trait(TRAIT_TRANSFORM_TO_GUN, 1);
    c.self().inc_trait(TRAIT_UNABLE_TO_MOVE, +1);
    c.self().inc_trait(TRAIT_UNABLE_TO_STEP, +1);

    if (c.self().has_upgrade(UPGRADE_CLAW_PITONS)) {
        c.self().inc_trait(TRAIT_IMMUNE_TO_PUSH, +1);
        c.self().inc_trait(TRAIT_IMMUNE_TO_PULL, +1);
    }
    return c.action_resolved();
}


// Curse, Range 2-4. Effect: Inflict 2 vulnerable, (4+): 3 vulnerable, (6+): Remove any vitality first.
action_result deathmark(combat &c)
{
    bool gun = c.self().trait(TRAIT_TRANSFORM_TO_GUN);
    int max_range = gun ? 6 : 4;
    list<unit *> us = c.self().units_in_range(2, max_range, SELECT_UNIT_FOR_CURSE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    roll_tag tags = gun ? ROLL_TAG_IGNORE_COVER : ROLL_TAG_NONE;
    int d6 = c.player_roll_d6(c.self(), tags);
    int x = d6 >= 4 ? 3 : 2;
    token *t = u->find_token(TOKEN_VITALITY);
    if (d6 >= 6 && t)
        u->remove_token(t->type(), t->count());

    u->gain_token(TOKEN_VULNERABLE, x);
    return c.action_resolved();
}


// Self, push. Effect: Step 1, then splash (self) push foes 1, (4+) push foes 2. May move into and destroy walls with this step.
action_result juggernaut(combat &c)
{
    if (c.self().has_upgrade(UPGRADE_FORTIFY)) {
        for (token *t : c.self().tokens()) {
            if (t->is_negative())
                c.self().remove_token(t->type(), t->count());
        }
    }

    c.unit_step(c.self(), MOVEMENT_DESTROY_WALLS);

    list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_NO_ALLY);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    int d6 = c.player_roll_d6(c.self());
    int push = c.effect_gradations(d6, {{0, 1}, {5, 2}});
    for (unit *u : us)
        u->push(c.self(), push);
    return c.action_resolved();
}


// Attack, Range 3-4, reload, charge. On hit: 1 damage then Splash: 1 fire damage. Headshot: +1 damage on main target.
action_result mortar(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD))
        return reload(c, TRAIT_RELOAD);

    if (c.round() == 1)
        return c.action_prevented(action_prevented::CHARGE);

    list<unit *> us = c.self().units_in_range(3, 4);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().set_trait(TRAIT_RELOAD, 1);

    bool gunner = c.self().has_upgrade(UPGRADE_GUNNER_PIVOT) && c.self().is_adjacent_to_ally();
    roll_tag tags = gunner ? enum_or(ROLL_TAG_ATTACK, ROLL_TAG_IGNORE_COVER) : ROLL_TAG_ATTACK;
    int d6 = c.player_roll_d6(c.self(), tags);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int dmg = c.is_headshot(d6) ? 2 : 1;
    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    list<unit *> splash = u->units_in_range(1, 1);
    for (unit *u : splash)
        u->take_damage(1, DAMAGE_FIRE, &c.self());
    return c.action_resolved();
}


// Line, reload. Effect: Line 5, 1 fire damage, and inflicts 1 vulnerable on the first unit in the line. Pierces through walls and can target through walls.
action_result catechism_devil_cannon(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD_2))
        return reload(c, TRAIT_RELOAD_2);
    c.self().set_trait(TRAIT_RELOAD_2, 1);

    list<map_space *> line;
    list<unit *> us = c.player_must_select_line(5, &line);

    bool heavy = c.self().has_upgrade(UPGRADE_HEAVY_CALIBER_CANNON);
    if (heavy) {
        for (map_space *p : line)
            p->set_wall(false);
    }

    bool first = true;
    for (unit *u : us) {
        u->take_damage(1, DAMAGE_FIRE, &c.self());
        if (heavy)
            u->push(c.self(), 1);
        if (first) {
            first = false;
            u->gain_token(TOKEN_VULNERABLE, +1);
        }
    }
    return c.action_resolved();
}



// Once a round, reloading grants you 1 strength
action_result hot_clip(combat &c)
{
    c.self().set_trait(TRAIT_HOT_CLIP, 1);
    return c.action_resolved();
}


action_result bone_wall(combat &c)
{
    c.self().set_trait(TRAIT_BONE_WALL, 1);
    return c.action_resolved();
}


action_result gun_kata(combat &c)
{
    c.self().set_trait(TRAIT_GUN_KATA, 1);
    return c.action_resolved();
}


action_result clutch_reload(combat &c)
{
    c.self().set_trait(TRAIT_CLUTCH_RELOAD, 1);
    return c.action_resolved();
}


// Attack, range 2-4, reload. On hit: 2 damage. On hit:(4+): One other foe in range takes 1 damage, (6+) one other foe in range takes 1 damage.
action_result akimbo(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD))
        return reload(c, TRAIT_RELOAD);

    bool bone_wall = c.self().trait(TRAIT_BONE_WALL);
    bool gun_kata = c.self().trait(TRAIT_GUN_KATA);
    int min_range = gun_kata ? 1 : 2;
    select_unit_filter f = bone_wall ? SELECT_UNIT_IF_COVER_MAX_RANGE_2 : SELECT_UNIT_ANY;
    list<unit *> us = c.self().units_in_range(min_range, 4, f);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().set_trait(TRAIT_RELOAD, 1);

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    damage_type t = DAMAGE_PHYSICAL;
    bool headshot = c.is_headshot(d6);
    if (c.self().trait(TRAIT_DEVIL_BULLET)) {
        c.self().inc_trait(TRAIT_DEVIL_BULLET, -1);
        t = enum_or(t, DAMAGE_PIERCING);
        headshot = true;
    }

    if (c.self().trait(TRAIT_CLUTCH_RELOAD) && headshot)
        c.reload(c.self());

    u->take_damage(2, DAMAGE_PHYSICAL, &c.self());
    if (gun_kata && u->is_adjacent(c.self()))
        u->push(c.self(), 1);

    d6 = c.player_roll_d6(c.self());
    f = bone_wall ? enum_or(SELECT_UNIT_IF_COVER_MAX_RANGE_2, SELECT_UNIT_NO_ALLY) : SELECT_UNIT_NO_ALLY;
    us = c.self().units_in_range(min_range, 4, f);
    us.remove(u);
    if (d6 < 4 || us.empty())
        return c.action_resolved(action_resolved::PREMATURELY);

    u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    if (gun_kata && u->is_adjacent(c.self()))
        u->push(c.self(), 1);
    us.remove(u);
    if (d6 < 6 || us.empty())
        return c.action_resolved(action_resolved::PREMATURELY);

    u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    if (gun_kata && u->is_adjacent(c.self()))
        u->push(c.self(), 1);
    return c.action_resolved();
}


// Self. Effect: Splash (self): 1 fire damage, then step 2 with free movement. You may move through foes with this step.
action_result grenade_jump(combat &c)
{
    for (unit *u : c.self().units_in_range(1, 1))
        u->take_damage(1, DAMAGE_FIRE, &c.self());

    c.unit_step(c.self(), 2, enum_or(MOVEMENT_FREE, MOVEMENT_THROUGH_FOES));
    return c.action_resolved();
}


// Self. Effect: Push all adjacent foes 1. Until the end of your next turn, you gain super armor, cover from all directions, and allied units can use you as a cover object. You cannot MOVE, step or be pushed or pulled for the same duration.
action_result armor_lock(combat &c)
{
    for (unit *u : c.self().units_in_range(1, 1)) {
        if (!u->is_ally(c.self()))
            u->push(c.self(), 1);
    }

    list<trait_id> traits = {
        TRAIT_SUPER_ARMOR,
        TRAIT_HAS_COVER_FROM_ALL_DIRECTIONS,
        TRAIT_CAN_BE_USED_AS_COVER_BY_ALLIES,
        TRAIT_UNABLE_TO_MOVE,
        TRAIT_UNABLE_TO_STEP,
        TRAIT_IMMUNE_TO_PUSH,
        TRAIT_IMMUNE_TO_PULL,
    };
    for (trait_id t : traits) {
        c.self().inc_trait(t, +1);
        c.self().inc_trait_after(t, -1, TRIGGER_TURN_END, 2);
    }
    return c.action_resolved();
}


// Attack, range 3-6, charge, reload*. On hit: 1 fire damage and 1 vulnerable. Splash (target): 1 fire damage. Headshot: +1 damage on main target. *Effect: Must reload twice before this is usable.
action_result lv4_living_cannon(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD_2))
        return reload(c, TRAIT_RELOAD_2);

    if (c.round() == 1)
        return c.action_prevented(action_prevented::CHARGE);

    bool bone_wall = c.self().trait(TRAIT_BONE_WALL);
    bool gun_kata = c.self().trait(TRAIT_GUN_KATA);
    int min_range = gun_kata ? 1 : 3;
    select_unit_filter f = bone_wall ? SELECT_UNIT_IF_COVER_MAX_RANGE_2 : SELECT_UNIT_ANY;
    list<unit *> us = c.self().units_in_range(min_range, 6, f);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().set_trait(TRAIT_RELOAD_2, 2);

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_FIRE), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    damage_type t = DAMAGE_FIRE;
    bool headshot = c.is_headshot(d6);
    if (c.self().trait(TRAIT_DEVIL_BULLET)) {
        c.self().inc_trait(TRAIT_DEVIL_BULLET, -1);
        t = enum_or(t, DAMAGE_PIERCING);
        headshot = true;
    }
    if (c.self().trait(TRAIT_CLUTCH_RELOAD) && headshot)
        c.reload(c.self());

    if (gun_kata && u->is_adjacent(c.self()))
        u->push(c.self(), 1);
    int dmg = headshot ? 2 : 1;
    u->take_damage(dmg, t, &c.self());
    u->gain_token(TOKEN_VULNERABLE, +1);

    us = u->units_in_range(1, 1);
    for (unit *u : us)
        u->take_damage(1, DAMAGE_FIRE, &c.self());
    return c.action_resolved();
}


// Attack, range 2-2, reload*. On hit: 1 damage, then 1 damage. On Hit: push 1, then push 1. Headshot: increase all push by +1. *Effect: Must reload twice before this is usable.
action_result sg88_two_barrel_shotgun(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD_3))
        return reload(c, TRAIT_RELOAD_3);

    bool bone_wall = c.self().trait(TRAIT_BONE_WALL);
    bool gun_kata = c.self().trait(TRAIT_GUN_KATA);
    int min_range = gun_kata ? 1 : 2;
    select_unit_filter f = bone_wall ? SELECT_UNIT_IF_COVER_MAX_RANGE_2 : SELECT_UNIT_ANY;
    list<unit *> us = c.self().units_in_range(min_range, 2, f);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().set_trait(TRAIT_RELOAD_3, 2);

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    damage_type t = DAMAGE_PHYSICAL;
    bool headshot = c.is_headshot(d6);
    if (c.self().trait(TRAIT_DEVIL_BULLET)) {
        c.self().inc_trait(TRAIT_DEVIL_BULLET, -1);
        t = enum_or(t, DAMAGE_PIERCING);
        headshot = true;
    }
    if (c.self().trait(TRAIT_CLUTCH_RELOAD) && headshot)
        c.reload(c.self());

    if (gun_kata && u->is_adjacent(c.self()))
        u->push(c.self(), 1);

    int push = headshot ? 2 : 1;
    u->take_damage(1, t, &c.self());
    u->push(c.self(), 1);

    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    u->take_damage(1, t, &c.self());
    u->push(c.self(), 1);
    return c.action_resolved();
}


// Attack, Range 2-4, reload. On hit: 2 damage. Headshot: this attack deals +1 base damage on hit for the rest of combat, up to a maximum of 5.
action_result unholy_sixgun(combat &c)
{
    if (c.self().trait(TRAIT_RELOAD_4))
        return reload(c, TRAIT_RELOAD_4);

    bool bone_wall = c.self().trait(TRAIT_BONE_WALL);
    bool gun_kata = c.self().trait(TRAIT_GUN_KATA);
    int min_range = gun_kata ? 1 : 2;
    select_unit_filter f = bone_wall ? SELECT_UNIT_IF_COVER_MAX_RANGE_2 : SELECT_UNIT_ANY;
    list<unit *> us = c.self().units_in_range(min_range, 2, f);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().set_trait(TRAIT_RELOAD_4, 1);

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    damage_type t = DAMAGE_PHYSICAL;
    bool headshot = c.is_headshot(d6);
    if (c.self().trait(TRAIT_DEVIL_BULLET)) {
        c.self().inc_trait(TRAIT_DEVIL_BULLET, -1);
        t = enum_or(t, DAMAGE_PIERCING);
        headshot = true;
    }
    if (c.self().trait(TRAIT_CLUTCH_RELOAD) && headshot)
        c.reload(c.self());

    if (gun_kata && u->is_adjacent(c.self()))
        u->push(c.self(), 1);

    int dmg = min(5, 2 + c.self().trait(TRAIT_UNHOLY_SIXGUN_HEADSHOTS));
    u->take_damage(dmg, t, &c.self());
    if (headshot)
        c.self().inc_trait(TRAIT_UNHOLY_SIXGUN_HEADSHOTS, +1);
    return c.action_resolved();
}

// Self. Effect: Reload. On hit, your next attack gains piercing and triggers headshot effects.
action_result devil_bullet(combat &c)
{
    c.reload(c.self());
    c.self().inc_trait(TRAIT_DEVIL_BULLET, +1);
    return c.action_resolved();
}


// Melee. Effect: Push an adjacent unit 3. Foes gain 1 vulnerable if they would be pushed into a wall or elevated space.
action_result superkick(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->push(c.self(), 3);
    // TODO: how to check if they would be pushed?
    u->gain_token(TOKEN_VULNERABLE, +1);
    return c.action_resolved();
}


// Line 4, reload. Effect: First unit in the line takes 1 piercing damage. If catching an allied unit in the line, also inflict 2 vulnerable.
action_result cross_fire(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Step. Effect: Step 1, then push an adjacent foe 1 or (5+) 2 spaces. That foe must spend a MOVE before next attacking.
action_result slide_kick(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Self. Effect: Reload and gain 1 strength (4+) then regain this unit’s ACT. This ability can’t be used again this turn.
action_result trick_reload(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Ignores adverse terrain and elevation attack and movement penalties
action_result flight(combat &c)
{
    c.self().set_trait(TRAIT_FLIGHT, 1);
    return c.action_resolved();
}


// Starts combat with 4 mutation tokens
action_result warpflesh(combat &c)
{
    c.self().gain_token(TOKEN_MUTATION, +4);
    return c.action_resolved();
}


// 2x2 unit
action_result large(combat &c)
{
    c.self().set_trait(TRAIT_IS_LARGE, 1);
    return c.action_resolved();
}


// If a unit with this trait is reduced to 0 HP, they are not slain, but instead gain 1 strength and remain at 0 HP, no matter how much damage they took. If they take any further damage at 0 HP, they are slain as normal.
action_result blood_rage(combat &c)
{
    // already activated
    if (c.self().trait(TRAIT_BLOOD_RAGE))
        return c.action_resolved();

    if (c.self().hp() > 0)
        return c.action_resolved();

    c.self().set_trait(TRAIT_BLOOD_RAGE, 1);
    c.self().set_hp(0);
    c.self().gain_token(TOKEN_STRENGTH, +1);
    return c.action_resolved();
}


// After a berserk unit ACTs, it splashes (self) 1 damage, then discards a berserk token. This damage can't be increased in any way. If this effect does not damage any other characters, it deals 1 damage to itself instead, ignoring armor. Berserk counts as a positive and a negative token.
action_result berserk(combat &c)
{
    token *t = c.self().find_token(TOKEN_BERSERK);
    if (!t)
        return c.action_resolved();

    c.self().remove_token(t->type(), 1);
    list<unit *> splash = c.self().units_in_range(1, 1);
    if (splash.empty()) {
        c.self().take_damage(1, enum_or(DAMAGE_PIERCING, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved();
    }
    for (unit *u : splash)
        u->take_damage(1, DAMAGE_CANT_BE_INCREASED, &c.self());
    return c.action_resolved();
}


// This unit may spend speed tokens as strength. May pass through units but not end their turn in their spaces.
action_result hellwheel(combat &c)
{
    c.self().may_treat_token_a_as_b(TOKEN_SPEED, TOKEN_STRENGTH);
    return c.action_unimplemented();
    return c.action_resolved();
}


// When taking damage, gain 1 berserk and 1 strength. This effect can't trigger more than once a round.
action_result steaming_rage(combat &c)
{
    if (c.trigger() == TRIGGER_ROUND_START) {
        c.self().set_trait(TRAIT_STEAMING_RAGE, 1);
        return c.action_resolved();
    }
    if (c.trigger() == TRIGGER_AFTER_DAMAGED) {
        if (!c.self().trait(TRAIT_STEAMING_RAGE))
            return c.action_prevented(action_prevented::ALREADY_USED);

        c.self().set_trait(TRAIT_STEAMING_RAGE, 0);
        c.self().gain_token(TOKEN_BERSERK, +1);
        c.self().gain_token(TOKEN_STRENGTH, +1);
        return c.action_resolved();
    }
    return c.action_resolved();
}


// Self. Effect: MOVE as far as possible in a straight line, passing through units. Then, units passed through during this movement take 1 damage.
action_result charge(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Attack, melee. On hit: 1 damage and 1 damage to another adjacent unit.
action_result cleave(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    us.remove(u);
    if (us.empty())
        return c.action_failed();

    u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    return c.action_resolved();
}


// Self. Effect: Gain 1 strength and speed (4+) and retaliation until start of this unit's next turn.
action_result rev(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Attack, melee. On hit: 1 damage, Effect: (4+): 1 damage again, (6+): 1 damage again.
action_result chainsaw(combat &c)
{
    bool swing = c.self().has_upgrade(UPGRADE_HEAVY_SWING);
    if (swing && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 1);

    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty()) {
        if (swing)
            return c.action_resolved(action_resolved::PREMATURELY);
        return c.action_prevented(action_prevented::NO_TARGET);
    }

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
    else {
        bool bloody = c.self().has_upgrade(UPGRADE_BLOODY_TEETH) && c.self().hp() == 0;
        damage_type type = bloody ? enum_or(DAMAGE_PIERCING, DAMAGE_PHYSICAL) : DAMAGE_PHYSICAL;
        int d6 = c.player_roll_d6(c.self());
        int times = c.effect_gradations(d6, {{0, 1}, {4, 2}, {6, 3}});
        while (times--) {
            u->take_damage(1, type, &c.self());
            if (!c.then())
                return c.action_resolved(action_resolved::PREMATURELY);
        }
    }

    if (swing && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 1);
    return c.action_resolved();
}


// Self. Effect: MOVE as far as possible in a straight line. Then, all units adjacent to any space of this MOVE take 1 fire damage.
action_result wild_slashes(combat &c)
{

    return c.action_resolved();
}


// Curse, Range 1-3. Effect: Unit gains 2 berserk or 2 speed.
action_result pain_frenzy(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_FOR_CURSE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    optional<token_type> tt = c.player_must_select_token_type({TOKEN_BERSERK, TOKEN_SPEED});
    if (!tt)
        return c.action_failed();

    u->gain_token(*tt, 2);
    return c.action_resolved();
}


// Curse, Range 1-3. Effect: Clear all negative tokens on self or a unit, then deal 1 piercing fire damage to them. This damage can't slay a unit.
action_result cauterize(combat &c)
{
    list<unit *> us = c.self().units_in_range(3, enum_or(SELECT_UNIT_FOR_CURSE, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int removed = 0;
    for (token *t : u->tokens()) {
        if (t->is_negative()) {
            removed += t->count();
            u->remove_token(t->type(), t->count());
        }
    }

    u->take_damage(removed, enum_or(DAMAGE_FIRE, DAMAGE_CANT_SLAY), &c.self());

    if (c.self().has_upgrade(UPGRADE_ADRENALIZE) && u->is_ally(c.self()))
        u->gain_token(TOKEN_SPEED, removed);
    return c.action_resolved();
}


// Pull, Range 2-3. Effect: Pull unit 3.
action_result meat_hook(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    bool is3 = c.self().has_upgrade(UPGRADE_FRENZY_HOOK) && c.self().hp() <= 1;
    int max = is3 ? 3 : 1;
    us = c.player_must_select_units(us, 1, max);

    for (unit *u : us)
        u->pull(c.self(), 3);
    return c.action_resolved();
}

// Attack, Melee. On hit: 1 damage. Effect: (4+) 1 damage again. (6+) 1 damage again.
action_result exfoliate(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int d6 = c.player_roll_d6(c.self());
    int times = c.effect_gradations(d6, {{0, 1}, {4, 2}, {6, 3}});
    while (times--) {
        u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
        if (!c.then())
            return c.action_resolved(action_resolved::PREMATURELY);
    }
    return c.action_resolved();
}


// Self. Unit MOVEs as far as possible in a straight line. The first unit passed through during this MOVE take 1 fire damage. Then this unit gains 2 speed.
action_result spin_out(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// At 1 hp or lower, gains super armor.
action_result machineheart(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_MACHINEHEART))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    bool was = c.self().trait(TRAIT_MACHINEHEART);
    bool will = c.self().hp() <= 1;
    if (was == will)
        return c.action_failed();

    if (will) {
        c.self().inc_trait(TRAIT_MACHINEHEART, +1);
        c.self().inc_trait(TRAIT_SUPER_ARMOR, +1);
    } else {
        c.self().inc_trait(TRAIT_MACHINEHEART, -1);
        c.self().inc_trait(TRAIT_SUPER_ARMOR, -1);
    }
    return c.action_resolved();
}


// Self. Effect: Pull all units in range 2 one space toward this unit. Then splash (self): 1 fire damage and destroy all adjacent walls.
action_result pulverize(combat &c)
{
    if (c.self().has_upgrade(UPGRADE_FUEL_INJECTORS) && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 2);

    list<unit *> us = c.self().units_in_range(1, 2);
    unit *u = c.player_must_select_unit(us);
    if (u)
        u->pull(c.self(), 2);

    us = c.self().units_in_range(1, 1);
    for (unit *u : us)
        u->take_damage(1, DAMAGE_FIRE, &c.self());

    list<map_space *>ps = c.self().spaces_in_range(1, 1);
    for (map_space *p : ps)
        p->set_wall(false);
    return c.action_resolved();
}


// Self, retaliation. Effect: Gain 1 strength and 1 berserk, (4+) then gain retaliation until the end of this unit's next turn.
action_result building_rage(combat &c)
{
    c.self().gain_token(TOKEN_STRENGTH, +1);
    c.self().gain_token(TOKEN_BERSERK, +1);

    int d6 = c.player_roll_d6(c.self());
    if (d6 >= 4) {
        c.self().inc_trait(TRAIT_RETALIATION, +1);
        c.self().inc_trait_after(TRAIT_RETALIATION, -1, TRIGGER_TURN_END, 2);
    }
    return c.action_resolved();
}


// Attack, melee. On hit: 2 damage. Deal 1 damage again if this unit has 3 or less HP, then deal 1 damage again if it is at 1 HP or less.
action_result rip_and_tear(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int d6 = c.player_roll_d6(c.self());
    damage_type t = c.self().has_upgrade(UPGRADE_BIFURCATE) ? enum_or(DAMAGE_PHYSICAL, DAMAGE_OBLITERATE_ON_SLAY) : DAMAGE_PHYSICAL;
    u->take_damage(2, t, &c.self());
    if (!c.then() || c.self().hp() > 3)
        return c.action_resolved(action_resolved::PREMATURELY);

    u->take_damage(1, t, &c.self());
    if (!c.then() || c.self().hp() > 1)
        return c.action_resolved(action_resolved::PREMATURELY);

    u->take_damage(1, t, &c.self());
    return c.action_resolved();
}


// Melee. Effect: Tinker with an adjacent unit. Unit gains 1 berserk, then roll the effect die (1-3): Increase unit’s MV stat by +1 this battle, (4-5): increase a unit’s DF stat by +1 this battle, to a max of 6+, (6+): Increase unit’s current and max HP by 1 this battle.
action_result tinker(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Range 1-3. Effect: Self or a unit in range gains 2 speed. If target is at 1 HP or lower, splash (target) 1 fire damage.
action_result overlock(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Ally, melee. Effect: Self or an adjacent ally has their HP set to 1.
action_result restart_engine(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Units affected by plague take 1 toxic damage at the end of their turn, then that unit discards a plague token. This damage cannot be reduced or ignored in any way (by tokens or armor) but cannot slay a unit. Gargamox units don't take damage from plague and don't remove plague tokens, but can still be affected by plague.
action_result plague(combat &c)
{
    token *t = c.self().find_token(TOKEN_PLAGUE);
    if (!t)
        return c.action_failed();

    if (c.self().faction() == FACTION_GARGAMOX)
        return c.action_resolved();

    c.self().take_damage(1, enum_or(DAMAGE_TOXIC, DAMAGE_CANT_BE_REDUCED, DAMAGE_CANT_SLAY), nullptr);
    c.self().remove_token(t->type(), 1);
    return c.action_resolved();
}


// Has Deathburst: Splash (self): 1 plague and 1 toxic damage to already already plagued foes.
action_result toxic_revenge(combat &c)
{
    bool push = c.self().has_upgrade(UPGRADE_BLOAT) && c.self().find_token(TOKEN_PLAGUE);
    list<unit *> us = c.self().units_in_range(1, 1);
    for (unit *u : us) {
        bool dmg = u->find_token(TOKEN_PLAGUE) && !u->is_ally(c.self());
        u->gain_token(TOKEN_PLAGUE, +1);
        if (dmg)
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
        if (push)
            u->push(c.self(), 1);
    }
    if (c.self().has_upgrade(UPGRADE_AFTERMATH))
        c.self().inc_trait(TRAIT_LEAVE_HAZARD_INSTEAD_OF_CORPSE, +1);
    return c.action_resolved();
}

// Immune to hazards. May treat plague tokens as strength.
action_result plaguebearer(combat &c)
{
    c.self().set_trait(TRAIT_IMMUNE_TO_HAZARDS, 1);
    c.self().may_treat_token_a_as_b(TOKEN_PLAGUE, TOKEN_STRENGTH);
    return c.action_resolved();
}

// Has Deathburst: Splash (self): Remove any 1 token from all units in the area.
action_result deathwash(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    for (unit *u : us) {
        if (!u->n_tokens())
            continue;
        token *t = c.player_must_select_token(u->tokens());
        if (!t)
            return c.action_failed();
        u->remove_token(t->type(), 1);
    }
    return c.action_resolved();
}

// Has Deathburst: create a hazard under a number of units in range 2 equal to the number of plague tokens in this unit.
action_result swarm_release(combat &c)
{
    int n = c.self().n_tokens(SELECT_TOKEN_PLAGUE);
    if (!n)
        return c.action_prevented(action_prevented::NO_TOKENS);

    list<unit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    us = c.player_must_select_units(us, min(n, (int)us.size()), n);
    for (unit *u : us)
        u->space()->set_hazard(true);
    return c.action_resolved();
}


// Immune to hazards.
action_result toxic_avenger_immune_to_hazards(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_TOXIC_AVENGER))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    c.self().inc_trait(TRAIT_IMMUNE_TO_HAZARDS, +1);
    return c.action_resolved();
}


// While standing in a hazard, gain 1 plague at turn start.
action_result toxic_avenger(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_TOXIC_AVENGER))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    if (c.self().space()->is_hazard())
        c.self().gain_token(TOKEN_PLAGUE, +1);
    return c.action_resolved();
}


// Has Deathburst: Remove up to three plague tokens on this unit, then splash (self): 1 toxic damage, once, per plague token removed.
action_result vile_rupture(combat &c)
{
    int n = c.self().n_tokens(SELECT_TOKEN_PLAGUE);
    if (!n)
        return c.action_prevented(action_prevented::NO_TOKENS);

    optional<int> removed = c.player_must_select_token_count(min(3, n));
    if (!removed)
        return c.action_failed();

    int times = *removed;
    while (times--) {
        c.self().remove_token(TOKEN_PLAGUE, 1);
        for (unit *u : c.self().units_in_range(1, 1))
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
    }
    return c.action_resolved();
}

// Has deathburst: summon a slimelet in a free adjacent space. Unlike other summons, the slimelet can be freely activated this round.
action_result burst_guts(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}

// Follows all summon rules, but can act this round.
action_result spit_up(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}

// Can be activated two at a time.
action_result summoned_thrall(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}

// Range 2-3. Effect: Pull unit one. This gains +1 range and pull for each plague token on this unit.
action_result pseudopod(combat &c)
{
    int n = c.self().n_tokens(SELECT_TOKEN_PLAGUE);
    list<unit *> us = c.self().units_in_range(2, 3 + n);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->pull(c.self(), 1 + n);

    if (c.self().has_upgrade(UPGRADE_TENTACLE_WHIP) && u->find_token(TOKEN_PLAGUE))
        u->gain_token(TOKEN_SLOW, +1);
    return c.action_resolved();
}

// Attack, melee. On hit: 1 damage and 1 plague.
action_result shamble(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    u->gain_token(TOKEN_PLAGUE, +1);
    return c.action_resolved();
}


// splash (self): 1 plague, and allies gain 1 strength.
action_result invigorating_viscera(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_INVIGORATING_VISCERA))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    list<unit *> us = c.self().units_in_range(1, 1);
    for (unit *u : us) {
        u->gain_token(TOKEN_PLAGUE, +1);
        if (u->is_ally(c.self()))
            u->gain_token(TOKEN_STRENGTH, +1);
    }
    return c.action_resolved();
}


// Curseproof and cover from all directions while standing in a hazard.
action_result smog_shroud(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_SMOG_SHROUD))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    bool was = c.self().trait(TRAIT_SMOG_SHROUD);
    bool will = c.self().space()->is_hazard();
    if (was == will)
        return c.action_resolved();

    if (will) {
        c.self().inc_trait(TRAIT_SMOG_SHROUD, +1);
        c.self().inc_trait(TRAIT_CURSEPROOF, +1);
        c.self().inc_trait(TRAIT_HAS_COVER_FROM_ALL_DIRECTIONS, +1);
    } else {
        c.self().inc_trait(TRAIT_SMOG_SHROUD, -1);
        c.self().inc_trait(TRAIT_CURSEPROOF, -1);
        c.self().inc_trait(TRAIT_HAS_COVER_FROM_ALL_DIRECTIONS, -1);
    }
    return c.action_resolved();
}


// Range 1-3. Effect: Create a hazard in a free space in range 3 and inflict plague on an adjacent target (4+) all adjacent targets.
action_result pustulate(combat &c)
{
    map_space *p = c.player_must_select_space(c.self().space(), 3, SELECT_SPACE_FREE);
    if (!p)
        return c.action_failed();

    p->set_hazard(true);

    int d6 = c.player_roll_d6(c.self());
    list<unit *> us = c.units_in_range(*p, 1, 1);
    if (us.empty())
        return c.action_resolved(action_resolved::PREMATURELY);

    if (d6 < 4) {
        unit *u = c.player_must_select_unit(us);
        if (!u)
            return c.action_failed();
        us = {u};
    }

    for (unit *u : us)
        u->gain_token(TOKEN_PLAGUE, +1);
    return c.action_resolved();
}

// Line 4. Line: 1 plague. Already plagued foes gain 1 slow.
action_result vomitous_mass(combat &c)
{
    list<unit *> us = c.player_must_select_line(4);
    bool dmg = c.self().has_upgrade(UPGRADE_CATALYZE);

    for (unit *u : us) {
        bool slow = u->find_token(TOKEN_PLAGUE);
        u->gain_token(TOKEN_PLAGUE, +1);
        if (slow)
            u->gain_token(TOKEN_SLOW, +1);
        if (dmg && u->space()->is_hazard())
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
    }
    return c.action_resolved();
}

// Attack, melee. On hit: 1 toxic damage and infect: 1 toxic damage.
action_result rotblade(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_TOXIC), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_TOXIC, &c.self());

    us = c.player_must_select_infect(*u);
    for (unit *u : us)
        u->take_damage(1, DAMAGE_TOXIC, &c.self());

    return c.action_resolved();
    return c.action_resolved();
}

// Curse, Range 1-3. Effect: Unit permanently gains death burst: splash (self): 1 toxic damage and 1 plague. This effect cannot stack with itself but stacks with other death burst effects.
action_result suppurate(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_FOR_CURSE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->inc_trait(TRAIT_SUPPURATE, +1);

    if (c.self().has_upgrade(UPGRADE_ACID_BLOOD))
        u->inc_trait(TRAIT_LEAVE_HAZARD_INSTEAD_OF_CORPSE, +1);
    return c.action_resolved();
}

// Curse, Range 1-3. Effect: Strip all plague tokens from a unit in range. Foes take 1 toxic damage. Then, they gain 1 slow per token removed. Allies gain 1 strength per token removed.
action_result evolve_strain(combat &c)
{
    list<unit *> us;
    bool all = false;
    if (c.self().has_upgrade(UPGRADE_PANDEMIC)) {
        all = false;
    } else {
        us = c.self().units_in_range(1, 3, enum_or(SELECT_UNIT_FOR_CURSE, SELECT_UNIT_WITH_MUTATION_TOKENS));
        if (us.empty())
            return c.action_prevented(action_prevented::NO_TARGET);

        unit *u = c.player_must_select_unit(us);
        if (!u)
            return c.action_failed();
        us = {u};
        all = true;
    }

    for (unit *u : us) {
        if (!u->is_ally(c.self()))
            u->take_damage(1, DAMAGE_TOXIC, &c.self());

        token *t = u->find_token(TOKEN_PLAGUE);
        if (!t)
            continue;

        int removed = all ? t->count() : 1;
        u->remove_token(t->type(), removed);
        u->gain_token(u->is_ally(c.self()) ? TOKEN_STRENGTH : TOKEN_SLOW, removed);
    }
    return c.action_resolved();
}

// Range 1-3. Effect: Unit triggers deathburst without being slain.
action_result swell_with_corruption(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_WITH_DEATHBURST);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    bool twice = c.self().has_upgrade(UPGRADE_MASSIVE_SWELL) && c.player_roll_d6(c.self()) >= 4;
    int times = twice ? 2 : 1;
    while (times--)
        c.trigger_deathburst(*u);
    return c.action_resolved();
}

// Range 2-4. Effect: Create one, (3+) two, or (5+) three hazards in range. May spend a plague token on self to re-roll the effect die once.
action_result propagate_swarm(combat &c)
{
    int d6 = c.player_roll_d6(c.self());
    if (c.self().find_token(TOKEN_PLAGUE) && c.player_may_take_action(TAKE_ACTION_PROPAGATE_SWARM)) {
        d6 = max(d6, c.player_roll_d6(c.self()));
        c.self().remove_token(TOKEN_PLAGUE, 1);
    }

    int n = c.effect_gradations(d6, {{0, 1}, {3, 2}, {5, 3}});
    while (n--) {
        map_space *p = c.player_must_select_space(c.self().space(), 3);
        if (!p)
            return c.action_failed();
        p->set_hazard(true);
    }
    return c.action_resolved();
}

// Attack, Range 2-4. On hit: 2 damage and pull 1. Infect: 1 damage and pull 1. Effect: Allies take no damage.
action_result driving_vermin(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (c.self().has_upgrade(UPGRADE_DEFILER))
        u->space()->set_hazard(true);

    optional<int> removed = 0;
    if (c.self().has_upgrade(UPGRADE_SWARM_FEED)) {
        int max = c.self().n_tokens(SELECT_TOKEN_PLAGUE);
        removed = c.player_must_select_token_count(max);
        if (!removed)
            return c.action_resolved();
        c.self().remove_token(TOKEN_PLAGUE, *removed);
    }

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK, *removed))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    if (!u->is_ally(c.self()))
        u->take_damage(2, DAMAGE_PHYSICAL, &c.self());
    u->pull(c.self(), 1);

    us = c.player_must_select_infect(*u);
    for (unit *u : us) {
        if (!u->is_ally(c.self()))
            u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
        u->pull(c.self(), 1);
    }

    return c.action_resolved();
    return c.action_resolved();
}

// Self. Effect: Inflict two, (5+) or three plague tokens on self, then may inflict 1 slow on an adjacent unit per plague token gained.
action_result percolate(combat &c)
{
    int d6 = c.player_roll_d6(c.self());
    int n = c.effect_gradations(d6, {{0, 2}, {5, 3}});
    c.self().gain_token(TOKEN_PLAGUE, n);

    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->gain_token(TOKEN_SLOW, n);

    return c.action_resolved();
    return c.action_resolved();
}

// Self. Effect: MOVE with free movement in a straight line. During this move, is immune to hazards and may move through walls and units. Any foe this unit passes through takes 1 toxic damage and this unit may pass off any plague tokens on this unit to affected units.
action_result surge(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}

// Attack, Range 2-3. On hit: 1 toxic damage (3+) and create a hazard under target. Ignores armor against units already in hazards.
action_result toxic_spit(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}

// Range 1-2. Effect: Push target unit 1 space, then create one hazard in a space they vacated. If this unit is plagued, push 2 instead.
action_result projectile_vomit(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Any unit affected by at least one Doom token takes 4 devil damage at the end of round 4. A unit can only clear a Doom token by slaying another unit and they cannot be cleared in any other way.
action_result doom(combat &c)
{
    token *t = c.self().find_token(TOKEN_DOOM);
    if (t)
        return c.action_resolved(action_resolved::PREMATURELY);

    if (c.round() == 4)
        c.self().take_damage(4, DAMAGE_DEVIL, nullptr);
    return c.action_resolved();
}


// When slain, does not remove Doom, and (5+) Dooms slayer.
action_result inverted_crucifix(combat &c)
{
    c.self().set_trait(TRAIT_INVERTED_CRUCIFIX, 1);
    return c.action_resolved();
}


// Has free movement while adjacent to a wall.
action_result slither(combat &c)
{
    list<map_space *>ps = c.self().spaces_in_range(1, 1);
    bool will = false;
    for (const map_space *p : ps)
        will |= p->is_wall();

    bool was = c.self().trait(TRAIT_SLITHER);
    if (was == will)
        return c.action_resolved();

    if (will) {
        c.self().inc_trait(TRAIT_SLITHER, +1);
        c.self().inc_trait(TRAIT_MOVEMENT_FREE, +1);
    } else {
        c.self().inc_trait(TRAIT_SLITHER, -1);
        c.self().inc_trait(TRAIT_MOVEMENT_FREE, -1);
    }
    return c.action_resolved();
}


// When MOVEing a second time or more in a turn, can remove this unit from the battlefield and place it any free space in range 4.
action_result teleport(combat &c)
{
    if (!c.self().n_moves()) // first MOVE
        return c.action_prevented(action_prevented::CONDITION_UNSATISFIED);

    if (!c.player_may_take_action(TAKE_ACTION_TELEPORT))
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

    map_space *p = c.self().space();
    c.self().inc_moves(-1);
    c.self().teleport(4);

    if (c.self().has_upgrade(UPGRADE_SOUL_FROST))
        p->set_adverse_terrain(true);
    return c.action_resolved();
}


// Before MOVEing, may remove and place any adjacent walls in any other free adjacent spaces.
action_result labyrinth_master(combat &c)
{
    if (c.self().spaces_in_range(1, 1, SELECT_SPACE_WALLS).empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    // nowhere to put
    if (c.self().spaces_in_range(1, 1, SELECT_SPACE_FREE).empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_take_action(TAKE_ACTION_LABYRINTH_MASTER))
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

    map_space *src = c.player_must_select_space(c.self().space(), 1, SELECT_SPACE_WALLS);
    if (!src)
        return c.action_failed();

    map_space *dst = c.player_must_select_space(c.self().space(), 1, SELECT_SPACE_NO_WALLS);
    if (!dst)
        return c.action_failed();

    src->set_wall(false);
    dst->set_wall(true);
    return c.action_resolved();
}


// The Geist is bound to the stone of Anzenmezzeron. It cannot MOVE or step. At the start of its turn, teleport it to any space adjacent to a wall. At the start of combat, place a wall in a free adjacent space to it.
action_result buried_alive(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// If not adjacent to a wall, the geist loses all armor.
action_result tomb_bound(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// May move through walls.
action_result squirm(combat &c)
{
    if (c.self().has_upgrade(UPGRADE_SQUIRM))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    c.self().inc_trait(TRAIT_MOVEMENT_THROUGH_WALLS, +1);
    return c.action_resolved();
}


// Range 2-4. Effect: Pull 1. Ignores line of sight.
action_result beckon(combat &c)
{
    select_unit_filter f = c.self().has_upgrade(UPGRADE_DEAD_GRASP) ? enum_or(SELECT_UNIT_IGNORE_LINE_OF_SIGHT, SELECT_UNIT_IF_ISOLATED_MAX_RANGE_INF) : SELECT_UNIT_IGNORE_LINE_OF_SIGHT;
    list<unit *> us = c.self().units_in_range(2, 4, f);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->pull(c.self(), 1);

    if (c.self().has_upgrade(UPGRADE_IMPENDING_DEATH) && u->is_isolated())
        u->gain_token(TOKEN_WEAK, +1);
    return c.action_resolved();
}

// Attack, melee. On hit: 1 damage and (4+) Dooms unit.
action_result shudder(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    int d6 = c.player_roll_d6(c.self());
    if (d6 >= 4)
        u->gain_token(TOKEN_DOOM, +1);
    return c.action_resolved();
}


// May step 1 before ACTing, or step 2 and clear a token if adjacent to a wall.
action_result leap(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_LEAP))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    list<map_space *>ps = c.self().spaces_in_range(1, 1);
    bool wall = false;
    for (const map_space *p : ps)
        wall |= p->is_wall();

    if (!c.player_may_take_action(TAKE_ACTION_LEAP))
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

    c.unit_step(c.self(), wall ? 2 : 1);

    if (!c.self().n_tokens(SELECT_TOKEN_REMOVABLE))
        return c.action_resolved(action_resolved::PREMATURELY);

    token *t = c.player_may_select_token(c.self().tokens(), SELECT_TOKEN_REMOVABLE);
    if (!t)
        return c.action_failed();

    c.self().remove_token(t->type(), 1);
    return c.action_resolved();
}


// Melee, Range 1-2. Effect: Create a wall, (3+) then create adverse terrain (5+) then create adverse terrain again.
action_result tombraiser(combat &c)
{
    int range = c.self().has_upgrade(UPGRADE_FOUL_MONUMENTS) ? 4 : 2;
    map_space *p = c.player_must_select_space(c.self().space(), 1, range, SELECT_SPACE_UNIT);
    if (!p)
        return c.action_failed();
    p->set_wall(true);

    int d6 = c.player_roll_d6(c.self());
    int n = c.effect_gradations(d6, {{0, 0}, {3, 1}, {5, 2}});
    while (n--) {
        p = c.player_must_select_space(c.self().space(), 1, 2);
        if (p)
            p->set_adverse_terrain(true);
    }
    return c.action_resolved();
}

// Range 2-4. Effect: Pull 1, ignoring line of sight. If pulling a unit into a wall, pull 3 instead and the kidnapped unit can pass through walls and units during this pull.
action_result kidnap(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 4, enum_or(SELECT_UNIT_IGNORE_LINE_OF_SIGHT, SELECT_UNIT_FOR_CURSE));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->pull(c.self(), 1, MOVEMENT_KIDNAP);
    return c.action_resolved();
}

// Attack, melee. On hit: 2 damage, then against isolated units inflict 1 weak (4+) and Doom them.
action_result serpents_kiss(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(2, DAMAGE_PHYSICAL, &c.self());

    bool kiss = u->is_isolated();
    if (c.self().has_upgrade(UPGRADE_IVORY_SERPENT))
        kiss |= u->space()->is_adverse_terrain();

    if (kiss) {
        u->gain_token(TOKEN_WEAK, +1);
        int d6 = c.player_roll_d6(c.self());
        if (d6 >= 4)
            u->gain_token(TOKEN_DOOM, +1);
    }
    return c.action_resolved();
}

// Curse, Self. Curse: Splash (self): foes gain 1 weak, (5+) and are Doomed. Doomed units take 1 curse damage.
action_result horrendous_shriek(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1, enum_or(SELECT_UNIT_FOR_CURSE, SELECT_UNIT_NO_ALLY));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    int d6 = c.player_roll_d6(c.self());
    bool hit_isolated = false;
    for (unit *u : us) {
        bool dmg = u->find_token(TOKEN_DOOM);
        u->gain_token(TOKEN_WEAK, +1);
        if (d6 >= 5)
            u->gain_token(TOKEN_DOOM, +1);
        if (dmg)
            u->take_damage(1, DAMAGE_CURSE, &c.self());
        hit_isolated |= u->is_isolated();
    }

    if (c.self().has_upgrade(UPGRADE_TERRORIZE) && hit_isolated)
        c.self().gain_token(TOKEN_STRENGTH, 2);

    if (c.self().has_upgrade(UPGRADE_CONDEMN)) {
        if (!c.then())
            return c.action_resolved(action_resolved::PREMATURELY);
        list<unit *> us = c.self().units_in_range(1, 1, enum_or(SELECT_UNIT_FOR_CURSE, SELECT_UNIT_NO_ALLY, SELECT_UNIT_WITH_HP_1_OR_LOWER));
        unit *u = c.player_must_select_unit(us);
        if (u)
            c.obliterate(*u);
    }
    return c.action_resolved();
}

// Attack, melee. On hit: 2 curse damage. Effect: Against isolated units, gain 1 strength and may then MOVE again.
action_result urgal_blade(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_CURSE), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    bool effect = u->is_isolated();
    if (effect)
        c.self().gain_token(TOKEN_STRENGTH, +1);

    u->take_damage(1, DAMAGE_CURSE, &c.self());

    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    if (effect && c.player_may_take_action(TAKE_ACTION_MOVE_AGAIN))
        c.unit_move_again(c.self());
    return c.action_resolved();
}


// Line 4. Effect: Line: 1 curse damage, Isolated units take 1 curse damage again. May penetrate walls and does not damage walls. Effect: If this line passes through a wall, this unit gains 1 strength.
action_result bale_scream(combat &c)
{
    list<unit *> us = c.player_must_select_line(4);
    bool wall = false;
    bool pull = c.self().has_upgrade(UPGRADE_SIREN);
    bool wall_instead_corpse = c.self().has_upgrade(UPGRADE_FREEZE_SOUL);
    for (unit *u : us) {
        if (u->is_wall()) {
            wall = true;
            continue;
        }
        if (pull)
            u->pull(c.self(), 2);
        u->take_damage(1, DAMAGE_CURSE, &c.self());
        if (u->is_isolated())
            u->take_damage(1, DAMAGE_CURSE, &c.self());
        if (wall_instead_corpse && u->is_slain())
            u->inc_trait(TRAIT_LEAVE_ADVERSE_TERRAIN_INSTEAD_OF_CORPSE, +1);
    }
    if (wall)
        c.self().gain_token(TOKEN_STRENGTH, +1);
    return c.action_resolved();
}


// Range 2-4. Effect: Create a wall in range and (5+) adjacent foes to the wall gain 1 weak.
action_result tombstone(combat &c)
{
    map_space *p = c.player_must_select_space(c.self().space(), 2, 4, enum_or(SELECT_SPACE_UNIT, SELECT_SPACE_NO_WALLS));
    if (!p)
        return c.action_failed();

    p->set_wall(true);

    int d6 = c.player_roll_d6(c.self());
    bool doom = c.self().has_upgrade(UPGRADE_DOOM_BELL) && d6 >= 6;
    for (unit *u : c.units_in_range(*p, 1, 1)) {
        if (u->is_ally(c.self()))
            continue;
        if (d6 >= 5)
            u->gain_token(TOKEN_WEAK, +1);
        if (doom)
            u->gain_token(TOKEN_DOOM, +1);
    }
    return c.action_resolved();
}


// Curse, Range 1-4, requires isolated unit. Effect: Choose an isolated unit. That unit gains 2 weak and may no longer MOVE or step until the end of its next turn or unit it's no longer isolated.
action_result hells_grasp(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 4, enum_or(SELECT_UNIT_FOR_CURSE, SELECT_UNIT_ISOLATED));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->gain_token(TOKEN_WEAK, 2);
    u->inc_trait(TRAIT_HELLS_GRASP, +1);
    u->inc_trait_after(TRAIT_HELLS_GRASP, -1, TRIGGER_TURN_END, 1);
    return c.action_resolved();
}


// Curse, Range 2-4. Effect: Unit gains 1 weak and is pulled 3.
action_result beckon_lamb(combat &c)
{
    select_unit_filter f = c.self().has_upgrade(UPGRADE_TO_THE_SLAUGHTER) ? enum_or(SELECT_UNIT_FOR_CURSE, SELECT_UNIT_IF_ISOLATED_MAX_RANGE_INF) : SELECT_UNIT_FOR_CURSE;
    list<unit *> us = c.self().units_in_range(2, 4, f);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->gain_token(TOKEN_WEAK, +1);
    u->pull(c.self(), 3);
    return c.action_resolved();
}

// Attack, melee. On hit: Deals 1 damage, +1 for each of the following that is true about the target: Doomed, Isolated, Weak, In adverse terrain.
action_result horrendous_end(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int dmg = 1;
    if (u->find_token(TOKEN_DOOM))
        dmg++;
    if (u->is_isolated())
        dmg++;
    if (u->find_token(TOKEN_WEAK))
        dmg++;
    if (u->space()->is_adverse_terrain())
        dmg++;
    bool supremacy = c.self().has_upgrade(UPGRADE_SUPREMACY) && u->type() == UNIT_TYRANT;
    damage_type type = supremacy ? enum_or(DAMAGE_CANT_BE_REDUCED, DAMAGE_OBLITERATE_ON_SLAY) : DAMAGE_PHYSICAL;
    bool slayed = false;
    u->take_damage(dmg, type, &c.self());
    return c.action_resolved();
}


// This unit is curseproof
action_result strong_pact(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_STRONG_PACT))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    c.self().inc_trait(TRAIT_CURSEPROOF, +1);
    return c.action_resolved();
}


// Attack, Range 1-2. Effect: Pull 1 before making attack. On hit: 2 damage. Against isolated units, inflicts 1 weak.
action_result strangle(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}

// Range 2-3, Curse. Effect: Target unit gains 1 or (5+) 2 weak. At the end of their turn, create a wall in a free adjacent space to them.
action_result grave_bind(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}

// A unit affected by a Miracle has a 5+ effect chance to instantly return to life at 1 HP at the end of any turn it is slain, clearing all tokens.
action_result miracle(combat &c)
{
    if (c.trigger() == TRIGGER_COMBAT_START) {
        c.self().set_trait(TRAIT_MIRACLE, 1);
        return c.action_resolved();
    }

    if (c.trigger() == TRIGGER_BEFORE_SLAINED && c.self().trait(TRAIT_MIRACLE)) {
        auto resurrect = [](combat &c) {
            int dc = 5;
            unit *vessel = nullptr;
            if (c.self().trait(TRAIT_DELAY_JUDGEMENT))
                dc = 2;
            else {
                list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_ALLY);
                for (unit *u : us) {
                    if (u->trait(TRAIT_HOLY_VESSEL)) {
                        dc = 4;
                        vessel = u;
                        break;
                    }
                }
            }
            if (c.player_roll_d6(c.self()) < dc)
                return c.action_resolved();

            for (token *t : c.self().tokens())
                c.self().remove_token(t->type(), t->count());
            c.self().set_slain(false);
            c.self().set_hp(1);

            if (vessel && vessel->trait(TRAIT_HOLY_VESSEL_READY)) {
                c.self().set_trait(TRAIT_HOLY_VESSEL_READY, 0);
                c.inc_soul(+1);
            }
            return c.action_resolved();
        };
        c.self().do_after(resurrect, TRIGGER_AFTER_UNIT_TURN_END, 1);
    }
    return c.action_resolved();
}


// When MOVEing for the second time or more in a turn, gain 1 vitality.
action_result zealotry(combat &c)
{
    bool already_moved_once = c.self().n_moves() >= 1;
    if (already_moved_once)
        c.self().gain_token(TOKEN_VITALITY, +1);
    return c.action_resolved();
}


// Lacks miracle, but starts combat with 4 vitality tokens.
action_result miraculous_flesh(combat &c)
{
    c.self().gain_token(TOKEN_VITALITY, 4);
    return c.action_resolved();
}


// Self. Splash (self): Remove one negative token from all allies in range and place them on this unit. Then remove one, (4+) two, or (6+) all negative tokens from this unit, and gain 1 strength for each negative token removed this way.
action_result mea_culpa(combat &c)
{
    bool may_step = c.self().has_upgrade(UPGRADE_CAGER);
    if (may_step && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 2);

    list<unit *> us = c.self().units_in_range(1, 1, enum_or(SELECT_UNIT_NO_FOE, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
    if (us.empty()) {
        if (!may_step)
            return c.action_prevented(action_prevented::NO_TARGET);
        return c.action_failed();
    }

    set<token_type> ts;
    for (unit *u : us) {
        for (token *t : u->tokens())
            ts.insert(t->type());
    }

    list<token_type> ts_list(ts.begin(), ts.end());
    ts_list.sort();

    optional<token_type> tt = c.player_must_select_token_type(ts_list);
    if (!tt)
        return c.action_failed();

    for (unit *u : us) {
        token *t = u->find_token(*tt);
        if (!t)
            continue;
        u->remove_token(t->type(), 1);
        c.self().gain_token(t->type(), +1);
    }
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    int d6 = c.player_roll_d6(c.self());
    int x = c.effect_gradations(d6, {{0, 1}, {4, 2}, {6, -1}});
    int removed = 0;
    if (x == -1) {
        for (token *t : c.self().tokens()) {
            if (t->is_negative()) {
                removed += t->count();
                c.self().remove_token(t->type(), t->count());
            }
        }
    } else {
        list<token *> ts = c.player_must_select_tokens(c.self().tokens(), x, SELECT_TOKEN_NEGATIVE);
        for (token *t : ts) {
            removed++;
            c.self().remove_token(t->type(), 1);
        }
    }
    c.self().gain_token(TOKEN_STRENGTH, removed);

    if (c.self().has_upgrade(UPGRADE_HOLY_BLOOD)) {
        int x = d6 >= 4 ? 2 : 1;
        list<map_space *>ps = c.player_must_select_spaces(c.self().space(), x, 1, 1, enum_or(SELECT_SPACE_UNIT, SELECT_SPACE_NO_WALLS));
        for (map_space *p : ps)
            p->set_hazard(true);
    }
    return c.action_resolved();
}


// Range 2-3. Effect: 1 holy damage and (3+) create a hazard under target.
action_result holy_water(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, DAMAGE_HOLY, &c.self());
    if (c.player_roll_d6(c.self()) >= 3)
        u->space()->set_hazard(true);
    return c.action_resolved();
}


// Attack, melee. On hit: 1 damage, and Stance (Odd): Splash (self): 1 holy damage. (Even): gain 1 vitality.
action_result excoriate(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    bool even = c.round_even();
    bool both = c.self().has_upgrade(UPGRADE_TASTE_THE_LASH) && c.round(4);
    if (!even || both) {
        for (unit *u : c.self().units_in_range(1, 1))
            u->take_damage(1, DAMAGE_HOLY, &c.self());
    }
    if (even || both)
        c.self().gain_token(TOKEN_VITALITY, +1);
    return c.action_resolved();
}


// Self. Stance: The next use of Whirling Chain deals +damage equal to half the round number, rounded up and this unit may also step that many spaces before using it.
action_result smite(combat &c)
{
    c.self().set_trait(TRAIT_SMITE, 1);
    return c.action_resolved();
}


// Attack, Range 1-2. On hit: 2 damage. Stance (Odd): and MOVE again, Stance (Even): and push units 2.
action_result whirling_chain(combat &c)
{
    int dmg = 2;
    if (c.self().trait(TRAIT_SMITE)) {
        c.self().set_trait(TRAIT_SMITE, 0);
        int x = (c.round() + !c.round_even() * 1) / 2;
        dmg += x;
        if (c.player_may_take_action(TAKE_ACTION_STEP))
            c.unit_step(c.self(), x);
    }

    list<unit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (c.self().has_upgrade(UPGRADE_SUFFUSE) && d6 >= 5) {
        c.self().gain_token(TOKEN_VITALITY, +1);
        if (c.self().n_tokens(SELECT_TOKEN_NEGATIVE)) {
            token *t = c.player_must_select_token(c.self().tokens(), SELECT_TOKEN_NEGATIVE);
            if (!t)
                return c.action_failed();
            c.self().remove_token(t->type());
        }
    }

    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    bool slayed = false;
    damage_type t = c.self().has_upgrade(UPGRADE_PUNISHER) && c.round(3) ? enum_or(DAMAGE_PHYSICAL, DAMAGE_SLAY_ON_OBLITERATE) : DAMAGE_PHYSICAL;
    u->take_damage(dmg, t, &c.self());

    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    if (c.round_even())
        u->push(c.self(), 2);
    else
        c.unit_move_again(c.self());

    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    if (c.self().has_upgrade(UPGRADE_FIERY_CHAIN)) {
        map_space *p = c.player_must_select_space(u->space(), 1, 1, SELECT_SPACE_NO_WALLS);
        if (!p)
            return c.action_failed();
        p->set_hazard(true);
    }
    return c.action_resolved();
}


// Self. Effect: Become unable to MOVE or step until start of this unit’s next turn. At the start of that turn, allied units adjacent to this unit gain 1 vitality.
action_result gentleness(combat &c)
{
    return c.action_unimplemented();
    return c.action_resolved();
}


// Range 1-3. Effect: An allied unit in range gains miracle. If it already had miracle, miracle triggers on a 2+ until the end of their next turn. At the end of that unit's turn, they lose miracle.
action_result delay_judgement(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!u->trait(TRAIT_MIRACLE)) {
        u->set_trait(TRAIT_MIRACLE, 1);
    return c.action_resolved();
    }

    u->inc_trait(TRAIT_DELAY_JUDGEMENT, +1);
    u->inc_trait_after(TRAIT_DELAY_JUDGEMENT, -1, TRIGGER_TURN_END, 1);
    return c.action_resolved();
}


// Self. Effect: Splash (self): Stance (Even): Allies gain 1 vitality. (Odd): Foes take 1 holy damage.
action_result blessed_censer(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    bool can_affect_units = !us.empty();
    bool can_clean_corpses = c.self().has_upgrade(UPGRADE_CONSECRATE) && c.self().corpses_in_range(1, 1);

    if (!can_affect_units && !can_clean_corpses)
        return c.action_prevented(action_prevented::NO_TARGET);

    if (can_clean_corpses && c.player_may_take_action(TAKE_ACTION_CONSECRATE)) {
        for (const map_space *p : c.self().spaces_in_range(1, 1))
            c.inc_corpse(*p, -c.inc_corpse(*p, 0));
        if (!c.then())
            return c.action_resolved(action_resolved::PREMATURELY);
    }

    int times = c.self().has_upgrade(UPGRADE_BOILING_CENSER) ? 2 : 1;
    while (times--) {
        for (unit *u : us) {
            if (c.round_even() && u->is_ally(c.self()))
                u->gain_token(TOKEN_VITALITY, 1);
            if (!c.round_even() && !u->is_ally(c.self()))
                u->take_damage(1, DAMAGE_HOLY, &c.self());
        }
    }
    return c.action_resolved();
}


// If this unit does not move during its turn, it may remove a negative token from itself.
action_result focus(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_FOCUS))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    if (c.self().n_moves())
        return c.action_prevented(action_prevented::CONDITION_UNSATISFIED);

    if (!c.self().n_tokens(SELECT_TOKEN_NEGATIVE))
        return c.action_prevented(action_prevented::NO_TARGET);

    token *t = c.player_may_select_token(c.self().tokens(), SELECT_TOKEN_NEGATIVE);
    if (!t)
        return c.action_failed();
    c.self().remove_token(t->type());
    return c.action_resolved();
}


// Self. Effect: Step 1. Next attack from this unit gains +1D and +2 maximum range. This effect stacks.
action_result winch(combat &c)
{
    c.unit_step(c.self(), 1);
    c.self().inc_trait(TRAIT_WINCH, +1);
    return c.action_resolved();
}


// Attack, Range 2-3. On hit: 2 damage. Stance (Round 3+): with piercing. (Round 5+): 4 damage with piercing.
action_result requiesce_en_pace(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int dmg = c.round(5) ? 4 : 2;
    damage_type type = c.round(3) ? enum_or(DAMAGE_PIERCING, DAMAGE_PHYSICAL) : DAMAGE_PHYSICAL;
    type = c.self().has_upgrade(UPGRADE_HEART_DESTROYER) ? enum_or(type, DAMAGE_OBLITERATE_ON_SLAY) : type;
    u->take_damage(dmg, type, &c.self());

    if (c.self().has_upgrade(UPGRADE_EXPLOSIVE_BOLTS))
        u->push(c.self(), round(5) ? 4 : round(3) ? 2 : 1);
    return c.action_resolved();
}

// Range 2-6. Effect: Create one, (3+) two, (5+) or three hazards in range. May destroy a wall instead of creating a hazard.
action_result bolides(combat &c)
{
    int d6 = c.player_roll_d6(c.self());
    int n = c.effect_gradations(d6, {{0, 1}, {3, 2}, {5, 3}});
    bool scathe = c.self().has_upgrade(UPGRADE_SCATHE);
    while (n--) {
        map_space *p = c.player_must_select_space(c.self().space(), 2, 6);
        if (!p)
            return c.action_failed();
        if (!p->is_wall()) {
            p->set_hazard(true);
            continue;
        }
        p->set_wall(false);
        if (scathe) {
            list<unit *> us = c.units_in_range(*p, 1, 1, enum_or(SELECT_UNIT_NO_ALLY, SELECT_UNIT_NO_SELF));
            if (us.empty())
                continue;
            unit *u = c.player_may_select_unit(us);
            if (u)
                u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
        }
    }
    return c.action_resolved();
}


// Self, Range 3-6. Effect: Remove up to two vitality tokens on this unit, then deal 1 holy damage to that many units in range, ignoring line of sight, and push those units 1.
action_result indignation(combat &c)
{
    int n = min(2, c.self().n_tokens(SELECT_TOKEN_VITALITY));
    if (!n)
        return c.action_prevented(action_prevented::NO_TOKENS);

    list<unit *> us = c.self().units_in_range(3, 6, SELECT_UNIT_IGNORE_LINE_OF_SIGHT);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    optional<int> removed = c.player_must_select_token_count(n);
    if (!removed)
        return c.action_failed();

    us = c.player_must_select_units(us, 0, n);
    if (us.empty())
        return c.action_resolved(action_resolved::PREMATURELY);

    int dmg = c.self().has_upgrade(UPGRADE_WITNESS) && c.round(4) ? 2 : 1;
    for (unit *u : us) {
        u->take_damage(dmg, DAMAGE_HOLY, &c.self());
        u->push(c.self(), 1);
    }
    return c.action_resolved();
}


// Self. Effect: Gain two vitality. Until the start of this unit's next turn, adjacent allies may spend vitality tokens on this unit as if they were their own.
action_result ablutions(combat &c)
{
    int x = 2;
    if (c.self().has_upgrade(UPGRADE_HOLY_FONT)) {
        if (c.round(3))
            x = 3;
        if (c.round(5))
            x = 4;
    }
    c.self().gain_token(TOKEN_VITALITY, x);
    c.self().inc_trait(TRAIT_ABLUTIONS, +1);
    c.self().inc_trait_after(TRAIT_ABLUTIONS, -1, TRIGGER_TURN_START, 1);
    return c.action_resolved();
}


// Melee, Attack. On hit: Self and an adjacent ally gain 1 vitality (Stance: R4+): 2 vitality. Then deal 1 damage and push 1. Cannot miss allies and pushes them +1.
action_result bible_thump(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, SELECT_UNIT_NO_SELF);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    bool hit = u->is_ally(c.self()) ? true : c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK));
    if (!hit) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int x = c.round(4) ? 2 : 1;
    c.self().gain_token(TOKEN_VITALITY, x);

    list<unit *> allies = c.self().units_in_range(1, enum_or(SELECT_UNIT_NO_SELF, SELECT_UNIT_NO_FOE));
    unit *ally = c.player_must_select_unit(allies);
    if (ally)
        ally->gain_token(TOKEN_VITALITY, x);

    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    u->push(c.self(), u->is_ally(c.self()) ? 2 : 1);
    return c.action_resolved();
}


// Self, Ally, Melee. Effect: Choose self or an adjacent ally. At the end of that unit’s turn, slay them. They splash (self) 1 holy damage to foes, 1 vitality to allies. Increase damage and vitality by +1 if the unit had 3 or more vitality.
action_result blood_of_the_covenant(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, SELECT_UNIT_NO_FOE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->set_trait(TRAIT_BLOOD_OF_THE_COVENANT, 1);
    return c.action_resolved();
}


// Range 1-3, Self, Ally. Effect: Target unit converts all vitality into strength, and then becomes unable to gain vitality tokens for the rest of combat.
action_result wrath(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, enum_or(SELECT_UNIT_NO_FOE, SELECT_UNIT_WITH_VITALITY_TOKENS));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    token *t = u->find_token(TOKEN_VITALITY);
    if (t) {
        u->remove_token(t->type(), t->count());
        u->gain_token(TOKEN_STRENGTH, t->count());
    }
    u->set_trait(TRAIT_CANT_GET_VITALITY, 1);
    return c.action_resolved();
}


// 2 of these units are worth 1 unit slot. Can be activated two at a time.
action_result thrall(combat &c)
{
    c.self().set_trait(TRAIT_COST_HALF_UNIT_SLOT, 1);
    c.self().set_trait(TRAIT_ACTIVATED_TWO_AT_A_TIME, 1);
    return c.action_resolved();
}


// When slain, leaves an extra corpse token in an adjacent space.
action_result fall_to_shambles(combat &c)
{
    map_space *p = c.player_must_select_space(c.self().space(), 1, 1);
    if (!p)
        return c.action_failed();

    c.inc_corpse(*p, +1);
    return c.action_resolved();
}

// Self.Effect: Gain 1 strength, (4+) 1 speed, (5+) and 1 vitality, (6+) and explode for splash (self): 1 damage. Spare parts: Roll +1D per corpse and pick the highest result.
action_result unstable_mutation(combat &c)
{
    if (c.self().has_upgrade(UPGRADE_WARPING_MUTATE) && c.player_may_take_action(TAKE_ACTION_STEP)) {
        int d6 = c.player_roll_d6(c.self());
        int x = c.effect_gradations(d6, {{0, 1}, {4, 3}});
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
        for (unit *u : c.self().units_in_range(1, 1))
            u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    }
    return c.action_resolved();
}


// Attack, Melee. Effect: Mutate. On hit: 1 damage.
action_result twisting_strike(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    unit *m = &c.self();
    if (c.self().has_upgrade(UPGRADE_SPREAD_MUTATE)) {
        m = c.player_must_select_unit(c.self().units_in_range(1, SELECT_UNIT_NO_FOE));
        if (!m)
            return c.action_failed();
    }
    c.mutate(*m);

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    if (c.self().has_upgrade(UPGRADE_GENESTEALER) && d6 >= 5 && u->n_tokens(SELECT_TOKEN_POSITIVE)) {
        list<token *> ts = u->tokens();
        token *t = c.player_may_select_token(ts);
        if (!t)
            return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

        u->remove_token(t->type());
        c.self().gain_token(t->type(), +1);
    }
    return c.action_resolved();
}


// After MOVEing, drop a corpse in a free adjacent space.
action_result leftovers(combat &c)
{
    if (c.player_roll_d6(c.self()) < 4)
        return c.action_resolved(action_resolved::PREMATURELY);

    map_space *p = c.player_must_select_space(c.self().space(), 1, enum_or(SELECT_SPACE_UNIT, SELECT_SPACE_NO_WALLS));
    if (!p)
        return c.action_failed();

    c.inc_corpse(*p, +1);
    return c.action_resolved();
}


// Range 3. Effect: One or two units in range mutate. Spare parts: and also dole out 1 strength per corpse consumed.
action_result inject_mutagen(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    us = c.player_must_select_units(us, 1, 2);
    if (us.empty())
        return c.action_failed();

    int tokens = c.player_may_spare_parts(c.self());
    for (unit *u : us) {
        c.mutate(*u);
        if (tokens)
            u->gain_token(TOKEN_STRENGTH, tokens);
    }
    return c.action_resolved();
}


// Curse, Range 1-3. Effect: A unit in range removes one token of this unit's choice (3+): two (5+): all.
action_result purge(combat &c)
{
    list<unit *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_TOKENS);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_resolved(action_resolved::PREMATURELY);

    int d6 = c.player_roll_d6(c.self());
    int tokens = c.effect_gradations(d6, {{0, 1}, {3, 2}, {5, -1}});
    list<token *> stolen;
    while (tokens--) {
        token *t = c.player_may_select_token(u->tokens());
        if (!t)
            break;
        u->remove_token(t->type(), 1);
        stolen.push_back(t);
    }

    if (c.self().has_upgrade(UPGRADE_SCOUR_FLESH) && !u->is_ally(c.self()))
        u->take_damage(1, enum_or(DAMAGE_PIERCING, DAMAGE_TOXIC), &c.self());

    if (c.self().has_upgrade(UPGRADE_ABSORB)) {
        list<unit *> us = c.self().units_in_range(1, 3);
        if (us.empty())
            return c.action_failed();

        unit *another = c.player_may_select_unit(us, {u});
        if (!another)
            return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

        token *t = c.player_may_select_token(stolen);
        if (t)
            return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

        another->gain_token(t->type(), +1);
    }
    return c.action_resolved();
}


// Marriage: Range 1-2. Effect: Instantly slay self or an allied unit in range. Another allied unit in range gains 2 speed, vitality, and strength, or just 1 if the slain unit was a thrall.
action_result marriage(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 2);
    if (us.size() < 2)
        return c.action_prevented(action_prevented::NO_TARGET);

    list<unit *> srcs = us;
    srcs.push_back(&c.self());
    unit *src = c.player_must_select_unit(srcs);
    if (!src)
        return c.action_failed();

    unit *dst = c.player_must_select_unit(us);
    if (!dst)
        return c.action_failed();

    int x = src->type() == UNIT_THRALL ? 1 : 2;
    c.slay(*src);

    if (c.self().has_upgrade(UPGRADE_CONJOIN) && src == &c.self()) {
        for (token *t : dst->tokens())
            dst->remove_token(t->type(), t->count());
    }

    dst->gain_token(TOKEN_SPEED, x);
    dst->gain_token(TOKEN_VITALITY, x);
    dst->gain_token(TOKEN_STRENGTH, x);
    return c.action_resolved();
}


action_result the_hunger(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_THE_HUNGER))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    if (c.self().corpses_in_range(0, 2))
        c.unit_step(c.self(), 2);
    return c.action_resolved();
}


action_result autophagia(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_AUTOPHAGIA))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    if (!c.player_may_take_action(TAKE_ACTION_AUTOPHAGIA))
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

    map_space *p = c.player_must_select_space(c.self().space(), 1, 1);
    if (!p)
        return c.action_failed();

    c.self().take_damage(1, enum_or(DAMAGE_PHYSICAL, DAMAGE_PIERCING), &c.self());
    c.inc_corpse(*p, +1);
    return c.action_resolved();
}


// Self Effect: Mutate, then gain 1 strength. Spare Parts: Then gain (3+) 1 speed, (5+) and 1 vitality, (6+) then mutate again. Roll 1D per corpse consumed for the effect.
action_result bloodgorger(combat &c)
{
    c.mutate(c.self());
    c.self().gain_token(TOKEN_STRENGTH, +1);

    int times = c.player_may_spare_parts(c.self());
    while (times--) {
        int d6 = c.player_roll_d6(c.self());
        if (d6 >= 3)
            c.self().gain_token(TOKEN_SPEED, +1);
        if (d6 >= 5)
            c.self().gain_token(TOKEN_VITALITY, +1);
        if (d6 >= 6) {
            if (!c.then())
                return c.action_resolved(action_resolved::PREMATURELY);
            c.mutate(c.self());
        }
    }
    return c.action_resolved();
}

// Melee, Attack On hit: 1 damage. Effect: splash (self): 1 damage.
action_result bloody_slashes(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    list<unit *> splash = c.self().units_in_range(1, 1);
    for (unit *u : splash)
        u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    return c.action_resolved();
}


// Range 2-4 Effect: Splash (target): Create a corpse in the area for every unit in the area, up to three times, then mutate.
action_result regurgitate(combat &c)
{
    const auto regurgitate_drown_in_viscera = [](combat &c, unit *target)
    {
        if (c.player_roll_d6(c.self()) < 5)
            return c.action_resolved(action_resolved::PREMATURELY);

        list<unit *> us = target->units_in_range(1, 1, enum_or(SELECT_UNIT_NO_ALLY, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
        if (us.empty())
            return c.action_resolved(action_resolved::PREMATURELY);

        set<token_type> ts;
        for (unit *u : us) {
            for (token *t : u->tokens())
                ts.insert(t->type());
        }

        list<token_type> ts_list(ts.begin(), ts.end());
        ts_list.sort();

        optional<token_type> tt = c.player_may_select_token_type(ts_list);
        if (!tt)
            return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

        for (unit *u : us) {
            token *t = u->find_token(*tt);
            if (t)
                u->remove_token(t->type(), 1);
        }
        return c.action_resolved();
    };

    const auto regurgitate_cleansing_wash = [](combat &c, unit *target)
    {
        if (c.player_roll_d6(c.self()) < 5)
            return c.action_resolved(action_resolved::PREMATURELY);

        list<unit *> us = target->units_in_range(1, 1, enum_or(SELECT_UNIT_NO_FOE, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
        if (us.empty())
            return c.action_resolved(action_resolved::PREMATURELY);

        unit *u = c.player_must_select_unit(us);
        if (!u)
            return c.action_failed();

        token *t = c.player_must_select_token(u->tokens(), SELECT_TOKEN_NEGATIVE);
        if (!t)
            return c.action_failed();

        u->remove_token(t->type());
        return c.action_resolved();
    };

    list<unit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    list<unit *> splash = u->units_in_range(1, SELECT_UNIT_NO_SELF);
    int corpses = min(3, (int)splash.size());
    int mutations = c.self().has_upgrade(UPGRADE_RAPID_ADAPTATION) ? corpses : 1;

    while (corpses--) {
        map_space *p = c.player_must_select_space(u->space(), 1, 1);
        if (!p)
            return c.action_failed();
        c.inc_corpse(*p, +1);
    }

    if (c.self().has_upgrade(UPGRADE_DROWN_IN_VISCERA))
        regurgitate_drown_in_viscera(c, u);

    if (c.self().has_upgrade(UPGRADE_CLEANSING_WASH))
        regurgitate_cleansing_wash(c, u);

    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);
    while (mutations--)
        c.mutate(c.self());
    return c.action_resolved();
}


// Range 2-4, Attack On hit: 1 damage. Effect: Steal a positive token from target. Spare Parts: plus one more token per corpse consumed. If target has no positive tokens, gain 1 strength instead of stealing a token
action_result sin_eater(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    if (u->n_tokens(SELECT_TOKEN_POSITIVE)) {
        c.self().gain_token(TOKEN_STRENGTH, +1);
        return c.action_resolved();
    }

    int times = 1 + c.player_may_spare_parts(c.self());
    while (times--) {
        token *t = c.player_may_select_token(u->tokens());
        if (!t)
            return c.action_failed();
        u->remove_token(t->type(), 1);
        c.self().gain_token(t->type(), 1);
    }
    return c.action_resolved();
}


// Self Effect: Gain 1 strength, OR 1 speed, OR 1 vitality. Spare parts: Repeat this effect once for each corpse consumed.
action_result sculpt_flesh(combat &c)
{
    int times = max(1, c.player_may_spare_parts(c.self()));

    while (times--) {
        optional<token_type> t = c.player_must_select_token_type({TOKEN_STRENGTH, TOKEN_SPEED, TOKEN_VITALITY});
        if (!t)
            return c.action_failed();

        c.self().gain_token(*t, +1);
    }
    return c.action_resolved();
}


// Melee Effect: Deal 1 devil damage to an adjacent unit. If reduce to 0 hp, obliterates unit and the homunculus gains any tokens the absorbed unit had.
action_result absorb(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    bool meld = c.self().has_upgrade(UPGRADE_MELD) && u->is_ally(c.self());
    damage_type t = meld ? enum_or(DAMAGE_DEVIL, DAMAGE_SLAY_ON_OBLITERATE) : DAMAGE_DEVIL;
    u->take_damage(1, t, &c.self());
    if (!u->is_slain())
        return c.action_resolved(action_resolved::PREMATURELY);

    for (token *t : u->tokens())
        c.self().gain_token(t->type(), t->count());

    if (c.self().has_upgrade(UPGRADE_FORM_CARAPACE))
        c.self().inc_trait(TRAIT_PHYSICAL_ARMOR, +1);

    if (meld)
        c.mutate(c.self());
    return c.action_resolved();
}


// Flesh Whip: Attack, Range 1-2 On hit: 1 damage and splash (target): 1 damage and (4+) create one (6+) or two corpse tokens in an adjacent space to target.
action_result flesh_whip(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    // self + splash
    for (unit *near : u->units_in_range(1, 1))
        u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    int n = c.effect_gradations(d6, {{1, 0}, {4, 1}, {6, 2}});
    if (!n)
        return c.action_resolved(action_resolved::PREMATURELY);

    map_space *p = c.player_must_select_space(u->space(), 1);
    if (!p)
        return c.action_failed();

    c.inc_corpse(*p, n);
    return c.action_resolved();
}


action_result ball_of_limbs(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_BALL_OF_LIMBS))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    bool start_second_move = c.trigger() == TRIGGER_BEFORE_MOVE && c.self().n_moves() >= 1;
    bool end_second_move = c.trigger() == TRIGGER_AFTER_MOVE && c.self().n_moves() >= 2;
    if (start_second_move) {
        c.self().inc_trait(TRAIT_MOVEMENT_DESTROY_WALLS, +1);
        c.self().inc_trait(TRAIT_MOVEMENT_ABSORB_CORPSES, +1);
        return c.action_resolved();
    }

    if (end_second_move) {
        c.self().inc_trait(TRAIT_MOVEMENT_DESTROY_WALLS, -1);
        c.self().inc_trait(TRAIT_MOVEMENT_ABSORB_CORPSES, -1);
        int walls = c.self().trait(TRAIT_LAST_MOVEMENT_WALLS_DESTROYED);
        int corpses = c.self().trait(TRAIT_LAST_MOVEMENT_CORPSES_ABSORBED);
        int times = walls + corpses;
        while (times--)
            c.mutate(c.self());
        return c.action_resolved();
    }
    return c.action_resolved();
}


// At turn start, may convert one of this unit’s mutation tokens into strength, speed, or vitality. Then, mutate.
action_result polyglot(combat &c)
{
    token *t = c.self().find_token(TOKEN_MUTATION);
    if (!t)
        return c.action_prevented(action_prevented::NO_TOKENS);
    optional<token_type> tt = c.player_may_select_token_type({TOKEN_STRENGTH, TOKEN_SPEED, TOKEN_VITALITY});
    if (tt) {
        c.self().remove_token(t->type());
        c.self().gain_token(*tt, +1);
    }
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);
    c.mutate(c.self());
    return c.action_resolved();
}


// At turn end, may remove one token from this unit and grant to a unit in range 2.
action_result accelerate_evolution(combat &c)
{
    list<token *> ts = c.self().tokens();
    if (ts.empty())
        return c.action_prevented(action_prevented::NO_TOKENS);
    list<unit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    token *t = c.player_may_select_token(ts);
    if (!t)
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);
    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().remove_token(t->type(), 1);
    u->gain_token(t->type(), 1);
    return c.action_resolved();
}


// May remove a mutation token at start or end of own turn to step 2.
action_result rapid_move(combat &c)
{
    if (c.trigger() == TRIGGER_TURN_START) {
        c.self().inc_trait(TRAIT_RAPID_MOVE_AVAILABLE, +1);
        token *t = c.self().find_token(TOKEN_MUTATION);
        if (!t)
            return c.action_prevented(action_prevented::NO_TOKENS);
        if (!c.player_may_take_action(TAKE_ACTION_RAPID_MOVE))
            return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

        c.self().remove_token(t->type());
        c.unit_step(c.self(), 2);
        c.self().inc_trait(TRAIT_RAPID_MOVE_AVAILABLE, -1);
        return c.action_resolved();
    }

    if (c.trigger() == TRIGGER_TURN_END) {
        bool rm_available = c.self().trait(TRAIT_RAPID_MOVE_AVAILABLE);
        token *t = c.self().find_token(TOKEN_MUTATION);
        if (!rm_available || !t)
            return c.action_prevented(action_prevented::ALREADY_USED);

        if (c.player_may_take_action(TAKE_ACTION_RAPID_MOVE)) {
            c.self().remove_token(t->type());
            c.unit_step(c.self(), 2);
        }
        c.self().set_trait(TRAIT_RAPID_MOVE_AVAILABLE, 0);
        return c.action_resolved();
    }
    return c.action_unreachable();
}


// May remove a mutation token on self to grant +1D on any attack and allow it to ignore cover.
action_result ancillary_limbs(combat &c)
{
    token *t = c.self().find_token(TOKEN_MUTATION);
    if (!t)
        return c.action_prevented(action_prevented::NO_TOKENS);
    if (!c.player_may_take_action(TAKE_ACTION_ANCILLARY_LIMBS))
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);
    c.self().inc_trait(TRAIT_ANCILLARY_LIMBS, +1);
    return c.action_resolved();
}


// Line 4. Line: 1 damage. Effect: Allies in the line mutate instead of taking damage.
action_result experimental_surgery(combat &c)
{
    list<unit *> us = c.player_must_select_line(4);
    for (unit *u : us) {
        if (c.self().is_ally(*u))
            c.mutate(*u);
        else
            u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
        if (!c.then())
            return c.action_resolved(action_resolved::PREMATURELY);
    }
    return c.action_resolved();
}


// Self. Effect: Create one, (3+) two, (5+) three corpses in free adjacent spaces, then mutate once for each corpse created.
action_result new_material(combat &c)
{
    int d6 = c.player_roll_d6(c.self());
    int cs = c.effect_gradations(d6, {{1, 1}, {3, 2}, {5, 3}});
    int n = 0;
    while (cs--) {
        map_space *p = c.player_must_select_space(c.self().space(), 1, enum_or(SELECT_SPACE_UNIT, SELECT_SPACE_NO_WALLS));
        if (p) {
            c.inc_corpse(*p, +1);
            ++n;
        }
        if (!c.then())
            return c.action_resolved(action_resolved::PREMATURELY);
    }
    while (n--) {
        c.mutate(c.self());
        if (!c.then())
            return c.action_resolved(action_resolved::PREMATURELY);
    }
    return c.action_resolved();
}


// Range 2. Effect: Create a perfect copy of an allied unit in range in any other free space in range. Then obliterate the original as it collapses into flesh and replace it with a corpse token.
action_result clone(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, enum_or(SELECT_UNIT_NO_SELF, SELECT_UNIT_NO_FOE));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);
    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();
    map_space *p = c.player_must_select_space(c.self().space(), 2, enum_or(SELECT_SPACE_UNIT, SELECT_SPACE_NO_WALLS));
    if (!p)
        return c.action_prevented(action_prevented::NO_TARGET);
    c.copy_unit(*u, *p);
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    c.obliterate(*u);
    c.inc_corpse(*u->space(), +1);
    return c.action_resolved();
}


// Range 3. Effect: Remove up to three negative tokens from a unit, then it may step 1 and it mutates. Spare Parts: The unit mutates and steps once for each negative token removed instead.
action_result stitch_fix(combat &c)
{
    list<unit *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_NEGATIVE_TOKENS);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);
    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int removed = 0;
    for (int i = 0; i < 3; ++i) {
        list<token *> ts = u->tokens();
        token *t = c.player_may_select_token(ts, SELECT_TOKEN_NEGATIVE);
        if (!t)
            break;
        removed++;
    }
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);
    if (c.player_may_spare_parts(*u)) {
        if (c.player_may_take_action(TAKE_ACTION_STEP))
            c.unit_step(*u, removed);
        while (removed--)
            c.mutate(*u);
        return c.action_resolved();
    }
    // no spare parts
    if (c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(*u, 1);
    c.mutate(*u);
    return c.action_resolved();
}


// Range 3. Effect: Choose a unit in range. That unit may step 2. If it ends its turn in the space of a corpse, it mutates, removes the corpse, then may repeat this effect.
action_result inject_stimulant(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);
    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    while (true) {
        if (!c.player_may_take_action(TAKE_ACTION_STEP))
            break;

        c.unit_step(*u, 2);
        bool on_corpse = c.inc_corpse(*u->space());
        if (!on_corpse)
            break;

        c.inc_corpse(*u->space(), -1);
        c.mutate(*u);
        if (!c.then())
            break;
    }
    return c.action_resolved();
}


// Attack, range 2-5. On hit: 1 toxin damage. Effect: At end of target’s next turn, they explode for a splash (target) effect for 1 toxin damage. Your allies in the area mutate instead of taking damage.
action_result biotoxin_injector(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);
    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();
    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_TOXIC), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_TOXIC, &c.self());
    u->inc_trait(TRAIT_BIOTOXIN_INJECTOR, +1);
    return c.action_resolved();
}


// Attack, Range 2-5. On hit: May remove one positive token from target, then deal 1 toxin damage. Spare Parts: Repeat this effect once.
action_result mutagen_injector(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);
    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();
    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK))) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int effect = c.player_may_spare_parts(c.self()) ? 2 : 1;
    while (effect--) {
        list<token *> ts = u->tokens();
        token *t = c.player_may_select_token(ts, SELECT_TOKEN_POSITIVE);
        if (!t)
            break;
        u->remove_token(t->type());
        u->take_damage(1, DAMAGE_TOXIC, &c.self());
    }
    return c.action_resolved();
}

// Line 3. Effect: Line: 1 fire damage and inflict 1 vulnerable, (3-4) OR 1 toxic damage and inflict 1 slow, (5-6) OR 1 curse damage and inflict 1 weak. Spare Parts: Roll 1D for effect per corpse consumed and choose any result.
action_result chaos_beam(combat &c)
{
    list<unit *> us = c.player_must_select_line(3);

    optional<int> effect;
    {
        int n = max(1, c.player_may_spare_parts(c.self()));
        list<int> rolls;
        while (n--)
            rolls.push_back(c.player_roll_d6(c.self()));
        effect = c.player_must_select_roll(rolls);
    }
    if (!effect)
        return c.action_resolved(action_resolved::PREMATURELY);

    for (unit *u : us) {
        switch (*effect) {
        case 1:
        case 2:
            u->take_damage(1, DAMAGE_FIRE, &c.self());
            u->gain_token(TOKEN_VULNERABLE, +1);
            break;
        case 3:
        case 4:
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
            u->gain_token(TOKEN_SLOW, +1);
            break;
        case 5:
        case 6:
            u->take_damage(1, DAMAGE_CURSE, &c.self());
            u->gain_token(TOKEN_WEAK, +1);
            break;
        }
        if (!c.then())
            return c.action_resolved(action_resolved::PREMATURELY);
    }
    return c.action_resolved();
}


// (1 SOUL): Own or Allied Turn, Range 4. Trigger: Turn start. Effect: Unit gains 1 strength, (3-4) OR 1 speed, (5-6) OR 1 vitality. Spare Parts: May choose one token per corpse consumed instead of rolling.
action_result wild_mutation(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 4);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

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
        u->gain_token(t, +1);
    return c.action_resolved();
}


// (3 SOUL): Own or allied turn. Range 4. Copy all positive tokens on target unit, then grant them to another unit in range.
action_result sample_genome(combat &c)
{
    list<unit *> srcs = c.self().units_in_range(4, SELECT_UNIT_WITH_POSITIVE_TOKENS);
    if (srcs.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    list<unit *> dsts = c.self().units_in_range(1, 4);
    if (dsts.size() < 2)
        return c.action_prevented(action_prevented::NO_TARGET);

    if (c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *src = c.player_must_select_unit(srcs);
    if (!src)
        return c.action_failed();

    dsts.remove(src);
    unit *dst = c.player_must_select_unit(dsts);
    if (!dst)
        return c.action_failed();

    for (token *t : src->tokens()) {
        if (t->is_positive())
            dst->gain_token(t->type(), t->count());
    }
    return c.action_resolved();
}


// (1 SOUL): Own or allied turn. Range 3. Effect: Swap places with an allied unit, then both of you mutate.
action_result flesh_jump(combat &c)
{
    list<unit *> us = c.self().units_in_range(3, enum_or(SELECT_UNIT_NO_SELF, SELECT_UNIT_NO_FOE));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.swap_unit_pos(c.self(), *u);
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);
    c.mutate(c.self());
    c.mutate(*u);
    return c.action_resolved();
}


// (2 SOUL): Own or allied turn. Range 3. Effect: Remove any number of mutation tokens on self or target unit, then target may step 2 per token removed with free movement, ignoring hazards.
action_result grow_bonus_legs(combat &c)
{
    list<unit *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_MUTATION_TOKENS);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(2))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    token *t = u->find_token(TOKEN_MUTATION);
    optional<int> remove = c.player_must_select_token_count(t->count());
    if (!remove)
        return c.action_failed();

    u->remove_token(t->type(), *remove);
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);
    c.unit_step(*u, *remove * 2, enum_or(MOVEMENT_FREE, MOVEMENT_IGNORE_HAZARDS));
    return c.action_resolved();
}


// (2 SOUL): Own or allied turn. Range 3. Trigger: Turn start. Effect: Remove any number of mutation tokens from target, then target gains +1D on attacks this turn per token removed, and their damage ignores armor.
action_result grow_bonus_limbs(combat &c)
{
    list<unit *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_MUTATION_TOKENS);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(2))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    token *t = u->find_token(TOKEN_MUTATION);
    optional<int> remove = c.player_must_select_token_count(t->count());
    if (!remove)
        return c.action_failed();

    u->remove_token(t->type(), *remove);
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);
    u->inc_trait(TRAIT_GROW_BONUS_LIMBS, *remove);
    return c.action_resolved();
}


// (3 SOUL): Curse, Any turn. Trigger: Turn end. Effect: Deal 1 toxic damage to unit. If this reduces them to 0 hp, they are obliterated. Create up to 3 corpse tokens under their space or in free adjacent spaces.
action_result recycle(combat &c)
{
    unit &u = c.activated();
    if (u.is_curseproof())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    u.take_damage(1, enum_or(DAMAGE_TOXIC, DAMAGE_OBLITERATE_ON_SLAY), &c.self());
    if (!u.is_slain())
        return c.action_resolved(action_resolved::PREMATURELY);
    map_space *p = c.player_must_select_space(u.space(), 1, enum_or(SELECT_SPACE_UNIT, SELECT_SPACE_NO_WALLS));
    if (!p)
        return c.action_failed();
    optional<int> n = c.player_must_select_corpse_count(3);
    if (!n)
        return c.action_failed();
    c.inc_corpse(*p, *n);
    return c.action_resolved();
}


// (4 SOUL): Curse, Foe turn. Range 3. Trigger: Turn start. Effect: Inflict 1 slow, weak, and vulnerable on a foe (5+) twice.
action_result devolve(combat &c)
{
    unit &u = c.activated();
    if (u.is_curseproof())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(4))
        return c.action_prevented(action_prevented::NO_SOUL);

    int d6 = c.player_roll_d6(c.self());
    int times = c.effect_gradations(d6, {{1, 1}, {5, 2}});
    while (times--) {
        u.gain_token(TOKEN_SLOW, +1);
        u.gain_token(TOKEN_WEAK, +1);
        u.gain_token(TOKEN_VULNERABLE, +1);
    }
    return c.action_resolved();
}


// (6 SOUL): Own turn. Trigger: Turn start. Effect: Increase MV to 6, DF to 6+, gain 6 strength, become curseproof, gain free movement, and gain super armor. Keep any damage taken. At the end of your next turn, your form destabilizes and you are obliterated.
action_result final_form(combat &c)
{
    if (!c.player_may_spend_soul(6))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.self().inc_trait(TRAIT_CURSEPROOF, +1);
    c.self().inc_trait(TRAIT_MOVEMENT_FREE, +1);
    c.self().inc_trait(TRAIT_SUPER_ARMOR, +1);

    c.self().set_trait(TRAIT_ALTERED_MV, 6);
    c.self().set_trait(TRAIT_ALTERED_DF, 6);
    c.self().gain_token(TOKEN_STRENGTH, 6);

    c.self().set_trait(TRAIT_FINAL_FORM, 2);
    return c.action_resolved();
}


// (1 SOUL) Own or allied turn. Effect: Step 1, then you or an adjacent ally reload.
action_result reload_slide(combat &c)
{
    c.unit_step(c.self(), 1);
    list<unit *> us = c.self().units_in_range(0, 1, SELECT_UNIT_ALLY);
    if (us.empty())
        return c.action_resolved(action_resolved::PREMATURELY);
    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();
    c.reload(*u);
    return c.action_resolved();
}


// (3 SOUL) Own turn. Effect: Clear one (5+) or two negative tokens. Ranged attacks automatically miss you until the end of your next turn (5+) and you are immune to splash and line damage for the duration.
action_result bullet_time(combat &c)
{

    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    int d6 = c.player_roll_d6(c.self());
    int tokens_to_clear = d6 >= 5 ? 2 : 1;

    if (c.self().n_tokens(SELECT_TOKEN_NEGATIVE)) {
        list<token *> ts = c.player_must_select_tokens(c.self().tokens(), SELECT_TOKEN_NEGATIVE);
        for (token *t : ts)
            c.self().remove_token(t->type(), 1);
    }
    c.self().inc_trait(TRAIT_RANGED_ATTACKS_AUTO_MISS, +1);
    c.self().inc_trait_after(TRAIT_RANGED_ATTACKS_AUTO_MISS, -1, TRIGGER_TURN_END, 2);
    if (d6 >= 5) {
        c.self().inc_trait(TRAIT_IMMUNE_TO_SPLASH_DAMAGE, +1);
        c.self().inc_trait_after(TRAIT_IMMUNE_TO_SPLASH_DAMAGE, -1, TRIGGER_TURN_END, 2);
        c.self().inc_trait(TRAIT_IMMUNE_TO_LINE_DAMAGE, +1);
        c.self().inc_trait_after(TRAIT_IMMUNE_TO_LINE_DAMAGE, -1, TRIGGER_TURN_END, 2);
    }
    return c.action_resolved();
}


// Own turn. Trigger: you reduce a unit to exactly 1 HP. Effect: you may immediately activate this ability to deal 1 damage to it.
action_result double_tap(combat &c)
{
    unit &u = c.activated();
    if (u.hp() != 1)
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    u.take_damage(1, DAMAGE_PHYSICAL, &c.self());
    return c.action_resolved();
}


// Own turn. Trigger: You hit an attack. Effect: Target gains 3 vulnerable after the ability resolves.
action_result frag_bullet(combat &c)
{
    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit &u = c.activated();
    u.gain_token(TOKEN_VULNERABLE, 3);
    return c.action_resolved();
}


// Any turn. Trigger: After any unit MOVEs. Effect: Step 2, then deal 1 damage to a unit in range 3.
action_result bullet_hustle(combat &c)
{
    if (!c.player_may_spend_soul(2))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.unit_step(c.self(), 2);

    list<unit *> us = c.self().units_in_range(1, 3);
    if (us.empty())
        return c.action_resolved(action_resolved::PREMATURELY);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    return c.action_resolved();
}


// (3 SOUL) Own turn. Effect: Reload. Until the start of your next turn, enemy units entering adjacent spaces without being pushed or pulled there take 1 damage and are pushed 1, interrupting their movement. This effect can only trigger once a turn.
action_result overwatch(combat &c)
{
    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.reload(c.self());
    c.self().inc_trait(TRAIT_OVERWATCH, +1);
    c.self().inc_trait_after(TRAIT_OVERWATCH, -1, TRIGGER_TURN_START, 1);
    return c.action_resolved();
}


// (4 SOUL) Own turn. Effect: Every foe in range 2 and line of sight takes 1 piercing damage and is pushed 1.
action_result bullet_sonata(combat &c) {
    list<unit *> us = c.self().units_in_range(1, 2, SELECT_UNIT_NO_ALLY);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(4))
        return c.action_prevented(action_prevented::NO_SOUL);

    for (unit *u : us) {
        u->take_damage(1, DAMAGE_PIERCING, &c.self());
        u->push(c.self(), 1);
    }
    return c.action_resolved();
}


// (6 SOUL) Own turn. Trigger: Headshot. Effect: Deal 6 devil damage to targeted unit, sending them Straight to the Grave.
action_result straight_to_the_grave(combat &c)
{
    if (!c.player_may_spend_soul(6))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.activated().take_damage(6, DAMAGE_DEVIL, &c.self());
    return c.action_resolved();
}


// Gain 1 berserk token at the start of your turn. You may spend berserk tokens as speed tokens and vice versa.
action_result fueled_by_rage(combat &c)
{
    c.self().gain_token(TOKEN_BERSERK, 1);
    c.self().may_treat_token_a_as_b(TOKEN_BERSERK, TOKEN_SPEED);
    c.self().may_treat_token_a_as_b(TOKEN_SPEED, TOKEN_BERSERK);
    return c.action_resolved();
}


// Nerve Twitch: At 3 HP or lower, DF increases to 5+.
action_result nerve_twitch(combat &c)
{
    bool was = c.self().trait(TRAIT_NERVE_TWITCH);
    bool will = c.self().hp() <= 3;
    if (was == will)
        return c.action_resolved(action_resolved::PREMATURELY);

    if (will) {
        c.self().inc_trait(TRAIT_NERVE_TWITCH, +1);
        c.self().set_trait(TRAIT_ALTERED_DF, 5);
    } else {
        c.self().inc_trait(TRAIT_NERVE_TWITCH, -1);
        c.self().set_trait(TRAIT_ALTERED_DF, 0);
    }
    return c.action_resolved();
}


// Blood Drinker: Slaying a unit clears two negative tokens.
action_result blood_drinker(combat &c)
{
    int n = c.self().n_tokens(SELECT_TOKEN_NEGATIVE);
    if (!n)
        return c.action_prevented(action_prevented::NO_TARGET);

    n = std::min(2, n);
    while (n--) {
        token *t = c.player_may_select_token(c.self().tokens(), SELECT_TOKEN_NEGATIVE);
        if (!t)
            return c.action_failed();
        c.self().remove_token(t->type(), 1);
    }
    return c.action_resolved();
}


// At 3 HP or lower, gain retaliation.
action_result blood_rush(combat &c)
{
    bool was = c.self().trait(TRAIT_BLOOD_RUSH);
    bool will = c.self().hp() <= 3;
    if (was == will)
        return c.action_resolved(action_resolved::PREMATURELY);

    if (will) {
        c.self().inc_trait(TRAIT_BLOOD_RUSH, +1);
        c.self().set_trait(TRAIT_RETALIATION, +1);
    } else {
        c.self().inc_trait(TRAIT_BLOOD_RUSH, -1);
        c.self().set_trait(TRAIT_RETALIATION, -1);
    }
    return c.action_resolved();
}


// Attack, melee. On hit: 1 damage, Effect: (4+): 1 fire damage again, (6+): 1 fire damage again.
action_result superheated_chainblade(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }
    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    d6 = c.player_roll_d6(c.self());
    if (d6 >= 4)
        u->take_damage(1, DAMAGE_FIRE, &c.self());
    if (d6 >= 6)
        u->take_damage(1, DAMAGE_FIRE, &c.self());
    return c.action_resolved();
}


// Self. Effect: Convert up to 3 negative tokens on self or adjacent allies to speed or strength tokens. Then this unit becomes curseproof until the start of its next turn.
action_result quench(combat &c)
{
    list<unit *> us = c.self().units_in_range(0, 1, enum_or(SELECT_UNIT_NO_FOE, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
    list<token *> ts;
    int n = 0;
    for (unit *u : us) {
        n += u->n_tokens(SELECT_TOKEN_NEGATIVE);
        list<token *> uts = u->tokens();
        ts.insert(ts.end(), uts.begin(), uts.end());
    }

    n = min(3, n);
    while (n--) {
        token *t = c.player_may_select_token(ts, SELECT_TOKEN_NEGATIVE);
        if (!t)
            break;
        t->host().remove_token(t->type(), 1);
        optional<token_type> tt = c.player_may_select_token_type({TOKEN_SPEED, TOKEN_STRENGTH});
        if (!tt)
            return c.action_failed();
        t->host().gain_token(*tt, +1);
    }

    c.self().inc_trait(TRAIT_CURSEPROOF, +1);
    c.self().inc_trait_after(TRAIT_CURSEPROOF, -1, TRIGGER_TURN_START, 1);
    return c.action_resolved();
}


// Range 2-3. Pull a unit in range 2 spaces. Effect (3+): Pull another unit. (5+): Pull another unit.
action_result frenzy_chain(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->pull(c.self(), 2);
    us.remove(u);

    int d6 = c.player_roll_d6(c.self());
    if (d6 >= 3 && !us.empty()) {
        u = c.player_must_select_unit(us);
        if (!u)
            return c.action_failed();
        u->pull(c.self(), 2);
        us.remove(u);
    }
    if (d6 >= 5 && !us.empty()) {
        u = c.player_must_select_unit(us);
        if (!u)
            return c.action_failed();
        u->pull(c.self(), 2);
        us.remove(u);
    }
    return c.action_resolved();
}


// Self. Destroy adjacent walls. Then, self and two other adjacent units may step 1 and gain 1 speed.
action_result furious_roar(combat &c)
{
    list<map_space *> walls = c.self().spaces_in_range(1, 1, SELECT_SPACE_WALLS);
    for (map_space *wall : walls)
        wall->set_wall(false);

    list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_NO_FOE);
    us = c.player_must_select_units(us, 0, 2);
    us.push_front(&c.self());

    for (unit *u : us) {
        c.unit_step(*u, 1);
        u->gain_token(TOKEN_SPEED, 1);
    }
    return c.action_resolved();
}


// Self. Gain 1 strength and 1 berserk, then double all berserk and strength tokens.
action_result apoplexy(combat &c)
{
    c.self().gain_token(TOKEN_STRENGTH, 1);
    c.self().gain_token(TOKEN_BERSERK, 1);
    c.self().gain_token(TOKEN_BERSERK, c.self().find_token(TOKEN_BERSERK)->count());
    c.self().gain_token(TOKEN_STRENGTH, c.self().find_token(TOKEN_STRENGTH)->count());
    return c.action_resolved();
}


// Attack, melee. On hit: 2 damage, then 1 fire damage again. Effect: This attack rolls -1D.
action_result pilebunker(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK, -1);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(2, DAMAGE_PHYSICAL, &c.self());
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);
    u->take_damage(1, DAMAGE_FIRE, &c.self());
    return c.action_resolved();
}


// Line 4. Line: 1 damage. If only one unit is caught in the area, increase this damage to 2 instead and this line passes through and destroys walls.
action_result saw_toothed_slayer_axe(combat &c)
{
    list<map_space *> ps;
    list<unit *> us = c.player_must_select_line(4, &ps);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    bool single = us.size() == 1;
    int damage = single ? 2 : 1;
    for (unit *u : us)
        u->take_damage(damage, DAMAGE_PHYSICAL, &c.self());

    if (single) {
        for (map_space *p : ps)
            p->set_wall(false);
    }
    return c.action_resolved();
}


// Attack, melee. On hit: 1 fire damage. Effect: Increase this damage by +1 on hit for every 3 HP this unit is missing. If this unit is at 0 HP, this damage obliterates its target if it slays them.
action_result spine_devil_blade(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_FIRE), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int missing_hp = c.self().max_hp() - c.self().hp();
    int dmg = 1 + missing_hp / 3;
    damage_type t = (c.self().hp() == 0) ? enum_or(DAMAGE_FIRE, DAMAGE_OBLITERATE_ON_SLAY) : DAMAGE_FIRE;
    u->take_damage(dmg, t, &c.self());
    return c.action_resolved();
}


// Ignite (1 SOUL) Own or allied turn. Range 1-3: Deal 1 piercing fire damage to self or a unit in range. That damage cannot slay them. Unit gains 1 berserk and 1 speed.
action_result ignite(combat &c)
{
    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    list<unit *> us = c.self().units_in_range(0, 3);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, enum_or(DAMAGE_FIRE, DAMAGE_PIERCING, DAMAGE_CANT_SLAY), &c.self());
    u->gain_token(TOKEN_BERSERK, 1);
    u->gain_token(TOKEN_SPEED, 1);
    return c.action_resolved();
}


// (3 SOUL) Own or allied turn. Effect: Obliterate an adjacent unit at 1 HP or lower. Gain 2 strength, then step 2.
action_result glory_kill(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_WITH_HP_1_OR_LOWER);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.obliterate(*u);
    c.self().gain_token(TOKEN_STRENGTH, 2);
    c.unit_step(c.self(), 2);
    return c.action_resolved();
}


// (1 SOUL) Own or allied turn. Effect: MOVE with free movement, then deal 1 fire damage to an adjacent foe.
action_result hellrider_kick(combat &c)
{
    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.unit_move_again(c.self(), MOVEMENT_FREE);

    list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_NO_ALLY);
    if (us.empty())
        return c.action_failed();

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, DAMAGE_FIRE, &c.self());
    return c.action_resolved();
}


// (1 SOUL) Own or allied turn, range 3. Effect: Self or unit in range gains 2 speed. If the unit is at 1 HP or lower, splash (target): 1 fire damage.
action_result overclock(combat &c)
{
    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    list<unit *> us = c.self().units_in_range(0, 3);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->gain_token(TOKEN_SPEED, 2);
    if (u->hp() <= 1) {
        list<unit *> splash = u->units_in_range(1, 1);
        for (unit *u : splash)
            u->take_damage(1, DAMAGE_FIRE, &c.self());
    }
    return c.action_resolved();
}


// (2 SOUL) Any turn. Trigger: You slay a unit. Effect: Step 1. All allied units anywhere may step 1.
action_result gorebather(combat &c)
{
    if (!c.player_may_spend_soul(2))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.unit_step(c.self());

    list<unit *> us = c.self().units_in_range(1, 999, enum_or(SELECT_UNIT_IGNORE_LINE_OF_SIGHT, SELECT_UNIT_ALLY));
    us = c.player_must_select_units(us, 0, (int)us.size());
    for (unit *u : us)
        c.unit_step(*u, 1);
    return c.action_resolved();
}


// (3 SOUL) Any turn. Trigger: After a unit ACTs. Effect: Step 1, then deal 1 damage to an adjacent unit, ignoring armor. Speed can be spent to extend this step by +2. If that unit is slain, repeat this effect. It can repeat any number of times.
action_result killing_spree(combat &c)
{
    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    while (true) {
        c.unit_step(c.self(), 1, MOVEMENT_STEP_CAN_SPEND_SPEED);

        list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_NO_ALLY);
        if (us.empty())
            break;

        unit *u = c.player_must_select_unit(us);
        if (!u)
            return c.action_failed();

        u->take_damage(1, enum_or(DAMAGE_PHYSICAL, DAMAGE_PIERCING), &c.self());
        if (!u->is_slain())
            break;
    }
    return c.action_resolved();
}


// (4 SOUL) Any turn. Trigger: Unit is slain. Effect: Unit is not slain but remains at 0 HP and becomes immune to all damage. At the end of its next turn, it explodes, obliterating it, and dealing splash (self): 1 fire damage.
action_result blood_boil(combat &c)
{
    if (!c.player_may_spend_soul(4))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit &u = c.activated();

    u.set_hp(0);
    u.set_slain(false);

    u.inc_trait(TRAIT_IMMUNE_TO_ALL_DAMAGE, +1);
    u.inc_trait_after(TRAIT_IMMUNE_TO_ALL_DAMAGE, -1, TRIGGER_TURN_END, 1);

    auto explode = [](combat &c) {
        c.obliterate(c.self());
        for (unit *u : c.self().units_in_range(1, 1))
            u->take_damage(1, DAMAGE_FIRE, &c.self());
    return c.action_resolved();
    };
    u.do_after(explode, TRIGGER_TURN_END, 1);
    return c.action_resolved();
}


// (6 SOUL) Own turn. Deal 6 devil damage to an adjacent unit. Then deal 6 devil damage to yourself.
action_result devil_impact(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(6))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(6, DAMAGE_DEVIL, &c.self());
    c.self().take_damage(6, DAMAGE_DEVIL, &c.self());
    return c.action_resolved();
}


//Immune to hazards. At turn start, gain 1 plague, then distribute any number of plague tokens on this unit to any other units in range 2.
action_result blessed_with_filth(combat &c)
{
    if (c.trigger() == TRIGGER_COMBAT_START) {
        c.self().inc_trait(TRAIT_IMMUNE_TO_HAZARDS, +1);
        return c.action_resolved();
    }
    if (c.trigger() == TRIGGER_TURN_START) {
        c.self().gain_token(TOKEN_PLAGUE, 1);

        list<unit *> us = c.self().units_in_range(1, 2);
        if (us.empty())
            return c.action_failed();

        optional<int> tt = c.player_must_select_token_count(c.self().n_tokens(SELECT_TOKEN_PLAGUE));
        if (!tt)
            return c.action_failed();

        int n = *tt;
        while (n--) {
            unit *u = c.player_must_select_unit(us);
            if (!u)
                return c.action_failed();

            c.self().remove_token(TOKEN_PLAGUE, 1);
            u->gain_token(TOKEN_PLAGUE, 1);
        }
    }
    return c.action_resolved();
}


//While standing in a hazard, you have cover from all directions and are curseproof.
action_result pollution_shroud(combat &c)
{
    bool was = c.self().trait(TRAIT_POLLUTION_SHROUD);
    bool will = c.self().space()->is_hazard();
    if (will == was)
        return c.action_resolved();

    int inc = will ? +1 : -1;
    c.self().inc_trait(TRAIT_POLLUTION_SHROUD, inc);
    c.self().inc_trait(TRAIT_CURSEPROOF, inc);
    c.self().inc_trait(TRAIT_HAS_COVER_FROM_ALL_DIRECTIONS, inc);
    return c.action_resolved();
}


//After you MOVE, create a hazard in an adjacent space.
action_result corruptor(combat &c)
{
    map_space *p = c.player_must_select_space(c.self().space(), 1, SELECT_SPACE_NO_WALLS);
    if (!p)
        return c.action_failed();

    p->set_hazard(true);
    return c.action_resolved();
}


//Infect abilities that you use or that target you may jump an extra time.
action_result spreader(combat &c)
{
    c.self().inc_trait(TRAIT_INFECT_JUMP_EXTRA, 1);
    return c.action_resolved();
}


//Attack, Range 1-3. On hit: 1 damage and inflict 1 plague. Effect: Then, may immediately remove a plague token on a target to deal 1 toxic damage, again (3+) one more, (5+) one more.
action_result virulence(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    u->gain_token(TOKEN_PLAGUE, 1);

    token *t = u->find_token(TOKEN_PLAGUE);
    if (!t)
        return c.action_prevented(action_prevented::NO_TOKENS);

    if (!c.player_may_take_action(TAKE_ACTION_VIRULENCE))
        return c.action_failed();
    d6 = c.player_roll_d6(c.self());
    int n = c.effect_gradations(d6, {{3, 1}, {5, 2}});
    while(n--) {
        u->remove_token(TOKEN_PLAGUE, 1);
        u->take_damage(1, DAMAGE_TOXIC, &c.self());
    }
    return c.action_resolved();
}


// Curse, Range 1-3. Effect: Unit takes 1 toxic damage, ignoring armor. If this reduces it to 0 HP, it is obliterated and it melts, creating a hazard in its space.
action_result melt(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_FOR_CURSE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, enum_or(DAMAGE_TOXIC, DAMAGE_PIERCING, DAMAGE_OBLITERATE_ON_SLAY), &c.self());
    if (!u->is_slain())
        return c.action_failed();

    u->space()->set_hazard(true);
    return c.action_resolved();
}


// Range 1-4, Corpse. Effect: Choose a corpse in range. It explodes for a splash effect centered on it for 1 plague and push 1. Then remove it and replace it with a hazard.
action_result corpse_explosion(combat &c)
{
    map_space *p = c.player_must_select_space(c.self().space(), 1, 4, SELECT_SPACE_CORPSES);
    if (!p)
        return c.action_failed();

    list<unit *> splash = c.units_in_range(*p, 1, 1);
    for (unit *u : splash) {
        u->gain_token(TOKEN_PLAGUE, 1);
        u->push(*p, 1);
    }

    p->set_hazard(true);
    p->inc_corpses(-1);
    return c.action_resolved();
}


// Range 2-6, Corpse. Choose a corpse or a hazard in range and swap places with it. You push all adjacent units 1 and become curseproof until the end of your next turn.
action_result vermin_form(combat &c)
{
    map_space *p = c.player_must_select_space(c.self().space(), 2, 6, enum_or(SELECT_SPACE_CORPSES, SELECT_SPACE_HAZARD));
    if (!p)
        return c.action_failed();

    p->inc_corpses(-1);
    c.self().space()->inc_corpses(+1);
    c.swap_unit_pos(c.self(), *p);

    list<unit *> u = c.units_in_range(*p, 1, 1);
    for (unit *u : u)
        u->push(c.self(), 1);

    c.self().inc_trait(TRAIT_CURSEPROOF, +1);
    c.self().inc_trait_after(TRAIT_CURSEPROOF, -1, TRIGGER_TURN_END, 2);
    return c.action_resolved();
}


// Line 6. Effect: 1 plague (6+) or 2 plague. Already plagued foes take 1 toxic damage. Pull all affected units 1.
action_result ceaseless_spew(combat &c)
{
    list<unit *> us = c.player_must_select_line(6);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    int d6 = c.player_roll_d6(c.self());
    int n = (d6 >= 6) ? 2 : 1;

    for (unit *u : us) {
        if (!u->is_ally(c.self()) && u->find_token(TOKEN_PLAGUE))
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
        u->gain_token(TOKEN_PLAGUE, n);
        u->pull(c.self(), 1);
    }
    return c.action_resolved();
}


// On hit: 1 damage and Infect: 1 plague to foes and 1 strength to self or allied units. Double this effect if it slays a foe.
action_result flesh_feeder_blade(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    bool slained = !u->is_ally(c.self()) && u->is_slain();

    us = c.player_must_select_infect(*u);
    for (unit *u : us)
        u->gain_token(u->is_ally(c.self()) ? TOKEN_STRENGTH : TOKEN_PLAGUE, slained ? 2 : 1);
    return c.action_resolved();
}


// Attack, Range 2-4: On hit: 1 damage and create a hazard under target (3+) and slow them (5+) twice.
action_result scourge(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    u->space()->set_hazard(true);


    d6 = c.player_roll_d6(c.self());
    int n = c.effect_gradations(d6, {{0, 0}, {3, 1}, {5, 2}});
    u->gain_token(TOKEN_SLOW, n);
    return c.action_resolved();
}


// Attack, Range 2-3: On hit: 1 toxic damage. Infect: 1 toxic damage. Effect: After this ability resolves, gain 1 plague on self for each unit affected.
action_result necrocide(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_TOXIC), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    u->take_damage(1, DAMAGE_TOXIC, &c.self());
    int affected = 1;

    us = c.player_must_select_infect(*u);
    for (unit *u : us) {
        u->take_damage(1, DAMAGE_TOXIC, &c.self());
        affected++;
    }

    c.self().gain_token(TOKEN_PLAGUE, affected);
    return c.action_resolved();
}


// (1 SOUL) Curse, Any turn, Range 1-3. Trigger: Turn start. Effect: Grant 2 plague tokens to a unit in range.
action_result infest(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_FOR_CURSE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->gain_token(TOKEN_PLAGUE, 2);
    return c.action_resolved();
}

// (3 SOUL) Any Turn. Trigger: Turn start. Effect: MOVE with free movement. Can pass through walls and units during this movement. After the movement finishes, push or pull all units in range 2 one space (5+) two spaces.
action_result slime_form(combat &c)
{
    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.unit_move_again(c.self(), enum_or(MOVEMENT_FREE, MOVEMENT_THROUGH_FOES, MOVEMENT_THROUGH_WALLS));

    list<unit *> u = c.self().units_in_range(1, 2);
    if (u.empty())
        return c.action_failed();

    int d6 = c.player_roll_d6(c.self());
    int n = (d6 >= 5) ? 2 : 1;
    for (unit *u : u)
        u->push(c.self(), n);
    return c.action_resolved();
}


// (1 SOUL) Self or Allied turn, Range 1-3. Remove up to three plague tokens from self or allied unit and grant them to any other adjacent unit to target, then remove a negative token for each plague token removed.
action_result purge_guts(combat &c)
{
    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    list<unit *> us = c.self().units_in_range(0, 3, SELECT_UNIT_NO_FOE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    token *t = u->find_token(TOKEN_PLAGUE);
    optional<int> removed = c.player_must_select_token_count(min(3, t->count()));
    if (!removed)
        return c.action_failed();

    c.self().remove_token(t->type(), *removed);
    us = c.units_in_range(*u->space(), 1, 1);
    if (us.empty())
        return c.action_failed();

    unit *a = c.player_must_select_unit(us);
    a->gain_token(t->type(), *removed);

    if (!c.then())
        return c.action_failed();

    int n = min(*removed, u->n_tokens(enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE)));
    while (n--) {
        token *t = c.player_may_select_token(u->tokens(), enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE));
        u->remove_token(t->type(), 1);
    }
    return c.action_resolved();
}


// (1 SOUL) Curse, Any turn, Range 1-3. Trigger: Turn start. Unit gains 1 slow and creates a hazard under themselves at the end of this turn.
action_result insides_out(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_FOR_CURSE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    auto effect = [](combat &c){
        c.self().gain_token(TOKEN_SLOW);
        c.self().space()->set_hazard(true);
    return c.action_resolved();
    };
    u->do_after(effect, TRIGGER_TURN_END, 1);
    return c.action_resolved();
}


// (2 SOUL) Any turn, Range 1-3. Trigger: Turn start. Double all plague tokens on a unit. If this puts them at 5 or more tokens, they also explode with splash (target): 1 toxic damage.
action_result fecundity(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_WITH_PLAGUE_TOKENS);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(2))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int n = u->n_tokens(SELECT_TOKEN_PLAGUE);
    u->gain_token(TOKEN_PLAGUE, n);

    if (u->n_tokens(SELECT_TOKEN_PLAGUE) >= 5) {
        list<unit *> splash = u->units_in_range(1, 1);
        for (unit *u : splash)
            u->take_damage(1, DAMAGE_TOXIC, &c.self());
    }
    return c.action_resolved();
}

// (3 SOUL) Curse, Any turn, Range 1-3. Trigger: Target turn start. At the end of the targeted unit's turn, targeted unit takes 1 piercing toxic damage for every hazard in range 2 of them.
action_result unholy_vapors(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_FOR_CURSE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    auto effect = [](combat &c) {
        int n = c.self().spaces_in_range(0, 2, SELECT_SPACE_HAZARD).size();
        c.self().take_damage(1, DAMAGE_PIERCING, nullptr);
    return c.action_resolved();
    };
    u->do_after(effect, TRIGGER_TURN_END, 1);
    return c.action_resolved();
}


// (4 SOUL) Own turn, Range 1-2. Place one hazard in range for every other hazard in range.
action_result praise_the_filth(combat &c)
{
    list<map_space *> ps = c.self().spaces_in_range(1, 2, SELECT_SPACE_HAZARD);
    int n = ps.size();
    if (!n)
        return c.action_prevented(action_prevented::NO_HAZARD);

    if (!c.player_may_spend_soul(4))
        return c.action_prevented(action_prevented::NO_SOUL);

    while (n--) {
        map_space *p = c.player_must_select_space(c.self().space(), 1, 2, SELECT_SPACE_NO_WALLS);
        if (!p)
            break;
        p->set_hazard(true);
    }
    return c.action_resolved();
}


// (6 SOUL) Own turn. Remove up to four corpse tokens or hazards and summon a SCUM unit for each. Then splash (target) on all scum created this way for 1 toxic damage to foes. Characters can only be damaged once by this effect.
void scum(unit_card &c);
action_result mox_populi(combat &c)
{
    list<map_space *> ps = c.self().spaces_in_range(0, 999, enum_or(SELECT_SPACE_CORPSES, SELECT_SPACE_HAZARD, SELECT_SPACE_IGNORE_LINE_OF_SIGHT));
    int n = 0;
    for (map_space *p : ps)
        n += p->n_corpses() + 1 * p->is_hazard();

    if (!n)
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(6))
        return c.action_prevented(action_prevented::NO_SOUL);

    int removed = 0;
    while (removed < min(4, n)) {
        map_space *p = c.player_must_select_space(c.self().space(), 0, 999, enum_or(SELECT_SPACE_CORPSES, SELECT_SPACE_HAZARD, SELECT_SPACE_IGNORE_LINE_OF_SIGHT));
        if (!p)
            return c.action_failed();
        if (p->n_corpses()) {
            removed++;
            p->inc_corpses(-1);
            continue;
        }
        if (p->is_hazard()) {
            removed++;
            p->set_hazard(false);
            continue;
        }
    }

    set<unit *> splash;
    ps = c.player_must_select_spaces(c.self().space(), min(4, removed), 1, 999, enum_or(SELECT_SPACE_FREE, SELECT_SPACE_IGNORE_LINE_OF_SIGHT));
    for (map_space *p : ps) {
        c.summon(*p, scum);

        list<unit *> us = c.units_in_range(*p, 1, 1, SELECT_UNIT_FOE);
        splash.insert(us.begin(), us.end());
    }

    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    list<unit *> us(splash.begin(), splash.end());
    us.sort();
    for (unit *u : us)
        u->take_damage(1, DAMAGE_TOXIC, &c.self());
    return c.action_resolved();
}


// May move through walls. Foes count as isolated when adjacent to this unit unless they have two or more allies adjacent
action_result dread_presence(combat &c)
{
    c.self().inc_trait(TRAIT_MOVEMENT_THROUGH_WALLS, +1);
    c.self().inc_trait(TRAIT_ADJACENT_FOES_COUNT_ISOLATED_UNLESS_2_ALLIES, +1);
    return c.action_resolved();
}


action_result body_block(combat &c)
{
    c.self().set_trait(TRAIT_BODY_BLOCK, 1);
    return c.action_resolved();
}


// May use body block from within range 2 instead of adjacent.
action_result puppet_master(combat &c)
{
    c.self().set_trait(TRAIT_BODY_BLOCK, 2);
    return c.action_resolved();
}


// +1D on attacks and ignores cover against Doomed units.
action_result maggot_destroyer(combat &c)
{
    c.self().set_trait(TRAIT_MAGGOT_DESTROYER, 1);
    return c.action_resolved();
}


// Attack, Range 1-2. // On hit: 1 curse damage, or 2 curse damage against Doomed foes, then (Effect: 3+): foe is Doomed.
action_result doomblade(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 2);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    bool maggot = c.self().trait(TRAIT_MAGGOT_DESTROYER) && u->find_token(TOKEN_DOOM);
    roll_tag tag = maggot ? enum_or(ROLL_TAG_ATTACK, ROLL_TAG_IGNORE_COVER) : ROLL_TAG_ATTACK;
    int mod = maggot ? 1 : 0;
    int d6 = c.player_roll_d6(c.self(), tag, mod);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_CURSE, DAMAGE_GRAZE), &c.self());
    return c.action_resolved();
    }

    int dmg = u->find_token(TOKEN_DOOM) ? 2 : 1;
    u->take_damage(2, DAMAGE_CURSE, &c.self());

    d6 = c.player_roll_d6(c.self());
    if (d6 >= 3)
        u->gain_token(TOKEN_DOOM);
    return c.action_resolved();
}


// Curse, Range 2-4. Effect: Pull 2. Ignores line of sight. If the unit is an ally, may then pull another allied unit and clear a token on both allies.
action_result unholy_summoning(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 4, enum_or(SELECT_UNIT_FOR_CURSE, SELECT_UNIT_IGNORE_LINE_OF_SIGHT));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    if (!u->is_ally(c.self()))
        return c.action_resolved();

    u->pull(c.self(), 2);

    list<unit *> allies = {u};

    us = c.self().units_in_range(2, 4, enum_or(SELECT_UNIT_FOR_CURSE, SELECT_UNIT_IGNORE_LINE_OF_SIGHT, SELECT_UNIT_NO_FOE));
    us.remove(u);
    u = c.player_may_select_unit(us);
    if (u) {
        u->pull(c.self(), 2);
        allies.push_back(u);
    }

    for (unit *u : allies) {
        if (!u->n_tokens(enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE)))
            continue;
        token *t = c.player_must_select_token(u->tokens(), enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE));
        if (!t)
            return c.action_failed();
        u->remove_token(t->type(), 1);
    }
    return c.action_resolved();
}


// Self. MOVE with free movement, moving through walls and units. Foes passed through gain 1 weak. Doomed foes take 1 curse damage.
action_result vapor_form(combat &c)
{
    list<map_space *> ps = c.unit_move_again(c.self(), enum_or(MOVEMENT_FREE, MOVEMENT_THROUGH_WALLS, MOVEMENT_THROUGH_FOES));

    for (map_space *p : ps) {
        unit *u = p->unit_standing();
        if (!u || u->is_ally(c.self()))
            continue;
        u->gain_token(TOKEN_WEAK);
        if (u->find_token(TOKEN_DOOM))
            u->take_damage(1, DAMAGE_CURSE, &c.self());
    }
    return c.action_resolved();
}


// Tear Soul: Curse, Range 1-3. Effect: Unit takes 1 curse damage. Then, it gains 1 weak for every 1 HP it is missing. If it’s missing more than half its HP, it is then Doomed.
action_result tear_soul(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3, SELECT_UNIT_FOR_CURSE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->take_damage(1, DAMAGE_CURSE, &c.self());
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    int missing_hp = u->max_hp() - u->hp();
    u->gain_token(TOKEN_WEAK, missing_hp);

    if (missing_hp > u->max_hp() / 2)
        u->gain_token(TOKEN_DOOM);
    return c.action_resolved();
}


// Range 1-2. Create up to three spaces of adverse terrain in range. Units standing in adverse terrain in range take 1 piercing damage.
action_result frozen_hell(combat &c)
{
    list<map_space *> ps = c.player_must_select_spaces(c.self().space(), 3, 1, 2, enum_or(SELECT_SPACE_NO_WALLS, SELECT_SPACE_NO_ADVERSE_TERRAIN));
    for (map_space *p : ps)
        p->set_adverse_terrain(true);

    list<unit *> us = c.self().units_in_range(1, 2);
    for (unit *u : us) {
        if (u->space()->is_adverse_terrain())
            u->take_damage(1, DAMAGE_PIERCING, &c.self());
    }
    return c.action_resolved();
}


// Attack, melee. On hit: 1 damage. Effect: At round 4 or later, deals 4 curse damage on hit instead.
action_result great_urgal_blade(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    bool maggot = c.self().trait(TRAIT_MAGGOT_DESTROYER) && u->find_token(TOKEN_DOOM);
    roll_tag tag = maggot ? enum_or(ROLL_TAG_ATTACK, ROLL_TAG_IGNORE_COVER) : ROLL_TAG_ATTACK;
    int mod = maggot ? 1 : 0;
    int d6 = c.player_roll_d6(c.self(), tag, mod);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_PHYSICAL, DAMAGE_GRAZE), &c.self());
    return c.action_resolved();
    }

    if (c.round() >= 4 && !u->is_wall()) {
        u->take_damage(4, DAMAGE_CURSE, &c.self());
        return c.action_resolved();
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    return c.action_resolved();
}


// Attack, Range 2-2. On hit: 1 damage. Effect: Pull all foes in range 1 space, then splash (self): 1 curse damage. Against doomed units, damage cannot be reduced in any way.
action_result bloody_flail(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 2);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    bool maggot = c.self().trait(TRAIT_MAGGOT_DESTROYER) && u->find_token(TOKEN_DOOM);
    roll_tag tag = maggot ? enum_or(ROLL_TAG_ATTACK, ROLL_TAG_IGNORE_COVER) : ROLL_TAG_ATTACK;
    int mod = maggot ? 1 : 0;
    int d6 = c.player_roll_d6(c.self(), tag, mod);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_PHYSICAL, DAMAGE_GRAZE), &c.self());
    return c.action_resolved();
    }

    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    if (u->is_wall())
        return c.action_resolved();

    us = c.self().units_in_range(2, 2, SELECT_UNIT_FOE);
    for (unit *u : us)
        u->pull(c.self(), 1);

    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    us = c.self().units_in_range(1, 1);
    for (unit *u : us) {
        damage_type t = u->find_token(TOKEN_DOOM) ? enum_or(DAMAGE_CURSE, DAMAGE_CANT_BE_REDUCED) : DAMAGE_CURSE;
        u->take_damage(1, t, &c.self());
    }
    return c.action_resolved();
}


// Attack, Range 2-3. On hit: 1 curse damage. Effect: Cannot benefit from strength. Instead, may remove up to three weak tokens from target to increase curse damage by +1 per weak token removed.
action_result lathean_devil_whip(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    bool maggot = c.self().trait(TRAIT_MAGGOT_DESTROYER) && u->find_token(TOKEN_DOOM);
    roll_tag tag = maggot ? enum_or(ROLL_TAG_ATTACK, ROLL_TAG_IGNORE_COVER) : ROLL_TAG_ATTACK;
    int mod = maggot ? 1 : 0;
    int d6 = c.player_roll_d6(c.self(), tag, mod);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_CURSE, DAMAGE_GRAZE), &c.self());
    return c.action_resolved();
    }

    if (u->is_wall()) {
        u->take_damage(1, DAMAGE_CURSE, &c.self());
        return c.action_resolved();
    }

    int dmg = 1;
    if (u->find_token(TOKEN_WEAK)) {
        int max = min(3, u->find_token(TOKEN_WEAK)->count());
        optional<int> removed = c.player_must_select_token_count(max);
        if (removed)
            return c.action_failed();
        u->remove_token(TOKEN_WEAK, *removed);
        dmg += *removed;
    }
    u->take_damage(dmg, enum_or(DAMAGE_CURSE, DAMAGE_CANT_BENEFIT_FROM_STRENGTH), &c.self());
    return c.action_resolved();
}


// (1 SOUL) Own or Allied turn. Range 2-3. Trigger: Start of turn. Create a wall in range and inflict 1 weak on adjacent foes to the wall.
action_result cyclopean_monolith(combat &c)
{
    if (c.self().spaces_in_range(2, 3, SELECT_SPACE_FREE).empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    map_space *p = c.player_must_select_space(c.self().space(), 2, 3, SELECT_SPACE_FREE);
    if (!p)
        return c.action_failed();

    p->set_wall(true);
    for (unit *u : c.units_in_range(*p, 1, 1, SELECT_UNIT_FOE))
        u->gain_token(TOKEN_WEAK, 1);
    return c.action_resolved();
}


// (3 SOUL) Self or Allied Turn. Range 1-2. All isolated or doomed foes in range take 1 curse damage, gain 1 weak, and this unit gains 1 strength per such foe.
action_result soulfeed(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 2, SELECT_UNIT_FOE);
    int n = 0;
    for (unit *u : us)
        n += u->is_isolated() || u->find_token(TOKEN_DOOM);
    if (!n)
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    for (unit *u : us) {
        if (!u->is_isolated() && !u->find_token(TOKEN_DOOM))
            continue;

        u->take_damage(1, DAMAGE_CURSE, &c.self());
        u->gain_token(TOKEN_WEAK);
    }
    c.self().gain_token(TOKEN_STRENGTH, n);
    return c.action_resolved();
}


// (1 SOUL) Curse, Any turn, Range 2-4. Trigger: Start of turn. Pull unit 1 in any direction.
action_result twist_sinews(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.unit_step(*u, 1, MOVEMENT_FORCED);
    return c.action_resolved();
}

// (1 SOUL) Foe turn, Curse, Range 1-4. Trigger: Turn start. At the end of their turn, foe inflicts splash(self) 1 curse damage and 1 weak, only affecting their allies.
action_result writhing_curse(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 4, enum_or(SELECT_UNIT_FOE, SELECT_UNIT_FOR_CURSE));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    auto splash = [](combat &c){
        list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_ALLY);
        for (unit *u : us) {
            u->take_damage(1, DAMAGE_CURSE, nullptr);
            u->gain_token(TOKEN_WEAK);
        }
    return c.action_resolved();
    };
    u->do_after(splash, TRIGGER_TURN_END, 1);
    return c.action_resolved();
}


// (2 SOUL) Own or allied turn. Range 1-4. Remove self and an adjacent allied unit, then place self in a free space in range 3, then place ally adjacent. If there is no room to place allies, return them at their original location.
action_result disincorporate(combat &c)
{
    if (!c.player_may_spend_soul(2))
        return c.action_prevented(action_prevented::NO_SOUL);

    map_space *p = c.self().space();
    map_space *dst = c.player_must_select_space(p, 1, 4, SELECT_SPACE_FREE);
    if (!dst)
        return c.action_failed();

    c.swap_unit_pos(c.self(), *dst);
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    list<unit *> us = c.units_in_range(*p, 1, 1, SELECT_UNIT_ALLY);
    bool no_ally_tp = us.empty();
    bool no_dst_free = c.self().spaces_in_range(1, 1, SELECT_SPACE_FREE).empty();
    if (no_ally_tp || no_dst_free)
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_may_select_unit(us);
    if (u)
        return c.action_failed();
    dst = c.player_must_select_space(dst, 1, 1, SELECT_SPACE_FREE);
    if (!dst)
        return c.action_failed();

    c.swap_unit_pos(*u, *dst);
    return c.action_resolved();
}


// (3 SOUL) Any turn. Trigger: A unit is slain. Transfer all negative tokens to a different unit anywhere, then increase those tokens by 1.
action_result eternal_curse(combat &c)
{
    if (!c.activated().n_tokens(SELECT_TOKEN_NEGATIVE))
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit &u = c.activated();

    list<unit *> us = c.self().units_in_range(0, 999);
    us.remove(&u);

    unit *dst = c.player_must_select_unit(us);
    if (!dst)
        return c.action_failed();

    for (token *t : u.tokens()) {
        if (t->is_negative()) {
            int removed = t->count();
            u.remove_token(t->type(), removed);
            dst->gain_token(t->type(), removed + 1);
        }
    }
    return c.action_resolved();
}


// (4 SOUL) Own turn. Scour the battlefield with frozen wind, pulling all foes 1 space in the same direction. Foes that would be pulled into walls or adverse terrain take 1 damage.
action_result malebolge(combat &c)
{
    if (!c.player_may_spend_soul(4))
        return c.action_prevented(action_prevented::NO_SOUL);

    optional<direction> d = c.player_must_select_direction();
    if (!d)
        return c.action_failed();

    list<unit *> us = c.self().units_in_range(1, 999, enum_or(SELECT_UNIT_FOE, SELECT_UNIT_IGNORE_LINE_OF_SIGHT));
    for (unit *u : us) {
        map_space *dst = u->space()->adjacent(*d);
        if (!dst)
            continue;
        bool dmg = dst->is_wall() || dst->is_adverse_terrain();
        if (dst->is_free())
            c.swap_unit_pos(*u, *dst, MOVEMENT_FORCED);
        if (dmg)
            u->take_damage(1, DAMAGE_PHYSICAL, &c.self());
    }
    return c.action_resolved();
}


// (6 SOUL) Own turn, Range 1-2. Doom all enemy units in range. All Doomed enemy units instead take 1 curse damage, +1 per Doom token they have.
action_result great_satania(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 2, SELECT_UNIT_NO_ALLY);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (!c.player_may_spend_soul(6))
        return c.action_prevented(action_prevented::NO_SOUL);

    for (unit *u : us) {
        token *t = u->find_token(TOKEN_DOOM);
        if (!t) {
            u->gain_token(TOKEN_DOOM);
            continue;
        }
        u->take_damage(1 + t->count(), DAMAGE_CURSE, &c.self());
    }
    return c.action_resolved();
}


// Lacks Miracle. However, while alive, miracle triggers for adjacent allies on a 4+, and gain 1 SOUL the first time it triggers in a round.
action_result holy_vessel(combat &c)
{
    if (c.trigger() == TRIGGER_COMBAT_START)
        c.self().inc_trait(TRAIT_HOLY_VESSEL, +1);
    if (c.trigger() == TRIGGER_ROUND_START)
        c.self().set_trait(TRAIT_HOLY_VESSEL_READY, 1);
    return c.action_resolved();
}

// Stance: (Round 3+): Attacks cannot miss (all misses turn into hits).
action_result winter_rose_stance(combat &c)
{
    if (c.round(3))
        c.self().set_trait(TRAIT_WINTER_ROSE_STANCE, 1);
    return c.action_resolved();
}


// Corpse Violet Stance: Stance: (Round 3+): Gain either PHYS or MAG armor (choose).
action_result corpse_violet_stance(combat &c)
{
    if (c.round() == 3) {
        optional<armor> a = c.player_must_select_armor({ARMOR_MAG, ARMOR_PHYS});
        if (!a)
            return c.action_failed();
        c.self().inc_trait(*a == ARMOR_PHYS ? TRAIT_PHYSICAL_ARMOR : TRAIT_MAGICAL_ARMOR, +1);
    }
    return c.action_resolved();
}

// Stance (Round 3+): May step 2 after ACTing.
action_result royal_chrysanthemum_stance(combat &c)
{
    if (c.round(3) && c.player_may_take_action(TAKE_ACTION_STEP))
        c.unit_step(c.self(), 2);
    return c.action_resolved();
}


// Attack, melee. On hit: 1 damage. Stance (Odd): and step 2, Stance (Even): and gain 1 vitality, Stance (Round 5+): Increase damage to 3.
action_result starmetal_godsword(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.self().trait(TRAIT_WINTER_ROSE_STANCE) ? 6 : c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_PHYSICAL, DAMAGE_GRAZE), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int dmg = c.round(5) ? 3 : 1;
    u->take_damage(dmg, DAMAGE_PHYSICAL, &c.self());
    if (c.round_even())
        u->gain_token(TOKEN_VITALITY);
    else
        c.unit_step(c.self(), 2);
    return c.action_resolved();
}


// Self. Effect: Splash (self). Self and allies in the area may convert any number of vitality to strength.
action_result grand_oath(combat &c)
{
    list<unit *> us = c.self().units_in_range(0, 1, SELECT_UNIT_NO_FOE);
    list<token *> ts;
    for (unit *u : us) {
        token *t = u->find_token(TOKEN_VITALITY);
        if (t)
            ts.push_back(t);
    }

    ts = c.player_must_select_tokens(ts, ts.size());
    for (token *t : ts) {
        unit &u = t->host();
        u.remove_token(t->type(), t->count());
        u.gain_token(TOKEN_STRENGTH, t->count());
    }
    return c.action_resolved();
}


// Range 1-4. Effect: Remove all negative tokens on an ally, then transfer to self. That ally gains 1 vitality per negative token removed and becomes curseproof until the start of its next turn.
action_result redempta(combat &c)
{
     list<unit *> us = c.self().units_in_range(1, 4, enum_or(SELECT_UNIT_NO_FOE, SELECT_UNIT_WITH_NEGATIVE_TOKENS));
     if (us.empty())
         return c.action_prevented(action_prevented::NO_TARGET);
     unit *u = c.player_must_select_unit(us);
     if (!u)
         return c.action_failed();
     list<token *> ts = u->tokens();
     ts = c.player_must_select_tokens(ts, ts.size(), SELECT_TOKEN_NEGATIVE);
     for (token *t : ts) {
         int removed = t->count();
         u->remove_token(t->type(), removed);
         u->gain_token(TOKEN_VITALITY, removed);
         c.self().gain_token(t->type(), removed);
     }
     u->inc_trait(TRAIT_CURSEPROOF, +1);
     u->inc_trait_after(TRAIT_CURSEPROOF, -1, TRIGGER_TURN_START, 1);
     return c.action_resolved();
}


// Self. Effect: Gain 1 vitality. Until end of next turn, self and adjacent allies may spend this unit's HP as if it were vitality tokens.
action_result communion(combat &c)
{
    c.self().inc_trait(TRAIT_COMMUNION, +1);
    c.self().inc_trait_after(TRAIT_COMMUNION, -1, TRIGGER_TURN_END, 2);
    return c.action_resolved();
}


// Curse, Self. Effect: Until end of next turn, foes ending their turn in range 2 of this unit have a hazard created under them, then are pushed 1.
action_result decree_of_forbiddance(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 999, enum_or(SELECT_UNIT_FOE, SELECT_UNIT_FOR_CURSE, SELECT_UNIT_IGNORE_LINE_OF_SIGHT));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);
    for (unit *u : us) {
        const auto foo = [](combat &c) {
            list<unit *> us = c.self().units_in_range(1, 2, SELECT_UNIT_FOE);
            for (unit *u : us) {
                c.self().space()->set_hazard(true);
                c.self().push(*u, 1);
            }
        return c.action_resolved();
        };
        u->do_after(foo, TRIGGER_TURN_END, 1);
    }
    c.self().inc_trait(TRAIT_DECREE_OF_FORBIDDANCE, +1);
    c.self().inc_trait_after(TRAIT_DECREE_OF_FORBIDDANCE, -1, TRIGGER_TURN_END, 2);
    return c.action_resolved();
}


// Attack, Range 2-4, charge. On hit: 1 holy damage and splash (target): 1 holy damage. Stance (Round 3+): and create a hazard under target (round 5+) all targets in the area.
action_result holy_water_flail(combat &c)
{
    if (c.round() == 1)
        return c.action_prevented(action_prevented::CHARGE);

    list<unit *> us = c.self().units_in_range(2, 4);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.self().trait(TRAIT_WINTER_ROSE_STANCE) ? 6 : c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_HOLY), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }
    u->take_damage(1, DAMAGE_HOLY, &c.self());
    us = u->units_in_range(1, 1);
    if (c.round(3))
        u->space()->set_hazard(true);
    for (unit *u : us) {
        u->take_damage(1, DAMAGE_HOLY, &c.self());
        if (c.round(5))
            u->space()->set_hazard(true);
    }
    return c.action_resolved();
}


// Attack, melee. On hit: 2 damage. Stance: May step spaces equal to the round number with free movement, in a straight line, before the attack. On hit, push target half that many spaces.
action_result relic_lance(combat &c)
{
    bool stepped = false;
    int steps = c.round();
    if (c.player_may_take_action(TAKE_ACTION_STEP)) {
        c.unit_step(c.self(), steps, enum_or(MOVEMENT_FREE, MOVEMENT_STRAIGHT_LINE));
        stepped = true;
    }

    list<unit *> us = c.self().units_in_range(2, 4);
    if (us.empty()) {
        if (stepped)
            return c.action_resolved(action_resolved::PREMATURELY);
        return c.action_prevented(action_prevented::NO_TARGET);
    }

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.self().trait(TRAIT_WINTER_ROSE_STANCE) ? 6 : c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }
    u->take_damage(2, DAMAGE_PHYSICAL, &c.self());

    int half = (steps + steps % 2) / 2;
    u->push(c.self(), half);
    return c.action_resolved();
}


// Attack, Range 2-5. On hit: 1 damage. Stance: Roll the effect die. If you roll under the round number, deal 1 holy damage again. If you roll exactly the round number, deal 2 holy damage again instead.
action_result starmetal_shuriken(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    int d6 = c.self().trait(TRAIT_WINTER_ROSE_STANCE) ? 6 : c.player_roll_d6(c.self(), ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }
    u->take_damage(1, DAMAGE_PHYSICAL, &c.self());

    d6 = c.player_roll_d6(c.self());
    if (d6 < c.round())
        u->take_damage(1, DAMAGE_HOLY, &c.self());
    else if (d6 == c.round())
        u->take_damage(2, DAMAGE_HOLY, &c.self());
    return c.action_resolved();
}


// (1 SOUL) Any turn, Range 2-6. Trigger: Turn start. Effect: Self takes 1 piercing damage, which cannot slay self. One allied unit in range gains one, (3+) two, or (5+) three vitality.
action_result absolution(combat &c)
{
    list<unit *> us = c.self().units_in_range(2, 6, SELECT_UNIT_ALLY);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    c.self().take_damage(1, enum_or(DAMAGE_PIERCING, DAMAGE_CANT_SLAY), &c.self());

    int d6 = c.player_roll_d6(c.self());
    int n = c.effect_gradations(d6, {{0, 1}, {3, 2}, {5, 2}});
    u->gain_token(TOKEN_VITALITY, n);
    return c.action_resolved();
}


// (3 SOUL) Own or Allied Turn, Range 1-3. Trigger: Turn start. Effect: For this turn only, self or allied unit may treat the current round number as either 2 or 5.
action_result will_of_god(combat &c)
{
    list<unit *> us = c.self().units_in_range(0, 3, SELECT_UNIT_NO_FOE);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    optional<take_action> a =c.player_must_select_action({TAKE_ACTION_STANCE_2, TAKE_ACTION_STANCE_5});
    if (!a)
        return c.action_failed();

    trait_id t = *a == TAKE_ACTION_STANCE_2 ? TRAIT_STANCE_2 : TRAIT_STANCE_5;
    u->inc_trait(t, +1);
    u->inc_trait_after(t, -1, trigger_type::TRIGGER_TURN_END, 1);
    return c.action_resolved();
}


// (1 SOUL) Any turn. Trigger: A unit MOVEs, and that move resolves. Effect: Stance: Step spaces equal to 1 + the round number, then push an adjacent unit 1.
action_result sword_art_drifting_blossoms(combat &c)
{
    if (c.player_may_spend_soul(1))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.unit_step(c.self(), 1 + c.round());

    list<unit *> us = c.self().units_in_range(1, 1);
    if (!us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->push(c.self(), 1);
    return c.action_resolved();
}


// (2 SOUL) Own or allied turn. Line, Stance: Step 1, then cut a line area with spaces equal to the round number +2, dealing 1 damage to all characters within and pushing those characters 1. Stance (Round 5+): This damage becomes Devil Damage.
action_result sword_art_safflower_cut(combat &c)
{
    if (c.player_may_spend_soul(2))
        return c.action_prevented(action_prevented::NO_SOUL);

    c.unit_step(c.self(), 1);
    list<unit *> us = c.player_must_select_line(c.round() + 2);
    bool devil = c.round(5);
    for (unit *u : us) {
        damage_type t = devil ? DAMAGE_DEVIL : DAMAGE_PHYSICAL;
        u->take_damage(1, t, &c.self());
        u->push(c.self(), 1);
    }
    return c.action_resolved();
}


// (2 SOUL) Own or allied turn, Range 1-3. Trigger: Turn start. Effect: Unit becomes curseproof until end of its next turn and attacks against it gain -1D for the duration.
action_result grace(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 3);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (c.player_may_spend_soul(2))
        return c.action_prevented(action_prevented::NO_SOUL);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->inc_trait(TRAIT_CURSEPROOF, +1);
    u->inc_trait_after(TRAIT_CURSEPROOF, +1, TRIGGER_TURN_END, 1);

    u->inc_trait(TRAIT_MINUS_1D_FROM_ALL_ATTACKS, +1);
    u->inc_trait_after(TRAIT_MINUS_1D_FROM_ALL_ATTACKS, -1, TRIGGER_TURN_END, 1);
    return c.action_resolved();
}


// (3 SOUL) Any turn. Trigger: This unit takes damage from an ACT ability, and that ability resolves. Effect: Splash (self): remove 1 negative token on self or gain 1 vitality for every foe in the splash area. Stance (Round 4+): Both effects trigger.
action_result sword_art_winter_sprout(combat &c)
{
    if (c.player_may_spend_soul(3))
        return c.action_prevented(action_prevented::NO_SOUL);

    int n = c.self().units_in_range(1, 1, SELECT_UNIT_FOE).size();
    if (!n)
        return c.action_prevented(action_prevented::NO_TARGET);

    bool rm = c.self().n_tokens(enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE))
        && c.player_may_take_action(TAKE_ACTION_REMOVE_NEGATIVE_TOKEN);
    if (rm) {
        while (n-- && c.self().n_tokens(enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE))) {
            token *t = c.player_may_select_token(c.self().tokens(), enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE));
            if (!t)
                return c.action_failed();
            c.self().remove_token(t->type(), 1);
        }
    }
    if (!rm || c.round(4))
        c.self().gain_token(TOKEN_VITALITY, n);
    return c.action_resolved();
}


// (4 SOUL) Own Turn. Trigger: You hit an attack. Effect: Splash (self): 1 holy damage. Stance (R3+): 2 holy damage (R5+): 4 holy damage.
action_result supreme_sword_art_merciful_snowdrop_cut(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1);
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    if (c.player_may_spend_soul(4))
        return c.action_prevented(action_prevented::NO_SOUL);

    int dmg = 1;
    if (c.round(3))
        dmg = 2;
    if (c.round(5))
        dmg = 4;
    for (unit *u : us)
        u->take_damage(dmg, DAMAGE_HOLY, &c.self());
    return c.action_resolved();
}


// (6 SOUL) Any turn. Trigger: Allied unit is slain. Effect: Return unit to life at 1 HP at the end of the turn. It becomes immune to all damage and curseproog until the start of its next turn. Stance (Round 3+): and it gains 1 vitality (Round 5+) 3 vitality instead.
action_result resurrection(combat &c)
{
    unit *u = &c.activated();
    if (u->is_ally(c.self()))
        return c.action_prevented(action_prevented::NO_TARGET);

    if (c.player_may_spend_soul(6))
        return c.action_prevented(action_prevented::NO_SOUL);

    auto resurrect = [](combat &c) {
        c.self().set_slain(false);
        c.self().set_hp(1);

        c.self().inc_trait(TRAIT_CURSEPROOF, +1);
        c.self().inc_trait_after(TRAIT_CURSEPROOF, -1, TRIGGER_TURN_START, 1);

        c.self().inc_trait(TRAIT_IMMUNE_TO_ALL_DAMAGE, +1);
        c.self().inc_trait_after(TRAIT_IMMUNE_TO_ALL_DAMAGE, -1, TRIGGER_TURN_START, 1);

        int n = 0;
        if (c.round(3))
            n = 1;
        if (c.round(5))
            n = 3;
        c.self().gain_token(TOKEN_VITALITY, n);
    return c.action_resolved();
    };
    u->do_after(resurrect, TRIGGER_AFTER_UNIT_TURN_END, 1);
    return c.action_resolved();
}


// Attack, melee. On hit: 1 damage. Rip Apart (3+): with piercing.
action_result bloodletting(combat &c)
{
    list<unit *> units = c.self().units_in_range(1, 1);
    if (units.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(units);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(*u, ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    damage_type t = u->rip_apart_for(3) ? enum_or(DAMAGE_PIERCING, DAMAGE_PHYSICAL) : DAMAGE_PHYSICAL;
    u->take_damage(1, t, &c.self());

    if (c.self().has_upgrade(UPGRADE_THE_RED_WET_HUE) && u->rip_apart_for(5))
        u->take_damage(1, DAMAGE_CURSE, &c.self());
    return c.action_resolved();
}


action_result blood_frenzy(combat &c)
{
    if (c.self().has_upgrade(UPGRADE_BLOOD_FRENZY))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    bool was = c.self().trait(TRAIT_BLOOD_FRENZY);
    bool will = c.round(5);
    if (will == was)
        return c.action_resolved();

    int inc = will ? +1 : -1;
    c.self().inc_trait(TRAIT_BLOOD_FRENZY, inc);
    c.self().inc_trait(TRAIT_IMMUNE_TO_GRAZE_DAMAGE, inc);
    c.self().inc_trait(TRAIT_IMMUNE_TO_LINE_DAMAGE, inc);
    c.self().inc_trait(TRAIT_IMMUNE_TO_SPLASH_DAMAGE, inc);
    return c.action_resolved();
}


// Once a round, at the end of own or an allied turn, a single steeplewrack unit may MOVE for free.
action_result dive_bomb(combat &c)
{
    if (c.trigger() == TRIGGER_ROUND_START) {
        c.self().set_trait(TRAIT_DIVE_BOMB_CHARGE, 1);
        c.self().set_trait(TRAIT_DIVE_BOMB_RABID_CHARGE, 1);
        return c.action_resolved();
    }
    if (c.trigger() != TRIGGER_TURN_END)
        return c.action_unreachable();

    list<unit *> us = c.self().units_in_range(0, 999, enum_or(SELECT_UNIT_NO_FOE, SELECT_UNIT_IGNORE_LINE_OF_SIGHT));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    bool rabid = any_of(us.begin(), us.end(), [](const unit *u) { return u->has_upgrade(UPGRADE_RABID); });
    bool charge = c.self().trait(TRAIT_DIVE_BOMB_CHARGE);
    bool charge_rabid = c.self().trait(TRAIT_DIVE_BOMB_RABID_CHARGE);
    bool has_charge =
        (!rabid && charge) || (rabid && (charge || charge_rabid));

    if (!has_charge)
        return c.action_prevented(action_prevented::ALREADY_USED);

    if (!c.player_may_take_action(TAKE_ACTION_DIVE_BOMB))
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    rabid = u->has_upgrade(UPGRADE_RABID);
    trait_id spent = rabid && charge_rabid ? TRAIT_DIVE_BOMB_RABID_CHARGE : TRAIT_DIVE_BOMB_CHARGE;
    c.self().inc_trait(spent, -1);

    c.unit_move_again(*u);
    return c.action_resolved();
}


// Self. Gain 1 speed or (6+) 2 speed. May immediately spend 2 speed to summon a Flock adjacent.
void flock(unit_card &);
action_result call_to_feast(combat &c)
{
    int n = c.player_roll_d6(c.self()) >= 6 ? 2 : 1;
    c.self().gain_token(TOKEN_SPEED, n);

    list<map_space *> ps = c.self().spaces_in_range(0, 1, SELECT_SPACE_FREE);
    token *t = c.self().find_token(TOKEN_SPEED);
    if (!t || t->count() < 2 || ps.empty())
        return c.action_resolved(action_resolved::PREMATURELY);

    if (!c.player_may_take_action(TAKE_ACTION_SUMMON_FLOCK))
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

    map_space *p = c.player_must_select_space(ps);
    if (!p)
        return c.action_failed();

    c.self().remove_token(TOKEN_SPEED, 2);
    c.summon(*p, flock);
    return c.action_resolved();
}


// Attack, melee. On hit: 1 damage. Rip Apart (3+): +1 damage and gain 1 speed. Rip Apart (5+): gain 1 strength and summon a Flock adjacent to target.
action_result disembowel(combat &c)
{
    list<unit *> units = c.self().units_in_range(1, 1);
    if (units.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(units);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(*u, ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    bool ra3 = u->rip_apart_for(3);
    int dmg = ra3 ? 2 : 1;
    u->take_damage(dmg, DAMAGE_PHYSICAL, &c.self());
    if (ra3)
        c.self().gain_token(TOKEN_SPEED);

    if (u->rip_apart_for(5)) {
        c.self().gain_token(TOKEN_STRENGTH);
        list<map_space *> ps = u->spaces_in_range(0, 1, SELECT_SPACE_FREE);
        if (!ps.empty()) {
            map_space *p = c.player_must_select_space(ps);
            if (!p)
                return c.action_failed();
            c.summon(*p, flock);
        }
    }
    return c.action_resolved();
}


// Range 1-3. Create a wall in range, then grant 1 speed to one (3+), two (5+), or all adjacent allied units.
action_result raise_roost(combat &c)
{
    bool noose = c.self().has_upgrade(UPGRADE_THE_NOOSE);
    list<map_space *> ps = c.self().spaces_in_range(1, noose ? 4 : 3, SELECT_SPACE_FREE);
    if (ps.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    map_space *p = c.player_must_select_space(ps);
    if (!p)
        return c.action_failed();

    p->set_wall(true);
    if (noose) {
        list<unit *> us = c.units_in_range(*p, 1, 2);
        if (!us.empty()) {
            unit *u = c.player_may_select_unit(us);
            if (u)
                u->pull(c.self(), 1);
        }
    }
    list<unit *> us = c.units_in_range(*p, 1, 1, SELECT_UNIT_ALLY);

    int d6 = c.player_roll_d6(c.self());
    int effect = c.effect_gradations(d6, {{0, 1}, {3, 2}, {5, -1}});
    if (effect > 0) {
        int n = min(effect, (int)us.size());
        us = c.player_must_select_units(us, n, n);
    }
    for (unit *u : us)
        u->gain_token(TOKEN_SPEED);
    return c.action_resolved();
}


// Melee. Slay an adjacent allied unit. Gain 1 strength, 1 speed, and may MOVE again.
action_result cannibalize(combat &c)
{
    list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_ALLY);

    bool vor = c.self().has_upgrade(UPGRADE_VORACIOUS);
    if (vor) {
        for (unit *u : c.self().units_in_range(1, 1, enum_or(SELECT_UNIT_FOE, SELECT_UNIT_WITH_HP_1_OR_LOWER))) {
            if (find(us.begin(), us.end(), u) == us.end())
                us.push_back(u);
        }
    }

    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(us);
    if (!u)
        return c.action_failed();

    u->set_slain(true);
    if (u->is_ally(c.self())) {
        c.self().gain_token(TOKEN_STRENGTH);
        c.self().gain_token(TOKEN_SPEED);
    }
    if (c.player_may_take_action(TAKE_ACTION_MOVE_AGAIN))
        c.unit_move_again(c.self());

    return c.action_resolved();
}


action_result sinew(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_SINEW))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    bool was = c.self().trait(TRAIT_SINEW);
    bool will = c.self().hp() <= 1;
    if (was == will)
        return c.action_resolved(action_resolved::PREMATURELY);

    int inc = will ? +1 : -1;
    c.self().inc_trait(TRAIT_SINEW, inc);
    c.self().inc_trait(TRAIT_IMMUNE_TO_LINE_DAMAGE, inc);
    c.self().inc_trait(TRAIT_IMMUNE_TO_SPLASH_DAMAGE, inc);
    c.self().inc_trait(TRAIT_IMMUNE_TO_GRAZE_DAMAGE, inc);
    return c.action_resolved();
}


action_result remove_paranoia(combat &c)
{
    int inc = c.self().has_upgrade(UPGRADE_PAINFUL_WHISPERS) ? -1 : -2;
    for (unit *u : c.self().units_in_range(0, 999, SELECT_UNIT_IGNORE_LINE_OF_SIGHT)) {
        if (u->has_trait_related_unit(TRAIT_PARANOIA, &c.self())) {
            u->remove_trait_related_unit(TRAIT_PARANOIA, &c.self());
            u->inc_trait(TRAIT_PARANOIA, inc);
            return c.action_resolved();
        }
    }
    return c.action_prevented(action_prevented::NO_TARGET);
}


// Curse, Range 1-3. Target enemy counts as having +2 allied units adjacent until reused or this unit is slain.
action_result paranoia(combat &c)
{
    list<unit *> units = c.self().units_in_range(1, 3, enum_or(SELECT_UNIT_FOE, SELECT_UNIT_FOR_CURSE));
    if (units.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(units);
    if (!u)
        return c.action_failed();

    remove_paranoia(c);

    bool pain = c.self().has_upgrade(UPGRADE_PAINFUL_WHISPERS);
    int inc = pain ? +1 : +2;
    if (pain)
        u->take_damage(1, enum_or(DAMAGE_CURSE, DAMAGE_PIERCING), &c.self());
    u->inc_trait(TRAIT_PARANOIA, inc);
    u->add_trait_related_unit(TRAIT_PARANOIA, &c.self());
    return c.action_resolved();
}


// Self. Splash (self): 1 curse damage. Push all units 1 (5+) or 2 spaces.
action_result sonic_screech(combat &c)
{
    if (c.self().has_upgrade(UPGRADE_EAR_SPLITTER)) {
        list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_ALLY);
        if (!us.empty()) {
            us = c.player_must_select_units(us, 1, max((int)us.size(), 2));
            if (us.empty())
                return c.action_failed();
        }
        for (unit *u : us) {
            optional<token_type> tt = c.player_must_select_token_type({TOKEN_STRENGTH, TOKEN_SPEED});
            if (!tt)
                return c.action_failed();
            u->gain_token(*tt);
        }
        return c.action_resolved();
    }

    list<unit *> us = c.self().units_in_range(1, 1);
    int d6 = c.player_roll_d6(c.self());
    for (unit *u : us) {
        u->take_damage(1, DAMAGE_CURSE, &c.self());
        u->push(c.self(), d6 >= 5 ? 2 : 1);
    }
    return c.action_resolved();
}


// Self. Until next turn, adjacent enemies take -1D on attacks and allies adjacent are immune to graze damage.
action_result sirens_song(combat &c)
{
    list<trait_id> ts = {
        TRAIT_AURA_MINUS_1D_FOR_ALL_FOE_ATTACKS,
        TRAIT_AURA_IMMUNE_TO_GRAZE_DAMAGE,
        TRAIT_IMMUNE_TO_GRAZE_DAMAGE,
    };
    for (trait_id t : ts) {
        c.self().inc_trait(t, +1);
        c.self().inc_trait_after(t, -1, TRIGGER_TURN_START, 1);
    }
    if (c.self().has_upgrade(UPGRADE_LULLABY)) {
        list<unit *> us = c.self().units_in_range(1, 1, SELECT_UNIT_ALLY);
        for (unit *u : us)
            u->gain_token(TOKEN_SPEED);
    }
    return c.action_resolved();
}


// Self. Step 2, then create a wall in an adjacent free space. (4+): gain 1 speed.
action_result prepare_the_pole(combat &c)
{
    c.unit_step(c.self(), 2);
    if (!c.then())
        return c.action_resolved(action_resolved::PREMATURELY);

    list<map_space *> ps = c.self().spaces_in_range(1, 1, SELECT_SPACE_FREE);
    if (ps.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    map_space *p = c.player_must_select_space(ps);
    if (!p)
        return c.action_failed();

    p->set_wall(true);

    int d6 = c.player_roll_d6(c.self());
    if (d6 >= 4)
        c.self().gain_token(TOKEN_SPEED, +1);

    return c.action_resolved();
}


// Attack, melee. On hit: 2 damage. Rip Apart (3+): with piercing. Rip Apart (5+): deal 1 damage again and summon a Flock adjacent to target.
action_result hang_for_the_owls(combat &c)
{
    list<unit *> units = c.self().units_in_range(1, 1);
    if (units.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(units);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(*u, ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    damage_type t = DAMAGE_PHYSICAL;
    if (u->rip_apart_for(3))
        t = enum_or(DAMAGE_PHYSICAL, DAMAGE_PIERCING);
    if (c.self().has_upgrade(UPGRADE_WRITHING) && c.self().spaces_in_range(1, 1, SELECT_SPACE_WALLS).size())
        t = enum_or(DAMAGE_PHYSICAL, DAMAGE_PIERCING, DAMAGE_IGNORE_VITALITY);

    u->take_damage(2, t, &c.self());

    while (u->rip_apart_for(5)) {
        u->take_damage(1, t, &c.self());
        list<map_space *> ps = u->spaces_in_range(1, 1, SELECT_SPACE_FREE);
        if (ps.empty())
            break;

        map_space *p = c.player_must_select_space(ps);
        if (!p)
            return c.action_failed();

        c.summon(*p, flock);
        break;
    }
    return c.action_resolved();
}


// Attack, melee. On hit: 2 damage. Rip Apart (3+): +1 damage. Rip Apart (5+): +1 damage and obliterate if slain.
action_result guzzle(combat &c)
{
    list<unit *> units = c.self().units_in_range(1, 1);
    if (units.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    unit *u = c.player_must_select_unit(units);
    if (!u)
        return c.action_failed();

    int d6 = c.player_roll_d6(*u, ROLL_TAG_ATTACK);
    if (!c.is_hit(*u, d6)) {
        u->take_damage(1, enum_or(DAMAGE_GRAZE, DAMAGE_PHYSICAL), &c.self());
        return c.action_resolved(action_resolved::MISSED_A_HIT);
    }

    int ra = u->rip_apart_for();
    int dmg = c.effect_gradations(ra, {{0, 2}, {3, 3}, {5, 4}});
    damage_type t = ra >= 5 ? enum_or(DAMAGE_OBLITERATE_ON_SLAY, DAMAGE_PHYSICAL) : DAMAGE_PHYSICAL;
    u->take_damage(dmg, t, &c.self());
    return c.action_resolved();
}


// Self. Splash (self): 1 damage. For each unit slain, clear one token and gain 1 speed or strength. Step 1 per slain unit and may destroy walls during this movement.
action_result fleshgorger(combat &c)
{
    return c.action_unimplemented();
}


// Range 2-4. Splash (target): deal 1 curse damage once for each wall in the area. The same unit may be targeted multiple times.
action_result concussive_shriek(combat &c)
{
    return c.action_unimplemented();
}


// Attack, melee. On hit: 1 damage. Rip Apart (3+): +1 damage. Rip Apart (5+): +1 damage. May spend speed to reduce Rip Apart costs by 1 each.
action_result flense(combat &c)
{
    return c.action_unimplemented();
}


// Self. Until end of next turn, after an adjacent unit is slain, summon a Flock in range. May trigger only once per turn.
action_result feeding_call(combat &c)
{
    return c.action_unimplemented();
}


// Self. Until this unit MOVEs or steps, foes ending a MOVE or ACT adjacent take 1 piercing damage. On slay, gain 1 speed. Triggers once per turn.
action_result horrendous_pecking(combat &c)
{
    return c.action_unimplemented();
}


// Gains additional bonuses if X or more allied units or walls are adjacent to the target, including this one.
action_result rip_apart(combat &c)
{
    return c.action_unimplemented();
}


// For every unit slot spent on Flock, summon an additional Flock adjacent to an allied unit at the end of every round.
action_result endless(combat &c)
{
    int slots = c.player_unit_slots_spent_on(FACTION_STEEPLEWRACK, UNIT_THRALL);
    if (c.trigger() == TRIGGER_ROUND_START) {
        c.self().set_trait(TRAIT_ENDLESS_CHARGE, slots);
        return c.action_resolved();
    }
    if (c.trigger() != TRIGGER_ROUND_END)
        return c.action_unreachable();

    if (!c.self().trait(TRAIT_ENDLESS_CHARGE))
        return c.action_prevented(action_prevented::ALREADY_USED);

    list<unit *> us = c.self().units_in_range(0, 999, enum_or(SELECT_UNIT_NO_FOE, SELECT_UNIT_IGNORE_LINE_OF_SIGHT));
    if (us.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    set<map_space *> ps;
    for (unit *u : us) {
        u->set_trait(TRAIT_ENDLESS_CHARGE, 0);
        list<map_space *> ap = u->spaces_in_range(0, 1, SELECT_SPACE_FREE);
        ps.insert(ap.begin(), ap.end());
    }

    list<map_space *> psl(ps.begin(), ps.end());
    psl.sort();
    if (psl.empty())
        return c.action_prevented(action_prevented::NO_TARGET);

    map_space *p = c.player_must_select_space(psl);
    if (!p)
        return c.action_failed();

    c.summon(*p, flock);
    return c.action_resolved();
}


// May spend a speed token when attacked by an ACT ability to give the attacker -1D and step 1 after resolution.
action_result feral_dodge(combat &c)
{
    token *t = c.self().find_token(TOKEN_SPEED);
    if (!t || t->count() <= 1)
        return c.action_prevented(action_prevented::NO_TOKENS);

    if (!c.player_may_take_action(TAKE_ACTION_FERAL_DODGE))
        return c.action_resolved(action_resolved::PLAYER_CHOSE_NOT_TO);

    c.self().remove_token(TOKEN_SPEED);
    c.self().inc_trait(TRAIT_MINUS_1D_FROM_ALL_ATTACKS, +1);

    auto a = [](combat &c) {
        c.unit_step(c.self());
        return c.action_resolved();
    };
    c.self().inc_trait_after(TRAIT_MINUS_1D_FROM_ALL_ATTACKS, -1, TRIGGER_AFTER_ATTACKED, 1);
    c.self().do_after(a, TRIGGER_AFTER_ATTACKED, 1);
    return c.action_resolved();
}


// May move through walls but not end movement in them. Moving through a wall grants free movement until end of turn.
action_result long_stilts(combat &c)
{
    c.self().inc_trait(TRAIT_MOVEMENT_THROUGH_WALLS, +1);
    return c.action_resolved();
}


action_result perching(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_PERCHING))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    int n = c.self().spaces_in_range(1, 1, SELECT_SPACE_WALLS).size();
    if (!n)
        return c.action_prevented(action_prevented::CONDITION_UNSATISFIED);

    c.self().gain_token(TOKEN_SPEED);
    if (!c.self().n_tokens(enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE)))
        return c.action_resolved();

    token *t = c.player_must_select_token(c.self().tokens(), enum_or(SELECT_TOKEN_NEGATIVE, SELECT_TOKEN_REMOVABLE));
    if (!t)
        return c.action_failed();
    c.self().remove_token(t->type());
    return c.action_resolved();
}


action_result clinging(combat &c)
{
    if (!c.self().has_upgrade(UPGRADE_CLINGING))
        return c.action_prevented(action_prevented::NO_UPGRADE);

    bool was = c.self().trait(TRAIT_CLINGING);
    bool will = c.self().spaces_in_range(1, 1, SELECT_SPACE_WALLS).size();
    if (was == will)
        return c.action_resolved(action_resolved::PREMATURELY);

    int inc = will ? +1 : -1;
    c.self().inc_trait(TRAIT_CLINGING, inc);
    c.self().inc_trait(TRAIT_CURSEPROOF, inc);
    return c.action_resolved();
}


action_result large_or_sleek_owl(combat &c)
{
    if (c.self().has_upgrade(UPGRADE_SLEEK_OWL))
        c.self().inc_trait(TRAIT_MOVEMENT_FREE, +1);
    else
        c.self().set_trait(TRAIT_IS_LARGE, 1);
    return c.action_resolved();
}


// Arrives at the start of round 3 in any free space. May destroy walls beneath itself before placement.
action_result circling(combat &c)
{
    if (c.round() != 3)
        return c.action_prevented(action_prevented::CONDITION_UNSATISFIED);

    map_space *p = c.player_must_select_any_space(SELECT_SPACE_NO_UNIT);
    if (!p)
        return c.action_failed();

    for (map_space *a : p->spaces_in_range(0, 1, SELECT_SPACE_WALLS))
        a->set_wall(false);

    c.swap_unit_pos(c.self(), *p);
    return c.action_resolved();
}


// Counts as two allied units for Rip Apart purposes, but not to itself.
action_result flesh_sacrifice(combat &c)
{
    c.self().set_trait(TRAIT_FLESH_SACRIFICE, 1);
    return c.action_resolved();
}


// Counts as a wall. Blocks line of sight and provides cover. Cannot be destroyed by wall-destruction effects.
action_result longlegs(combat &c)
{
    return c.action_unimplemented();
}


// *** UNITS ***
void gunwight(unit_card &c)
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


void enforcer(unit_card &c)
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


void ammo_goblin(unit_card &c)
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


void barrelform(unit_card &c)
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


void egis_weapon(unit_card &c)
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


void operator_necromancer(unit_card &c)
{
    c.set_faction_type(FACTION_CARCASS, UNIT_NECROMANCER);
    c.set_stats(4, 8, 4, ARMOR_PHYS);

    c.add_trait(TRIGGER_COMBAT_START, body_block);
    c.add_trait(TRIGGER_COMBAT_START, formation);
    c.add_trait(TRIGGER_ROUND_START, hot_clip);
    c.add_bonus_trait(TRIGGER_COMBAT_START, bone_wall);
    c.add_bonus_trait(TRIGGER_COMBAT_START, gun_kata);
    c.add_bonus_trait(TRIGGER_COMBAT_START, clutch_reload);

    c.add_act_ability(akimbo);
    c.add_upgrade_act_ability(grenade_jump);
    c.add_upgrade_act_ability(armor_lock);
    c.add_upgrade_act_ability(devil_bullet);
    c.add_upgrade_act_ability(superkick);
    c.add_upgrade_act_ability(lv4_living_cannon);
    c.add_upgrade_act_ability(sg88_two_barrel_shotgun);
    c.add_upgrade_act_ability(unholy_sixgun);

    c.add_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, reload_slide);
    c.add_soul_ability(TRIGGER_SOUL_OWN_TURN, bullet_time);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_OWN_TURN, TRIGGER_AFTER_ATTACK_DAMAGED), double_tap);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_OWN_TURN, TRIGGER_AFTER_ATTACK_HITED), frag_bullet);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_AFTER_UNIT_MOVED), bullet_hustle);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_TURN, overwatch);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_TURN, bullet_sonata);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_OWN_TURN, TRIGGER_AFTER_HEADSHOT), straight_to_the_grave);
}


void infiltrator(unit_card &c)
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


void warhead(unit_card &c)
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


void carnifex(unit_card &c)
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


void pain_ghoul(unit_card &c)
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


void painwheel(unit_card &c)
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


void berserker(unit_card &c)
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


void warlord(unit_card &c)
{
    c.set_faction_type(FACTION_GOREGRINDERS, UNIT_NECROMANCER);
    c.set_stats(4, 10, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, body_block);
    c.add_trait(TRIGGER_AFTER_DAMAGED, blood_rage);
    c.add_trait(TRIGGER_TURN_START, fueled_by_rage);
    c.add_bonus_trait(TRIGGER_COMBAT_START, nerve_twitch);
    c.add_bonus_trait(TRIGGER_COMBAT_START, blood_drinker);
    c.add_bonus_trait(TRIGGER_COMBAT_START, blood_rush);

    c.add_act_ability(superheated_chainblade);
    c.add_upgrade_act_ability(quench);
    c.add_upgrade_act_ability(frenzy_chain);
    c.add_upgrade_act_ability(furious_roar);
    c.add_upgrade_act_ability(apoplexy);
    c.add_upgrade_act_ability(pilebunker);
    c.add_upgrade_act_ability(saw_toothed_slayer_axe);
    c.add_upgrade_act_ability(spine_devil_blade);

    c.add_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, ignite);
    c.add_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, glory_kill);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, hellrider_kick);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, overclock);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_AFTER_SLAIN), gorebather);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_AFTER_UNIT_ACTED), killing_spree);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_AFTER_UNIT_SLAINED), blood_boil);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_TURN, devil_impact);
}


void gearhead(unit_card &c)
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


void scum(unit_card &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_THRALL);
    c.set_stats(3, 1, 3, ARMOR_SUPER);

    c.add_trait(TRIGGER_BEFORE_SLAINED, toxic_revenge);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(pseudopod);
    c.add_act_ability(shamble);

    c.add_upgrade(UPGRADE_BLOAT);
    c.add_upgrade(UPGRADE_TENTACLE_WHIP);
    c.add_upgrade(UPGRADE_AFTERMATH);
}


void rotten(unit_card &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_SCION);
    c.set_stats(4, 4, 3, ARMOR_PHYS);

    c.add_trait(TRIGGER_COMBAT_START, plaguebearer);
    c.add_trait(TRIGGER_BEFORE_SLAINED, invigorating_viscera);
    c.add_trait(enum_or(TRIGGER_AFTER_HAZARD_CHANGED, TRIGGER_AFTER_POS_CHANGED), smog_shroud);

    c.add_act_ability(pustulate);
    c.add_act_ability(vomitous_mass);
    c.add_act_ability(rotblade);

    c.add_upgrade(UPGRADE_CATALYZE);
    c.add_upgrade(UPGRADE_INVIGORATING_VISCERA);
    c.add_upgrade(UPGRADE_SMOG_SHROUD);
}


void leech(unit_card &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_FREAK);
    c.set_stats(4, 4, 4, ARMOR_MAG);

    c.add_trait(TRIGGER_BEFORE_SLAINED, deathwash);

    c.add_act_ability(suppurate);
    c.add_act_ability(evolve_strain);
    c.add_act_ability(swell_with_corruption);

    c.add_upgrade(UPGRADE_MASSIVE_SWELL);
    c.add_upgrade(UPGRADE_PANDEMIC);
    c.add_upgrade(UPGRADE_ACID_BLOOD);
}


void host(unit_card &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_HUNTER);
    c.set_stats(3, 4, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_BEFORE_SLAINED, swarm_release);
    c.add_trait(TRIGGER_COMBAT_START, toxic_avenger_immune_to_hazards);
    c.add_trait(TRIGGER_TURN_START, toxic_avenger);

    c.add_act_ability(propagate_swarm);
    c.add_act_ability(driving_vermin);

    c.add_upgrade(UPGRADE_SWARM_FEED);
    c.add_upgrade(UPGRADE_TOXIC_AVENGER);
    c.add_upgrade(UPGRADE_DEFILER);
}


void slime(unit_card &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_HORROR);
    c.set_stats(4, 4, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_BEFORE_SLAINED, vile_rupture);

    c.add_act_ability(percolate);
    c.add_act_ability(surge);

    c.add_upgrade(UPGRADE_ROTTEN_SURGE);
    c.add_upgrade(UPGRADE_STICKY_TRAIL);
    c.add_upgrade(UPGRADE_THE_GUNK);
}


void plaguelord(unit_card &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_NECROMANCER);
    c.set_stats(4, 10, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, body_block);
    c.add_trait(enum_or(TRIGGER_COMBAT_START, TRIGGER_TURN_START), blessed_with_filth);
    c.add_bonus_trait(TRIGGER_AFTER_POS_CHANGED, pollution_shroud);
    c.add_bonus_trait(TRIGGER_AFTER_MOVE, corruptor);
    c.add_bonus_trait(TRIGGER_COMBAT_START, spreader);

    c.add_act_ability(virulence);
    c.add_upgrade_act_ability(melt);
    c.add_upgrade_act_ability(corpse_explosion);
    c.add_upgrade_act_ability(vermin_form);
    c.add_upgrade_act_ability(ceaseless_spew);
    c.add_upgrade_act_ability(flesh_feeder_blade);
    c.add_upgrade_act_ability(scourge);
    c.add_upgrade_act_ability(necrocide);

    c.add_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_TURN_START), infest);
    c.add_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_TURN_START), slime_form);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, purge_guts);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_TURN_START), insides_out);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_TURN_START), fecundity);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_TURN_START), unholy_vapors);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_TURN, praise_the_filth);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_TURN, mox_populi);
}


void belcher(unit_card &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_THRALL);
    c.set_stats(3, 1, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_BEFORE_SLAINED, burst_guts);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(toxic_spit);
    c.add_act_ability(projectile_vomit);

    c.add_upgrade(UPGRADE_CONGEALED);
    c.add_upgrade(UPGRADE_PUTRIDITY);
    c.add_upgrade(UPGRADE_STUFFED);
}


void slimelet(unit_card &c)
{
    c.set_faction_type(FACTION_GARGAMOX, UNIT_THRALL);
    c.set_stats(3, 1, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, spit_up);
    c.add_trait(TRIGGER_COMBAT_START, summoned_thrall);

    c.add_act_ability(toxic_spit);
}


void sacrifice(unit_card &c)
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


void chosen(unit_card &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_SCION);
    c.set_stats(4, 3, 5, ARMOR_MAG);

    c.add_trait(enum_or(TRIGGER_AFTER_HAZARD_CHANGED, TRIGGER_AFTER_POS_CHANGED), slither);
    c.add_trait(TRIGGER_BEFORE_ACT, leap);

    c.add_act_ability(tombraiser);
    c.add_act_ability(kidnap);
    c.add_act_ability(serpents_kiss);

    c.add_upgrade(UPGRADE_IVORY_SERPENT);
    c.add_upgrade(UPGRADE_LEAP);
    c.add_upgrade(UPGRADE_FOUL_MONUMENTS);
}


void vizigheist(unit_card &c)
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


void banshee(unit_card &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_HUNTER);
    c.set_stats(3, 3, 5, ARMOR_MAG);

    c.add_act_ability(bale_scream);
    c.add_act_ability(tombstone);

    c.add_upgrade(UPGRADE_DOOM_BELL);
    c.add_upgrade(UPGRADE_FREEZE_SOUL);
    c.add_upgrade(UPGRADE_SIREN);
}


void bound_devil(unit_card &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_TYRANT);
    c.set_stats(3, 6, 2, ARMOR_MAG);

    c.add_trait(TRIGGER_BEFORE_MOVE, labyrinth_master);
    c.add_trait(TRIGGER_COMBAT_START, large);
    c.add_trait(TRIGGER_COMBAT_START, strong_pact);

    c.add_act_ability(hells_grasp);
    c.add_act_ability(beckon_lamb);
    c.add_act_ability(horrendous_end);

    c.add_upgrade(UPGRADE_TO_THE_SLAUGHTER);
    c.add_upgrade(UPGRADE_SUPREMACY);
    c.add_upgrade(UPGRADE_STRONG_PACT);
}


void dark_priest(unit_card &c)
{
    c.set_faction_type(FACTION_DEADSOULS, UNIT_NECROMANCER);
    c.set_stats(4, 8, 4, ARMOR_MAG);

    c.add_trait(TRIGGER_COMBAT_START, body_block);
    c.add_trait(TRIGGER_COMBAT_START, dread_presence);
    c.add_bonus_trait(TRIGGER_BEFORE_MOVE, teleport);
    c.add_bonus_trait(TRIGGER_COMBAT_START, puppet_master);
    c.add_bonus_trait(TRIGGER_COMBAT_START, maggot_destroyer);

    c.add_act_ability(doomblade);
    c.add_upgrade_act_ability(unholy_summoning);
    c.add_upgrade_act_ability(vapor_form);
    c.add_upgrade_act_ability(tear_soul);
    c.add_upgrade_act_ability(frozen_hell);
    c.add_upgrade_act_ability(great_urgal_blade);
    c.add_upgrade_act_ability(bloody_flail);
    c.add_upgrade_act_ability(lathean_devil_whip);

    c.add_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, cyclopean_monolith);
    c.add_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, soulfeed);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_TURN_START), twist_sinews);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_FOE_TURN, TRIGGER_TURN_START), writhing_curse);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, disincorporate);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_AFTER_UNIT_SLAINED), eternal_curse);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_TURN, malebolge);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_TURN, great_satania);
}


void lithogeist(unit_card &c)
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


void penitent(unit_card &c)
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


void zealot(unit_card &c)
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


void antipriest(unit_card &c)
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


void inquisitor(unit_card &c)
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


void holy_body(unit_card &c)
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


void abbot(unit_card &c)
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


void exorcist(unit_card &c)
{
    c.set_faction_type(FACTION_ABHORRER, UNIT_NECROMANCER);
    c.set_stats(4, 10, 4, ARMOR_NONE);

    c.add_trait(enum_or(TRIGGER_COMBAT_START, TRIGGER_ROUND_START), holy_vessel);
    c.add_bonus_trait(TRIGGER_ROUND_START, winter_rose_stance);
    c.add_bonus_trait(TRIGGER_ROUND_START, corpse_violet_stance);
    c.add_bonus_trait(TRIGGER_AFTER_ACT, royal_chrysanthemum_stance);

    c.add_act_ability(starmetal_godsword);
    c.add_upgrade_act_ability(grand_oath);
    c.add_upgrade_act_ability(redempta);
    c.add_upgrade_act_ability(communion);
    c.add_upgrade_act_ability(decree_of_forbiddance);
    c.add_upgrade_act_ability(holy_water_flail);
    c.add_upgrade_act_ability(relic_lance);
    c.add_upgrade_act_ability(starmetal_shuriken);

    c.add_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_TURN_START), absolution);
    c.add_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, will_of_god);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_AFTER_UNIT_MOVED), sword_art_drifting_blossoms);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, sword_art_safflower_cut);
    c.add_upgrade_soul_ability(TRIGGER_SOUL_OWN_OR_ALLIED_TURN, grace);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_AFTER_UNIT_DAMAGED), sword_art_winter_sprout);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_OWN_TURN, TRIGGER_AFTER_ATTACK_HITED), supreme_sword_art_merciful_snowdrop_cut);
    c.add_upgrade_soul_ability(enum_or(TRIGGER_SOUL_ANY_TURN, TRIGGER_AFTER_UNIT_SLAINED), resurrection);
}


// TODO: update this and other IGORRI according to the latest balance changes
void stitch(unit_card &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_THRALL);
    c.set_stats(3, 2, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_BEFORE_SLAINED, fall_to_shambles);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(unstable_mutation);
    c.add_act_ability(twisting_strike);

    c.add_upgrade(UPGRADE_GENESTEALER);
    c.add_upgrade(UPGRADE_SPREAD_MUTATE);
    c.add_upgrade(UPGRADE_WARPING_MUTATE);
}


void chop_doc(unit_card &c)
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


void lycan(unit_card &c)
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


void strigoi(unit_card &c)
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


void homonculus(unit_card &c)
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


void chirurgeon(unit_card &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_NECROMANCER);
    c.set_stats(4, 10, 3, ARMOR_MAG);

    c.add_trait(TRIGGER_COMBAT_START, body_block);
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


void flock(unit_card &c)
{
    c.set_faction_type(FACTION_STEEPLEWRACK, UNIT_THRALL);
    c.set_stats(3, 1, 5, ARMOR_NONE);

    c.add_trait(enum_or(TRIGGER_ROUND_START, TRIGGER_ROUND_END), endless);
    c.add_trait(TRIGGER_COMBAT_START, thrall);
    c.add_trait(TRIGGER_ROUND_START, blood_frenzy);

    c.add_act_ability(bloodletting);
    c.add_act_ability(call_to_feast);

    c.add_upgrade(UPGRADE_THE_RED_WET_HUE);
    c.add_upgrade(UPGRADE_BLOOD_FRENZY);
    c.add_upgrade(UPGRADE_RABID);
}


void harpy(unit_card &c)
{
    c.set_faction_type(FACTION_STEEPLEWRACK, UNIT_SCION);
    c.set_stats(4, 4, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_BEFORE_ATTACKED, feral_dodge);
    c.add_trait(TRIGGER_AFTER_HP_CHANGED, sinew);

    c.add_act_ability(disembowel);
    c.add_act_ability(raise_roost);
    c.add_act_ability(cannibalize);

    c.add_upgrade(UPGRADE_SINEW);
    c.add_upgrade(UPGRADE_VORACIOUS);
    c.add_upgrade(UPGRADE_THE_NOOSE);
}


void siren(unit_card &c)
{
    c.set_faction_type(FACTION_STEEPLEWRACK, UNIT_FREAK);
    c.set_stats(4, 3, 5, ARMOR_MAG);

    c.add_trait(TRIGGER_COMBAT_START, flight);
    c.add_trait(TRIGGER_BEFORE_SLAINED, remove_paranoia);

    c.add_act_ability(paranoia);
    c.add_act_ability(sonic_screech);
    c.add_act_ability(sirens_song);

    c.add_upgrade(UPGRADE_EAR_SPLITTER);
    c.add_upgrade(UPGRADE_PAINFUL_WHISPERS);
    c.add_upgrade(UPGRADE_LULLABY);
}


void stiltwalker(unit_card &c)
{
    c.set_faction_type(FACTION_STEEPLEWRACK, UNIT_HORROR);
    c.set_stats(4, 3, 5, ARMOR_MAG);

    c.add_trait(TRIGGER_COMBAT_START, long_stilts);
    c.add_trait(TRIGGER_TURN_START, perching);
    c.add_trait(TRIGGER_AFTER_POS_CHANGED, clinging);

    c.add_act_ability(prepare_the_pole);
    c.add_act_ability(hang_for_the_owls);

    c.add_upgrade(UPGRADE_PERCHING);
    c.add_upgrade(UPGRADE_WRITHING);
    c.add_upgrade(UPGRADE_CLINGING);
}


void carniphargous_owl(unit_card &c)
{
    c.set_faction_type(FACTION_STEEPLEWRACK, UNIT_TYRANT);
    c.set_stats(4, 5, 4, ARMOR_SUPER);

    c.add_trait(TRIGGER_ROUND_START, circling);
    c.add_trait(TRIGGER_COMBAT_START, flesh_sacrifice);
    c.add_trait(TRIGGER_COMBAT_START, large_or_sleek_owl);

    c.add_act_ability(guzzle);
    c.add_act_ability(fleshgorger);
    c.add_act_ability(concussive_shriek);

    c.add_upgrade(UPGRADE_MESSY_ARRIVAL);
    c.add_upgrade(UPGRADE_SLEEK_OWL);
    c.add_upgrade(UPGRADE_BONE_RESONANCE);
}


void great_stork(unit_card &c)
{
    c.set_faction_type(FACTION_STEEPLEWRACK, UNIT_SCION);
    c.set_stats(3, 3, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, longlegs);

    c.add_act_ability(flense);
    c.add_act_ability(feeding_call);
    c.add_act_ability(horrendous_pecking);

    c.add_upgrade(UPGRADE_ENTRAIL_SCATTER);
    c.add_upgrade(UPGRADE_JITTER);
    c.add_upgrade(UPGRADE_WOLF_DOWN);
}


void haruspex(unit_card &c)
{
    c.set_faction_type(FACTION_STEEPLEWRACK, UNIT_NECROMANCER);
    c.set_stats(4, 8, 5, ARMOR_NONE);

    c.add_trait(enum_or(TRIGGER_ROUND_START, TRIGGER_ROUND_END), dive_bomb);
    c.add_trait(TRIGGER_COMBAT_START, flesh_sacrifice);
}


void carcass(faction &c)
{
    c.add_unit(gunwight);
    c.add_unit(enforcer);
    c.add_unit(ammo_goblin);
    c.add_unit(barrelform);
    c.add_unit(infiltrator);
    c.add_unit(egis_weapon);
    c.add_unit(operator_necromancer);
}


void goregrinders(faction &c)
{
    c.add_unit(warhead);
    c.add_unit(carnifex);
    c.add_unit(pain_ghoul);
    c.add_unit(gearhead);
    c.add_unit(painwheel);
    c.add_unit(berserker);
    c.add_unit(warlord);
}


void gargamox(faction &c)
{
    c.add_unit(scum);
    c.add_unit(belcher);
    c.add_unit(rotten);
    c.add_unit(leech);
    c.add_unit(host);
    c.add_unit(slime);
    c.add_unit(plaguelord);
}


void deadsouls(faction &c)
{
    c.add_unit(sacrifice);
    c.add_unit(chosen);
    c.add_unit(vizigheist);
    c.add_unit(lithogeist);
    c.add_unit(banshee);
    c.add_unit(bound_devil);
    c.add_unit(dark_priest);
}


void abhorrer(faction &c)
{
    c.add_unit(penitent);
    c.add_unit(zealot);
    c.add_unit(antipriest);
    c.add_unit(abbot);
    c.add_unit(inquisitor);
    c.add_unit(holy_body);
    c.add_unit(exorcist);
}


void igorri(faction &c)
{
    c.add_unit(stitch);
    c.add_unit(chop_doc);
    c.add_unit(lycan);
    c.add_unit(strigoi);
    c.add_unit(homonculus);
    // c.add_unit(vessel);
    c.add_unit(chirurgeon);
}


int main()
{
    cout << "hi!\n";
    return 0;
}

// *** utils implementation ***

bool token::passes_filter(select_token_filter f) const
{
    if ((f & SELECT_TOKEN_REMOVABLE) && !is_removable()) return false;
    if ((f & SELECT_TOKEN_POSITIVE) && !is_positive()) return false;
    if ((f & SELECT_TOKEN_NEGATIVE) && !is_negative()) return false;
    if ((f & SELECT_TOKEN_PLAGUE) && type() != TOKEN_PLAGUE) return false;
    if ((f & SELECT_TOKEN_VITALITY) && type() != TOKEN_VITALITY) return false;
    return true;
}

int unit::n_tokens(select_token_filter f) const
{
    int count = 0;
    for (token *t : tokens()) {
        if (t->passes_filter(f))
            count++;
    }
    return count;
}

token *unit::find_token(token_type type) const
{
    for (token *t : tokens()) {
        if (t->type() == type)
            return t;
    }
    return nullptr;
}

int unit::corpses_in_range(int min, int max) const
{
    int corpses = 0;
    for (const map_space *s : spaces_in_range(min, max))
        corpses += s->n_corpses();
    return corpses;
}
