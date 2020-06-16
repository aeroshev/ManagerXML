// Created by Ерошев Артём

#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <filesystem>
#include "xml_combiner.h"
#include "../pugixml/pugixml.hpp"


namespace xml_rd
{
    class ManagerXML {
    public:
        explicit ManagerXML(size_t size_cache = 1024);
        ~ManagerXML() = default;

        void load_file(const std::string &);

        void show_tree(std::ostream &out = std::cout) const;
        void put(std::unique_ptr<XMLBlock>);
        void save();
        void rollback();
        void step_back();

        bool exist(std::string&);
    private:
        pugi::xml_document xml_doc;
        std::string path;
		bool is_open;

        std::vector<XMLBlock> cache_;
		size_t size_cache;
		unsigned long pointer_last_record;

        std::unordered_map<std::string, std::unordered_set<XMLEmploy> > tree;
    };
}
