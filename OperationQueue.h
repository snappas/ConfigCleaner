#ifndef OPR_QUE_H
#define OPR_QUE_H

#include <string>
#include <sstream>
#include <queue>
#include <vector>
#include <fstream>
using namespace std;

class Operation
{
	private:
		string name;
		vector<string> parameters;
		string exp;
		string scanUntil(string s, char sign, unsigned int &pos);
	public:
		Operation(){ }
		Operation(string operationText);
		void parse(string operationText);
		int parameterCount();
		string getParameter(int i);
		string getName();
		string getExpression();
		bool isExpression();
		string toString();
		friend ostream& operator << (ostream &out, Operation &op);
};

class OperationQueue
{
	private:
		queue<Operation> operQueue;
	public:
		OperationQueue(string filename);
		bool isEmpty();
		Operation pop();
};

#endif
