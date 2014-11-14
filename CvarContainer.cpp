#pragma once
#include "CvarContainer.h"
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <locale>
#include "Helpers.h"


CvarContainer::CvarContainer(void){
}
CvarContainer::CvarContainer(std::string n, std::string vn){
	this->setName(n);
	this->setVarName(vn);
}


CvarContainer::~CvarContainer(void){
}
void CvarContainer::sortContainer(){
	std::sort(this->container.begin(),this->container.end(),sortbyfirst);
}
int CvarContainer::getContainerSize(){
	return this->container.size();
}
std::string CvarContainer::getCvar(int x){
	return std::get<0>(container[x]);
}
bool CvarContainer::findCvar(std::string f,std::string parameter){
	for(std::vector<std::string>::size_type i = 0; i != findthis.size(); i++) {
		if(ci_find_substr( f,this->findthis[i]) != -1){
			this->addCvar(f,parameter);
			return true;
		}
	}
	return false;
}
void CvarContainer::addFindCvar(std::string ft){
	this->findthis.push_back(ft);
}

void CvarContainer::addCvar(std::string fs, std::string p){
	this->container.push_back(std::make_tuple(fs,p));
}

void CvarContainer::setVarName(const std::string s){
	this->varname = s;
}

std::string CvarContainer::getVarName(){
	return this->varname;
}
void CvarContainer::setName(const std::string s){
	this->name = s;
}

std::string CvarContainer::getName(){
	return this->name;
}