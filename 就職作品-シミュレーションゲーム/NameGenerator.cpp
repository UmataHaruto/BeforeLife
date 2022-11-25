#include "NameGenerator.h"
#include <stdio.h>
#include <iostream>
#include <regex>

NameGenerator::NameGenerator()
{

}

void NameGenerator::Init()
{
	FILE* fp;

	fp = fopen("assets/name/Family.txt", "r");
	for (int i = 0; i < 100; i++)
	{
		char str[20];
        fscanf(fp, "%s,",&str);
		//カンマを置き換える
		std::string str2 = std::regex_replace(std::string(str),std::regex(",")," ");
		m_familly[i] = str2;
	}

	fp = fopen("assets/name/male.txt", "r");
	for (int i = 0; i < 100; i++)
	{
		char str[20];
		fscanf(fp, "%s,", &str);
		//カンマを置き換える
		std::string str2 = std::regex_replace(std::string(str), std::regex(","), " ");

		m_male[i] = std::string(str2);

	}
	fp = fopen("assets/name/female.txt", "r");
	for (int i = 0; i < 100; i++)
	{
		char str[20];
		fscanf(fp, "%s,", &str);
		//カンマを置き換える
		std::string str2 = std::regex_replace(std::string(str), std::regex(","), " ");

		m_female[i] = std::string(str2);

	}

	fclose(fp);
}

std::string NameGenerator::CreateName(NAMETYPE type)
{
	int random = rand() % 100;
	switch (type)
	{
	case NameGenerator::FAMILLY:
		return m_familly[random];
		break;
	case NameGenerator::MALE:
		return m_male[random];
		break;
	case NameGenerator::FEMALE:
		return m_female[random];
		break;
	default:
		break;
	}
	return std::string("error!!!");
}
