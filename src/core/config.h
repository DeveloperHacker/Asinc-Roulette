#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <vector>

#define PRINT_LINE_NUMBER() \
    std::cout << __func__ << ":" << __LINE__ << std::endl;

namespace address {
    const std::string SERVER_HOST("192.168.1.63"); // NOLINT
    const uint16_t SERVER_PORT = 2000;
}

namespace parts {
    const std::string STATUS("code"); // NOLINT
    const std::string COMMAND("command"); // NOLINT
    const std::string DATA("data"); // NOLINT
    const std::string MESSAGE("message"); // NOLINT
    const std::string LOGIN("login"); // NOLINT
    const std::string PASSWORD("password"); // NOLINT
    const std::string NAME("name"); // NOLINT
    const std::string ROLE("role"); // NOLINT
    const std::string PERMITION("permission"); // NOLINT
    const std::string LIST("list"); // NOLINT
    const std::string NUM_PLAYERS("num_players"); // NOLINT
    const std::string MAX_PLAYERS("max_players"); // NOLINT
    const std::string LOCK("lock"); // NOLINT
    const std::string BALANCE("balance"); // NOLINT
    const std::string RANDOM_NUMBER("random_number"); // NOLINT
    const std::string TYPE("type"); // NOLINT
    const std::string NUMBER("number"); // NOLINT
    const std::string VALUE("value"); // NOLINT
    const std::string BETS("bets"); // NOLINT
}

namespace stats {
    const std::string STATUS_SUCCESS("success"); // NOLINT
    const std::string STATUS_ERROR("error"); // NOLINT
}

namespace commands {
    const std::string HELP("help"); // NOLINT
    const std::string SIGNIN("signin"); // NOLINT
    const std::string SINGUP("signup"); // NOLINT
    const std::string SINGOUT("signout"); // NOLINT
    const std::string JOIN("join"); // NOLINT
    const std::string CREATE("create"); // NOLINT
    const std::string LEAVE("leave"); // NOLINT
    const std::string TABLES("tables"); // NOLINT
    const std::string USERS("users"); // NOLINT
    const std::string WRITE("write"); // NOLINT
    const std::string DISCONNECT("disconnect"); // NOLINT
    const std::string SYNC("sync"); // NOLINT
    const std::string SET_PERMISSION("set"); // NOLINT
    const std::string BALANCE("balance"); // NOLINT
    const std::string BET("bet"); // NOLINT
    const std::string SPIN("spin"); // NOLINT
    const std::string BETS("bets"); // NOLINT
    const std::string PAY("pay"); // NOLINT
    const std::string KICK("kick"); // NOLINT
    const std::string LIST("list"); // NOLINT
}

namespace other {
    const int START_UP_CAPITAL = 100; // NOLINT
    const std::string ADMIN("admin"); // NOLINT
    const std::string GUEST("guest"); // NOLINT
    const std::string CROUPIER("croupier"); // NOLINT
    const std::string PLAYER("player"); // NOLINT
    const size_t MAX_PLAYERS = 5; // NOLINT
    const std::string DATA_BASE("data_base.db"); // NOLINT
}

namespace bets {
    const int MAX_NUMBER = 37;

    const std::string RED("red"); // NOLINT
    const std::string BLACK("black"); // NOLINT
    const std::string EVEN("even"); // NOLINT
    const std::string ODD("odd"); // NOLINT
    const std::string LOW("low"); // NOLINT
    const std::string HIGH("high"); // NOLINT
    const std::string ZERO("zero"); // NOLINT
    const std::string BASKET("basket"); // NOLINT

    const std::string COLUMN("column"); // NOLINT
    const std::string LINE("line"); // NOLINT
    const std::string DOZEN("dozen"); // NOLINT
    const std::string STREET("street"); // NOLINT
    const std::string CORNER("corner"); // NOLINT
    const std::string HSPLIT("hsplit"); // NOLINT
    const std::string VSPLIT("vsplit"); // NOLINT
    const std::string STRAIGHT("straight"); // NOLINT

    const std::vector<std::string> bets({RED, BLACK, EVEN, ODD, LOW, HIGH, ZERO, BASKET, //NOLINT
                                         COLUMN, DOZEN, LINE, STREET, CORNER, HSPLIT, VSPLIT, STRAIGHT});

    const std::unordered_map<std::string, std::tuple<int, int>> ranges( //NOLINT
            {{RED,      {1, 1}},
             {BLACK,    {1, 1}},
             {EVEN,     {1, 1}},
             {ODD,      {1, 1}},
             {LOW,      {1, 1}},
             {HIGH,     {1, 1}},
             {ZERO,     {1, 1}},
             {BASKET,   {1, 1}},
             {COLUMN,   {1, 3}},
             {DOZEN,    {1, 3}},
             {LINE,     {1, 11}},
             {STREET,   {1, 12}},
             {CORNER,   {1, 22}},
             {HSPLIT,   {1, 24}},
             {VSPLIT,   {1, 33}},
             {STRAIGHT, {1, 36}}}
    );

    const std::unordered_map<std::string, int> multipliers( //NOLINT
            {{RED,      1},
             {BLACK,    1},
             {EVEN,     1},
             {ODD,      1},
             {LOW,      1},
             {HIGH,     1},
             {ZERO,     17},
             {BASKET,   6},
             {COLUMN,   2},
             {DOZEN,    2},
             {LINE,     5},
             {STREET,   11},
             {CORNER,   8},
             {HSPLIT,   17},
             {VSPLIT,   17},
             {STRAIGHT, 35}}
    );

    const std::unordered_set<int> reds = {1, 3, 5, 7, 9, 12, 14, 16, 18, 19, 21, 23, 25, 27, 30, 32, 34, 36}; //NOLINT

    const std::array<std::unordered_set<int>, 22> corners = { //NOLINT
            std::unordered_set<int>{1, 2, 4, 5},
            std::unordered_set<int>{2, 3, 5, 6},
            std::unordered_set<int>{4, 5, 7, 8},
            std::unordered_set<int>{5, 6, 8, 9},
            std::unordered_set<int>{7, 8, 10, 11},
            std::unordered_set<int>{8, 9, 11, 12},
            std::unordered_set<int>{10, 11, 13, 14},
            std::unordered_set<int>{11, 12, 14, 15},
            std::unordered_set<int>{13, 14, 16, 17},
            std::unordered_set<int>{14, 15, 17, 18},
            std::unordered_set<int>{16, 17, 19, 20},
            std::unordered_set<int>{17, 18, 20, 21},
            std::unordered_set<int>{19, 20, 22, 23},
            std::unordered_set<int>{20, 21, 23, 24},
            std::unordered_set<int>{22, 23, 25, 26},
            std::unordered_set<int>{23, 24, 26, 27},
            std::unordered_set<int>{25, 26, 28, 29},
            std::unordered_set<int>{26, 27, 29, 30},
            std::unordered_set<int>{28, 29, 31, 32},
            std::unordered_set<int>{29, 30, 32, 33},
            std::unordered_set<int>{31, 32, 34, 35},
            std::unordered_set<int>{32, 33, 35, 36}
    };
}
