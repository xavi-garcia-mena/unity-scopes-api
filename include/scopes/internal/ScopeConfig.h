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

#ifndef UNITY_API_SCOPES_INTERNAL_SCOPECONFIG_H
#define UNITY_API_SCOPES_INTERNAL_SCOPECONFIG_H

#include <scopes/internal/ConfigBase.h>

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

class ScopeConfig : public ConfigBase
{
public:
    static constexpr const char* SCOPE_CONFIG_GROUP = "ScopeConfig";

    ScopeConfig(std::string const& configfile);
    ~ScopeConfig() noexcept;

    bool overrideable() const;

private:
    bool overrideable_;
};

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity

#endif