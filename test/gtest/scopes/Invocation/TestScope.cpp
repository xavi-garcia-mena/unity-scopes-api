/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include "TestScope.h"

#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/ScopeBase.h>
#include <unity/scopes/SearchReply.h>

#include <mutex>
#include <thread>

using namespace std;
using namespace unity::scopes;

namespace
{

class TestQuery : public SearchQueryBase
{
public:
    TestQuery(CannedQuery const& query, SearchMetadata const& metadata)
        : SearchQueryBase(query, metadata)
    {
    }

    virtual void cancelled() override
    {
    }

    virtual void run(SearchReplyProxy const& reply) override
    {
        auto cat = reply->register_category("cat1", "Category 1", "");
        CategorisedResult res(cat);
        res.set_uri("uri");
        res.set_title("title");
        res.set_art("art");
        res.set_dnd_uri("dnd_uri");
        reply->push(res);
    }
};

}  // namespace

namespace
{

mutex m;
int call_count = 0;

}  // namespace

SearchQueryBase::UPtr TestScope::search(CannedQuery const& query, SearchMetadata const& metadata)
{
    lock_guard<mutex> lock(m);

    if (call_count++ == 0)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));  // Force timeout on first call
    }
    return SearchQueryBase::UPtr(new TestQuery(query, metadata));
}

PreviewQueryBase::UPtr TestScope::preview(Result const&, ActionMetadata const&)
{
    return nullptr;  // unused
}
