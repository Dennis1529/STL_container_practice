#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
//#include<stack>
using namespace std;


// deque class template definition
template< typename T >
class deque
{
public:
   deque(); // Constructor; Constructs a zero deque

   ~deque(); // Destroys the deque.

   unsigned int size() const; // Returns the number of elements in the deque.

   bool empty() const;

   T& back();

   void pop_back();

   // Adds a new element at the end of the deque, after its current last element.
   // The content of val is copied to the new element.
   // This effectively increases the deque size by one,
   // which causes an automatic reallocation of the allocated storage space
   // if and only if the new deque size surpasses the current deque capacity.
   void push_back( const T val );

private:
   T **map = nullptr;         // pointer to array of pointers to blocks
   unsigned int mapSize = 0;  // size of map array, zero or 2^N
   unsigned int myOff = 0;    // offset of initial element
   unsigned int mySize = 0;   // current length of sequence
}; // end class template deque


// constructor; constructs a zero deque
template< typename T >
deque< T >::deque()
{
} // end deque default constructor

// destructor; destroys the deque
template< typename T >
deque< T >::~deque()
{
   if( mySize != 0 )
   {
      for( unsigned int i = 0; i < mapSize; i++ )
         if( map[ i ] != nullptr )
            delete[] map[ i ];
      delete[] map;
   }
} // end destructor

template< typename T >
unsigned int deque< T >::size() const
{
   return mySize;
}

template< typename T >
bool deque< T >::empty() const
{
   return ( mySize == 0 );
}

template< typename T >
T& deque< T >::back()
{
	int block = ((mySize + myOff) % (mapSize * 4) - 1) / 4,
		offset = ((mySize + myOff) % (mapSize * 4) - 1) % 4;
	return 	map[block][offset];
	//return map[myOff / 4][myOff % 4];
}

template< typename T >
void deque< T >::pop_back()
{
   mySize--;
}

template< typename T >
void deque< T >::push_back( const T val )
{
	if (map == nullptr)
	{
		mapSize = 8;
		map = new T*[mapSize]();
		map[0] = new T[4]();
	}
	if (mySize != 0)
	{
		if (mySize == (mapSize*4-4))
		{
			T **tmp = new T*[mapSize * 2]();
			for (int i = 0; i < mapSize; i++)
				for (int j = 0; j < 3; j++)
					tmp[i][j] = map[i][j];
			this->~deque();
			map = tmp;
			tmp = nullptr;
			mapSize = mapSize * 2;
		}
		if (mySize % 4 == 0)
			map[mySize / 4] = new T[4]();
	}
	map[mySize / 4][mySize % 4] = val;
	mySize++;
}


// stack class definition.
template< typename T >
class stack
{
public:
   stack(); // Constructs a stack container adaptor object, which is initially empty.

   bool empty() const; // Returns whether the stack is empty.

   unsigned int size() const; // Returns the number of elements in the stack.

   T& top(); // Returns a reference to the top element in the stack.

   // Inserts a new element at the top of the stack, above its current top element.
   // The content of this new element is initialized to a copy of val.
   void push( const T &val );

   // Removes the element on top of the stack, effectively reducing its size by one.
   void pop();

private:
   deque< T > d; // a deque which is used to store the elements of the stack
}; // end class stack


// default constructor
template< typename T >
stack< T >::stack()
{
}

template< typename T >
bool stack< T >::empty() const
{
   return d.empty();
}

template< typename T >
unsigned int stack< T >::size() const
{
   return d.size();
}

template< typename T >
T& stack< T >::top()
{
   return d.back();
}

template< typename T >
void stack< T >::push( const T &val )
{
   d.push_back( val );
}

template< typename T >
void stack< T >::pop()
{
   d.pop_back();
}
/*int priority(char c)
{
	if (c == '+'|| c == '-')
		return 0;
	if (c == '*'|| c == '/')
		return 1;
	return -1;
}*/
bool cmp(string op1, string op2) {
	if (op1 != "+" && op1 != "-" && op1 != "*" && op1 != "/") return false;

	int x = (op1 == "+" || op1 == "-" ? 0 : 1),
		y = (op2 == "+" || op2 == "-" ? 0 : 1);

	return (x - y);
}
string convert(string str)
{
	string newExpression;
	stack<string> token;
	stringstream ss(str);

	while (ss >> str) {
		if (str == "(")
			token.push(str);
		else if (str == ")") {
			while (token.top() != "(")
				newExpression += " " + token.top(), token.pop();
			token.pop();
		}
		else if (str.size() == 1) {
			if (cmp(token.top(), str))
				newExpression += token.top(), token.pop();
			token.push(str);
		}
		else {
			if (newExpression.size() == 0)
				newExpression += str;
			else
				newExpression += " " + str;
		}
	}
	return newExpression;
}
	/*stack<char> op;
	string postfix;
	int check;
	for (int i = 0;i<str.length();i++)
	{
		check = priority(str[i]);
		if (str[i] == ')')
		{
			while (op.top() != '('&&op.size() != 0)
			{
				postfix.push_back(op.top());
				op.pop();
			}
			op.pop();
		}
		if (check == -1 &&str[i]!='('&&str[i] != ')')
		{
			if(postfix.length()!=0 || str[i]!=' ')
				postfix.push_back(str[i]);
		}
		else
		{
			if (op.empty() && str[i] != ')')
				op.push(str[i]);
			else
			{
				int check2=-1;
				if(str[i] == '(')
					op.push(str[i]);
				else
				{
					if(op.size()!=0)
						check2 = priority(op.top());
					if (check2 < check && str[i] != ')')
						op.push(str[i]);
					else if (check2 >= check && str[i] != ')')
					{
						postfix.push_back(op.top());
						op.pop();
						op.push(str[i]);
					}
				}
			}
		}
	}
	if (op.size() != 0)
	{
		if (op.top() != ' ')
			postfix.push_back(op.top());
		op.pop();
	}
	return postfix;*/

double caculate(string postfix)
{
	/*stack<double> op;
	stringstream ss(postfix);
	return op.top();*/
	double x, y;
	stringstream ss(postfix);
	stack<double> token;

	while (ss >> postfix) {
		if (postfix.size() == 1) { //如果是運算子的話
			y = token.top(), token.pop();
			x = token.top(), token.pop();

			if (postfix == "+") token.push(x + y);
			if (postfix == "-") token.push(x - y);
			if (postfix == "*") token.push(x * y);
			if (postfix == "/") token.push(x / y);
		}
		else {
			stringstream temp(postfix);
			temp >> x;
			token.push(x);
		}
	}
	return token.top();
}
int main()
{
	int n;
	cin >> n;
	cin.ignore();
	while (n--)
	{
		string str,postfix;
		getline(cin, str);
		postfix = convert(str);
		cout << fixed << setprecision(2) << caculate(postfix) << endl;
	}
    system( "pause" );
}