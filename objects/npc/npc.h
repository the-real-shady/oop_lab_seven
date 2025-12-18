#pragma once

#include <iostream>
#include <memory>
#include <cstring>
#include <string>
#include <random>
#include <fstream>
#include <set>
#include <cmath>
#include <vector>
#include <algorithm>
#include <mutex>

struct NPC;
struct Princess;
struct Dragon;
struct Knight;
using set_t = std::set<std::shared_ptr<NPC>>;

enum NpcType {
    Unknown = 0,
    PrincessType = 1,
    DragonType = 2,
    KnightType = 3
};

struct IFightObserver {
    virtual void on_fight(const std::shared_ptr<NPC> attacker, 
                          const std::shared_ptr<NPC> defender, bool win) = 0;
};

struct NPC : public std::enable_shared_from_this<NPC> {
    NpcType type;
    std::string name;
    int x{0};
    int y{0};
    bool alive{true};
    std::vector<std::shared_ptr<IFightObserver>> observers;
    mutable std::mutex mtx;

    NPC(NpcType t, const std::string& n, int _x, int _y);
    NPC(NpcType t, std::istream& is);

    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(const std::shared_ptr<NPC> defender, bool win);
    bool is_close(const std::shared_ptr<NPC>& other, size_t distance) const;

    virtual bool visit(std::shared_ptr<Princess> other) = 0;
    virtual bool visit(std::shared_ptr<Dragon> other) = 0;
    virtual bool visit(std::shared_ptr<Knight> other) = 0;

    virtual bool accept(std::shared_ptr<NPC> attacker) = 0;

    virtual void print(std::ostream& os) = 0;
    virtual void save(std::ostream& os);

    void move(int shift_x, int shift_y, int max_x, int max_y);
    bool is_alive() const;
    void must_die();
    std::pair<int, int> position() const;
    int get_move_distance() const;
    int get_kill_distance() const;

    std::string get_color() const;

    friend std::ostream& operator<<(std::ostream& os, NPC& npc);
};
