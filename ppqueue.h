//Copyright (C), Selman Özkurt, 2020
//Licensed under GPL 3.0
//See github.com/SelmanB/ppqueue


#ifndef PPQUEUE_INCL
#define PPQUEUE_INCL

#if __cplusplus <= 201103L
#error ppqueue is designed for c++11 standard or newer
#else


#include <vector>    //for vector template
#include <mutex>
#include <thread>
#include <functional>//for less template
#include <algorithm>//for heap


template <typename T ,typename Container = std::vector<T>, typename Compare  = std::less<typename Container::value_type>>
class ppqueue{
public:
	typedef T 			value_type;
	typedef Container 	container_type;
	typedef T& 			reference;
	typedef const T&	const_reference;
	typedef size_t 		size_type;


private:
	Compare lss;	//constructor using cmp=Compare(); use as lss(a,b)
	container_type data;

	std::thread pusher;
	std::thread popper;
	mutable std::mutex accessible;
	mutable std::mutex pushing;
	mutable std::mutex popping;
	bool killsig;

	void init(void)//beginning common part of initialization
	{
		accessible.lock();
		pushing.lock();
		popping.lock();
		killsig=false;
		pusher=std::thread(&ppqueue::t_push_heap,this);
		popper=std::thread(&ppqueue::t_pop_heap,this);
	}
	void initl(void)//ending common part of the initialization
	{
		std::make_heap(data.begin(),data.end());
		accessible.unlock();
	}
	void t_push_heap()//pusher thread function
	{
		while(true){
			pushing.lock();
			if(killsig)return;
			std::push_heap(data.begin(),data.end());
			accessible.unlock();
		}
	}
	void t_pop_heap()//popper thread function
	{
		while(true){
			popping.lock();
			if(killsig)return;
			std::pop_heap(data.begin(),data.end());
			accessible.unlock();
		}
	}


public:	
	ppqueue(const Compare& comp, const Container& ctnr)//copy ctnr and make_heap always copy comp
	{
		init();
		lss=comp;
		data=ctnr;
		initl();
	
	}

	template <class InputIterator>
	ppqueue(InputIterator first, InputIterator last,const Compare& comp, Container& ctnr)//copy ctnr and make_heap
	{
		init();
		lss=comp;
		data=ctnr;
		ctnr.insert(ctnr.cend(),first,last);
		initl();
	}	
	explicit ppqueue(const Compare& comp=Compare(), Container&& ctnr=Container()) //move construct cntr, then make_heap
	{
		init();
		lss=comp;
		data=ctnr;
		initl();
	}

	template <class InputIterator>
  	ppqueue(InputIterator first, InputIterator last,const Compare& comp, Container&& ctnr = Container())//move construct ctnr then insert iterated elements and make_heap
  	{
  		init();
		lss=comp;
		data=ctnr;
		ctnr.insert(ctnr.cend(),first,last);
		initl();
  	}
  	//allocator constructor not implemented
	~ppqueue()
	{
		killsig=true;
		pushing.unlock();
		popping.unlock();

		pusher.join();
		popper.join();
	}


	bool empty() const	//call empty of the container
	{
		accessible.lock();
		register bool res=data.empty();
		accessible.unlock();
	}
	size_type size() const	//call size of the container
	{
		accessible.lock();
		register size_type res=data.size();
		accessible.unlock();
		return res;
	}
	const_reference top() const	//call front of the container
	{
		accessible.lock();
		const_reference res=data.front();
		accessible.unlock();
		return res;
	}
	void push(const value_type& val)//call push_back and then push_heap
	{
		accessible.lock();
		data.push_back(val);
		pushing.unlock();
	}
	void push(value_type&& val)//call push_back and then push_heap
	{
		accessible.lock();
		data.push_back(val);
		pushing.unlock();
	}
	template <class... Args>
	void emplace(Args&&... args)//call emplace_back and push_heap
	{
		accessible.lock();
		data.emplace_back(std::forward<Args>(args)...);
		pushing.unlock();
	}
	void pop()//call member pop_back and then pop_heap
	{
		accessible.lock();
		data.pop_back();
		popping.unlock();
	}
	void swap(ppqueue& x)
	noexcept(noexcept(std::swap(data,x.c)) && noexcept(std::swap(lss,x.lss))) // call swap for the container
	{
		accessible.lock();
		x.accessible.lock();
		std::swap(data,x.data);
		accessible.unlock();
		x.accessible.unlock();
	}
};

#endif
#endif