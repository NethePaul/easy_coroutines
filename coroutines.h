//Interface at end of file
#ifndef _COROUTINE_THREAD_SYNCHRONISATION_
#ifndef initialset//coroutine is a template class that needs sligetly diffrent code for <void> than all other datatypes
#define initialset 1//generic definition
#elif initialset == 1
#define initialset 2//void definintion
#define _COROUTINE_THREAD_SYNCHRONISATION_
#endif
#if initialset==1//generic definition
#include<thread>
#include<condition_variable>
#include<mutex>
template<class return_type>class coroutine{
#else initialset==2//void definintion
template<>class coroutine<void> {
	using return_type = void;
#endif
	std::condition_variable cv;
	std::mutex mtx;
	volatile bool initiated = 0;
	volatile bool executing = 0;
	volatile bool terminating = 0;
	std::thread*coroutine_code;
#if initialset==1
	return_type return_value = {};
#endif
protected:
#if initialset==1
	virtual return_type f() { return{}; };
#else
	virtual return_type f() {};
#endif
	coroutine() :mtx(), coroutine_code(0), terminating(0),executing(1) {}
	~coroutine() {destroy_this();}
	void prepare_reset() {
		destroy_this();
		terminating = 0;
		executing = 1;
	}
	void destroy_this() {
		terminating = 1;
		executing = 0;
		cv.notify_all();
		if (coroutine_code) {
			coroutine_code->join();
			delete coroutine_code;
			coroutine_code = 0;
		}
	}
	void set_coroutine(){
		coroutine_code = new std::thread([this]() {init_coroutine();
#if initialset==1
		_terminate(f());
#else
		f();_terminate();
#endif
		});
	}
	void init_coroutine() {
		std::unique_lock<std::mutex>ul(mtx);
		executing = 0;
		cv.notify_all();
		cv.wait(ul, [this]() {return executing||terminating; });
	};
#if initialset==1
	bool _yield(return_type rv) {
		return_value = rv;
#elif initialset==2
	bool _yield() {
#endif
		if (terminating)return true;
		executing = 0;
		std::unique_lock<std::mutex>ul(mtx);
		cv.notify_all();
		cv.wait(ul, [this]() {return executing||terminating; });
		ul.unlock();
		if (terminating)return true;
		return false;
	}
	return_type call() {
#if initialset==1
		if (!coroutine_code||terminating) {//error return 0
			unsigned char i[sizeof(return_type)] = {};
			return *(return_type*)&i;
		}
#endif
		executing = 1;
		std::unique_lock<std::mutex>ul(mtx);
		cv.notify_all();
		cv.wait(ul, [this]() {return !executing; });
		ul.unlock();
#if initialset==1
		return return_value;
#endif
	}
#if initialset==1
	void _terminate(return_type rv){
		if(!terminating)return_value = rv;
#else
	void _terminate(){
#endif
		terminating = 1;
		if (coroutine_code) {
			executing = 0;
			cv.notify_all();
		}
	}
public:
	coroutine(const coroutine&rhs) = delete;
	coroutine&operator=(const coroutine&rhs) = delete;
	operator bool()const{ return coroutine_code && !terminating; }
	return_type operator()() { return call(); }
	bool is_terminated()const { return !coroutine_code||terminating; }
	void reset() { prepare_reset(); set_coroutine(); }
};
#endif//_COROUTINE_THREAD_SYNCHRONISATION
#ifndef _COROUTINE_PREPROZESSOR_GUARD_
#define _COROUTINE_PREPROZESSOR_GUARD_
#include"coroutines.h"//this file has to be included twice for the <void> implementation
#undef initialset
#endif
#ifndef _COROUTINE_INTERFACE_
#define _COROUTINE_INTERFACE_
#define COROUTINE_BEGIN(return_type,cname)class cname : public coroutine<return_type> {public:cname(){set_coroutine();}private: return_type f()override
#define COROUTINE_END }
#define COROUTINE_CALLER(return_type) public:return_type operator()
#define COROUTINE_CALL return call()
#define COROUTINE_VAR private:
#define COROUTINE_SUBROUTINE private:
#define COROUTINE_YIELD(...)if(_yield(__VA_ARGS__))return __VA_ARGS__;
#define COROUTINE_YIELD2(extra,...)if(_yield(__VA_ARGS__)){extra;return __VA_ARGS__;}//executes extra code on termination
#define COROUTINE_STD_CALLER(return_type) public:return_type operator()(){return call();}
#define FRIEND_COROUTINE(cname)friend class cname
#endif
