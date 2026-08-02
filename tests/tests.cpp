#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "../src/headers/playerManager.hpp"
#include "../src/headers/worldManager.hpp"

// Player Tests
TEST_CASE("Player - Initialization", "[player]") {
    Player p;
    SECTION("Default values") {
        CHECK(p.m_IsMoving == false);
        CHECK(p.m_health > 0);
        CHECK(p.m_StateEnum == State::idle);
    }
}

TEST_CASE("Player - States", "[player]") {
    Player p;
    SECTION("Idle") { p.m_StateEnum = State::idle; CHECK(p.m_StateEnum == State::idle); }
    SECTION("Walk") { p.m_StateEnum = State::walk; CHECK(p.m_StateEnum == State::walk); }
    SECTION("Run") { p.m_StateEnum = State::run; CHECK(p.m_StateEnum == State::run); }
    SECTION("Jump") { p.m_StateEnum = State::jump; CHECK(p.m_StateEnum == State::jump); }
    SECTION("Slash") { p.m_StateEnum = State::slash; CHECK(p.m_StateEnum == State::slash); }
    SECTION("Sit") { p.m_StateEnum = State::sit; CHECK(p.m_StateEnum == State::sit); }
}

TEST_CASE("Player - Directions", "[player]") {
    Player p;
    SECTION("Up") { p.m_DirectionEnum = Directions::up; CHECK(p.m_DirectionEnum == Directions::up); }
    SECTION("Down") { p.m_DirectionEnum = Directions::down; CHECK(p.m_DirectionEnum == Directions::down); }
    SECTION("Left") { p.m_DirectionEnum = Directions::left; CHECK(p.m_DirectionEnum == Directions::left); }
    SECTION("Right") { p.m_DirectionEnum = Directions::right; CHECK(p.m_DirectionEnum == Directions::right); }
}

TEST_CASE("Player - Position", "[player]") {
    Player p;
    p.m_playerXY = {100.0f, 200.0f};
    CHECK(p.m_playerXY.x > 99.9f);
    CHECK(p.m_playerXY.x < 100.1f);
    CHECK(p.m_playerXY.y > 199.9f);
    CHECK(p.m_playerXY.y < 200.1f);
}

TEST_CASE("Player - Coordinate Utility", "[player]") {
    auto [x, y] = getPlayerXY({150.5f, 250.7f});
    CHECK(x == 150);
    CHECK(y == 250);
}

TEST_CASE("Player - Animation Frames", "[player]") {
    CHECK(kFrameCount[to_index(State::idle)] == 2);
    CHECK(kFrameCount[to_index(State::walk)] == 9);
    CHECK(kFrameCount[to_index(State::run)] == 8);
}

TEST_CASE("Player - Ocean Detection", "[player]") {
    Player p;
    p.m_IsTileOcean = false;
    CHECK(p.m_IsTileOcean == false);

    p.m_IsTileOcean = true;
    CHECK(p.m_IsTileOcean == true);
}

// Chunk Tests
TEST_CASE("Chunk - Initialization", "[chunk]") {
    Chunk c;
    CHECK(c.m_isGenerated == false);
}

TEST_CASE("Chunk - With Coordinates", "[chunk]") {
    Chunk c(5, 10);
    CHECK(c.m_chunkX == 5);
    CHECK(c.m_chunkY == 10);
    CHECK(c.tiles.size() == CHUNK_SIZE * CHUNK_SIZE);
}

TEST_CASE("Chunk - Negative Coordinates", "[chunk]") {
    Chunk c(-3, -7);
    CHECK(c.m_chunkX == -3);
    CHECK(c.m_chunkY == -7);
}

TEST_CASE("Chunk - Tile Access", "[chunk]") {
    Chunk c(0, 0);
    c.tileAt(0, 0).type = BlockType::plains;
    CHECK(c.tileAt(0, 0).type == BlockType::plains);
}

TEST_CASE("Chunk - Tile Modification", "[chunk]") {
    Chunk c(0, 0);
    c.tileAt(5, 5).type = BlockType::ocean;
    CHECK(c.tileAt(5, 5).type == BlockType::ocean);

    c.tileAt(10, 15).type = BlockType::mountain;
    CHECK(c.tileAt(10, 15).type == BlockType::mountain);
}

TEST_CASE("Chunk - BlockTypes", "[chunk]") {
    CHECK(static_cast<int>(BlockType::plains) == 0);
    CHECK(static_cast<int>(BlockType::beach) == 1);
    CHECK(static_cast<int>(BlockType::dirt) == 2);
    CHECK(static_cast<int>(BlockType::ocean) == 3);
    CHECK(static_cast<int>(BlockType::forest) == 4);
    CHECK(static_cast<int>(BlockType::mountain) == 5);
}

TEST_CASE("Chunk - Constants", "[chunk]") {
    CHECK(CHUNK_SIZE == 32);
    CHECK(CHUNK_RADIUS == 2);
    CHECK(pixel_size == 30);
    CHECK(chunk_area == 960);
}

TEST_CASE("Chunk - Coordinate Conversion at Origin", "[chunk]") {
    auto [cx, cy] = getChunkXY({0.0f, 0.0f});
    CHECK(cx == 0);
    CHECK(cy == 0);
}

TEST_CASE("Chunk - Coordinate Conversion in Chunk", "[chunk]") {
    auto [cx, cy] = getChunkXY({500.0f, 500.0f});
    CHECK(cx == 0);
    CHECK(cy == 0);
}

TEST_CASE("Chunk - Coordinate Conversion across Boundary", "[chunk]") {
    auto [cx, cy] = getChunkXY({1000.0f, 1000.0f});
    CHECK(cx == 1);
    CHECK(cy == 1);
}

TEST_CASE("Chunk - Corner Tiles Accessible", "[chunk]") {
    Chunk c(0, 0);
    CHECK_NOTHROW(c.tileAt(0, 0));
    CHECK_NOTHROW(c.tileAt(CHUNK_SIZE - 1, 0));
    CHECK_NOTHROW(c.tileAt(0, CHUNK_SIZE - 1));
    CHECK_NOTHROW(c.tileAt(CHUNK_SIZE - 1, CHUNK_SIZE - 1));
}

TEST_CASE("Chunk - All Tiles Initialized", "[chunk]") {
    Chunk c(0, 0);
    bool allPlains = true;
    for (int y = 0; y < CHUNK_SIZE; ++y) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            if (c.tileAt(x, y).type != BlockType::plains) {
                allPlains = false;
            }
        }
    }
    CHECK(allPlains == true);
}

// WorldManager Tests
TEST_CASE("WorldManager - Default Constructor", "[world]") {
    WorldManager wm;
    CHECK(wm.m_chunkGenerationLimit >= 0);
}

TEST_CASE("WorldManager - With Parameters", "[world]") {
    std::pair<float, float> pos = {100.0f, 200.0f};
    uint32_t seed = 12345;
    WorldManager wm(pos, seed);
    CHECK(wm.m_playerchunk_X > 99.9f);
    CHECK(wm.m_playerchunk_X < 100.1f);
    CHECK(wm.m_playerchunk_Y > 199.9f);
    CHECK(wm.m_playerchunk_Y < 200.1f);
    CHECK(wm.m_world_seed == 12345);
}

TEST_CASE("WorldManager - Different Seeds", "[world]") {
    std::pair<float, float> pos = {0.0f, 0.0f};
    WorldManager wm1(pos, 12345);
    WorldManager wm2(pos, 54321);
    CHECK(wm1.m_world_seed != wm2.m_world_seed);
}

TEST_CASE("WorldManager - Same Seed Consistency", "[world]") {
    std::pair<float, float> pos = {0.0f, 0.0f};
    WorldManager wm1(pos, 99999);
    WorldManager wm2(pos, 99999);
    CHECK(wm1.m_world_seed == wm2.m_world_seed);
}

TEST_CASE("WorldManager - Biomes Count", "[world]") {
    WorldManager wm;
    CHECK(wm.tilevariation.size() == 10);
}

TEST_CASE("WorldManager - Biome Names", "[world]") {
    WorldManager wm;
    CHECK(wm.tilevariation[0] == "plains");
    CHECK(wm.tilevariation[1] == "beach");
    CHECK(wm.tilevariation[2] == "dirt");
    CHECK(wm.tilevariation[3] == "ocean");
    CHECK(wm.tilevariation[4] == "forest");
    CHECK(wm.tilevariation[5] == "mountain");
    CHECK(wm.tilevariation[6] == "swamp");
    CHECK(wm.tilevariation[7] == "jungle");
    CHECK(wm.tilevariation[8] == "frozenplain");
    CHECK(wm.tilevariation[9] == "snow");
}

TEST_CASE("WorldManager - Chunk Queues", "[world]") {
    std::pair<float, float> pos = {50.0f, 50.0f};
    WorldManager wm(pos, 42);
    CHECK(wm.m_active_chunks.size() >= 0);
    CHECK(wm.m_required_chunks.size() >= 0);
    CHECK(wm.m_unrequired_chunks.size() >= 0);
}

TEST_CASE("WorldManager - Tile Type Consistency", "[world]") {
    std::pair<float, float> pos = {0.0f, 0.0f};
    WorldManager wm(pos, 999);
    std::string type1 = wm.getTileType({50.0f, 50.0f});
    std::string type2 = wm.getTileType({50.0f, 50.0f});
    CHECK(type1 == type2);
    CHECK(!type1.empty());
}

TEST_CASE("WorldManager - Tile Color", "[world]") {
    WorldManager wm;
    wm.m_tileColor = 0xFF0000FF;
    CHECK(wm.m_tileColor == 0xFF0000FF);
}

TEST_CASE("Integration - Player Coordinate to XY", "[integration]") {
    auto [px, py] = getPlayerXY({500.5f, 600.7f});
    CHECK(px == 500);
    CHECK(py == 600);
}

TEST_CASE("Integration - World and Player Position", "[integration]") {
    std::pair<float, float> playerPos = {1500.0f, 2000.0f};
    WorldManager wm(playerPos, 12345);
    CHECK(wm.m_playerchunk_X > 1499.9f);
    CHECK(wm.m_playerchunk_X < 1500.1f);
    CHECK(wm.m_playerchunk_Y > 1999.9f);
    CHECK(wm.m_playerchunk_Y < 2000.1f);
}

// Parametrized Tests
TEST_CASE("Parametrized - Player Positions", "[parametrized]") {
    auto [x, y] = GENERATE(
        std::make_tuple(0.0f, 0.0f),
        std::make_tuple(100.0f, 200.0f),
        std::make_tuple(-50.0f, 150.0f),
        std::make_tuple(999.9f, 1000.1f)
    );
    Player p;
    p.m_playerXY = {x, y};
    CHECK(p.m_playerXY.x > x - 1.0f);
    CHECK(p.m_playerXY.x < x + 1.0f);
    CHECK(p.m_playerXY.y > y - 1.0f);
    CHECK(p.m_playerXY.y < y + 1.0f);
}

TEST_CASE("Parametrized - Chunk Coordinates", "[parametrized]") {
    auto [cx, cy] = GENERATE(
        std::make_tuple(0, 0),
        std::make_tuple(5, 10),
        std::make_tuple(-3, -7),
        std::make_tuple(100, -50)
    );
    Chunk c(cx, cy);
    CHECK(c.m_chunkX == cx);
    CHECK(c.m_chunkY == cy);
}

TEST_CASE("Parametrized - World Seeds", "[parametrized]") {
    auto seed = GENERATE(1u, 42u, 12345u, 999999u);
    std::pair<float, float> pos = {0.0f, 0.0f};
    WorldManager wm(pos, seed);
    CHECK(wm.m_world_seed == seed);
}

// Edge Cases
TEST_CASE("Edge Cases - Large Positive Coordinates", "[edge-cases]") {
    auto [cx, cy] = getChunkXY({10000.0f, 10000.0f});
    CHECK(cx >= 0);
    CHECK(cy >= 0);
}

TEST_CASE("Edge Cases - Large Negative Coordinates", "[edge-cases]") {
    auto [cx, cy] = getChunkXY({-10000.0f, -10000.0f});
    CHECK(cx <= 0);
    CHECK(cy <= 0);
}

TEST_CASE("Edge Cases - Zero Coordinates", "[edge-cases]") {
    auto [cx, cy] = getChunkXY({0.0f, 0.0f});
    CHECK(cx == 0);
    CHECK(cy == 0);
}

TEST_CASE("Edge Cases - Chunk Health Positive", "[edge-cases]") {
    Player p;
    CHECK(p.m_health > 0);
}

TEST_CASE("Edge Cases - Player Default Not Moving", "[edge-cases]") {
    Player p;
    CHECK(p.m_IsMoving == false);
    CHECK(p.m_IsJumping == false);
    CHECK(p.m_IsRunning == false);
}