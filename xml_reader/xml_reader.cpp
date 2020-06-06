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
    cache_(size_cache),
    size_cache(size_cache),
    pointer_last_record(0)
    {}

    void ManagerXML::open_file(const std::string &path = "")
    {
        if (!xml_doc.load_file(path.c_str()))
            throw std::invalid_argument("Can't read this file");
    }

    void ManagerXML::show_tree(std::ostream &out = std::cout) const
    {
        pugi::xml_node departments = xml_doc.child("departments");
        for (auto department = departments.first_child(); department; department = department.next_sibling()) {
            std::cout << "Departments name -> " << department.attribute("name").value() << '\n';
            pugi::xml_node employments = department.child("employments");
            for (auto employment = employments.first_child(); employment; employment = employment.next_sibling()) {
                for (auto attr = employment.first_child(); attr; attr = attr.next_sibling()) {
                    std::cout << '\t' << attr.name() << " -> " << attr.child_value() << '\n';
                }
                std::cout << '\n';
            }
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
        for (auto &pointer : cache_) {
            // TODO
        }
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
        int choice = 1;
        while (choice != 0) {
            show_menu();
            std::cin >> choice;

            switch (choice) {
                case 0:
                    break;
                case 1:
                    current_choice = type_operation::write;

                    break;
                case 2:
                    current_choice = type_operation::erase;
                    break;
                case 3:
                    current_choice = type_operation::change;
                    break;
                case 4:

                    break;
                case 5:
                    break;
                case 6:
                    manager->show_tree();
                    break;
                default:
                    std::cerr << "Try choose from 0 to 6" << '\n';
                    break;
            }
        }
    }

    inline void Interface::show_menu()
    {
        std::cout << "1. Insert" << '\n';
        std::cout << "2. Delete" << '\n';
        std::cout << "3. Change" << '\n';
        std::cout << "4. Step back" << '\n';
        std::cout << "5. Rollback" << '\n';
        std::cout << "6. Show tree" << '\n';
        std::cout << "0. Quit" << '\n';
    }

    inline void Interface::show_choice()
    {
        std::cout << "1. Department" << '\n';
        std::cout << "2. Employ" << '\n';
    }

    void Interface::insert(type_object object)
    {

        switch (object)
        {
            case type_object::employ:
                auto pointer = std::move(CombineBlock::insert_employ());

                break;
            case type_object::department:

            default:
        }
    }


}
