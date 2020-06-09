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
#include <list>
#include <boost/ptr_container/ptr_vector.hpp>
#include "../pugixml/pugixml.hpp"

namespace xml_rd {

    enum type_operation {
        write,
        erase,
        change
    };

    class XMLBase {
    public:
        virtual void nothing() = 0;
        XMLBase() = default;
        virtual ~XMLBase() = default;
        virtual XMLBase* clone() const = 0;
        virtual std::string show() const = 0;
    };

    class XMLEmploy: public XMLBase {
    public:
        XMLEmploy() = default;

        XMLEmploy(const XMLEmploy& pBase)
        {
            surname = pBase.surname;
            name = pBase.name;
            middleName = pBase.middleName;
            function = pBase.function;
            salary = pBase.salary;
        }

        XMLEmploy * clone() const override
        {
            return new XMLEmploy(*this);
        }

        std::string show() const override
        {
            return surname;
        }

        std::string surname;
        std::string name;
        std::string middleName;
        std::string function;
        unsigned int salary;

        void nothing() override
        {
            std::cout << "Nothing" << '\n';
        }
    };

    class XMLBlock: public XMLBase {
    public:
        XMLBlock() = default;
        type_operation type;
        XMLEmploy employ;
        std::string depart;

        XMLBlock* clone() const override
        {
            return new XMLBlock(*this);
        }

        std::string show() const override
        {
            std::string a = "fuck off";
            return a;
        }

        void nothing() override
        {
            std::cout << "Nothing" << '\n';
        }
    };

    struct CombineBlock {
        // TODO Memory handle
        static std::unique_ptr<XMLBase> create_employ(std::istream &, const std::string&);
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
        std::unordered_map<unsigned long, std::shared_ptr<XMLBase> > cache_;
        std::unordered_map<std::string, boost::ptr_vector<XMLBase> > tree;
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