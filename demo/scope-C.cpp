/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <unity/api/scopes/ScopeBase.h>
#include <unity/api/scopes/Reply.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <list>
#include <mutex>
#include <unistd.h>

#define EXPORT __attribute__ ((visibility ("default")))

using namespace std;
using namespace unity::api::scopes;

// Simple queue that stores query/reply pairs.
// The put() method adds a pair at the tail, and the get() method returns a pair at the head.
// get() suspends the caller until an item is available or until the queue is told to finish.
// get() returns true if it returns a pair, false if the queue was told to finish.

class Queue
{
public:
    void put(string const& query, ReplyProxy const& reply_proxy)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queries_.push_back(QueryData { query, reply_proxy });
        }
        condvar_.notify_one();
    }

    bool get(string& query, ReplyProxy& reply_proxy)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condvar_.wait(lock, [this] { return !queries_.empty() || done_; });
        if (done_)
        {
            lock.unlock();
            condvar_.notify_all();
        }
        else
        {
            auto qd = queries_.front();
            queries_.pop_front();
            query = qd.query;
            reply_proxy = qd.reply_proxy;
        }
        return !done_;
    }

    void remove(ReplyProxy const& reply_proxy)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        QueryData qd { "", reply_proxy };
        auto it = std::find(queries_.begin(), queries_.end(), qd);
        if (it != queries_.end())
        {
            cerr << "Queue: removed query: " << it->query << endl;
            queries_.erase(it);
        }
        else
        {
            cerr << "Queue: did not find entry to be removed" << endl;
        }
    }

    void finish()
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queries_.clear();
            done_ = true;
        }
        condvar_.notify_all();
    }

    Queue()
        : done_(false)
    {
    }

private:
    struct QueryData
    {
        string query;
        ReplyProxy reply_proxy;

        bool operator==(QueryData const& rhs) const
        {
            // No need to compare query strings because
            // reply proxies are unique per query.
            return reply_proxy == rhs.reply_proxy;
        }
    };

    std::list<QueryData> queries_;
    bool done_;
    std::mutex mutex_;
    std::condition_variable condvar_;
};

// Example scope C: Does not use the query's run() method other than to remember the query.
// The run() method of the scope acts as a worker thread to push replies to remembered queries.
// This example shows that letting run() return immediately is OK, and that the MyQuery instance stays
// alive as long as it can still be cancelled, which is while there is at least one
// ReplyProxy still in existence for this query.

class MyQuery : public QueryBase
{
public:
    MyQuery(string const& scope_name, string const& query, Queue& queue) :
        QueryBase(scope_name),
        query_(query),
        queue_(queue)
    {
        cerr << "My Query created" << endl;
    }

    ~MyQuery() noexcept
    {
        cerr << "My Query destroyed" << endl;
    }

    virtual void cancelled(ReplyProxy const& reply) override
    {
        // Remove this query from the queue, if it is still there.
        // If it isn't, and the worker thread is still working on this
        // query, the worker thread's next call to push() will return false,
        // causing the worker thread to stop working on this query.
        queue_.remove(reply);
        cerr << "scope-C: \"" + query_ + "\" cancelled" << endl;
    }

    virtual void run(ReplyProxy const& reply) override
    {
        queue_.put(query_, reply);
        cerr << "scope-C: run() returning" << endl;
    }

private:
    string query_;
    Queue& queue_;
};

class MyScope : public ScopeBase
{
public:
    virtual int start(RegistryProxy const&) override
    {
        return VERSION;
    }

    virtual void stop() override
    {
        queue.finish();
    }

    virtual void run() override
    {
        for (;;)
        {
            string query;
            ReplyProxy reply;
            if (!queue.get(query, reply))
            {
                cerr << "worker thread terminating, queue was cleared" << endl;
                break;  // stop() was called.
            }
            for (int i = 0; i < 3; ++i)
            {
                cerr << "worker thread: pushing" << endl;
                if (!reply->push("scope-C: result " + to_string(i) + " for query \"" + query + "\""))
                {
                    cerr << "worker thread: push returned false" << endl;
                    break; // Query was cancelled
                }
                sleep(1);
            }
        }
    }

    virtual QueryBase::UPtr create_query(string const& q) override
    {
        cout << "scope-C: created query: \"" << q << "\"" << endl;
        return QueryBase::UPtr(new MyQuery("scope-C", q, queue));  // TODO: scope name should come from the run time
    }

private:
    Queue queue;
};

extern "C"
{

    EXPORT
    unity::api::scopes::ScopeBase*
    // cppcheck-suppress unusedFunction
    UNITY_API_SCOPE_CREATE_FUNCTION()
    {
        return new MyScope;
    }

    EXPORT
    void
    // cppcheck-suppress unusedFunction
    UNITY_API_SCOPE_DESTROY_FUNCTION(unity::api::scopes::ScopeBase* scope_base)
    {
        delete scope_base;
    }

}