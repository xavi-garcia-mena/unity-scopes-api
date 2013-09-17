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

#include <unity/api/scopes/internal/zmq_middleware/ZmqScope.h>

#include <unity/api/scopes/internal/ScopeImpl.h>
#include <unity/api/scopes/ScopeExceptions.h>

using namespace std;

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

namespace zmq_middleware
{

ZmqScope::ZmqScope(ZmqMiddleware* mw_base, string const& endpoint, string const& identity) :
    MWObjectProxy(mw_base),
    ZmqObjectProxy(mw_base, endpoint, identity, RequestType::Twoway),
    MWScope(mw_base)
{
}

ZmqScope::~ZmqScope() noexcept
{
}

QueryCtrlProxy ZmqScope::create_query(std::string const& q, VariantMap const& hints, MWReplyProxy const& reply)
{
    // TODO
    return QueryCtrlProxy();
}

} // namespace zmq_middleware

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity
