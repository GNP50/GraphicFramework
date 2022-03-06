//
// Created by Pierluigi Carlino on 06/03/22.
//

#ifndef ASS0_TERMINAL_H
#define ASS0_TERMINAL_H

#include <iostream>
#include <string>

using namespace std;

enum class ForeGroudColor{
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
    magenta = 35,
    cyan = 36,
    white = 37
};

enum class BackGroudColor{
    black = 40,
    red = 41,
    green = 42,
    yellow = 43,
    blue = 44,
    magenta = 45,
    cyan = 46,
    white = 47
};
class Terminal {
private:
    static Terminal *instance;
    Terminal(){};
public:
    static Terminal& getInstance(){
        if(instance == nullptr)
            instance = new Terminal();
        return *instance;
    }
    void print(
            string toPrint,
            BackGroudColor backcolor = BackGroudColor::black,
            ForeGroudColor foreGroudColor = ForeGroudColor::white,
            string endChar = "\n"
           ){
            cout<<"\033["<<
                static_cast<int>(foreGroudColor)<<
                ";"<<
                   static_cast<int>(backcolor)<<"m"<<toPrint<<"\033[0m"<<endChar;
    };
};


#endif //ASS0_TERMINAL_H
