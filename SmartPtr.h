#include "Mutex.h"
#include <iostream>
class CRef  //as smarty pointer counter,
{
public:
    CRef():_ref(0){}
    virtual ~CRef(){}
	CRef(const CRef&):_ref(0)
	{}
	CRef& operator=(const CRef&)
	{
		return *this;
	}
	virtual void IncRef()
	{
		mutex.Lock();
		++_ref;
		mutex.UnLock();
	}
	virtual void DecRef()
	{
		mutex.Lock();
		--_ref;
		mutex.UnLock();
		if(_ref == 0)
		{
			delete this;			
		}
	}
	virtual int getRef()
	{
		mutex.Lock();
		return _ref;
		mutex.UnLock();
	}
private:
	int _ref;
	Mutex mutex;
};

template <typename T>
class SmartPtr
{
public:
    SmartPtr(T* ptr):m_ptr(ptr)
	{
		if(m_ptr)
		{
			m_ptr->IncRef();			
		}
		std::cout << "SmatePtr:: SmatePtr()invoke\n";
	}
	SmartPtr():m_ptr(new T())
	{
		if(m_ptr)
		{
			m_ptr->IncRef();			
		}
		std::cout << "SmatePtr:: SmatePtr()invoke\n";		
	}
    virtual ~SmartPtr()
	{
		if(m_ptr)
		{
			m_ptr->DecRef();
		}
		std::cout << "SmatePtr:: ~SmatePtr()invoke\n";
	}
	SmartPtr(const SmartPtr& sptr)
	{
		this->m_ptr = sptr.m_ptr;
		if(this->m_ptr)
		{
			this->m_ptr->IncRef();
		}
		std::cout << "SmatePtr:: SmatePtr()invoke\n";
	}
	SmartPtr& operator=(const T& p)
	{
		if(this->m_ptr != p)
		{
			if(p)
			{
				p->IncRef();
			}
			T* ptr = this->m_ptr;
			this->m_ptr = p;
			if(ptr)
			{
				ptr->DecRef();
			}
		}
		return *this;
	}
	SmartPtr& operator=(const SmartPtr& sptr)
	{
		if(this->m_ptr != sptr.m_ptr)
		{
			if(sptr.m_ptr)
			{
				sptr.m_ptr->IncRef();
			}
			T* ptr = this->m_ptr;
			this->m_ptr = sptr.m_ptr;
			if(ptr)
			{
				ptr->DecRef();
			}
		}
		return *this;
	}
	T* get()const { return m_ptr;}
	T* operator->()const
	{
		if(m_ptr)
		{
			return m_ptr;
		}
	}
	T& operator*() const
	{
		if(m_ptr)
		{
			return *(m_ptr);
		}
	}
	operator bool() const
	{ return m_ptr?true:false;}

private:
    T* m_ptr;
};

template <typename T, class U>
inline bool operator==(const SmartPtr<T>& lhs, const SmartPtr<U>& rhs)
{
	T* l = lhs.get();
	U* r = rhs.get();
	if(l && r)
	{
		return *l == *r;
	}
	return !l && !r;
}
template <typename T, class U>
inline bool operator!=(const SmartPtr<T>& lhs, const SmartPtr<U>& rhs)
{
	return !operator==(lhs, rhs);
}
	
