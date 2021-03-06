//----------------------------------------------------------------------------
/// \file   logger_impl_async_file.hpp
/// \author Serge Aleynikov
//----------------------------------------------------------------------------
/// \brief Front-end plugin implementing asynchronous writer for the <logger>
/// class.
///
/// This logger is optimized for imposing minimal overhead on the
/// threads calling logging macros.  That overhead on a 64-bit Linux platform
/// is under 2us.  This efficiency is accomplished by a loose guarantee that
/// the messages written to disk at the time of the logging function call.
/// Consequently, if the program crashes immediately after making the logging
/// call, some messages may be lost.
/// The implementation uses a lock-free cached memory allocator in order to
/// reduce the number of new/delete calls.
/// Note that the log file may contain timestamps out-of-order.
/// This is infrequent yet explained by the fact that time querying happens
/// at a time of calling the logging function rather than writing the message
/// to disk (done by another thread).
/// For performance testing run: "THREAD=3 VEBOSE=1 test_logger".
//----------------------------------------------------------------------------
// Copyright (C) 2003-2009 Serge Aleynikov <saleyn@gmail.com>
// Created: 2009-11-25
//----------------------------------------------------------------------------
/*
***** BEGIN LICENSE BLOCK *****

This file is part of the utxx open-source project.

Copyright (C) 2009 Serge Aleynikov <saleyn@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

***** END LICENSE BLOCK *****
*/
#ifndef _UTXX_LOGGER_ASYNC_FILE_HPP_
#define _UTXX_LOGGER_ASYNC_FILE_HPP_

#include <utxx/logger.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <utxx/container/concurrent_stack.hpp>
#include <utxx/alloc_cached.hpp>

namespace utxx {

class logger_impl_async_file: public logger_impl {
    std::string m_name;
    std::string m_filename;
    bool        m_append;
    bool        m_timestamp;
    int         m_levels;
    mode_t      m_mode;
    int         m_fd;
    bool        m_show_location;
    bool        m_show_ident;
    std::string m_error;

    typedef container::blocking_versioned_stack<> stack_t;
    struct timespec                 m_timeout;
    stack_t                         m_stack;
    bool                            m_terminated;
    boost::barrier*                 m_barrier;
    boost::thread*                  m_thread;
    memory::cached_allocator<char>  m_allocator;

    logger_impl_async_file(const char* a_name)
        : m_name(a_name), m_append(true), m_timestamp(true), m_levels(LEVEL_NO_DEBUG)
        , m_mode(0644), m_fd(-1), m_show_location(true), m_show_ident(false)
        , m_terminated(false), m_barrier(NULL), m_thread(NULL)
    {
        m_timeout.tv_sec = 2; m_timeout.tv_nsec = 0;
    }

    struct async_data : public stack_t::node_t {
        unsigned short size;
        log_level level;

        char*       data() { return reinterpret_cast<char*>(this + 1); } 
        async_data* next() { return reinterpret_cast<async_data*>(stack_t::node_t::next); }
        static size_t allocation_size(size_t sz) {
            return sz + sizeof(async_data) - stack_t::header_size();
        }
        static async_data* to_node(void* p) { return reinterpret_cast<async_data*>(p)-1; } 
    };

    void send_data(log_level level, const char* msg, size_t size) throw(io_error);

    int  write_data(async_data* p);
    void finalize();
public:
    static logger_impl_async_file* create(const char* a_name) {
        return new logger_impl_async_file(a_name);
    }

    virtual ~logger_impl_async_file() { finalize(); }

    const std::string& name() const { return m_name; }

    /// Dump all settings to stream
    std::ostream& dump(std::ostream& out, const std::string& a_prefix) const;

    bool init(const variant_tree& a_config)
        throw (badarg_error, io_error);

    void log_msg(const log_msg_info& info, const timestamp& a_tv,
        const char* fmt, va_list args) throw (std::runtime_error);
    void log_bin(const char* msg, size_t size)
        throw (std::runtime_error);

    void operator() ();     // Thread functor
};

} // namespace utxx

#endif // _UTXX_LOGGER_ASYNC_FILE_HPP_

