#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <fstream>
#include <libgcode/parser.h>

class interface
{
public:
    interface();
    void read_gcode_file(std::string filename);
};

#endif // INTERFACE_H
