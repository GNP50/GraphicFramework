//
// Created by Pierluigi Carlino on 19/03/22.
//

#include <Terminal.h>

Terminal* Terminal::instance = nullptr;

Terminal &Terminal::getInstance() {
    if(instance == nullptr)
        instance = new Terminal();
    return *instance;
}