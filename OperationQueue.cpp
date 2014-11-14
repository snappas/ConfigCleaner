#include "OperationQueue.h"

Operation::Operation(string operationText)
{
	parse(operationText);
}

string Operation::scanUntil(string s, char sign, unsigned int &pos)
{
	stringstream ss;
	string ret;
	while(pos != s.length())
	{
		if(s[pos] == sign)
		{
			++pos;
			break;
		}
		else
		{
			ss << s[pos];
			++pos;
		}
	}
	ret = ss.str();
	return ret;
}

void Operation::parse(string operationText)
{
	size_t equalPos = operationText.find('=');
	unsigned int pos = 0;
	if(equalPos == string::npos)
	{
		//get the operation type;
		name = scanUntil(operationText, '(', pos);
		//get the parameters;
		string rawParameters = scanUntil(operationText, ')', pos);
		pos = 0;
		while(pos != rawParameters.length())
		{
			string parameter = scanUntil(rawParameters, ',', pos);
			parameters.push_back(parameter);
		}
	}
	else
	{
		name = scanUntil(operationText, '=', pos);
		exp = operationText.substr(pos);
	}
}

string Operation::getExpression()
{
	return exp;
}

string Operation::getParameter(int i)
{
	return parameters[i];
}

string Operation::getName()
{
	return name;
}

int Operation::parameterCount()
{
	return parameters.size();
}

bool Operation::isExpression()
{
	return parameters.size() == 0;
}

string Operation::toString()
{
	stringstream ss;
	ss << "op.getName() = " << getName() << endl;
	ss << "op.isExpression() = " << (isExpression() ? "true" : "false") << endl;
	if(isExpression())
	{
		ss << "op.getExpression() = " << getExpression() << endl;
	}
	else
	{
		ss << "op.parameterCount() = " << parameterCount() << endl;
		for(int i=0; i<parameterCount(); ++i)
		{
			ss << "op.getParameter(" << i << ") = " << getParameter(i) << endl;
		}
	}
	return ss.str();
}

ostream& operator << (ostream &out, Operation &op)
{
	out << op.toString();
	return out;
}

OperationQueue::OperationQueue(string filename)
{
	ifstream in(filename.c_str());
	if(in)
	{
		string scriptLine;
		while(getline(in, scriptLine))
		{
			Operation newOperation(scriptLine);
			operQueue.push(newOperation);
		}
	} 
	else
	{
		printf("Error: %s not found.\n",filename.c_str());
	}
}

bool OperationQueue::isEmpty()
{
	return operQueue.empty();
}

Operation OperationQueue::pop()
{
	Operation ret = operQueue.front();
	operQueue.pop();
	return ret;
}
