#ifndef DISPLAY__HPP
#define DISPLAY__HPP
#ifdef ENABLE_DISPLAY

#include <SFML/Graphics.hpp>
#include <thread>

#include "Shape.hpp"
#include "Log.hpp"
#include "Layout.hpp"
#include "Parser.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

class CustomShape : public sf::Shape {
private:
    const ::Shape& _shape;
    std::vector<unsigned> _begins;
public:
    CustomShape(const ::Shape& s) :
        _shape(s),
        _begins(_shape._multiP.size()) {
        unsigned acc = 0;

        for (unsigned i = 0 ; i < _begins.size() ; ++i) {
            _begins[i] = acc;
            acc += _shape._multiP[i].outer().size();
        }
    }
    size_t getPointCount() const override {
        return _begins.back() + _shape._multiP.back().outer().size();
    }
    sf::Vector2f getPoint(std::size_t index) const override {
        unsigned i = 0;

        for (; i < _begins.size() and _begins[i] <= index; ++i);

        return sf::Vector2f(_shape._multiP[i - 1].outer()[index - _begins[i - 1]].x(),
                            _shape._multiP[i - 1].outer()[index - _begins[i - 1]].y());
    }
    void update() {
        sf::Shape::update();
    }
};

class Display {
    sf::RenderWindow _window;
    std::thread _thread;
    bool _mustExit;
    const Layout& _shapes;
    std::vector<CustomShape> _toDraw;
    std::vector<unsigned> _ids;
    bool _mustRender;
    sf::Font _font;
    std::string _text;
    void loop();
    void updateShape(unsigned i);
    void render();
    static bool _enabled;
    static double window_height;
    static Display& getInstance(const Layout& s = Layout()) {
        window_height = WINDOW_WIDTH * Parser::getDims().y() / Parser::getDims().x();
        static Display disp(WINDOW_WIDTH, window_height, s);
        return disp;
    }
    Display(int width, int height, const Layout& s) :
        _window(sf::VideoMode(width, height), "PACKER3000"),
        _thread(),
        _mustExit(false),
        _shapes(s),
        _toDraw(),
        _ids(s.size()),
        _mustRender(true),
        _text("DEFAULT TEXT") {
        _window.setActive(false);
        reset();

        if (!_font.loadFromFile("./font.ttf"))
            throw std::runtime_error("Unable to read font file");

        _thread = std::thread(&Display::loop, this);
    }
    ~Display() {
        _mustExit = true;
        _thread.join();
    }
    unsigned byIdentifier(unsigned identifier);
    void reset();
public:
    static void Init(const Layout& s) {
        _enabled = true;
        getInstance(s);
    }
    static void Update(unsigned id) {
        if (_enabled)
            getInstance().updateShape(getInstance().byIdentifier(id));
    }
    static void Reset() {
        if (_enabled)
            getInstance().reset();
    }
    static void Text(const std::string& text) {
        if (_enabled) {
            getInstance()._text = text;
            getInstance()._mustRender = true;
        }
    }
};

#else
struct Display {
    static void Init(const std::vector<Shape>&) {}
    static void Update(unsigned) {}
    static void Reset() {}
    static void Text(const std::string&) {}
};
#endif
#endif
