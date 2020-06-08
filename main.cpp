#include "xml_reader/xml_reader.h"


int main() {
    std::ios_base::sync_with_stdio(false);

    auto manager = std::make_shared<xml_rd::ManagerXML>();

    try {
        manager->load_file("/Users/artem/CLionProjects/AVSoft2/data.xml");
    } catch (std::invalid_argument& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    auto interface = xml_rd::Interface(manager);
    interface.start();

    return 0;
}
