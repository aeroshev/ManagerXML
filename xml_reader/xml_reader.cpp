//
// Created by Артём on 06.06.2020.
//
#include "xml_reader.h"

namespace xml_rd
{
//--------------------------------------------------CombineBlock--------------------------------------------------------
//    std::unique_ptr<XMLBlock> CombineBlock::create_employ(std::istream &in = std::cin, const std::string& dep = "")
//    {
//        auto handle = std::make_unique<XMLBlock>();
//        handle->type = type_operation::write;
//        handle->depart = dep;
//
//        std::cout << "Employ surname -> ";
//        in >> handle->employ.surname;
//        std::cout << "Employ name -> ";
//        in >> handle->employ.name;
//        std::cout << "Employ middleName -> ";
//        in >> handle->employ.middleName;
//        std::cout << "Employ function -> ";
//        in >> handle->employ.function;
//        std::cout << "Employ salary -> ";
//        in >> handle->employ.salary;
//
//        return handle;
//    }
//
//    std::unique_ptr<XMLBlock> CombineBlock::create_department(std::istream &in = std::cin)
//    {
//        auto handle = std::make_unique<XMLBlock>();
//        handle->type = type_operation::write;
//
//        std::cout << "Department name -> ";
//        in >> handle->depart;
//
//        return handle;
//    }
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
                tree[dep_name].push_back(boost::new_clone(block));
            }

        }
    }

    void ManagerXML::show_tree(std::ostream &out) const
    {
        unsigned int count_empl = 0;
        double avg_sal = 0;
        for (const auto& dep : tree)
        {
            count_empl = 0;
            avg_sal = 0;
            out << "Departament name -> " << dep.first << '\n';
            for (const auto & empl : dep.second)
            {
                auto *employ = dynamic_cast<const XMLEmploy*>(&empl);
                out << '\t' << "Surname -> " << employ->surname << '\n';
                out << '\t' << "Name -> " << employ->name << '\n';
                out << '\t' << "Middle name -> " << employ->middleName << '\n';
                out << '\t' << "Function -> " << employ->function << '\n';
                out << '\t' << "Salary -> " << employ->salary << '\n';
                out << '\n';
                count_empl++;
//                avg_sal += empl.salary;
            }
            avg_sal /= count_empl;
            std::cout << "Count of employee in department -> " << count_empl << '\n';
            std::cout << "Average salary in department -> " << std::round(avg_sal * 100) / 100 << '\n';
            out << '\n';
        }
    }

//    void ManagerXML::put(std::unique_ptr<XMLBlock> box)
//    {
//        if (pointer_last_record < size_cache)
//            cache_.insert(std::make_pair(pointer_last_record++, *box));
//        else
//            throw std::out_of_range("Cache is full");
//    }
//
////    void ManagerXML::save()
////    {
//        for (auto &pointer : cache_)
//        {
//            switch (pointer.second.type)
//            {
//                case type_operation::write:
//                    continue;
//                case type_operation::erase:
//                    continue;
//                case type_operation::change:
//                    continue;
//                default:
//                    continue;
//            }
//        }
//        pointer_last_record = 0;
//    }
//
//    void ManagerXML::rollback()
//    {
//        cache_.clear();
//        pointer_last_record = 0;
//    }
//
//    void ManagerXML::step_back()
//    {
//        if (pointer_last_record > 1) {
//            cache_.extract(pointer_last_record - 1);
//            pointer_last_record -= 1;
//        }
//    }
//
//    bool ManagerXML::exist(std::string &name)
//    {
//        bool result = true;
//        if (tree.end() == tree.find(name))
//            result = false;
//        return result;
//    }
////----------------------------------------------------Interface---------------------------------------------------------
//    Interface::Interface(std::shared_ptr<ManagerXML> manager):
//    manager(std::move(manager))
//    {}
//
//    Interface::~Interface()
//    {
//        manager.reset();
//    }
//
//    void Interface::start()
//    {
//        unsigned int depth = 0;
//        std::string command, op, dep, up;
//        while(true)
//        {
//            std::cout << "/" << current_department << '\n';
//            getline(std::cin, command);
//
//            op = command.substr(0, command.find(' '));
//            if (op == "q")
//                break;
//            if (op == "sw")
//            {
//                manager->show_tree();
//                continue;
//            }
//            if (op == "sb")
//            {
//                manager->step_back();
//                continue;
//            }
//            if (op == "rb")
//            {
//                manager->rollback();
//                continue;
//            }
//            if (op == "ad")
//            {
//                switch (depth)
//                {
//                    case 0:
//                        manager->put(std::move(CombineBlock::create_department()));
//                        break;
//                    case 1:
//                        manager->put(std::move(CombineBlock::create_employ(std::cin, current_department)));
//                        break;
//                    default:
//                        std::cout << "Depth incorrect" << '\n';
//                }
//                continue;
//            }
//            if (op == "sv")
//            {
//                manager->save();
//                continue;
//            }
//            // TODO bring out this
//            dep = command.substr(4);
//            dep = dep.substr(0, dep.length() - 1);
//
//            if (op == "cd")
//            {
//                up = command.substr(3);
//                if (up == "..")
//                    if (depth == 0)
//                        std::cout << "We're the top tree" << '\n';
//                    else
//                    {
//                        depth--;
//                        current_department = "";
//                    }
//
//                else
//                    if (depth == 1)
//                        std::cout << "We're the bottom tree" << '\n';
//                    else
//                    {
//                        if (manager->exist(dep))
//                        {
//                            depth++;
//                            current_department = dep;
//                        }
//                        else
//                            std::cout << "This department does't exist" << '\n';
//                    }
//            }
//            else if (op == "rm")
//            {
//                switch (depth)
//                {
//                    case 0:
//                        // delete department
//                        break;
//                    case 1:
//                        // delete employ
//                        break;
//                    default:
//                        std::cout << "Depth incorrect" << '\n';
//                }
//            }
//            else if (op == "ch")
//            {
//                std::cout << dep << '\n';
//                unsigned int pos = dep.find('\"') + 1;
//                std::string cur_name = dep.substr(0, pos);
//                cur_name = cur_name.substr(0, cur_name.length() - 1);
//                std::string new_name = dep.substr(pos, dep.find('\"'));
//                new_name = new_name.substr(2, new_name.length());
//
//                switch (depth)
//                {
//                    case 0:
//                        // change department
//                        break;
//                    case 1:
//                        // change employ
//                        break;
//                    default:
//                        std::cout << "Depth incorrect" << '\n';
//                }
//
//                std::cout << cur_name << '\n';
//                std::cout << new_name << '\n';
//            }
//            else
//                std::cout << "Can't recognise command" <<'\n';
//        }
//    }
}
