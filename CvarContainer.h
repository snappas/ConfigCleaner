#ifndef CVARCONTAINER_H
#define CVARCONTAINER_H
#include <string>
#include <vector>
#include <tuple>

class CvarContainer
{
public:
	CvarContainer(void);
	CvarContainer(std::string n, std::string vn);
	~CvarContainer(void);
	void addFindCvar(std::string);
	void addCvar(std::string,std::string);
	bool findCvar(std::string,std::string);
	void setVarName(std::string s);
	std::string getVarName();
	void setName(std::string s);
	std::string getName();
	int getContainerSize();
	std::string getCvar(int x);
	void sortContainer();
private:
	std::string varname;
	std::string name;
	std::vector<std::string> findthis;
	std::vector<std::tuple<std::string,std::string>> container;
};
#endif
