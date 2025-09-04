#include "BusInterface.h"

Data_Package::Data_Package()
{
    this->Address = 0;
    this->Length  = 0;
    this->Buffer  = NULL;
}

/*Define Master Class*/
Master::Master(const char* Name) 
{
    this->m_Name = Name;
}
Master::~Master()
{
  // Do nothing
}

/*Define Slaver Class*/
Slave::Slave(const char* Name) 
{
    this->m_Name = Name;
}
Slave::~Slave()
{
  // Do nothing
}

bool Master::Transmit(Data_Package* package) 
{
    if (!m_SlaveSet.empty())
    {
        for (auto slave : m_SlaveSet)
        {
            if (slave != NULL)
            {
                slave->Transmit(package);
                return true;
            }
        }

    }

    printf("This is Error-Slave is not found\n");
    return false;
}

bool Master::Received(Data_Package* package) 
{
    if (!m_SlaveSet.empty())
    {
        for (auto slave: m_SlaveSet)
        {
            if (slave != NULL)
            {
                slave->Transmit(package);
                return true;
            }
        }
    }

    printf("This is Error-Slave is not found\n");
    return false;
}

Master& Master::operator()(Slave &obj) 
{
    this->m_SlaveSet.push_back(&obj);
    return *this;
}