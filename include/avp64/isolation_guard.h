/******************************************************************************
 *                                                                            *
 * Copyright 2024 Nils Bosbach                                                *
 *                                                                            *
 * This software is licensed under the MIT license found in the               *
 * LICENSE file at the root directory of this source tree.                    *
 *                                                                            *
 ******************************************************************************/

#ifndef AVP64_ISOLATION_GUARD_H
#define AVP64_ISOLATION_GUARD_H

#include <condition_variable>
#include <mutex>
#include <thread>

namespace avp64 {

class isolation_guard
{
private:
    std::mutex m_mtx;
    std::thread::id m_thread_id;
    std::condition_variable m_cv;
    int m_cnt;

public:
    isolation_guard();
    isolation_guard(const isolation_guard&) = delete;
    ~isolation_guard() = default;

    void enter();
    void leave();
    bool try_enter();
    bool current_thread_entered();
};

class iguard_enter
{
private:
    isolation_guard& m_guard;

public:
    iguard_enter() = delete;
    iguard_enter(const iguard_enter&) = delete;
    iguard_enter(isolation_guard& guard): m_guard(guard) { m_guard.enter(); }
    ~iguard_enter() { m_guard.leave(); }
};

class iguard_leave
{
private:
    isolation_guard& m_guard;

public:
    iguard_leave() = delete;
    iguard_leave(const iguard_enter&) = delete;
    iguard_leave(isolation_guard& guard): m_guard(guard) { m_guard.leave(); }
    ~iguard_leave() { m_guard.enter(); }
};

} // namespace avp64
#endif
