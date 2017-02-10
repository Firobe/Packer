#ifdef ENABLE_DISPLAY
#ifndef DISPLAY__HPP
#define DISPLAY__HPP

#include <SFML/Graphics.hpp>
#include <thread>

#include "Shape.hpp"

class Display {
        sf::RenderWindow _window;
        std::thread _thread;
        bool _mustExit;
        const std::vector<Shape>& _shapes;
        std::vector<sf::ConvexShape> _toDraw;
        std::vector<unsigned> _ids;
        bool _mustRender;
        void loop();
        void updateShape(unsigned i);
        void render();
		static Display& getInstance(const std::vector<Shape>& s = std::vector<Shape>()){
			static Display disp(1024, 768, s);
			return disp;
		}
    public:
		static void Init(const std::vector<Shape>& s){
			getInstance(s);
		}
		static void Update(unsigned id){
			getInstance().updateByIdentifier(id);
		}
		static void Reset(){
			getInstance().reset();
		}
        Display(int width, int height, const std::vector<Shape>& s) :
            _window(sf::VideoMode(width, height), "PACKER3000"),
            _thread(),
            _mustExit(false),
            _shapes(s),
            _toDraw(s.size()),
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
        void updateByIdentifier(unsigned identifier);
		void reset();
    };

#endif
#endif
