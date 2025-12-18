#include <gtest/gtest.h>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <optional>
#include <chrono>
#include <atomic>
#include <array>

#include "princess.h"
#include "dragon.h"
#include "knight.h"

using namespace std::chrono_literals;
std::mutex print_mutex;

constexpr int MAP_X = 50;
constexpr int MAP_Y = 50;
constexpr int GRID = 25;

struct FightEvent {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class FightManager {
private:
    std::queue<FightEvent> events;
    std::mutex mtx;
    FightManager() {}

public:
    static FightManager& get() {
        static FightManager instance;
        return instance;
    }

    void add_event(FightEvent&& event) {
        std::lock_guard<std::mutex> lck(mtx);
        events.push(event);
    }

    void operator()() {
        std::lock_guard<std::mutex> lck(mtx);
        while (!events.empty()) {
            auto event = events.front();
            events.pop();
            if (event.attacker->is_alive() && event.defender->is_alive()) {
                if (event.defender->accept(event.attacker)) {
                    event.defender->must_die();
                }
            }
        }
    }
};

void draw_map(const std::vector<std::shared_ptr<NPC>>& npcs) {
    std::array<std::pair<std::string, char>, GRID * GRID> field{};
    int princesses = 0, dragons = 0, knights = 0;

    for (auto& npc : npcs) {
        if (!npc->is_alive()) continue;

        auto [x, y] = npc->position();
        int gx = std::min(x * GRID / MAP_X, GRID - 1);
        int gy = std::min(y * GRID / MAP_Y, GRID - 1);
        char symbol;
        std::string color = npc->get_color();

        switch (npc->type) {
            case PrincessType: symbol = 'P'; ++princesses; break;
            case DragonType: symbol = 'D'; ++dragons; break;
            case KnightType: symbol = 'K'; ++knights; break;
            default: continue;
        }

        field[gx + gy * GRID] = {color, symbol};
    }

    {
        std::lock_guard<std::mutex> lck(print_mutex);
        for (int y = 0; y < GRID; ++y) {
            for (int x = 0; x < GRID; ++x) {
                auto [color, c] = field[x + y * GRID];
                if (c != ' ') std::cout << "|" << color << c << "\033[0m|";
                else std::cout << "| |";
            }
            std::cout << "\n";
        }
        std::cout << std::string(GRID * 3, '=') << "\n";
        std::cout << "Принцессы: " << princesses << " | Драконы: " << dragons << " | Рыцари: " << knights << " | Всего: " << princesses + dragons + knights << "/50\n";
    }
}

TEST(NPCCreation, CreatePrincess) {
    Princess princess("TestPrincess", 100, 200);
    EXPECT_EQ(princess.type, PrincessType);
    EXPECT_EQ(princess.name, "TestPrincess");
    EXPECT_EQ(princess.x, 100);
    EXPECT_EQ(princess.y, 200);
}

TEST(NPCCreation, CreateDragon) {
    Dragon dragon("TestDragon", 150, 250);
    EXPECT_EQ(dragon.type, DragonType);
    EXPECT_EQ(dragon.name, "TestDragon");
    EXPECT_EQ(dragon.x, 150);
    EXPECT_EQ(dragon.y, 250);
}

TEST(NPCCreation, CreateKnight) {
    Knight knight("TestKnight", 200, 300);
    EXPECT_EQ(knight.type, KnightType);
    EXPECT_EQ(knight.name, "TestKnight");
    EXPECT_EQ(knight.x, 200);
    EXPECT_EQ(knight.y, 300);
}

TEST(Serialization, SaveAndLoadPrincess) {
    auto princess = std::make_shared<Princess>("SavedPrincess", 123, 456);
    
    std::stringstream ss;
    princess->save(ss);
    
    int type;
    ss >> type;
    EXPECT_EQ(type, PrincessType);
    
    Princess loaded(ss);
    EXPECT_EQ(loaded.name, "SavedPrincess");
    EXPECT_EQ(loaded.x, 123);
    EXPECT_EQ(loaded.y, 456);
}

TEST(Serialization, SaveAndLoadDragon) {
    auto dragon = std::make_shared<Dragon>("SavedDragon", 234, 567);
    
    std::stringstream ss;
    dragon->save(ss);
    
    int type;
    ss >> type;
    EXPECT_EQ(type, DragonType);
    
    Dragon loaded(ss);
    EXPECT_EQ(loaded.name, "SavedDragon");
    EXPECT_EQ(loaded.x, 234);
    EXPECT_EQ(loaded.y, 567);
}

TEST(Serialization, SaveAndLoadKnight) {
    auto knight = std::make_shared<Knight>("SavedKnight", 345, 678);
    
    std::stringstream ss;
    knight->save(ss);
    
    int type;
    ss >> type;
    EXPECT_EQ(type, KnightType);
    
    Knight loaded(ss);
    EXPECT_EQ(loaded.name, "SavedKnight");
    EXPECT_EQ(loaded.x, 345);
    EXPECT_EQ(loaded.y, 678);
}

TEST(Print, PrincessPrint) {
    Princess princess("PrintPrincess", 10, 20);
    std::stringstream ss;
    princess.print(ss);
    EXPECT_EQ(ss.str(), "Принцесса: PrintPrincess { x:10, y:20} \n");
}

TEST(Print, DragonPrint) {
    Dragon dragon("PrintDragon", 30, 40);
    std::stringstream ss;
    dragon.print(ss);
    EXPECT_EQ(ss.str(), "Дракон: PrintDragon { x:30, y:40} \n");
}

TEST(Print, KnightPrint) {
    Knight knight("PrintKnight", 50, 60);
    std::stringstream ss;
    knight.print(ss);
    EXPECT_EQ(ss.str(), "Странствующий рыцарь: PrintKnight { x:50, y:60} \n");
}

TEST(Movement, GetMoveDistance) {
    Princess princess("Princess", 0, 0);
    EXPECT_EQ(princess.get_move_distance(), 1);

    Dragon dragon("Dragon", 0, 0);
    EXPECT_EQ(dragon.get_move_distance(), 50);

    Knight knight("Knight", 0, 0);
    EXPECT_EQ(knight.get_move_distance(), 30);
}

TEST(Movement, StayWithinBounds) {
    Knight knight("Knight", 0, 0);
    knight.move(-10, -10, 100, 100);
    auto pos = knight.position();
    EXPECT_EQ(pos.first, 0);
    EXPECT_EQ(pos.second, 0);

    knight.move(150, 150, 100, 100);
    pos = knight.position();
    EXPECT_EQ(pos.first, 100);
    EXPECT_EQ(pos.second, 100);
}

TEST(Distance, GetKillDistance) {
    Princess princess("Princess", 0, 0);
    EXPECT_EQ(princess.get_kill_distance(), 1);

    Dragon dragon("Dragon", 0, 0);
    EXPECT_EQ(dragon.get_kill_distance(), 30);

    Knight knight("Knight", 0, 0);
    EXPECT_EQ(knight.get_kill_distance(), 10);
}

TEST(Distance, IsClose) {
    auto npc1 = std::make_shared<Princess>("Princess1", 0, 0);
    auto npc2 = std::make_shared<Knight>("Knight2", 3, 4);
    EXPECT_TRUE(npc1->is_close(npc2, 5));
    EXPECT_FALSE(npc1->is_close(npc2, 4));
}

TEST(Fight, DragonFailsToEatPrincess) {
    srand(0);
    auto dragon = std::make_shared<Dragon>("Dragon", 0, 0);
    auto princess = std::make_shared<Princess>("Princess", 0, 0);
    bool success = princess->accept(dragon);
    EXPECT_FALSE(success);
    EXPECT_TRUE(princess->is_alive());
}

class MockObserver : public IFightObserver {
public:
    bool called = false;
    std::shared_ptr<NPC> last_attacker;
    std::shared_ptr<NPC> last_defender;
    bool last_win;

    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) override {
        called = true;
        last_attacker = attacker;
        last_defender = defender;
        last_win = win;
    }
};

TEST(Observer, NoNotifyOnFightLose) {
    auto mock = std::make_shared<MockObserver>();
    auto knight = std::make_shared<Knight>("Knight", 0, 0);
    knight->subscribe(mock);
    auto dragon = std::make_shared<Dragon>("Dragon", 0, 0);
    srand(0);
    knight->visit(dragon);
    EXPECT_FALSE(mock->called);
}

TEST(MapDrawing, SimpleDrawWithOneNPC) {
    std::vector<std::shared_ptr<NPC>> npcs;
    auto princess = std::make_shared<Princess>("Princess", 25, 25);
    npcs.push_back(princess);

    ::testing::internal::CaptureStdout();
    draw_map(npcs);
    std::string output = ::testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("\033[35mP\033[0m"), std::string::npos);
    EXPECT_NE(output.find("Принцессы: 1"), std::string::npos);
}

TEST(EdgeCases, EmptyName) {
    Princess princess("", 0, 0);
    EXPECT_EQ(princess.name, "");
}

TEST(EdgeCases, NegativeCoordinates) {
    Dragon dragon("Dragon", -100, -200);
    EXPECT_EQ(dragon.x, -100);
    EXPECT_EQ(dragon.y, -200);
}

TEST(EdgeCases, NegativeCoordinatesMove) {
    Dragon dragon("Dragon", -100, -200);
    dragon.move(-10, -10, 100, 100);
    auto pos = dragon.position();
    EXPECT_EQ(pos.first, 0);
    EXPECT_EQ(pos.second, 0);
}

TEST(EdgeCases, ZeroDistance) {
    auto princess = std::make_shared<Princess>("Princess", 0, 0);
    auto dragon = std::make_shared<Dragon>("Dragon", 1, 1);
    EXPECT_FALSE(princess->is_close(dragon, 0));
}

TEST(Integration, SaveAndLoadFile) {
    set_t original;
    original.insert(std::make_shared<Princess>("Princess1", 100, 200));
    original.insert(std::make_shared<Dragon>("Dragon1", 150, 250));
    original.insert(std::make_shared<Knight>("Knight1", 200, 300));
    
    std::string filename = "test_npc.txt";
    std::ofstream ofs(filename);
    ofs << original.size() << std::endl;
    for (auto& npc : original)
        npc->save(ofs);
    ofs.close();
    
    set_t loaded;
    std::ifstream ifs(filename);
    ASSERT_TRUE(ifs.is_open());
    
    int count;
    ifs >> count;
    EXPECT_EQ(count, 3);
    
    ifs.close();
    std::remove(filename.c_str());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
