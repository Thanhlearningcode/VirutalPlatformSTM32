#pragma once 

#include <iostream>
#include <map>
#include <string>

namespace NAME_CTR
{
    class NameController
    {
        public: 
            NameController() = delete;
            NameController(std::string _name);
            virtual ~NameController(); 
    };

    /*Global*/
    NameController* Get_Object_Name(std::string name_obj);
    static std::map<std::string,NameController*> mListObj;
}

