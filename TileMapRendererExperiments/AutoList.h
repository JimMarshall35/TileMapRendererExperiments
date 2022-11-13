#pragma once
#include <list>

enum AUTOLIST_CTOR_OPTIONS {
	DO_NOT_ADD_TO_LIST
};
template< class T >
class AutoList {
protected:
	AutoList(AUTOLIST_CTOR_OPTIONS options) {
		_added = false;
	}

	AutoList() {
		T* self = static_cast<T*>(this);
		s_list.push_back(self);
	}

	AutoList(const AutoList&)
	{
		T* self = static_cast<T*>(this);
		s_list.push_back(self);
	}

	~AutoList() {
		if (_added) {
			T* self = static_cast<T*>(this);
			s_list.remove(self);
		}
	}
public:
	static const std::list<T*>& GetList() {
		return s_list;
	}
private:
	bool _added = true;
	static std::list<T*> s_list;
};

template<class T>
std::list<T*> AutoList<T>::s_list;