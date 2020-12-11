//rv is the value to yield uid_int is a 32Bit unsigned integer and must be unique for each staco_yield statement in a staco region
//uid_int can not be 0
#define co_yield(rv,uid_int){coroutine_index=uid_int;return rv;case uid_int:;}
#define co_return(rv){coroutine_index=0;return rv;}
#define co_var static
#define co_int static int
#define co_char static char

//all variables should be predeclared before staco_init is called
//under circumstances it is possible to declare a variable inside of a staco-region
//warning do not use for non static member functions
#define co_init static unsigned int coroutine_index = 0;switch (coroutine_index){default:break;case 0:;
#define co_end coroutine_index=0;}

//all variables have to be defined as member variables of the class
//co_member_id has to be written in the class member list
//co_member_id must be written once for every coroutine
//uid_str must be the same for the same coroutine
//uid_str must not be the same for two diffrent coroutines
//uid_str must be a valid variable name
#define co_member_id(uid_str)unsigned int uid_str=0;
#define co_member_init(uid_str)switch (uid_str){default:break;case 0:;
#define co_member_yield(uid_str,rv,uid_int){uid_str=uid_int;return rv;case uid_int:;}
#define co_member_return(uid_str,rv){uid_str=0;return rv;}
#define co_member_end(uid_str)uid_str=0; }

/*examples
int normal_coroutine(int parameter) {
	static int i;//all variables being used have to be declared ahead and must stored externally(for example by declaring them as static)
	co_init;
	co_yield(54, 489274982);//yield 54 the second number does not matter as long as it is unique in this coroutine
	for (i = 0; i < 100; i++)
	{
		co_yield(32, 5345);//yield 32
		if (parameter == i)
			co_return(42);//return 42 and reset the coroutine
	}
	co_end;
	return 7;//this line will be called once, if the coroutine reached co_end. the next time the coroutine is called, it will be reseted
}

class {
	co_member_id(COROUTINE1);//declare the coroutine , you can use any valid variable name for this
	int i = 0;//all variables being used have to be declared ahead and must be stored externally , but for member coroutines you can not declare them using static since that stores them gloabally
	//instead they should be stored as private member variables
public:
	int normal_coroutine(int parameter) {
		co_member_init(COROUTINE1);
		co_member_yield(COROUTINE1,54, 489274982);//yield 54 the second number does not matter as long as it is unique in this coroutine
		for (i = 0; i < 100; i++)
		{
			co_member_yield(COROUTINE1,32, 5345);//yield 32
			if (parameter == i)
				co_member_return(COROUTINE1,42);//return 42 and reset the coroutine
		}
		co_member_end(COROUTINE1);
		return 7;//this line will be called once, if the coroutine reached co_end. the next time the coroutine is called, it will be reseted
	}
};
*/