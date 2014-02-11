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

#include <unity/scopes/ScopeBase.h>
#include <unity/scopes/SearchReply.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Category.h>
#include <unity/scopes/Query.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/Query.h>
#include <unity/scopes/Annotation.h>

#include <unity/scopes/internal/smartscopes/SmartScopesClient.h>

#include <iostream>

namespace unity
{

namespace scopes
{

namespace internal
{

namespace smartscopes
{

class SmartQuery : public SearchQuery
{
public:
    SmartQuery(std::string const& scope_id, SSRegistryObject::SPtr reg, Query const& query)
        : scope_id_(scope_id)
        , query_(query)
    {
        SmartScopesClient::SPtr ss_client = reg->get_ssclient();
        std::string base_url = reg->get_base_url(scope_id_);

        ///! TODO: session_id, query_id, platform, locale, country, limit
        search_handle_ = ss_client->search(base_url, query_.query_string(), "session_id", 0, "platform", "en", "US", 10);
    }

    ~SmartQuery() noexcept
    {
    }

    virtual void cancelled() override
    {
        search_handle_->cancel_search();
    }

    virtual void run(SearchReplyProxy const& reply) override
    {
        std::vector<SearchResult> results = search_handle_->get_search_results();
        std::map<std::string, Category::SCPtr> categories;

        for (auto& result : results)
        {
            Category::SCPtr cat;

            auto cat_it = categories.find(result.category->id);
            if (cat_it == end(categories))
            {
                CategoryRenderer rdr(result.category->renderer_template);
                cat = reply->register_category(result.category->id, result.category->title, result.category->icon, rdr);
                categories[result.category->id] = cat;
            }
            else
            {
                cat = cat_it->second;
            }

            CategorisedResult res(cat);
            res.set_uri(result.uri);
            res.set_title(result.title);
            res.set_art(result.art);
            res.set_dnd_uri(result.dnd_uri);

            res["result_json"] = result.json;

            auto other_params = result.other_params;
            for (auto& param : other_params)
            {
                res[param.first] = param.second->to_variant();
            }

            reply->push(res);
        }

        std::cout << "SmartScope: query for \"" << scope_id_ << "\": \"" << query_.query_string() << "\" complete" << std::endl;
    }

private:
    std::string scope_id_;
    Query query_;
    SearchHandle::UPtr search_handle_;
};

class SmartPreview : public PreviewQuery
{
public:
    SmartPreview(std::string const& scope_id, SSRegistryObject::SPtr reg, Result const& result)
        : scope_id_(scope_id)
        , result_(result)
    {
        SmartScopesClient::SPtr ss_client = reg->get_ssclient();
        std::string base_url = reg->get_base_url(scope_id_);

        ///! TODO: session_id, platform, widgets_api_version, locale, country
        preview_handle_ = ss_client->preview(base_url, result_["result_json"].get_string(), "session_id", "platform", 0, "en", "US");
    }

    ~SmartPreview()
    {
    }

    virtual void cancelled() override
    {
        preview_handle_->cancel_preview();
    }

    virtual void run(PreviewReplyProxy const& reply) override
    {
        auto results = preview_handle_->get_preview_results();
        PreviewHandle::Columns columns = results.first;
        PreviewHandle::Widgets widgets = results.second;

        // register layout
        if (columns.size() > 0)
        {
            ColumnLayoutList layout_list;

            for (auto& column : columns)
            {
                ColumnLayout layout(column.size());
                for (auto& widget_lo : column)
                {
                    layout.add_column(widget_lo);
                }

                layout_list.emplace_back(layout);
            }

            reply->register_layout(layout_list);
        }

        // push wigdets
        PreviewWidgetList widget_list;

        for (auto& widget_json : widgets)
        {
            widget_list.emplace_back(PreviewWidget(widget_json));
        }

        reply->push(widget_list);

        std::cout << "SmartScope: preview for \"" << scope_id_ << "\": \"" << result_.uri() << "\" complete" << std::endl;
    }

private:
    std::string scope_id_;
    Result result_;
    PreviewHandle::UPtr preview_handle_;
};

class SmartScope
{
public:
    SmartScope(SSRegistryObject::SPtr reg)
        : reg_(reg)
    {
    }

    QueryBase::UPtr create_query(std::string const& id, Query const& q, SearchMetadata const&)
    {
        QueryBase::UPtr query(new SmartQuery(id, reg_, q));
        std::cout << "SmartScope: created query for \"" << id << "\": \"" << q.query_string() << "\"" << std::endl;
        return query;
    }

    QueryBase::UPtr preview(std::string const& id, Result const& result, ActionMetadata const&)
    {
        QueryBase::UPtr preview(new SmartPreview(id, reg_, result));
        std::cout << "SmartScope: created preview for \"" << id << "\": \"" << result.uri() << "\"" << std::endl;
        return preview;
    }

    ActivationBase::UPtr activate(std::string const& id, Result const& result, ActionMetadata const& hints)
    {
        ///! TODO
        (void)id;
        (void)result;
        (void)hints;
        return ActivationBase::UPtr();
    }

    ActivationBase::UPtr perform_action(std::string const& id, Result const& result, ActionMetadata const& hints, std::string const& widget_id, std::string const& action_id)
    {
        ///! TODO
        (void)id;
        (void)result;
        (void)hints;
        (void)widget_id;
        (void)action_id;
        return ActivationBase::UPtr();
    }

private:
    SSRegistryObject::SPtr reg_;
};

}  // namespace smartscopes

}  // namespace internal

}  // namespace scopes

}  // namespace unity
