#include "NameController.h"

namespace NAME_CTR
{
    NameController::NameController(std::string _name)
    {
        mListObj[_name] = this;
    }

    NameController::~NameController()
    {
        //<Nothing
    }

    NameController* Get_Object_Name(std::string name_obj)
    {
        if (mListObj.empty())
        {
            printf("EROR- List objs is emy\n");
            return NULL;
        }

        std::map<std::string,NameController*>::iterator it = mListObj.find(name_obj);
        if (it !=mListObj.end())
        {
            return it->second;
        }
        else
        {
            printf("ERROR - The object %s is not exist\n",name_obj.c_str());
            return NULL;
        }
    }
}