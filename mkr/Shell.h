//
// Created by roel on 7/17/22.
//


#pragma once


class Shell {
public:

    Shell();

    Shell(const Shell &) = delete;

    Shell(Shell &&) = delete;

    Shell &operator=(const Shell &) = delete;

    Shell &operator=(Shell &&) = delete;

    virtual ~Shell();

    void run();
};



