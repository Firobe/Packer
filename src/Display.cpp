#ifdef ENABLE_DISPLAY
#include <SFML/Graphics.hpp>

#include <stdexcept>

#include "Display.hpp"
#include "common.hpp"
#include "Log.hpp"

#define FPS 60.

using namespace std;

void Display::render() {
    _window.clear();

    for (auto && f : _toDraw)
        _window.draw(f);

    _mustRender = false;
    }

void Display::updateByIdentifier(unsigned id) {
    unsigned i;
	if(_ids.size() != _shapes.size()){
		LOG(warning) << "Forced reset" << endl;
		reset();
		return;
	}

    for (i = 0 ; i < _ids.size() ; ++i)
        if (_ids[i] == id)
            break;

    if (i == _shapes.size())
        throw runtime_error("Invalid update ID");

    updateShape(i);
    }

void Display::updateShape(unsigned i) {
    const Ring& r = _shapes[i].getMultiP()[0].outer();
    _toDraw[i].setPointCount(r.size());

    for (unsigned j = 0 ; j < r.size() ; ++j)
        _toDraw[i].setPoint(j, sf::Vector2f(r[j].x(), r[j].y()));
	//_toDraw[i].move(-500, -300);
    _mustRender = true;
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
	_toDraw.resize(_shapes.size());
	_ids.resize(_shapes.size());
    for (unsigned i = 0 ; i < _ids.size() ; ++i)
        _ids[i] = _shapes[i].getID();

    for (unsigned i = 0 ; i < _toDraw.size() ; ++i) {
        _toDraw[i].setFillColor(sf::Color::Transparent);
        _toDraw[i].setOutlineThickness(2);
        _toDraw[i].setOutlineColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
        updateShape(i);
        }
	_mustRender = true;
    }

#endif
