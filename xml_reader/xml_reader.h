// Created by Ерошев Артём

#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include "combiner.h"
#include "../pugixml/pugixml.hpp"


namespace xml_rd
{
    class ManagerXML {
    public:
        explicit ManagerXML(size_t size_cache = 1024);
        ~ManagerXML() = default;

        void load_file(const std::string &);

        // TODO Rewrite to iter
        void show_tree(std::ostream &out = std::cout) const;
        void put(std::unique_ptr<XMLBlock>);
        void save();
        void rollback();
        void step_back();
        bool exist(std::string&);
    private:
        pugi::xml_document xml_doc;
        // TODO needed this path?
        std::string path;
        std::vector<XMLBlock> cache_;
        std::unordered_map<std::string, std::unordered_set<XMLEmploy> > tree;
        size_t size_cache;
        unsigned long pointer_last_record;
        bool is_open;
    };
}
