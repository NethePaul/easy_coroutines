#pragma once
#include"coroutines.h"


COROUTINE_BEGIN(int,example1)//this coroutine does not have a standard caller for the user so you have to define one manually
{//the first makro parameter is the return value and the second is the class name
	for (;;) {
		COROUTINE_YIELD(42);
		f2(23);//call the subroutine defined below
	}
	_terminate(102);//to terminate the coroutine call _terminate or
	return(32);//return
}
COROUTINE_VAR int in = 0;//"in" will be usable in the coroutine and all its subroutines defined betwen COROUTINE_BEGIN and COROUTINE_END

COROUTINE_CALLER(template<class T>int)(T _in)//Called by the user in the first bracket is the return type written and optionally the template parameters
{//in the second brackets is the parameters list
	in = _in;
	COROUTINE_CALL;//resume the coroutine until the next yield() or return statemant than return the yielded value to the user
}
COROUTINE_SUBROUTINE int f2(int _in)
{
	int buffer = 32 + 23;
	COROUTINE_YIELD(buffer);//the subroutine can yield a value as the coroutine
	return buffer;//but when calling return it returns to the coroutine instead of terminating the coroutine
	//_terminate(23);//to terminate the coroutine
}
COROUTINE_END;
example1 e1;//create an instance of this coroutine


COROUTINE_BEGIN(int,example2)//usually this is enough for a coroutine
{//if no caller is defined a standard caller is included
	COROUTINE_YIELD(34);
	return 1;
}
COROUTINE_END;

COROUTINE_BEGIN(int,example3)
{
	int*in_heap_allocated=new int[10];
	COROUTINE_YIELD2(delete[]in_heap_allocated;,34);//a coroutine can return after every COROUTINE_YIELD call
	//if memory is allocated in the heap it has to be freed before termination
	//for that use COROUTINE_YIELD2() with the first parameter code to free memory on termination 
	//and the second parameter the value to yield
	
	//do some code
	delete[]in_heap_allocated;
	return 1;
}
COROUTINE_END;
example2 e2;

int main(){
	while(!e2.is_terminated())//you can check if the coroutine terminated by calling is_terminated() or calling the .operator bool() operator
		e2();//to call the coroutine simply use the normal syntax for calling a function
	e2.reset();//you can restart the coroutine
	while(e2)
		e2();
	e1(10);//example1 has no caller defined that takes no parameter
	//example1 has a custom caller that takes one parameter
}
