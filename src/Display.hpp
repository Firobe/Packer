#ifdef ENABLE_DISPLAY
#ifndef DISPLAY__HPP
#define DISPLAY__HPP

#include <SFML/Graphics.hpp>
#include <thread>

#include "Shape.hpp"
#include "Log.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

class CustomShape : public sf::Shape {
private:
    const ::Shape& _shape;
    std::vector<unsigned> _begins;
public:
    CustomShape(const ::Shape& s) :
        _shape(s),
        _begins(_shape.getMultiP().size()) {
        unsigned acc = 0;

        for (unsigned i = 0 ; i < _begins.size() ; ++i) {
            _begins[i] = acc;
            acc += _shape.getMultiP()[i].outer().size();
        }
    }
    size_t getPointCount() const override {
        return _begins.back() + _shape.getMultiP().back().outer().size();
    }
    sf::Vector2f getPoint(std::size_t index) const override {
        unsigned i = 0;

        for (; i < _begins.size() and _begins[i] <= index; ++i);

        return sf::Vector2f(_shape.getMultiP()[i - 1].outer()[index - _begins[i - 1]].x(),
                            _shape.getMultiP()[i - 1].outer()[index - _begins[i - 1]].y());
    }
    void update() {
        sf::Shape::update();
    }
};

class Display {
    sf::RenderWindow _window;
    std::thread _thread;
    bool _mustExit;
    const std::vector<Shape>& _shapes;
    std::vector<CustomShape> _toDraw;
    std::vector<unsigned> _ids;
    bool _mustRender;
    void loop();
    void updateShape(unsigned i);
    void render();
    static Display& getInstance(const std::vector<Shape>& s = std::vector<Shape>()) {
        static Display disp(WINDOW_WIDTH, WINDOW_HEIGHT, s);
        return disp;
    }
public:
    static void Init(const std::vector<Shape>& s) {
        getInstance(s);
    }
    static void Update(unsigned id) {
        getInstance().updateShape(getInstance().byIdentifier(id));
    }
    static void Reset() {
        getInstance().reset();
    }
    Display(int width, int height, const std::vector<Shape>& s) :
        _window(sf::VideoMode(width, height), "PACKER3000"),
        _thread(),
        _mustExit(false),
        _shapes(s),
        _toDraw(),
        _ids(s.size()),
        _mustRender(true) {
        _window.setActive(false);
        reset();
        _thread = std::thread(&Display::loop, this);
    }
    ~Display() {
        _mustExit = true;
        _thread.join();
    }
    unsigned byIdentifier(unsigned identifier);
    void reset();
};

#endif
#endif
