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
#include <gtest/gtest.h>

using namespace unity::scopes;
using namespace unity::scopes::internal;

TEST(Utils, uncamelcase)
{
    EXPECT_EQ("", uncamelcase(""));
    EXPECT_EQ("foo-bar", uncamelcase("FooBar"));
    EXPECT_EQ("foo-bar", uncamelcase("foo-bar"));
    EXPECT_EQ("foo_bar", uncamelcase("foo_bar"));
    EXPECT_EQ("foo-bar", uncamelcase("fooBAR"));
    EXPECT_EQ("foo-bar", uncamelcase("fooBAr"));
    EXPECT_EQ("foo-bar", uncamelcase("foo-Bar"));
}
