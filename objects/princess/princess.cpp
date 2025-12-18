#include "princess.h"
#include "dragon.h"
#include "knight.h"

Princess::Princess(const std::string& name, int x, int y) : NPC(PrincessType, name, x, y) {}
Princess::Princess(std::istream& is) : NPC(PrincessType, is) {}

void Princess::print(std::ostream& os) {
    os << *this;
}

void Princess::save(std::ostream& os) {
    os << PrincessType << std::endl;
    NPC::save(os);
}

bool Princess::visit([[maybe_unused]] std::shared_ptr<Princess> other) {
    return false;
}

bool Princess::visit([[maybe_unused]] std::shared_ptr<Dragon> other) {
    return false;
}

bool Princess::visit([[maybe_unused]] std::shared_ptr<Knight> other) {
    return false;
}

bool Princess::accept(std::shared_ptr<NPC> attacker) {
    return attacker->visit(std::dynamic_pointer_cast<Princess>(shared_from_this()));
}

std::ostream& operator<<(std::ostream& os, Princess& princess) {
    os << "Принцесса: " << princess.name << " " << *static_cast<NPC*>(&princess) << std::endl;
    return os;
}
