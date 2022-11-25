#pragma once
#include <string>
class NameGenerator
{
public:
    enum NAMETYPE
    {
        FAMILLY,
        MALE,
        FEMALE,
    };

    static NameGenerator& GetInstance() {
        static NameGenerator Instance;
        return Instance;
    }

    //�t�@�C���̓ǂݍ���
    NameGenerator();
    void Init();
    std::string CreateName(NAMETYPE type);

private:
   std::string m_familly[100];
   std::string m_male[100];
   std::string m_female[100];
   
};

