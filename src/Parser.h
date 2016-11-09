#ifndef PARSER__H
#define PARSER__H

#include <string>
#include <vector>

class Shape;

class Parser {
private:
    std::string _path;
public:
    Parser(std::string filePath) : _path(filePath) {}
    void fillShapes(std::vector<Shape>&);
};

#endif
