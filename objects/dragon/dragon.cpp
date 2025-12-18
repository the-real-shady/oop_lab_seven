#include "dragon.h"
#include "princess.h"
#include "knight.h"

Dragon::Dragon(const std::string& name, int x, int y) : NPC(DragonType, name, x, y) {}
Dragon::Dragon(std::istream& is) : NPC(DragonType, is) {}

void Dragon::print(std::ostream& os) {
    os << *this;
}

void Dragon::save(std::ostream& os) {
    os << DragonType << std::endl;
    NPC::save(os);
}

bool Dragon::visit([[maybe_unused]] std::shared_ptr<Princess> other) {
    int defense = std::rand() % 6 + 1;
    int attack = std::rand() % 6 + 1;

    if (attack > defense) {
        fight_notify(std::static_pointer_cast<NPC>(other), true);
        return true;
    }

    return false;
}

bool Dragon::visit([[maybe_unused]] std::shared_ptr<Dragon> other) {
    return false;
}

bool Dragon::visit([[maybe_unused]] std::shared_ptr<Knight> other) {
    return false;
}

bool Dragon::accept(std::shared_ptr<NPC> attacker) {
    return attacker->visit(std::dynamic_pointer_cast<Dragon>(shared_from_this()));
}

std::ostream& operator<<(std::ostream& os, Dragon& dragon) {
    os << "Дракон: " << dragon.name << " " << *static_cast<NPC*>(&dragon) << std::endl;
    return os;
}
