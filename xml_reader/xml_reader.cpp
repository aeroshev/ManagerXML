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
        this->path = path;
        if (!xml_doc.load_file(this->path.c_str()))
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
                tree[dep_name].insert(block);
            }
        }
    }

    void ManagerXML::show_tree(std::ostream &out) const
    {
        double avg_sal = 0;
        for (const auto& dep : tree)
        {
            avg_sal = 0;
            out << "Departament name -> " << dep.first << '\n';
            std::cout << "Count of employee in department -> " << dep.second.size() << '\n';
            for (const auto& empl : dep.second)
            {
                out << '\t' << "Surname -> " << empl.surname << '\n';
                out << '\t' << "Name -> " << empl.name << '\n';
                out << '\t' << "Middle name -> " << empl.middleName << '\n';
                out << '\t' << "Function -> " << empl.function << '\n';
                out << '\t' << "Salary -> " << empl.salary << '\n';
                out << '\n';
                avg_sal += empl.salary;
            }
            avg_sal /= dep.second.size();
            std::cout << "Average salary in department -> " << std::round(avg_sal * 100) / 100 << '\n';
            out << '\n';
            std::cout << std::is_base_of<std::unordered_set<XMLEmploy>, decltype(dep.second)>::value << '\n';
        }

    }

    void ManagerXML::put(const XMLBlock& box)
    {
        if (pointer_last_record < size_cache)
        {
            cache_.push_back(box);
            pointer_last_record++;
            switch (box.type) {
                case type_operation::write:
                    if (box.newEmploy != nullptr) {
                        tree[box.oldNameDepart].insert(*box.newEmploy);
                    } else {
                        tree.insert(std::make_pair(box.newNameDepart, std::unordered_set<XMLEmploy>()));
                    }
                    break;
                case type_operation::erase:
                    if (box.oldEmploy != nullptr) {
                        tree[box.oldNameDepart].erase(*box.oldEmploy);
                    } else {
                        tree.erase(box.oldNameDepart);
                    }
                    break;
                case type_operation::change:
                    if (box.oldEmploy != nullptr) {
                        auto emplHandler = tree[box.oldNameDepart].extract(*box.oldEmploy);
                        emplHandler.value() = *box.newEmploy;
                        tree[box.oldNameDepart].insert(std::move(emplHandler));
                    } else {
                        auto nodeHandler = tree.extract(box.oldNameDepart);
                        nodeHandler.key() = box.newNameDepart;
                        tree.insert(std::move(nodeHandler));
                    }
                    break;
                    default:
                        std::cout << "Something wrong" << '\n';
            }
        }
        else
            throw std::out_of_range("Cache is full");
    }

    void ManagerXML::save()
    {
        cache_.clear();
        pointer_last_record = 0;
        xml_doc.remove_child("departments");
        xml_doc.append_child("departments");
        pugi::xml_node departments = xml_doc.child("departments");

        for (const auto& dep : tree) {
            auto department = departments.append_child("department");
            department.append_attribute("name") = dep.first.c_str();
            auto employments = department.append_child("employments");
            for (const auto& node : dep.second) {
                auto employment = employments.append_child("employment");
                employment.append_child("surname").append_child(pugi::node_pcdata).set_value(node.surname.c_str());
                employment.append_child("name").append_child(pugi::node_pcdata).set_value(node.name.c_str());
                employment.append_child("middleName").append_child(pugi::node_pcdata).set_value(node.middleName.c_str());
                employment.append_child("function").append_child(pugi::node_pcdata).set_value(node.function.c_str());
                employment.append_child("salary").append_child(pugi::node_pcdata).set_value(std::to_string(node.salary).c_str());
            }
        }
        xml_doc.save_file(this->path.c_str(), "\t", pugi::format_default, pugi::encoding_utf8);
        xml_doc.print(std::cout);
    }

    void ManagerXML::rollback()
    {
        while (pointer_last_record > 1) {
            step_back();
        }
    }

    void ManagerXML::step_back()
    {
        auto tr_unit = cache_[pointer_last_record - 1];
        pointer_last_record--;
        switch (tr_unit.type) {
            case type_operation::write:
                if (tr_unit.newEmploy != nullptr) {
                    tree[tr_unit.oldNameDepart].erase(*tr_unit.newEmploy);
                } else {
                    tree.erase(tr_unit.newNameDepart);
                }
            case type_operation::erase:
                if (tr_unit.oldEmploy != nullptr) {
                    tree[tr_unit.oldNameDepart].insert(*tr_unit.oldEmploy);
                } else {
                    std::copy(tr_unit.old_data.begin(),
                            tr_unit.old_data.end(),
                            std::inserter(tree[tr_unit.oldNameDepart], tree[tr_unit.oldNameDepart].begin()));
                }
                break;
            case type_operation::change:
                if (tr_unit.oldEmploy != nullptr) {
                    auto emplHandler = tree[tr_unit.oldNameDepart].extract(*tr_unit.newEmploy);
                    emplHandler.value() = *tr_unit.oldEmploy;
                    tree[tr_unit.oldNameDepart].insert(std::move(emplHandler));
                } else {
                    auto nodeHandler = tree.extract(tr_unit.newNameDepart);
                    nodeHandler.key() = tr_unit.oldNameDepart;
                    tree.insert(std::move(nodeHandler));
                }
                break;
            default:
                std::cout << "Something wrong" << '\n';
        }
    }

    bool ManagerXML::exist(std::string &name)
    {
        return tree.end() != tree.find(name);
    }
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
