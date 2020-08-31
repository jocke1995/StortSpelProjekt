# Code Standards
This document provides a general overview of the code standards that should be used when submitting code to this project. 

---

## General
* Statements and loops should be written for blocks.
```cpp
if(SomeCondition)
{
    // code
}

while(someCondition)
{
    //code
}
```
* Make space between operands and operators
```cpp
num + num2; // Yes
num+num2; // NO!
```

## Functions and methods
Any function or public method should be named with an initial capital letter and then follow "CamelCasing" standard. Private methods should be named with normal "camelCasing".

```cpp
// function or public method
void MyFunction();

// private method in class
void myFunction();
```

## Variables
Variables should in general be named in a descriptive manner and follow the same rule as functions and methods. certain types of variables should have a prefix added.
* Class member variables should use **m_** as their prefix.
For example: 
```cpp
int m_MyVar;
```
* Static variables should use the prefix **s_**.
* Global variables should use the prefix **g_**.

## Pointers
Pointers should be named with a lower case p before the name. This should however be placed after any prefixes. An additional lower case p should be added if the variable or funtion is a double pointer.
```cpp
int* pVar;
int* m_pVar;
int** m_ppVar;
```

## Classes
* Separate public, protected and private functions and members in that same order. Make a clear separation between functions and variables by placing the variables at the top of the accessability block and the methods below with atleast one empty line between them.
* Use forward declarations if possible. Includes in a .h file should only be used if your class uses actual objects of other classtypes.
```cpp
#include A;
class B; // h file with definition of B included in cpp.

class Foo
{
public:
    int m_Var;
    
    void MyFunc();
    void MySecondFunc();
    
protected:
    A m_NeededObject;
    
    void SomeMethod();
    
private:
    B* m_pForwardDeclaredObject;
    
    void otherPrivateMethod();
}
```

## Enums
Enums should be named with camel casing. Starting with a capital letter. An enum should also use the prefix **F_** if it is used as a flag otherwise it should use the **E_** prefix.
Enum elements should be named with only capital letters and underscores.
```cpp
enum F_MyFlags
{
  MY_FLAG_FLAG = (1 << 0),
  MY_FLAG_FLAG = (1 << 1),
};

enum E_MyEnum
{
  MY_ENUM_0 = 0,
  MY_ENUM_1 = 1,
};
```