#pragma once
#include "BindsContainer.h"
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <locale>
#include "Helpers.h"


BindsContainer::BindsContainer(void){
}
BindsContainer::BindsContainer(std::string n, std::string vn){
	this->setName(n);
	this->setVarName(vn);
}


BindsContainer::~BindsContainer(void){
}
void BindsContainer::sortContainer(){
	std::sort(this->container.begin(),this->container.end(),sortbysecond);
}
int BindsContainer::getContainerSize(){
	return this->container.size();
}
std::string BindsContainer::getBinds(int x){
	return std::get<0>(container[x]);
}
bool BindsContainer::findBind(std::string f,std::string parameter){
	for(std::vector<std::string>::size_type i = 0; i != findthis.size(); i++) {
		if(ci_find_substr( f,this->findthis[i]) != -1){
			this->addBind(f,parameter);
			return true;
		}
	}
	return false;
}
void BindsContainer::addFind(std::string ft){
	this->findthis.push_back(ft);
}

void BindsContainer::addBind(std::string fs, std::string p){
	this->container.push_back(std::make_tuple(fs,p));
}

void BindsContainer::setVarName(const std::string s){
	this->varname = s;
}

std::string BindsContainer::getVarName(){
	return this->varname;
}
void BindsContainer::setName(const std::string s){
	this->name = s;
}

std::string BindsContainer::getName(){
	return this->name;
}