//
// Created by Артём on 06.06.2020.
//
#include "xml_reader.h"

namespace xml_rd
{
//--------------------------------------------------CombineBlock--------------------------------------------------------
    std::unique_ptr<XMLBlock> CombineBlock::insert_employ(std::istream &in = std::cin)
    {
        auto handle = std::make_unique<XMLBlock>();
        handle->type = type_operation::write;
        handle->employ = new XMLEmploy;
        handle->depart = nullptr;

        fill_employ(handle->employ, in);

        return handle;
    }

    void CombineBlock::fill_employ(XMLEmploy *pointer, std::istream &in)
    {
        std::cout << "Employ surname -> ";
        in >> pointer->surname;
        std::cout << "Employ name -> ";
        in >> pointer->name;
        std::cout << "Employ middleName -> ";
        in >> pointer->middleName;
        std::cout << "Employ function -> ";
        in >> pointer->function;
        std::cout << "Employ salary -> ";
        in >> pointer->salary;
    }

    void CombineBlock::fill_depart(XMLDepart *pointer, std::istream &in)
    {
        std::cout << "Department name -> ";
        in >> pointer->name;
        std::cout << "Department qty employee -> ";
        in >> pointer->qty_employ;
        std::cout << "Department avg salary -> ";
        in >> pointer->avg_salary;
    }
//-----------------------------------------------ManagerXML-------------------------------------------------------------
    ManagerXML::ManagerXML(size_t size_cache):
    size_cache(size_cache),
    cache_(1024),
    pointer_last_record(0)
    {}

    void ManagerXML::load_file(const std::string &path)
    {
        if (!xml_doc.load_file(path.c_str()))
            throw std::invalid_argument("Can't read this file");

        pugi::xml_node departments = xml_doc.child("departments");
        for (auto department = departments.first_child(); department; department = department.next_sibling())
        {
            auto dep_name = department.attribute("name").value();
            pugi::xml_node employments = department.child("employments");

            for (auto employment = employments.first_child(); employment; employment = employment.next_sibling())
            {
                XMLEmploy block;
                block.surname = employment.child("surname").child_value();
                block.name = employment.child("name").child_value();
                block.middleName = employment.child("middleName").child_value();
                block.function = employment.child("function").child_value();
                block.salary = std::stoi(employment.child("salary").child_value());
                tree[dep_name].push_back(block);
            }

        }
    }

    void ManagerXML::show_tree(std::ostream &out) const
    {
        for (const auto& dep : tree)
        {
            out << "Departament name -> " << dep.first << '\n';
            for (const auto& empl : dep.second)
            {
                out << '\t' << "Surname -> " << empl.surname << '\n';
                out << '\t' << "Name -> " << empl.name << '\n';
                out << '\t' << "Middle name -> " << empl.middleName << '\n';
                out << '\t' << "Function -> " << empl.function << '\n';
                out << '\t' << "Salary -> " << empl.salary << '\n';
                out << '\n';
            }
            out << '\n';
        }
    }

    void ManagerXML::put(const XMLBlock &box)
    {
        if (pointer_last_record < size_cache)
            cache_.insert(std::make_pair(pointer_last_record++, box));
        else
            throw std::out_of_range("Cache is full");
    }

    void ManagerXML::save()
    {
        for (auto &pointer : cache_)
        {
            switch (pointer.second.type)
            {
                case type_operation::write:
                    continue;
                case type_operation::erase:
                    continue;
                case type_operation::change:
                    continue;
                default:
                    continue;
            }
        }
        pointer_last_record = 0;
    }

    void ManagerXML::rollback()
    {
        cache_.clear();
        pointer_last_record = 0;
    }

    void ManagerXML::step_back()
    {
        if (pointer_last_record > 1) {
            cache_.extract(pointer_last_record - 1);
            pointer_last_record -= 1;
        }
    }

    bool ManagerXML::exist(std::string &name)
    {
        bool result = true;
        if (tree.end() == tree.find(name))
            result = false;
        return result;
    }
//----------------------------------------------------Interface---------------------------------------------------------
    Interface::Interface(std::shared_ptr<ManagerXML> manager):
    manager(std::move(manager))
    {}

    Interface::~Interface()
    {
        manager.reset();
    }

    void Interface::start()
    {
        unsigned int depth = 0;
        std::string command, op, dep, up;
        while(true)
        {
            std::cout << "Choose department or quit" << '\n';
            getline(std::cin, command);

            op = command.substr(0, command.find(' '));
            if (op == "q")
                break;
            if (op == "sw")
            {
                manager->show_tree();
                continue;
            }
            if (op == "sb")
            {
                manager->step_back();
                continue;
            }
            if (op == "rb")
            {
                manager->rollback();
                continue;
            }
            if (op == "ad")
            {
                continue;
            }
            if (op == "sv")
            {
                manager->save();
                continue;
            }

            dep = command.substr(4);
            dep = dep.substr(0, dep.length() - 1);

            if (op == "cd")
            {
                up = command.substr(3);
                if (up == "..")
                    if (depth == 0)
                        std::cout << "We're the top tree" << '\n';
                    else
                        depth--;
                else
                    if (depth == 1)
                        std::cout << "We're the bottom tree" << '\n';
                    else
                        depth++;
            }
            else if (op == "rm")
            {
                switch (depth)
                {
                    case 0:
                        // delete department
                    case 1:
                        // delete employ
                    default:
                        std::cout << "Depth incorrect" << '\n';
                }
            }
            else if (op == "ch")
            {
                std::string cur_name = dep.substr(0, dep.find(' '));
                cur_name = cur_name.substr(0, cur_name.length() - 1);
                std::string new_name = dep.substr(dep.find(' '));
                new_name = new_name.substr(2, new_name.length());

                switch (depth)
                {
                    case 0:
                        // change department
                    case 1:
                        // change employ
                    default:
                        std::cout << "Depth incorrect" << '\n';
                }

                std::cout << cur_name << '\n';
                std::cout << new_name << '\n';
            }
            else
                std::cout << "Can't recognise command" <<'\n';
        }
    }
}
