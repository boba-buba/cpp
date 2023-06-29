#ifndef PERS_PTR_H__
#define PERS_PTR_H__
#include "file_access.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <deque>

class EvictionOldestUnlock;
template <typename ValueType, size_t arity, typename EvictionPolicy>
class cached_container;

template < typename ValueType, size_t arity , typename EvictionPolicy > 
class pers_ptr {
public:
	pers_ptr(cached_container<ValueType, arity, EvictionPolicy>& cache, internal_ptr ip) : cache_(cache), ip_(ip)
	{  
		if (ip_ == null_internal_ptr) return;
		persistent_node<ValueType, arity>& found_ptr = cache_.get_node(ip_);
		p_node_ = std::make_unique < persistent_node<ValueType, arity>>(found_ptr); 
		//Proc ty data kopirujete? To je nadbytecne, uz je mate v cache
	};
	pers_ptr(const pers_ptr&) = delete;
	pers_ptr(pers_ptr&& other) noexcept : cache_(other.cache_) 
	{
		p_node_ = std::move(other.p_node_);
		ip_ = other.ip_;
	};
	~pers_ptr()
	{
		cache_.unlock_from_cache(ip_);
		ip_ = null_internal_ptr;
		p_node_ = nullptr;
	}
	pers_ptr& operator=(const pers_ptr&) = delete;
	pers_ptr& operator=(pers_ptr&& other) noexcept
	{
		if (this == &other)
			return *this;  
		cache_.unlock_from_cache(ip_);
		ip_ = other.ip_;
		p_node_ = std::move(other.p_node_);
		
		other.p_node_ = nullptr;
		other.ip_ = null_internal_ptr;
		return *this;
	}
	explicit operator bool() const noexcept { return ip_ != null_internal_ptr; };
	
	pers_ptr get_ptr(size_t index) 
	{ 
		internal_ptr index_ = null_internal_ptr;
		if (index < arity && index >= 0)
		{
			index_ = p_node_->ptr[index];
		}
		return pers_ptr<ValueType, arity, EvictionPolicy>(cache_, index_);
	};
	const ValueType& operator*()
	{
		return p_node_->value;
	}
	const ValueType* operator->()
	{
		return &p_node_->value;
	}
private:
	cached_container<ValueType, arity, EvictionPolicy>& cache_;
	internal_ptr ip_ = null_internal_ptr;
	std::unique_ptr<persistent_node<ValueType, arity>> p_node_ = nullptr;
	//zbtecne prekomplikovane a neefektivni reseni, stacilo mit nejaky odkaz na data v cache
};


template <typename ValueType, size_t arity, typename EvictionPolicy>
class cached_container {
public:
	
	cached_container(size_t cache_size, file_descriptor fd): cache_size_(cache_size), fd_(fd) {}
	pers_ptr<ValueType, arity, EvictionPolicy> root_ptr() { return pers_ptr<ValueType, arity, EvictionPolicy>(*this, root_internal_ptr); };
	pers_ptr<ValueType, arity, EvictionPolicy> null_ptr() { return pers_ptr<ValueType, arity, EvictionPolicy>(*this, null_internal_ptr); };

	persistent_node<ValueType, arity>&
	get_node(internal_ptr ip)
	{
		auto it = cache_map_.find(ip);
		if (it == cache_map_.end())
		{
			load_to_cache(ip);
			it = cache_map_.find(ip);
		}
		else
		{
			ep.relock(ip, ep.unlock(ip));
		}
		return it->second;
	}
	void load_to_cache(internal_ptr ip)
	{
		persistent_node<ValueType, arity> node;
		fa.read(fd_, ip, node);
		ep.load(ip);
		
		if (current_size == cache_size_)
		{
			internal_ptr to_release = ep.release();
			auto it = cache_map_.find(to_release);
			while (it == cache_map_.end())
			{
				to_release = ep.release();
				it = cache_map_.find(to_release);
			}
			cache_map_.erase(it);
			current_size--;
		}
		current_size++;
		cache_map_.emplace(ip, node);
	}
	
	void unlock_from_cache(internal_ptr ip)
	{
		if (ip != null_internal_ptr)
		{
			ep.unlock(ip);
		}
	}
private:
	file_access<persistent_node<ValueType, arity>> fa;
	EvictionPolicy ep;
	std::size_t cache_size_ = 0;
	std::size_t current_size = 0;
	file_descriptor fd_;
	std::unordered_map<internal_ptr, persistent_node<ValueType, arity>> cache_map_;
};


class EvictionOldestUnlock {
public:
	using hint_type = int;
	
	void load(internal_ptr ip) 
	{	
		locked_.emplace(ip);	
	}
	hint_type unlock(internal_ptr ip)
	{
		auto it = locked_.find(ip); 
		if (it != locked_.end()) locked_.erase(it);
		if (!locked_.contains(ip))
			unlocked_.push_back(ip);
		return hint_type{};
	}
	void relock(internal_ptr ip, hint_type t )
	{
		locked_.emplace(ip);
	}
	internal_ptr release()                  
	{
		internal_ptr to_release = unlocked_.front();
		unlocked_.pop_front();
		return to_release;
	}
private:
	std::unordered_multiset<internal_ptr> locked_;
	//Je to sice v prumeru konstantni, ale urcite ne okamzity pristup. Efektivnejsi by bylo si to pamatovat primo u node (coz stejne muste drzet) a vratit politice
	std::list<internal_ptr> unlocked_;
};

#endif