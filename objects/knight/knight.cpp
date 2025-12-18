#include "knight.h"
#include "princess.h"
#include "dragon.h"

Knight::Knight(const std::string& name, int x, int y) : NPC(KnightType, name, x, y) {}
Knight::Knight(std::istream& is) : NPC(KnightType, is) {}

void Knight::print(std::ostream& os) {
    os << *this;
}

void Knight::save(std::ostream& os) {
    os << KnightType << std::endl;
    NPC::save(os);
}

bool Knight::visit([[maybe_unused]] std::shared_ptr<Princess> other) {
    return false;
}

bool Knight::visit(std::shared_ptr<Dragon> other) {
    int defense = std::rand() % 6 + 1;
    int attack = std::rand() % 6 + 1;

    if (attack > defense) {
        fight_notify(std::static_pointer_cast<NPC>(other), true);
        return true;
    }

    return false;
}

bool Knight::visit([[maybe_unused]] std::shared_ptr<Knight> other) {
    return false;
}

bool Knight::accept(std::shared_ptr<NPC> attacker) {
    return attacker->visit(std::dynamic_pointer_cast<Knight>(shared_from_this()));
}

std::ostream& operator<<(std::ostream& os, Knight& knight) {
    os << "Странствующий рыцарь: " << knight.name << " " << *static_cast<NPC*>(&knight) << std::endl;
    return os;
}
