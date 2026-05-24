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
    TRIGGER_ON_SLAIN,

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
    ARMOR_PHYSICAL,
    ARMOR_MAGIC,
    ARMOR_SUPER,

    // factions
    FACTION_IGORRI,

    // unit type
    UNIT_THRALL,
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
    virtual map_pos pos() const = 0;
    virtual bool has_upgrade(int) const = 0;
    virtual int unit_type() const = 0;

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


struct unit_card_context
{
    virtual ~unit_card_context() = default;
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
void tyrant(unit_action_context &c)
{
    c.self().set_counter(COUNTER_IS_2X2, 1);
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


void stitch(unit_card_context &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_THRALL);
    c.set_stats(3, 2, 4, ARMOR_NONE);

    c.add_trait(TRIGGER_ON_SLAIN, fall_to_shambles);
    c.add_trait(TRIGGER_COMBAT_START, thrall);

    c.add_act_ability(unstable_mutation);
    c.add_act_ability(twisting_strike);

    c.add_upgrade(UPGRADE_GENESTEALER);
    c.add_upgrade(UPGRADE_SPREAD_MUTATE);
    c.add_upgrade(UPGRADE_WARPING_MUTATE);
}


void chop_doc(unit_card_context &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_FREAK);
    c.set_stats(4, 4, 4, ARMOR_MAGIC);

    c.add_trait(TRIGGER_AFTER_MOVE, leftovers);

    c.add_act_ability(inject_mutagen);
    c.add_act_ability(purge);
    c.add_act_ability(marriage);

    c.add_upgrade(UPGRADE_ABSORB);
    c.add_upgrade(UPGRADE_SCOUR_FLESH);
    c.add_upgrade(UPGRADE_CONJOIN);
}


void lycan(unit_card_context &c)
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


void strigoi(unit_card_context &c)
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


void homonculus(unit_card_context &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_TYRANT);
    c.set_stats(4, 6, 3, ARMOR_NONE);

    c.add_trait(TRIGGER_COMBAT_START, warpflesh);
    c.add_trait(TRIGGER_COMBAT_START, tyrant);

    c.add_act_ability(sculpt_flesh);
    c.add_act_ability(absorb);
    c.add_act_ability(flesh_whip);

    c.add_upgrade(UPGRADE_BALL_OF_LIMBS);
    c.add_upgrade(UPGRADE_MELD);
    c.add_upgrade(UPGRADE_FORM_CARAPACE);
    c.set_move_override(ball_of_limbs);
}


void chirurgeon(unit_card_context &c)
{
    c.set_faction_type(FACTION_IGORRI, UNIT_NECROMANCER);
    c.set_stats(4, 10, 3, ARMOR_MAGIC);

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

int main()
{
    cout << "hi!\n";
    return 0;
}
