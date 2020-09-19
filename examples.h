#pragma once
#include"coroutines.h"


COROUTINE_BEGIN(int,crng)//this coroutine does not have a standard caller for the user so you have to define one manually
{//the first makro parameter is the return value and the second is the class name
	for (int i = 0; i < 1;) {
		COROUTINE_YIELD(42);
		f2(23);//call the subroutine defined below
	}
	_terminate(102);
	return(32);
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
	COROUTINE_YIELD(buffer);//the subroutine can yield a value in place of the coroutine
	return buffer;//but when calling return it returns to the coroutine instead of terminating the coroutine
	//terminate(23);//to terminate the coroutine
}
COROUTINE_END;
crng rng;//create an instance of this coroutine


COROUTINE_BEGIN(int,cend)
{
	COROUTINE_YIELD(34);
	return 1;
}
COROUTINE_END;
cend coroutine;