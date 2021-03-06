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

#pragma once

#include <unity/scopes/ScopeBase.h>

class AggTestScope : public unity::scopes::ScopeBase
{
public:
    AggTestScope();

    virtual void start(std::string const&) override;

    virtual void stop() override;

    virtual void run() override;

    virtual unity::scopes::SearchQueryBase::UPtr search(unity::scopes::CannedQuery const &,
                                                        unity::scopes::SearchMetadata const &) override;

    virtual unity::scopes::PreviewQueryBase::UPtr preview(unity::scopes::Result const&,
                                                          unity::scopes::ActionMetadata const &) override;

    virtual unity::scopes::ChildScopeList find_child_scopes() const override;

private:
    std::string id_;
    mutable std::mutex mutex_;
    unity::scopes::SearchMetadata metadata_;

    std::set<std::string> get_keywords(std::string const& child_id) const;
};
