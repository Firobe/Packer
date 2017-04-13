#include "Display.hpp"
bool Display::_enabled = false; //Display is disabled until Init()

#ifdef ENABLE_DISPLAY
#include <SFML/Graphics.hpp>

#include <stdexcept>

#include "common.hpp"
#include "Parser.hpp"
#include "Log.hpp"

#define FPS 60.
double Display::window_height = 600;

using namespace std;

void Display::render() {
    _window.clear();

    for (auto && f : _toDraw)
        _window.draw(f);

    sf::Text text(_text, _font, 30);
    _window.draw(text);
    _mustRender = false;
}

unsigned Display::byIdentifier(unsigned id) {
    if (_ids.size() != _shapes.size()) {
        LOG(warning) << "WARNING(Display) : Forced reset" << endl;
        reset();
    }

    unsigned i;

    for (i = 0 ; i < _ids.size() ; ++i)
        if (_ids[i] == id)
            break;

    if (i == _shapes.size())
        throw runtime_error("Invalid update ID");

    return i;
}

void Display::updateShape(unsigned i) {
    _toDraw[i].update();
    _mustRender = true;
}

void Display::updateAll() {
    for (unsigned i = 0 ; i < _shapes.size() ; ++i)
        updateShape(i);
}

void Display::loop() {
    sf::Event event;
    _window.setActive(true);

    while (!_mustExit) {
        if (_mustRender)
            render();

        sf::sleep(sf::seconds(1. / FPS));
        _window.pollEvent(event);
        _window.display();
    }
}

void Display::reset() {
    _toDraw.clear();
    _ids.resize(_shapes.size());

    for (unsigned i = 0 ; i < _ids.size() ; ++i)
        _ids[i] = _shapes[i].getID();

    for (unsigned i = 0 ; i < _shapes.size() ; ++i) {
        _toDraw.emplace_back(_shapes[i]);
        _toDraw[i].setFillColor(sf::Color::Transparent);
        _toDraw[i].setOutlineThickness(2);
        _toDraw[i].setOutlineColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        _toDraw[i].scale(WINDOW_WIDTH / Parser::getDims().x(),
                         window_height / Parser::getDims().y());
        updateShape(i);
    }

    _mustRender = true;
}

#endif
