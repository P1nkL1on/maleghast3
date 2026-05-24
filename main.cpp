#include <cmath>
#include <optional>
#include <list>
#include <map>
#include <iostream>


using namespace std;

enum stuff
{
    TRIGGER_ACTION_MANUAL,
    TRIGGER_TURN_START,
    TRIGGER_TURN_END,
    TRIGGER_BEFORE_ATTACK,

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
    COUNTER_ALTERED_MV,
    COUNTER_ALTERED_DF,

    TAKE_ACTION_RAPID_MOVE,
    TAKE_ACTION_ANCILLARY_LIMBS,
    TAKE_ACTION_STEP,

    ROLL_TAG_NONE,
    ROLL_TAG_CURSE,
    ROLL_TAG_ATTACK,

    SELECT_TOKEN_ANY,
    SELECT_TOKEN_ONLY_NEGATIVE,
    SELECT_TOKEN_ONLY_POSITIVE,

    DAMAGE_TOXIN,
    DAMAGE_NORMAL,
    DAMAGE_GRAZE,
    DAMAGE_FIRE,
    DAMAGE_CURSE,
};


enum select_unit_filter
{
    SELECT_UNIT_EXCLUDE_NONE,
    SELECT_UNIT_EXCLUDE_SELF,
    SELECT_UNIT_EXCLUDE_ALLY,
    SELECT_UNIT_EXCLUDE_ENEMY,
    SELECT_UNIT_WITH_NEGATIVE_TOKENS,
    SELECT_UNIT_WITH_MUTATION_TOKENS,
    SELECT_UNIT_WITHOUT_CURSEPROOF,
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
};


template <typename T> T enum_or(T a, T b) { return T((int)a | (int)b); }


struct map_pos
{
    int x, y;
};


struct token_context;


struct unit_context
{
    virtual ~unit_context() = default;
    virtual list<token_context *> tokens() = 0;
    virtual token_context *find_token(token_type type) = 0;
    virtual bool remove_token(token_context &, int count = 1) = 0;
    virtual void add_token(token_type type, int count = 1) = 0;
    virtual void move_token(token_context *) = 0;

    virtual int inc_counter(int counter, int x, int def_value = 0) = 0;
    virtual void set_counter(int counter, int x) = 0;
    virtual int counter(int counter, int def_value = 0) const = 0;

    virtual list<unit_context *> units_in_range(int, select_unit_filter exclude = SELECT_UNIT_EXCLUDE_NONE) const = 0;
    virtual list<unit_context *> units_in_range(int, int, select_unit_filter exclude = SELECT_UNIT_EXCLUDE_NONE) const = 0;
    virtual bool is_ally(unit_context &) const = 0;
    virtual void take_damage(int x, int type, bool *dead = nullptr) = 0;
    virtual map_pos pos() const = 0;

    virtual bool is_curseproof() const { return counter(COUNTER_CURSEPROOF) > 0; }
};


struct token_context
{
    virtual ~token_context() = default;
    virtual int count() const = 0;
};


struct unit_action_context
{
    virtual ~unit_action_context() = default;

    virtual unit_context &self() = 0;
    virtual unit_context &activated() = 0;
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
    virtual token_context *player_may_select_token(const list<token_context *> &tokens, int filter = SELECT_TOKEN_ANY) = 0;
    virtual unit_context *player_must_select_unit(const list<unit_context *> &units) = 0;
    virtual list<unit_context *> player_must_select_line(int) = 0;
    virtual optional<map_pos> player_must_select_free_space(const map_pos &, int range) = 0;
    virtual bool player_may_take_action(int) = 0;
    virtual bool player_may_spend_soul(int x) = 0;
    virtual int player_roll_d6(unit_context &who, int tags = ROLL_TAG_NONE) = 0;
    virtual int d6_gradations(int d6, const map<int, int> &treshold_to_result = {}) const = 0;

    // TODO: may unit trigger something on step and die? then it should be [[no_discard]] bool unit_step
    virtual bool is_hit(unit_context &target, int d6) const = 0;
    virtual void unit_step(unit_context &, int range = 1, movement_tags tags = MOVEMENT_DEFAULT) = 0;
    virtual void obliterate(unit_context &) = 0;
    virtual int inc_corpse(const map_pos &, int x = 0) = 0;
    virtual unit_context &copy_unit(unit_context &, const map_pos &new_pos) = 0;
    virtual void swap_unit_pos(unit_context &, unit_context &) = 0;

    virtual void mutate(unit_context &) = 0;
    virtual int player_may_spare_parts(unit_context &) = 0;
};


using trigger = int;
using action_foo = void(*)(unit_action_context &);


struct action
{
    trigger t;
    action_foo foo = nullptr;
};


// At turn start, may convert one of this unit’s mutation tokens into strength, speed, or vitality. Then, mutate.
void polyglot(unit_action_context &c)
{
    token_context *t = c.self().find_token(TOKEN_MUTATION);
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
    list<token_context *> ts = c.self().tokens();
    if (ts.empty())
        return c.no_resources();
    list<unit_context *> us = c.self().units_in_range(2);
    if (us.empty())
        return c.no_target();

    token_context *t = c.player_may_select_token(ts);
    if (!t)
        return;
    unit_context *u = c.player_must_select_unit(us);
    if (!u)
        return;

    c.self().remove_token(*t);
    u->move_token(t);
}


// May remove a mutation token at start of own turn to step 2.
void rapid_move_ts(unit_action_context &c)
{
    c.self().inc_counter(COUNTER_RAPID_MOVE_AVAILABLE, +1);
    token_context *t = c.self().find_token(TOKEN_MUTATION);
    if (!t)
        return c.no_resources();
    if (!c.player_may_take_action(TAKE_ACTION_RAPID_MOVE))
        return;

    c.self().remove_token(*t);
    c.unit_step(c.self(), 2);
    c.self().inc_counter(COUNTER_RAPID_MOVE_AVAILABLE, -1);
}


// May remove a mutation token at end of own turn to step 2.
void rapid_move_te(unit_action_context &c)
{
    bool rm_available = c.self().counter(COUNTER_RAPID_MOVE_AVAILABLE);
    token_context *t = c.self().find_token(TOKEN_MUTATION);
    if (!rm_available || !t)
        return c.no_resources();

    if (c.player_may_take_action(TAKE_ACTION_RAPID_MOVE)) {
        c.self().remove_token(*t);
        c.unit_step(c.self(), 2);
    }
    c.self().set_counter(COUNTER_RAPID_MOVE_AVAILABLE, 0);
}


// May remove a mutation token on self to grant +1D on any attack and allow it to ignore cover.
void ancillary_limbs(unit_action_context &c)
{
    token_context *t = c.self().find_token(TOKEN_MUTATION);
    if (!t)
        return c.no_resources();
    if (!c.player_may_take_action(TAKE_ACTION_ANCILLARY_LIMBS))
        return;
    c.self().inc_counter(COUNTER_ANCILLARY_LIMBS, +1);
}


// Line 4. Line: 1 damage. Effect: Allies in the line mutate instead of taking damage.
void experimental_surgery(unit_action_context &c)
{
    list<unit_context *> us = c.player_must_select_line(4);
    for (unit_context *u : us) {
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
        optional<map_pos> p = c.player_must_select_free_space(c.self().pos(), 1);
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
    list<unit_context *> us = c.self().units_in_range(2, enum_or(SELECT_UNIT_EXCLUDE_SELF, SELECT_UNIT_EXCLUDE_ENEMY));
    if (us.empty())
        return c.no_target();
    unit_context *u = c.player_must_select_unit(us);
    if (!u)
        return;
    optional<map_pos> p = c.player_must_select_free_space(c.self().pos(), 2);
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
    list<unit_context *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_NEGATIVE_TOKENS);
    if (us.empty())
        return c.no_target();
    unit_context *u = c.player_must_select_unit(us);
    if (!u)
        return;

    int removed = 0;
    for (int i = 0; i < 3; ++i) {
        list<token_context *> ts = u->tokens();
        token_context *t = c.player_may_select_token(ts, SELECT_TOKEN_ONLY_NEGATIVE);
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
    list<unit_context *> us = c.self().units_in_range(3);
    if (us.empty())
        return c.no_target();
    unit_context *u = c.player_must_select_unit(us);
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
    list<unit_context *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.no_target();
    unit_context *u = c.player_must_select_unit(us);
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
    list<unit_context *> us = c.self().units_in_range(2, 5);
    if (us.empty())
        return c.no_target();
    unit_context *u = c.player_must_select_unit(us);
    if (!u)
        return;
    if (!c.is_hit(*u, c.player_roll_d6(c.self(), ROLL_TAG_ATTACK)))
        return u->take_damage(1, DAMAGE_GRAZE);

    int effect = c.player_may_spare_parts(c.self()) ? 2 : 1;
    while (effect--) {
        list<token_context *> ts = u->tokens();
        token_context *t = c.player_may_select_token(ts, SELECT_TOKEN_ONLY_POSITIVE);
        if (!t)
            break;
        u->remove_token(*t);
        u->take_damage(1, DAMAGE_TOXIN);
    }
}

// Line 3. Effect: Line: 1 fire damage and inflict 1 vulnerable, (3-4) OR 1 toxic damage and inflict 1 slow, (5-6) OR 1 curse damage and inflict 1 weak. Spare Parts: Roll 1D for effect per corpse consumed and choose any result.
void chaos_beam(unit_action_context &c)
{
    list<unit_context *> us = c.player_must_select_line(3);

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

    for (unit_context *u : us) {
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


// (1 SOUL): Own or allied turn. Range 3. Effect: Swap places with an allied unit, then both of you mutate.
void flesh_jump(unit_action_context &c)
{
    if (!c.player_may_spend_soul(1))
        return c.no_resources();

    list<unit_context *> us = c.self().units_in_range(3, enum_or(SELECT_UNIT_EXCLUDE_SELF, SELECT_UNIT_EXCLUDE_ENEMY));
    if (us.empty())
        return c.no_target();

    unit_context *u = c.player_must_select_unit(us);
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
    if (!c.player_may_spend_soul(2))
        return c.no_resources();

    list<unit_context *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_MUTATION_TOKENS);
    if (us.empty())
        return c.no_target();

    unit_context *u = c.player_must_select_unit(us);
    if (!u)
        return;

    token_context *t = u->find_token(TOKEN_MUTATION);
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
    if (!c.player_may_spend_soul(2))
        return c.no_resources();

    list<unit_context *> us = c.self().units_in_range(3, SELECT_UNIT_WITH_MUTATION_TOKENS);
    if (us.empty())
        return c.no_target();

    unit_context *u = c.player_must_select_unit(us);
    if (!u)
        return;

    token_context *t = u->find_token(TOKEN_MUTATION);
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
    if (!c.player_may_spend_soul(3))
        return c.no_resources();

    unit_context &u = c.activated();
    if (u.is_curseproof())
        return c.no_target();

    bool dead = false;
    u.take_damage(1, DAMAGE_TOXIN, &dead);
    if (!dead)
        return;
    c.obliterate(u);
    optional<map_pos> p = c.player_must_select_free_space(u.pos(), 1);
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
    if (!c.player_may_spend_soul(4))
        return c.no_resources();

    unit_context &u = c.activated();
    if (u.is_curseproof())
        return c.no_target();

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
    c.self().inc_counter(COUNTER_CURSEPROOF, +1);
    c.self().inc_counter(COUNTER_CURSEPROOF, +1);
    c.self().inc_counter(COUNTER_FREE_MOVEMENT, +1);

    c.self().set_counter(COUNTER_ALTERED_MV, 6);
    c.self().set_counter(COUNTER_ALTERED_DF, 6);
    c.self().add_token(TOKEN_STRENGTH, 6);

    c.self().set_counter(COUNTER_FINAL_FORM, 2);
}

list<action> chirurgeon_actions()
{
    return {
        {TRIGGER_TURN_START, polyglot},
        {TRIGGER_TURN_END, accelerate_evolution},
        {TRIGGER_TURN_START, rapid_move_ts}, {TRIGGER_TURN_END, rapid_move_te},
        {TRIGGER_BEFORE_ATTACK, ancillary_limbs},

        {TRIGGER_ACTION_MANUAL, experimental_surgery},
        {TRIGGER_ACTION_MANUAL, new_material},
        {TRIGGER_ACTION_MANUAL, clone},
        {TRIGGER_ACTION_MANUAL, stitch_fix},
        {TRIGGER_ACTION_MANUAL, inject_stimulant},
        {TRIGGER_ACTION_MANUAL, biotoxin_injector},
        {TRIGGER_ACTION_MANUAL, mutagen_injector},
        {TRIGGER_ACTION_MANUAL, chaos_beam},

        {TRIGGER_ACTION_MANUAL, flesh_jump},
        {TRIGGER_ACTION_MANUAL, grow_bonus_legs},
        {TRIGGER_ACTION_MANUAL, grow_bonus_limbs},
        {TRIGGER_TURN_END, recycle},
        {TRIGGER_TURN_START, devolve},
        {TRIGGER_TURN_START, final_form},
    };
}

int main()
{
    cout << "hi!\n";
    return 0;
}
