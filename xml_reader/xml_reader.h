//
// Created by Артём on 06.06.2020.
//
#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../pugixml/pugixml.hpp"



namespace xml_rd {

    enum type_operation {
        none,
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

        bool operator==(const XMLEmploy &compare) const noexcept {
            return (surname == compare.surname &&
                    name == compare.name &&
                    middleName == compare.middleName &&
                    function == compare.function &&
                    salary == compare.salary);
        }

        size_t operator()(const XMLEmploy &hashPoint) const noexcept {
            size_t hash = std::hash<std::string>{}(surname);
            hash += std::hash<std::string>{}(name);
            hash += std::hash<std::string>{}(middleName);
            return hash;
        }
    };
}
namespace std {
    template<> struct hash<xml_rd::XMLEmploy>
    {
        std::size_t operator()(const xml_rd::XMLEmploy& h) const noexcept
        {
            return h(h);
        }
    };
}
namespace xml_rd {

    struct XMLBlock {
        XMLBlock():
        type(type_operation::none),
        newEmploy(nullptr),
        oldEmploy(nullptr),
        newNameDepart(""),
        oldNameDepart("")
        {}

        type_operation type;
        std::shared_ptr<XMLEmploy> newEmploy;
        std::shared_ptr<XMLEmploy> oldEmploy;
        std::string newNameDepart;
        std::string oldNameDepart;
        std::unordered_set<XMLEmploy> old_data;
    };

    struct CombineBlock {
        // TODO Memory handle

    };

    class ManagerXML {
    public:
        explicit ManagerXML(size_t size_cache = 1024);
        ~ManagerXML() = default;

        void load_file(const std::string &path = "");

        // TODO Rewrite to iter
        void show_tree(std::ostream &out = std::cout) const;
        void put(const XMLBlock&);
        void save();
        void rollback();
        void step_back();
        bool exist(std::string&);
    private:
        pugi::xml_document xml_doc;
        std::string path;
        std::vector<XMLBlock> cache_;
        std::unordered_map<std::string, std::unordered_set<XMLEmploy> > tree;
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

//namespace std {
//    template<> struct hash<xml_rd::XMLEmploy>
//    {
//        std::size_t operator()(const xml_rd::XMLEmploy& h) const noexcept
//        {
//            return h(h);
//        }
//    };
//}