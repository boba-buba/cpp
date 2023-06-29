#ifndef LOCKER_HPP_
#define LOCKER_HPP_

#include <mutex>
#include <condition_variable>
#include "interval_tree.hpp"
//(smaller -) Mixed includes, standard library should come first
#include <iostream>
#include <memory>

class exclusive_lock;
class locker;
using value_type = int;
using size_type = std::size_t;
using interval_type = std::pair<size_type, size_type>;

class shared_lock {
//(smaller -) quite a lot of code repetition between the two lock types
public:
    shared_lock() noexcept  {} // produce invalid object
    shared_lock(locker* lck, interval_type& itrv) : lck_(lck), itrv_(itrv) {}
    //shared_lock(std::shared_ptr<locker> lck, interval_type& itrv) : lck_(lck), itrv_(itrv) {}

    shared_lock(const shared_lock&) = delete; // no support for copy
    shared_lock(shared_lock&& other) noexcept : lck_(other.lck_), itrv_(other.itrv_)  // move, invalidate source object
    {
        other.itrv_ = { 0, 0 };
        other.lck_ = nullptr;
    }
    shared_lock& operator=(const shared_lock&) = delete; // no support for copy
    shared_lock& operator=(shared_lock&& other) noexcept // unlock `*this` (if not invalid), move, invalidate source object
    {
        if (itrv_.second != 0) unlock();
        lck_ = std::move(other.lck_);
        itrv_ = std::move(other.itrv_);
        other.itrv_ = { 0,0 };
        other.lck_ = nullptr;
        return *this;
    }
    ~shared_lock() // unlock (if not invalid), noexcept by default
    {
        if (itrv_.second != 0)
            unlock();
    }
    void unlock() noexcept;  // unlock (if not invalid), invalidate 
    exclusive_lock upgrade();   // BLOCKING, upgrade to exclusive_lock, invalidate `*this`
private:
    //std::shared_ptr<locker> lck_ = nullptr;
    locker* lck_ = nullptr;
    interval_type itrv_ = { 0, 0 };
};

class exclusive_lock {
public:
    exclusive_lock() noexcept {}  // produce invalid object
    exclusive_lock(locker* lck, interval_type& itrv) : lck_(lck), itrv_(itrv) {}
    //exclusive_lock(std::shared_ptr<locker> lck, interval_type& itrv) : lck_(lck), itrv_(itrv) {}

    exclusive_lock(const exclusive_lock&) = delete; // no support for copy
    exclusive_lock(exclusive_lock&& other) noexcept : lck_(other.lck_), itrv_(other.itrv_) // move, invalidate source object
    {
        other.itrv_ = { 0, 0 };
        other.lck_ = nullptr;
    }
    exclusive_lock& operator=(const exclusive_lock&) = delete; // no support for copy
    exclusive_lock& operator=(exclusive_lock&& other) noexcept   // unlock `*this` (if not invalid), move, invalidate source object
    {
        if (itrv_.second != 0) unlock();
        lck_ = std::move(other.lck_);
        itrv_ = std::move(other.itrv_);
        other.itrv_ = { 0,0 };
        other.lck_ = nullptr;
        return *this;
    }
    ~exclusive_lock() // unlock (if not invalid), noexcept by default
    {
        if (itrv_.second != 0) unlock();
    }
    void unlock() noexcept;  // unlock (if not invalid), invalidate
    shared_lock downgrade() noexcept;   // downgrade to shared_lock, invalidate `*this`
private:
    //std::shared_ptr<locker> lck_ = nullptr;
    
    locker* lck_ = nullptr;
    //(+) no redundant validity flag
    interval_type itrv_ = { 0, 0 };
};

class locker {
public:
    locker() = default;
    locker(const locker&) = delete; // no support for copy
    locker(locker&&) = delete; // no support for move
    locker& operator=(const locker&) = delete; // no support for copy
    locker& operator=(locker&&) = delete; // no support for move
    ~locker()  // BLOCKING, wait until all locks are removed
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&]() {return locks_.empty(); });
    }//(-!) Time complexity
    //also, instead of this pattern, consider range-based for loop or std::for_each
    shared_lock lock_shared(size_type b, size_type e)  // BLOCKING, create a shared_lock
    {
        interval_type new_itrv = interval_type(b, e);

        std::unique_lock<std::mutex> lock(mutex_);
        
        cv_.wait(lock, [&]() {
            auto ol = locks_.get_overlaps(new_itrv); 
            for (auto it = ol.begin(); it != ol.end(); ++it)
            {
                if ((*it)->value == -1) return false;
            }
            return true;        
        });
        auto itrv = locks_.find(new_itrv);
        if (itrv != locks_.end())
        {
            itrv->value++;
        }
        else {
            locks_.emplace(new_itrv, 1);
        }   
        
        return shared_lock(this, new_itrv);

    }
    exclusive_lock lock_exclusive(size_type b, size_type e)    // BLOCKING, create an exclusive_lock
    {
        interval_type new_itrv = interval_type(b, e);
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock,[&]() {return locks_.get_overlaps(new_itrv).empty(); });       
        
        locks_.emplace(new_itrv, -1);
       //(-!) Time complexity
        return exclusive_lock(this, new_itrv);
    }
private:
    friend class shared_lock;
    friend class exclusive_lock;
    std::mutex mutex_;
    std::condition_variable cv_;
    //(-) Global condition variable
    interval_tree<value_type> locks_;

};

void shared_lock::unlock() noexcept  // unlock (if not invalid), invalidate
{
    if (itrv_.second != 0)
    {
        std::unique_lock<std::mutex> lock(lck_->mutex_);
        auto itrv = lck_->locks_.find(itrv_);
        if (itrv != lck_->locks_.end())
        {
            //(-) this should never evaluate to false in a correct program
            itrv->value--;
            if(itrv->value == 0)  lck_->locks_.erase(itrv_);
        }    
        itrv_ = { 0, 0 };
        lck_->cv_.notify_all();
        //(-) this is sometimes redundant, see the if above
    }
}

void exclusive_lock::unlock() noexcept  // unlock (if not invalid), invalidate
{
    if (itrv_.second != 0)
    {
        std::unique_lock<std::mutex> lock(lck_->mutex_);
        lck_->locks_.erase(itrv_);   
        itrv_ = { 0, 0 };
        lck_->cv_.notify_all();
    }
}

shared_lock exclusive_lock::downgrade() noexcept   // downgrade to shared_lock, invalidate `*this`
{
    std::unique_lock<std::mutex> lock(lck_->mutex_);
    auto node = lck_->locks_.find(itrv_);
    node->value = 1;
    shared_lock sl(lck_, itrv_); 
    itrv_ = { 0, 0 };
    lck_->cv_.notify_all();
    return sl;
}

exclusive_lock shared_lock::upgrade() //blocking
//(+) no redundant notify
{
    std::unique_lock<std::mutex> lock(lck_->mutex_);
    auto itrv = lck_->locks_.find(itrv_);
    //(smaller -) A redesign could allow us to remove such finds
    if (itrv != lck_->locks_.end())
    {
        itrv->value--;
        if (itrv->value == 0)  lck_->locks_.erase(itrv_);
    }
    lck_->cv_.wait(lock, [&]() { return lck_->locks_.get_overlaps(itrv_).empty(); });
    //(-!) Time complexity, erase allows other exclusive locks to block us
    lck_->locks_.emplace(itrv_, -1);
    exclusive_lock el(lck_, itrv_);
    itrv_ = { 0, 0 };
    return el;   
}


#endif
