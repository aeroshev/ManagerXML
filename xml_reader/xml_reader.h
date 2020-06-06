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
#include <sstream>
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

    struct XMLDepart {
        std::string name;
        unsigned int qty_employ;
        double avg_salary;
    };

    struct XMLBlock {
        type_operation type;
        XMLEmploy *employ;
        XMLDepart *depart;
    };

    class CombineBlock {
    public:
        CombineBlock() = default;
        ~CombineBlock() = default;
        // TODO Memory handle
        static std::unique_ptr<XMLBlock> insert_employ(std::istream &);

    private:
        static inline void fill_employ(XMLEmploy *, std::istream &);
        // TODO dynamic count avg salary and qty employee
        static inline void fill_depart(XMLDepart *, std::istream &);
    };

    class ManagerXML {
    public:
        explicit ManagerXML(size_t size_cache = 1024);
        ~ManagerXML() = default;

        void load_file(const std::string &path = "");

        // TODO Rewrite to iter
        void show_tree(std::ostream &out = std::cout) const;
        void put(const XMLBlock &);
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
    };
}