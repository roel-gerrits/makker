//
// Created by roel on 7/17/22.
//


#pragma once

#include <string>

class Shell;

class ShellHandler {
public:
    virtual void handle_up() = 0;

    virtual void handle_exit() = 0;

    virtual void handle_down() = 0;

    virtual void handle_tab() = 0;

    virtual void handle_line() = 0;
};

class Shell {
public:

    Shell();

    Shell(const Shell &) = delete;

    Shell(Shell &&) = delete;

    Shell &operator=(const Shell &) = delete;

    Shell &operator=(Shell &&) = delete;

    virtual ~Shell();

    void run(ShellHandler &handler);

    void println(const std::string &str);

    const std::string &get_line() const;

    void set_line(const std::string &str);

    void exit();

private:
    std::string line;
    bool exit_flag;
};



