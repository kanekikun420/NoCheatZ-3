/*
 * Copyright 2008-2013 Nicolas Maingot
 *
 * This file is part of CSSMatch.
 *
 * CSSMatch is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * CSSMatch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CSSMatch; if not, see <http://www.gnu.org/licenses>.
 *
 * Additional permission under GNU GPL version 3 section 7
 *
 * If you modify CSSMatch, or any covered work, by linking or combining
 * it with "Source SDK" (or a modified version of that SDK), containing
 * parts covered by the terms of Source SDK licence, the licensors of
 * CSSMatch grant you additional permission to convey the resulting work.
 */

#ifndef __THREADING_H__
#define __THREADING_H__

#include <stdexcept>
#include <string>

#ifdef _MSC_VER // VC++ only
#pragma warning(push)
#pragma \
    warning(disable:4290) /* C++ exception specification ignored except to indicate a function is
                             not __declspec(nothrow) */
#endif // _MSC_VER

namespace threading
{
    class ThreadException
    {
    private:
        std::string msg;
    public:
        ThreadException(const std::string & message) : msg(message) {}
        const std::string & getMessage() const { return msg; }
    };

    /**
     * Basic platform-independant java-like thread class.
     */
	struct ThreadData;
    class Thread
    {
    private:
        /** Private data. */
        ThreadData * data;

    public:
        Thread();
        virtual ~Thread();

        /**
         * Start the thread.
         */
        void start() throw(ThreadException);

        /**
         * Thread routine.
         * <b>DO NOT CALL ME, YOUNG PADAWAN. USE START.</b>
         */
        virtual void run() = 0;

        /**
         * Join the thread with the current one.
         */
        void join() throw(ThreadException);
    };

    /**
     * Cause the current thread to sleep.
     * @param ms Sleeping duration in milliseconds.
     */
    void sleep(long ms);

   
    struct MutexData;

    /**
     * Basic platform-independant mutex.
     */
    class Mutex
    {
    private:
        /** Private data. */
        MutexData * data;

    public:
        Mutex() throw(ThreadException);
        ~Mutex();

        /**
         * Lock the mutex.
         */
        void lock() throw(ThreadException);

        /**
         * Unlock the mutex.
         */
        void unlock() throw(ThreadException);
    };

#if 0
    struct EventData;

    /**
     * Event::wait() return values.
     */
    enum EventWaitResult
    {
        /** Event was signaled. */
        THREADING_EVENT_SIGNALED = 0,

        /** Wait timed out. */
        THREADING_EVENT_TIMEOUT
    };

    /**
     * Basic platform-independant auto-reset event.
     */
    class Event
    {
    private:
        /** Private data. */
        EventData * data;

    public:
        Event();
        ~Event();
   
        /**
         * Cause the caller thread to wait for an event.
         * @param timeoutMs Max wait time in milliseconds.
         * @return EventWaitResult
         */
        EventWaitResult wait(long timeoutMs) throw(ThreadException);

        /**
         * Fire an event.
         */
        void set() throw(ThreadException);
    };
#endif
} // namespace threading

#ifdef _MSC_VER // VC++ only
#pragma warning(pop)
#endif // _MSC_VER

#endif // __THREADING_H__
