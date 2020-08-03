#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

bool ToFindParentheses(std::vector<std::string> &token, size_t &tokenIter)
{
	size_t temp = tokenIter;
	bool flag = false;
	while (temp != token.size())
	{
		if (token[temp] == "(")
		{
			tokenIter = temp;
			flag = true;
		}
		temp++;
	}
	return flag;
}

void expressionToToken(std::string &str, std::vector<std::string> &token)
{
	if (str.size() < 2)
	{
		token.push_back(str);
		return;
	}
	std::string buf = "";
	int i = 0;
	auto it = str.begin();
	while (it != str.end())
	{
		switch (*it)
		{
		case '(':
			buf = "(";
			it++;
			break;
		case ')':
			buf = ")";
			it++;
			break;
		case '/':
			buf = "/";
			it++;
			break;
		case '+':
			buf = "+";
			it++;
			break;
		case '-':
			buf = "-";
			it++;
			i = std::distance(str.begin(),it);
			if (i==1) 
			{
				while (*it != '/' and *it != '*' and *it != '+' and *it != '-' and *it != ')' and *it != '(')
				{
					buf += *it;
					it++;
					if (it == str.end()) break;
				}
			}
			if ( i >= 2)
			{
				if ( *it >= '0' and *it <= '9' ) { // and *(it - 2) < '0' and *(it - 2) > ')'
					if (*(it - 2) == '(' or *(it - 2) == '+' or *(it - 2) == '*' or *(it - 2) == '/' or *(it - 2) == '-')
					{
						do
						{
							buf += *it;
							it++;
							if (it == str.end()) break;
						} while (*it != '/' and *it != '*' and *it != '+' and *it != '-' and *it != ')');
					}
				}
			}
			break;
		case '*':
			buf = "*";
			it++;
			break;
		default:
			do
			{
				buf += *it;
				it++;
				if (it == str.end()) break;
			} while (*it != '/' and *it != '*' and *it != '+' and *it != '-' and *it != ')');
			break;
		}
		token.push_back(buf);
		buf = "";
	}
}

bool FindNextOperator(size_t &tokenIter, std::vector<std::string>& token)
{
	size_t temp = tokenIter;
	bool flag = false;
	while (temp < token.size())
	{
		if (token[temp] == "*" or token[temp] == "/")
		{
			tokenIter = temp;
			return true;
		}
		if (token[temp] == "+" or token[temp] == "-")
		{
			if (token[tokenIter] == "-")
			{
				temp++;
				continue;
			}
			tokenIter = temp;
			flag = true;
		}
		temp++;
		if (!flag and temp == token.size()) return false;
		if (flag and temp == token.size() or token[temp] == ")" and flag) return true;
		if (token[temp] == ")" and !flag) return false;
	}
	if (token[temp] == ")" or temp == token.size()) return false;
	return true;
}

void TimeToInsertNum(std::vector<std::string>& token, size_t i1, double num)
{
	auto it = token.begin();
	std::advance(it, i1);
	std::ostringstream ss;
	ss << std::setprecision(23);
	ss << num;
	token.emplace(it, ss.str());
	it = token.begin();
	std::advance(it, i1 + 1);
	token.erase(it, it + 3);
}

double calc(std::string expression)
{
	std::cout << expression << std::endl;
	expression.erase(remove(expression.begin(), expression.end(), ' '), expression.end());
	double num = 0;
	std::vector<std::string> token;
	expressionToToken(expression, token);
	size_t tokenIter = 0;

	while (ToFindParentheses(token, tokenIter))
	{
		if (FindNextOperator(tokenIter, token) == false)
		{
			tokenIter++;
			auto it = token.begin();
			std::advance(it, tokenIter + 1);
			token.erase(it);
			it = token.begin();
			std::advance(it, tokenIter - 1);
			token.erase(it);
			if (tokenIter < 3 and token.size() < 3) // осталость три или два элемента есл искобка то (х) -> значит х, если есть минус значит х*(-1)
			{
				if (token[tokenIter - 2] == "-") return stod(token[tokenIter - 1]) *(-1);
				if (token[tokenIter - 2] == "(") return stod(token[tokenIter - 1]);
			}
			if (tokenIter < 3 and token.size() > 3 and token[0] == "-") // скобки в начале строки 1-2 позиция но строка длинная и в начале строки минус
			{
				it = token.begin();
				num = stod(token[tokenIter - 1]) *(-1);

				std::ostringstream ss;
				ss << std::setprecision(23);
				ss << num;
				token.emplace(it, ss.str());

				it = token.begin();
				std::advance(it, tokenIter - 1);
				token.erase(it, it + 2);
				tokenIter = 0;
				continue;
			}
			if (tokenIter > 3) 
			{
				if ((token[tokenIter - 3] == "-" or token[tokenIter - 3] == "+" or token[tokenIter - 3] == "*" or token[tokenIter - 3] == "/" or token[tokenIter - 3] == "(") and token[tokenIter - 2] == "-" ) // строка длинная, в месте где убирались скобки, перед нимим был минус И какой то другой знак 
				{
					it = token.begin();
					std::advance(it, tokenIter - 2);
					num = stod(token[tokenIter - 1]) *(-1);

					std::ostringstream ss;
					ss << std::setprecision(23);
					ss << num;
					token.emplace(it, ss.str());

					it = token.begin();
					std::advance(it, tokenIter - 1);
					token.erase(it, it + 2);
				}
			}
			tokenIter = 0;
			continue;
		}
		if (token[tokenIter] == "*")
		{
			num = stod(token[tokenIter - 1]) * stod(token[tokenIter + 1]);
			TimeToInsertNum(token, tokenIter - 1, num);
			tokenIter = 0;
			continue;
		}
		if (token[tokenIter] == "/")
		{
			num = stod(token[tokenIter - 1]) / stod(token[tokenIter + 1]);
			TimeToInsertNum(token, tokenIter - 1, num);
			tokenIter = 0;
			continue;
		}
		if (token[tokenIter] == "+")
		{
			num = stod(token[tokenIter - 1]) + stod(token[tokenIter + 1]);
			TimeToInsertNum(token, tokenIter - 1, num);
			tokenIter = 0;
			continue;
		}
		if (token[tokenIter] == "-")
		{
			num = stod(token[tokenIter - 1]) - stod(token[tokenIter + 1]);
			TimeToInsertNum(token, tokenIter - 1, num);
			tokenIter = 0;
			continue;
		}
	}

	while (FindNextOperator(tokenIter, token))
	{
		if (token[tokenIter] == "*")
		{
			num = stod(token[tokenIter - 1]) * stod(token[tokenIter + 1]);
			TimeToInsertNum(token, tokenIter - 1, num);
			tokenIter = 0;
			continue;
		}
		if (token[tokenIter] == "/")
		{
			num = stod(token[tokenIter - 1]) / stod(token[tokenIter + 1]);
			TimeToInsertNum(token, tokenIter - 1, num);
			tokenIter = 0;
			continue;
		}
		if (token[tokenIter] == "+")
		{
			num = stod(token[tokenIter - 1]) + stod(token[tokenIter + 1]);
			TimeToInsertNum(token, tokenIter - 1, num);
			tokenIter = 0;
			continue;
		}
		if (token[tokenIter] == "-")
		{
			num = stod(token[tokenIter - 1]) - stod(token[tokenIter + 1]);
			TimeToInsertNum(token, tokenIter - 1, num);
			tokenIter = 0;
			continue;
		}
	}
	num = stod(token[0]);
	return num;
}
int main()
{
	std::cout << calc("(-94) - (15 + 59 + -(55)) - (-11 / (((-(-71 * 62)))) + 76)");
	return 139;
}
