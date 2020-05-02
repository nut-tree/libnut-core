#pragma once
#ifndef WINDOW_H
#define WINDOW_H

class Window {
    public:
        Window(int x, int y, int width, int height);
        void show(int duration, float opacity);

    private:
        int _x;
        int _y;
        int _width;
        int _height;
};

#endif