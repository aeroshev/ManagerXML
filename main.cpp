// Created by Ерошев Артём

#include "xml_reader/xml_reader.h"
#include "xml_reader/xml_interface.h"


int main()
{
    std::ios_base::sync_with_stdio(false);

    auto manager = std::make_shared<xml_rd::ManagerXML>();

    std::string file;
    std::cout << "Enter file: ";
    getline(std::cin, file);

    try
    {
        manager->load_file(file);
    }
    catch (std::invalid_argument& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }

    auto interface = xml_rd::Interface(manager);
    interface.start();

    return 0;
}
