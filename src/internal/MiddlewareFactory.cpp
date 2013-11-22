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

#include <scopes/internal/MiddlewareFactory.h>

// #include <scopes/internal/ice_middleware/IceMiddleware.h>
#include <scopes/internal/MiddlewareFactoryConfig.h>
#include <scopes/internal/zmq_middleware/ZmqMiddleware.h>
#include <scopes/ScopeExceptions.h>

#include <cassert>

using namespace std;
using namespace unity::api::scopes;

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

MiddlewareFactory::MiddlewareFactory(string const& configfile, RuntimeImpl* runtime) :
    runtime_(runtime)
{
    assert(runtime);

    try
    {
        MiddlewareFactoryConfig config(configfile);
    }
    catch (unity::Exception const& e)
    {
        throw ConfigException("cannot instantiate MiddlewareFactory: config file: " + configfile);
    }
}

MiddlewareFactory::~MiddlewareFactory() noexcept
{
}

MiddlewareBase::SPtr MiddlewareFactory::create(string const& server_name,
                                               string const& kind,
                                               string const& configfile) const
{
    lock_guard<decltype(mutex_)> lock(mutex_);

    MiddlewareBase::SPtr mw = find_unlocked(server_name, kind);

    if (mw)
    {
        return mw;
    }

    switch (to_kind(kind))
    {
        case Kind_Zmq:
        {
            mw = make_shared<zmq_middleware::ZmqMiddleware>(server_name, configfile, runtime_);
            break;
        }
        case Kind_REST:
        {
            // TODO
            abort();
            break;
        }
        default:
        {
            assert(false);  // LCOV_EXCL_LINE
        }
    }

    MiddlewareData d = { server_name, kind };
    mw_map_[d] = mw;

    return mw;
}

MiddlewareBase::SPtr MiddlewareFactory::find(string const& server_name, string const& kind) const
{
    lock_guard<decltype(mutex_)> lock(mutex_);
    return find_unlocked(server_name, kind);
}

MiddlewareBase::SPtr MiddlewareFactory::find_unlocked(string const& server_name, string const& kind) const
{
    MiddlewareData d = { server_name, kind };
    auto it = mw_map_.find(d);
    return it != mw_map_.end() ? it->second : nullptr;
}

MiddlewareFactory::Kind MiddlewareFactory::to_kind(string const& kind)
{
    Kind k;
    if (kind == "Zmq")
    {
        k = Kind_Zmq;
    }
    else if (kind == "REST")
    {
        k = Kind_REST;
    }
    else
    {
        throw ConfigException("Invalid middleware kind: " + kind);
    }
    return k;
}

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity