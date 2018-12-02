#pragma once

#include <mutex>
#include "NativeType.h"
#include "Spinlock.h"

namespace std
{

// CLASS TEMPLATE lock_guard
template<>
class lock_guard<chrone::multithreading::Spinlock>
{	// class with destructor that unlocks a mutex
public:
	using mutex_type = chrone::multithreading::Spinlock;

	explicit lock_guard(mutex_type& _Mtx)
		: _MyMutex(_Mtx)
	{	// construct and lock
		_MyMutex.Lock();
	}

	lock_guard(mutex_type& _Mtx, adopt_lock_t)
		: _MyMutex(_Mtx)
	{	// construct but don't lock
	}

	~lock_guard() noexcept
	{	// unlock
		_MyMutex.Unlock();
	}

	lock_guard(const lock_guard&) = delete;
	lock_guard& operator=(const lock_guard&) = delete;
private:
	mutex_type& _MyMutex;
};


// CLASS TEMPLATE unique_lock
template<>
class unique_lock<chrone::multithreading::Spinlock>
{	// whizzy class with destructor that unlocks mutex
public:
	typedef chrone::multithreading::Spinlock mutex_type;
	typedef unique_lock<mutex_type> _Myt;

	// CONSTRUCT, ASSIGN, AND DESTROY
	unique_lock() noexcept
		: _Pmtx(0), _Owns(false)
	{	// default construct
	}

	explicit unique_lock(mutex_type& _Mtx)
		: _Pmtx(_STD addressof(_Mtx)), _Owns(false)
	{	// construct and lock
		_Pmtx->Lock();
		_Owns = true;
	}

	unique_lock(mutex_type& _Mtx, adopt_lock_t)
		: _Pmtx(_STD addressof(_Mtx)), _Owns(true)
	{	// construct and assume already locked
	}

	unique_lock(mutex_type& _Mtx, defer_lock_t) noexcept
		: _Pmtx(_STD addressof(_Mtx)), _Owns(false)
	{	// construct but don't lock
	}

	unique_lock(mutex_type& _Mtx, try_to_lock_t)
		: _Pmtx(_STD addressof(_Mtx)), _Owns(_Pmtx->TryLock())
	{	// construct and try to lock
	}

	unique_lock(unique_lock&& _Other) noexcept
		: _Pmtx(_Other._Pmtx), _Owns(_Other._Owns)
	{	// destructive copy
		_Other._Pmtx = 0;
		_Other._Owns = false;
	}

	unique_lock& operator=(unique_lock&& _Other)
	{	// destructive copy
		if (this != _STD addressof(_Other))
		{	// different, move contents
			if (_Owns)
				_Pmtx->Unlock();
			_Pmtx = _Other._Pmtx;
			_Owns = _Other._Owns;
			_Other._Pmtx = 0;
			_Other._Owns = false;
		}
		return (*this);
	}

	~unique_lock() noexcept
	{	// clean up
		if (_Owns)
			_Pmtx->Unlock();
	}

	unique_lock(const unique_lock&) = delete;
	unique_lock& operator=(const unique_lock&) = delete;

	void lock()
	{	// lock the mutex
		_Validate();
		_Pmtx->Lock();
		_Owns = true;
	}

	bool try_lock()
	{	// try to lock the mutex
		_Validate();
		_Owns = _Pmtx->TryLock();
		return (_Owns);
	}

	void unlock()
	{	// try to unlock the mutex
		if (!_Pmtx || !_Owns)
			throw(system_error(
				_STD make_error_code(errc::operation_not_permitted)));

		_Pmtx->Unlock();
		_Owns = false;
	}

	void swap(unique_lock& _Other) noexcept
	{	// swap with _Other
		_STD swap(_Pmtx, _Other._Pmtx);
		_STD swap(_Owns, _Other._Owns);
	}

	mutex_type *release() noexcept
	{	// disconnect
		mutex_type *_Res = _Pmtx;
		_Pmtx = 0;
		_Owns = false;
		return (_Res);
	}

	bool owns_lock() const noexcept
	{	// return true if this object owns the lock
		return (_Owns);
	}

	explicit operator bool() const noexcept
	{	// return true if this object owns the lock
		return (_Owns);
	}

	mutex_type *mutex() const noexcept
	{	// return pointer to managed mutex
		return (_Pmtx);
	}

private:
	mutex_type *_Pmtx;
	bool _Owns;

	void _Validate() const
	{	// check if the mutex can be locked
		if (!_Pmtx)
			throw(system_error(
				_STD make_error_code(errc::operation_not_permitted)));

		if (_Owns)
			throw(system_error(
				_STD make_error_code(errc::resource_deadlock_would_occur)));
	}
};

}

namespace chrone::multithreading::scheduler
{
	using LockGuardSpinLock = std::lock_guard<Spinlock>;
}