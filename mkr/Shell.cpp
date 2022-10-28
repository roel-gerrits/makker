//
// Created by roel on 7/17/22.
//

#include "Shell.h"

#include <string>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

constexpr int escape = 27;
constexpr int backspace = 127;

constexpr int control(char c) {
    return c & 0x1f;
}

Shell::Shell() :
        exit_flag(false) {}

Shell::~Shell() {}


enum class Key {
    CTRLD,
    DELETE,
    BACKSPACE,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    END,
    HOME,
    RETURN,
    TAB,
    CHARACTER
};

struct Event {
    Key key;
    char character;
};

Event getch() {
    while (true) {
        const int ch = getchar();

        if (ch == control('d')) {
            return {Key::CTRLD, 0};
        }

        if (ch == escape) {
            const int ctrl1 = getchar();
            const int ctrl2 = getchar();
            if (ctrl1 == 91) {
                if (ctrl2 == 65) return {Key::UP, 0};
                if (ctrl2 == 66) return {Key::DOWN, 0};
                if (ctrl2 == 68) return {Key::LEFT, 0};
                if (ctrl2 == 67) return {Key::RIGHT, 0};
                if (ctrl2 == 70) return {Key::END, 0};
                if (ctrl2 == 72) return {Key::HOME, 0};
                if (ctrl2 == 49) {
                    const int ctrl3 = getchar();
                    const int ctrl4 = getchar();
                    printf("#### %d %d %d %d\n", ctrl1, ctrl2, ctrl3, ctrl4);
                    continue;
                }
                if (ctrl2 == 50) {
                    const int ctrl3 = getchar();
                    printf("### %d %d %d\n", ctrl1, ctrl2, ctrl3);
                    continue;
                }
                if (ctrl2 == 51) {
                    const int ctrl3 = getchar();
                    if (ctrl3 == 126) return {Key::DELETE, 0};
                    printf("### %d %d %d\n", ctrl1, ctrl2, ctrl3);
                    continue;
                }
                if (ctrl2 == 53) {
                    const int ctrl3 = getchar();
                    printf("### %d %d %d\n", ctrl1, ctrl2, ctrl3);
                    continue;
                }
                if (ctrl2 == 54) {
                    const int ctrl3 = getchar();
                    printf("### %d %d %d\n", ctrl1, ctrl2, ctrl3);
                    continue;
                }
            }
            printf("## %d %d \n", ctrl1, ctrl2);
            continue;
        }

        if (ch == backspace) {
            return {Key::BACKSPACE, 0};
        }

        if (ch == '\n') {
            return {Key::RETURN, 0};
        }

        if (ch == '\t') {
            return {Key::TAB, 0};
        }

        if (ch >= ' ' && ch <= '~') {
            return {Key::CHARACTER, static_cast<char>(ch)};
        }

        printf("# %d \n", ch);

    }
}

void Shell::run(ShellHandler &handler) {
    termios old_term_settings{};
    tcgetattr(STDIN_FILENO, &old_term_settings);

    termios term_settings = old_term_settings;
    term_settings.c_lflag &= ~(ICANON);
    term_settings.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term_settings);

    unsigned int cursor = 0;

    while (true) {
//        printf("starting loop, exit_flag=%u, line=%s \n", exit_flag, line.c_str());

        printf("\u001b[1000D");
        printf("\u001b[2K");
        printf(">>> %s", line.c_str());
        printf("\u001b[1000D");
        printf("\u001b[%uC", cursor + 4);

        if (exit_flag) break;

        Event event = getch();
        if (event.key == Key::CTRLD) {
            handler.handle_exit();
//            printf("ctrld handled \n");
            continue;
        }

        if (event.key == Key::UP) {
            // TODO
            continue;
        }

        if (event.key == Key::DOWN) {
            // TODO
            continue;
        }

        if (event.key == Key::LEFT) {
            cursor = (cursor > 0) ? cursor - 1 : 0;
            continue;
        }

        if (event.key == Key::RIGHT) {
            cursor = (cursor < line.size()) ? cursor + 1 : line.size();
            continue;
        }

        if (event.key == Key::HOME) {
            cursor = 0;
            continue;
        }

        if (event.key == Key::END) {
            cursor = line.size();
            continue;
        }

        if (event.key == Key::DELETE) {
            line.erase(cursor, 1);
            continue;
        }

        if (event.key == Key::BACKSPACE) {
            cursor = (cursor > 0) ? cursor - 1 : 0;
            line.erase(cursor, 1);
            continue;
        }

        if (event.key == Key::RETURN) {
            handler.handle_line();
            cursor = line.length();
            printf("\n");
            continue;
        }

        if (event.key == Key::TAB) {
            // TODO
            continue;
        }

        if (event.key == Key::CHARACTER) {
            line.insert(cursor, 1, event.character);
            cursor++;
            continue;
        }
    }
    printf("\n");
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term_settings);
}

void Shell::println(const std::string &str) {
    printf("\n%s", str.c_str());
}

const std::string &Shell::get_line() const {
    return line;
}

void Shell::set_line(const std::string &str) {
    line = str;
}

void Shell::exit() {
    exit_flag = true;
}
