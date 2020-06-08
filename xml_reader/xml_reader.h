//
// Created by Артём on 06.06.2020.
//
#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "../pugixml/pugixml.hpp"

namespace xml_rd {

    enum type_operation {
        write,
        erase,
        change
    };

    struct XMLEmploy {
        std::string surname;
        std::string name;
        std::string middleName;
        std::string function;
        unsigned int salary;
    };

    struct XMLBlock {
        type_operation type;
        XMLEmploy employ;
        std::string depart;
    };

    struct CombineBlock {
        // TODO Memory handle
        static std::unique_ptr<XMLBlock> create_employ(std::istream &, const std::string&);
        static std::unique_ptr<XMLBlock> create_department(std::istream &);
    };

    class ManagerXML {
    public:
        explicit ManagerXML(size_t size_cache = 1024);
        ~ManagerXML() = default;

        void load_file(const std::string &path = "");

        // TODO Rewrite to iter
        void show_tree(std::ostream &out = std::cout) const;
        void put(std::unique_ptr<XMLBlock>);
        void save();
        void rollback();
        void step_back();
        bool exist(std::string&);
    private:
        pugi::xml_document xml_doc;
        std::unordered_map<unsigned long, XMLBlock> cache_;
        std::unordered_map<std::string, std::vector<XMLEmploy> > tree;
        size_t size_cache;
        unsigned long pointer_last_record;
    };

    class Interface {
    public:
        explicit Interface(std::shared_ptr<ManagerXML>);
        ~Interface();

        void start();
    private:
        std::shared_ptr<ManagerXML> manager;
        std::string current_department;
    };
}