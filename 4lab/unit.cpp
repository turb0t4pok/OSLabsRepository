#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <string>

constexpr int MSG_SIZE = 20;

bool isValidMessageLength(const std::string& msg) {
    return msg.length() <= MSG_SIZE;
}

TEST_CASE("Validating message length", "[validation]") {
    SECTION("Empty message - valid") {
        REQUIRE(isValidMessageLength(""));
    }

    SECTION("Message with length exactly MSG_SIZE - valid") {
        std::string msg(MSG_SIZE, 'x');
        REQUIRE(isValidMessageLength(msg));
    }

    SECTION("Message shorter than MSG_SIZE - valid") {
        std::string msg = "short";
        REQUIRE(isValidMessageLength(msg));
    }

    SECTION("Message longer than MSG_SIZE - invalid") {
        std::string msg(MSG_SIZE + 1, 'x');
        REQUIRE_FALSE(isValidMessageLength(msg));
    }

    SECTION("Boundary case - 1 less than MSG_SIZE") {
        std::string msg(MSG_SIZE - 1, 'x');
        REQUIRE(isValidMessageLength(msg));
    }

    SECTION("Boundary case - 5 more than MSG_SIZE") {
        std::string msg(MSG_SIZE + 5, 'y');
        REQUIRE_FALSE(isValidMessageLength(msg));
    }
}