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
 * Authored by: Pawel Stolowski <pawel.stolowski@canonical.com>
*/

#include <unity/scopes/internal/Utils.h>
#include <unity/UnityExceptions.h>
#include <sstream>

namespace unity
{

namespace scopes
{

namespace internal
{

VariantMap::const_iterator find_or_throw(std::string const& context, VariantMap const& var, std::string const& key)
{
    auto it = var.find(key);
    if (it == var.end())
    {
        std::stringstream str;
        str << context << ": missing '"  << key << "' element";
        throw unity::InvalidArgumentException(str.str());
    }
    return it;
}

} // namespace internal

} // namespace scopes

} // namespace unity