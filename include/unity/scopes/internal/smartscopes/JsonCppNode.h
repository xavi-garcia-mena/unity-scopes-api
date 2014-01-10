/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Authored by: Marcus Tomlinson <marcus.tomlinson@canonical.com>
 */

#ifndef UNITY_SCOPES_INTERNAL_SMARTSCOPES_JSONCPPNODE_H
#define UNITY_SCOPES_INTERNAL_SMARTSCOPES_JSONCPPNODE_H

#include <unity/scopes/internal/smartscopes/JsonNodeInterface.h>

#include <jsoncpp/json/reader.h>

namespace unity
{

namespace scopes
{

namespace internal
{

namespace smartscopes
{

class JsonCppNode : public JsonNodeInterface
{
public:
    explicit JsonCppNode(std::string const& json_string = "");
    explicit JsonCppNode(const Json::Value& root);
    ~JsonCppNode();

    void clear() override;
    void read_json(std::string const& json_string) override;

    int size() const override;
    NodeType type() const override;

    std::string as_string() const override;
    int as_int() const override;
    uint as_uint() const override;
    double as_double() const override;
    bool as_bool() const override;

    bool has_node(std::string const& node_name) const override;

    JsonNodeInterface::SPtr get_node() const override;
    JsonNodeInterface::SPtr get_node(std::string const& node_name) const override;
    JsonNodeInterface::SPtr get_node(uint node_index) const override;

private:
    Json::Value root_;
};

} // namespace smartscopes

} // namespace internal

} // namespace scopes

} // namespace unity

#endif // UNITY_SCOPES_INTERNAL_SMARTSCOPES_JSONCPPNODE_H