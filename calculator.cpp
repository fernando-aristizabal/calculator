/*//////////////////////////////////////////////////////////////////////////////////////////
    Fernando Aristizabal
    COP 3530
    Spring 2017
    Project #1
/*//////////////////////////////////////////////////////////////////////////////////////////

// libraries ///////////////////////////////////////////////////////////////////////////////
#include <iostream> // read and write to buffer
#include <fstream>  // read and write files
#include <sstream> // string conversion
#include <list> // lists, queues
#include <stack> // stacks
#include <map> // simple hashmaps
#include <string> // generate strings
#include <vector> // generate vectors
#include <iterator>
#include <limits>
#include <stdlib.h>
#include <cmath> // math functions
#include <cctype>
#include <unordered_map> // hashmap
#include <cstring>
using namespace std; // no need to use std

////////////////////////////////////////////////////////////////////////////////////////////
// global declarations /////////////////////////////////////////////////////////////////////

const int leftAssoc  = 0;
const int rightAssoc = 1;

typedef map<string, pair<int,int> > OperatorMap;

const OperatorMap::value_type assocs[] =
{
    OperatorMap::value_type("+", make_pair(1, leftAssoc)), // <operator , (priority,assoc)>
    OperatorMap::value_type("-", make_pair(1, leftAssoc)),
    OperatorMap::value_type("*", make_pair(2, leftAssoc)),
    OperatorMap::value_type("/", make_pair(2, leftAssoc)),
    OperatorMap::value_type("^", make_pair(3, rightAssoc))
};

const OperatorMap operatorMap(assocs,assocs+sizeof(assocs)/sizeof(assocs[0]));

// declare hashmap for variables with constants
unordered_map<string,double> variables = { {"Pi", 3.14169} , {"e", 2.718} };


////////////////////////////////////////////////////////////////////////////////////////////
// functions ///////////////////////////////////////////////////////////////////////////////

bool isParenthesis(const string& token)
{
    return token == "(" || token == ")";
}

bool isOperator(const string& token)
{
    return token == "+" || token == "-" || token == "*" || token == "/" || token == "^";
}

bool isAssociative(const string& token, const int& type) // compares associativity of token to "type"
{
    const pair<int,int> p = operatorMap.find(token)->second;
    return p.second == type;
}

bool isLetter(const string& token)
{
    return (token <= "z" && token >= "a") || (token <= "Z" && token >= "A");
}

bool isDigit(const string& token)
{
    return (token == "0" || token == "1" || token == "2" || token == "3" || token == "4" ||
    token == "5" || token == "6" || token == "7" || token == "8" || token == "9");
}

bool isFunction(const string& token)
{
    return (token == "sin" || token == "cos" || token == "log");
}

int cmpPrecedence(const string& token1, const string& token2)
{
    const pair<int,int> p1 = operatorMap.find(token1)->second;
    const pair<int,int> p2 = operatorMap.find(token2)->second;

    return p1.first - p2.first;
}

// Shunting-Yard Algorithm that converts infix mathematical expression to reverse polish notation
bool shuntingYard(const vector<string>& inputTokens, const int& size,vector<string>& strArray)
{
    bool completed = true;

    list<string> outputQ;
    stack<string> operatorStack;

    // loop through all tokens
    for (int i = 0; i < size; i++)
    {
        // read token
        const string token = inputTokens[i];

        // push token to operator stack
        if (isFunction(token)) {operatorStack.push(token);}

        // if token is operator
        else if (isOperator(token))
        {
            // assigns operator to o1
            const string o1 = token;

            if (!operatorStack.empty())
            {
                // assigns top of operator stack to o2
                string o2 = operatorStack.top();

                // while o2 is operator and (o1 is left associative and it's precendence
                // is less than or equal to that of o2) or (o1 is right associative and
                // has precedence less than that of o2
                while (isOperator(o2) &&
                        ((isAssociative(o1,leftAssoc) &&  cmpPrecedence(o1,o2) <= 0 )) ||
                        (isAssociative(o1,rightAssoc) && cmpPrecedence(o1,o2) < 0))
                {
                    operatorStack.pop(); // popoff o2
                    outputQ.push_back(o2); // push onto output queue

                    // reassign o2 for next iteration
                    if (!operatorStack.empty())
                    {
                        o2 = operatorStack.top();
                    } else {break;}
                }
            }
            operatorStack.push(o1); // push o1 to operator stack
        }
        // If the token is a left parenthesis, then push it onto the stack.
        else if (token == "(")
        {
            operatorStack.push(token);
        }
        // If token is a right bracket ')'
        else if (token == ")")
        {
            string topToken  = operatorStack.top();

            // Until the token at the top of the stack is a left parenthesis,
            // pop operators off the stack onto the output queue.
            while (topToken != "(")
            {
                outputQ.push_back(topToken );
                operatorStack.pop();

                if (operatorStack.empty()) break;
                topToken = operatorStack.top();
            }

            // Pop the left parenthesis from the stack, but not onto the output queue.
            if (!operatorStack.empty()) operatorStack.pop();
            if (!operatorStack.empty()) topToken = operatorStack.top();

            // if top of stack if function then pop into output queue
            if (isFunction(topToken))
            {
                outputQ.push_back(topToken);
                operatorStack.pop();
            }

        }
        // If the token is a number, then add it to the output queue.
        else
        {
            outputQ.push_back(token);
        }
    }

    // While there are still operators in the stack:
    while (!operatorStack.empty())
    {
        const string stackToken = operatorStack.top();

        // Pop the operator onto the output queue./
        outputQ.push_back(stackToken);
        operatorStack.pop();
    }

    // writes output queue to a string vector
    strArray.assign(outputQ.begin(), outputQ.end());

    return completed;
}


double evaluateRPN(vector<string> tokens, bool write)
{
    stack<string> st;

    // For each token
    for (int i = 0; i < (int) tokens.size(); ++i)
    {
        // read token
        const string token = tokens[i];

        // If the token is a value (not a function of operator)
        // push it onto the stack
        if (!isOperator(token) && !isFunction(token))
        {
            st.push(token);
        }
        else
        {
            double answer =  0.0;

            // Token is an operator or function, pop at least one value
            const string val2 = st.top();
            st.pop();
            const double d2 = strtod(val2.c_str(),NULL); // converts to double

            if ( !st.empty() && isOperator(token))
            {
                // for operators pop the 2nd arguments
                const string val1 = st.top();
                st.pop();
                const double d1 = strtod( val1.c_str(), NULL );

                //Get the answer
                if (token == "+") {answer = d1 + d2;}
                if (token == "-") {answer = d1 - d2;}
                if (token == "*") {answer = d1 * d2;}
                if (token == "/")
                {
                    // deal with division by zero
                    if (d2 == 0)
                    {
                        if (write) {throw invalid_argument("Division-By-Zero");} // returns exception for writing
                        else {double a; return a;} // returns empty for non-writing
                    }
                    else {answer = d1 / d2;}
                }
                if (token == "^") {answer = pow(d1,d2);}
            }
            else // functions and negatives
            {
                if (token == "-") {answer = d2 * -1;}
                else if (token == "sin") {answer = sin(d2);}
                else if (token == "cos") {answer = cos(d2);}
                else if (token == "log") {answer = log(d2);}
                else {answer = d2;}
            }


            // Push answer onto stack
            ostringstream s;
            s << answer;
            st.push(s.str());
        }
    }

    return strtod( st.top().c_str(), NULL );
}

vector<string> parseFormula(const string& expression, bool write)
{
    vector<string> tokens;

    // for all chars in expression
    for ( int i = 0; i < (int) expression.length(); ++i )
    {
        // assign token
        const string token(1, expression[i]);

        // for no more tokens, return what you have
        if (token.empty()) {return tokens;}

        // if operator or parenthesis
        else if (isOperator(token) || isParenthesis(token)) {tokens.push_back(token);}

        // letters could be function or variable
        else if (isLetter(token))
        {
            // parse sin,cos,log functions
            if (token == "s" && expression[i+1] == 'i' && expression[i+2] == 'n' && (expression[i+3] == '(' || expression[i+3] == ' '))
            {
                tokens.push_back("sin");
                i += 2; // advance to next i
            } else if (token == "c" && expression[i+1] == 'o' && expression[i+2] == 's' && (expression[i+3] == '(' || expression[i+3] == ' '))
            {
                tokens.push_back("cos");
                i += 2; // advance to next i
            } else if (token == "l" && expression[i+1] == 'o' && expression[i+2] == 'g' && (expression[i+3] == '(' || expression[i+3] == ' '))
            {
                tokens.push_back("log");
                i += 2; // advance to next i
            } else if ((expression[i] <= 'z' && expression[i] >= 'a') || (expression[i] <= 'Z' && expression[i] >= 'A'))
            {
                // parse variable
                string next = "";
                string var;
                var = expression[i];

                do
                {
                    var += next;
                    i++;
                    next = expression[i];
                } while ((next <= "z" && next >= "a") || (next <= "Z" && next >= "A"));
                i = i - 1;

                // check hashmap for previous assignment
                auto exists = variables.find(var);
                if (exists != variables.end())
                {
                    ostringstream strs;
                    strs << variables[var];
                    string token;
                    token = strs.str();
                    tokens.push_back(token); // push token

                } else // throw error if not found
                {
                    if (write) {throw invalid_argument("Undeclared-Variable");}
                    else {return vector<string>();}
                }
            }
        } else if (isDigit(token) || token == ".")
        {
            // parse multi-digit numbers and decimals
            i = i + 1;
            string next;
            next = expression[i];
            string decimal = token;
            while (next == "0" ||  next == "1" ||  next == "2" ||  next == "3" ||  next == "4" ||
                    next == "5" ||  next == "6" ||  next == "7" ||  next == "8" ||  next == "9" || next == ".")
            {

                decimal += next;
                i++;
                next = expression[i];
            }
            tokens.push_back(decimal);
            i = i - 1;
        }

        // digits
        else if (!token.empty() && token != " ") {tokens.push_back(token);}
    }

    return tokens;
}

double evaluateFormula(const string& expression, bool write)
{
    try
    {
        // generate parse vector from formula
        vector<string> tokens = parseFormula(expression,write);
		double a;

        if (tokens.empty()) {return a;}

        // convert to RPN with Shunting-Yard
        vector<string> rpn;
        if (shuntingYard(tokens, tokens.size(), rpn))
        {
            try
            {
 			a = evaluateRPN(rpn,write); // answer               
			if (write) {cout << a << endl;} // write answer
                else {return a;}
            } catch (invalid_argument& e)
            {
                if (write) {cout << e.what() << endl;} // write zero division error
            }
        }
    } catch (invalid_argument& e)
    {
        if (write) {cout << e.what() << endl;} // write undeclared variable
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// main function ///////////////////////////////////////////////////////////////////////////

int main()
{
    char line[10000];
    cin.getline(line,10000,'\n'); // get first line
    string strLine = line; // assign char to string

    while (strcmp(line,"quit") != 0) // run until line == "quit"
    {
        if (line[0] == 'l' && line[1] == 'e' && line[2] == 't' && line[3] == ' ') // if "let" continue to assigment
        {
            int i = 0; // counter variable
            do {i++;} while (line[i] != ' '); // advance i until space is found
            i = i + 1; // advance one more
            string next; // next is the next string
            string var; // var is the name of the variable for assignment
            next = line[i]; // assigns first next
            while ((next <= "z" && next >= "a") || (next <= "Z" && next >= "A")) //checks for letter
            {
                var += next; // concatenates to var
                i++; // increase counter
                next = line[i]; // assign next "next"
            }
            while (line[i] == ' ' || line[i] == '=') {i++;} // advance i until something other then whitespace or = sign is found
            double value = evaluateFormula(strLine.substr(i,(strLine.size()-i)), 0); // evaluate expression with no printing
            if ((value == 0 || value > 2e-322 || value < -2e-322) && !isFunction(var) && var != "Pi" && var !="e") {variables[var] = value;} // hash the result

        }
        else // if no "let" then evaluate the line and print results
        {
            evaluateFormula(line,1);
        }

        cin.getline(line,10000,'\n'); // get next line
        strLine = line; // assign new string

    }

    return 0;
}
