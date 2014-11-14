#ifndef BINDSCONTAINER_H
#define BINDSCONTAINER_H
#include <string>
#include <vector>
#include <tuple>

class BindsContainer
{
public:
	BindsContainer(void);
	BindsContainer(std::string n, std::string vn);
	~BindsContainer(void);
	void addFind(std::string);
	void addBind(std::string,std::string);
	bool findBind(std::string,std::string);
	void setVarName(std::string s);
	std::string getVarName();
	void setName(std::string s);
	std::string getName();
	int getContainerSize();
	std::string getBinds(int x);
	void sortContainer();
private:
	std::string varname;
	std::string name;
	std::vector<std::string> findthis;
	std::vector<std::tuple<std::string,std::string>> container;
};
#endif
