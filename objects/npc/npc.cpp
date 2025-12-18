#include "npc.h"

using lm = std::lock_guard<std::mutex>;

NPC::NPC(NpcType t, const std::string& n, int _x, int _y) : type(t), name(n), x(_x), y(_y) {}
NPC::NPC(NpcType t, std::istream& is) : type(t) {
    is >> name >> x >> y;
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win) {
    for (auto& o : observers)
        o->on_fight(shared_from_this(), defender, win);
}

bool NPC::is_close(const std::shared_ptr<NPC>& other, size_t distance) const {
    int dx = x - other->x;
    int dy = y - other->y;
    return dx * dx + dy * dy <= (int)(distance * distance);
}

void NPC::save(std::ostream& os) {
    os << name << std::endl << x << std::endl << y << std::endl;
}

void NPC::move(int shift_x, int shift_y, int max_x, int max_y) {
    lm lck(mtx);

    int new_x = x + shift_x;
    int new_y = y + shift_y;

    x = std::clamp(new_x, 0, max_x);
    y = std::clamp(new_y, 0, max_y);
}

bool NPC::is_alive() const {
    lm lck(mtx);
    return alive;
}

void NPC::must_die() {
    lm lck(mtx);
    alive = false;
}

std::pair<int, int> NPC::position() const {
    lm lck(mtx);
    return {x, y};
}

int NPC::get_move_distance() const {
    switch (type) {
        case PrincessType: return 1;
        case DragonType: return 50;
        case KnightType: return 30;
        default: return 0;
    }
}

int NPC::get_kill_distance() const {
    switch (type) {
        case PrincessType: return 1;
        case DragonType: return 30;
        case KnightType: return 10;
        default: return 0;
    }
}

std::string NPC::get_color() const {
    switch (type) {
        case PrincessType: return "\033[35m";
        case DragonType: return "\033[31m";
        case KnightType: return "\033[34m";
        default: return "\033[0m";
    }
}

std::ostream& operator<<(std::ostream& os, NPC& npc) {
    os << "{ x:" << npc.x << ", y:" << npc.y << "} ";
    return os;
}
