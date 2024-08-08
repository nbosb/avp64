/******************************************************************************
 *                                                                            *
 * Copyright 2024 Nils Bosbach                                                *
 *                                                                            *
 * This software is licensed under the MIT license found in the               *
 * LICENSE file at the root directory of this source tree.                    *
 *                                                                            *
 ******************************************************************************/

#include "avp64/isolation_guard.h"

#include "vcml/core/types.h"
#include <thread>

namespace avp64 {

isolation_guard::isolation_guard(): m_mtx(), m_thread_id(0), m_cv(), m_cnt(0) {
}

void isolation_guard::enter() {
    std::unique_lock lk(m_mtx);
    std::thread::id id = std::this_thread::get_id();
    if (m_cnt == 0) {
        m_thread_id = id;
    } else {
        if (m_thread_id != id) {
            m_cv.wait(lk, [&] { return m_cnt == 0 || m_thread_id == id; });
            m_thread_id = id;
        }
    }
    ++m_cnt;
}

bool isolation_guard::try_enter() {
    std::lock_guard lk(m_mtx);
    std::thread::id id = std::this_thread::get_id();
    if (m_cnt == 0) {
        m_thread_id = id;
        ++m_cnt;
        return true;
    } else {
        if (m_thread_id == id) {
            ++m_cnt;
            return true;
        }
    }
    return false;
}

void isolation_guard::leave() {
    {
        std::lock_guard lk(m_mtx);
        VCML_ERROR_ON(m_cnt == 0, "leave without entry");
        VCML_ERROR_ON(m_thread_id != std::this_thread::get_id(),
                      "leave from another thread");
        --m_cnt;
    }
    m_cv.notify_all();
}

bool isolation_guard::current_thread_entered() {
    std::lock_guard lk(m_mtx);

    if (m_cnt == 0)
        return false;

    return m_thread_id == std::this_thread::get_id();
}

} // namespace avp64
