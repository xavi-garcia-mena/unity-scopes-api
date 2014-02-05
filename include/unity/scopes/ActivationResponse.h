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
 * Authored by: Pawel Stolowski <pawel.stolowski@canonical.com>
 */

#ifndef UNITY_SCOPES_ACTIVATIONRESPONSE_H
#define UNITY_SCOPES_ACTIVATIONRESPONSE_H

#include <unity/SymbolExport.h>
#include <unity/scopes/Variant.h>
#include <unity/scopes/Query.h>
#include <memory>

namespace unity
{

namespace scopes
{

namespace internal
{
class ActivationResponseImpl;
}

/**
\brief Carries response to a Result activation request.
*/
class UNITY_API ActivationResponse final
{
public:
    enum Status
    {
        NotHandled, //<! Activation of this result wasn't handled by the scope
        ShowDash,   //<! Activation of this result was handled, show the Dash
        HideDash,   //<! Activation of this result was handled, hide the Dash
        ShowPreview,//<! Preview should be requested for this result.
        Search      //<! Perform new search. This state is implied if creating ActivationResponse with Query object and is invalid otherwise
    };

    /**
    \brief Creates ActivationResponse with given status.
    Throws unity::InvalidArgumentException if status is Status::Search - to
    create ActivationResponse of that type, use ActivationResponse(Query const&)
    constructor.
    \param status activation status
    */
    ActivationResponse(Status status);

    /**
    \brief Creates ActivationResponse with activation status of Status::Search and a search query to be executed.
    \param query search query to be execute by client
     */
    ActivationResponse(Query const& query);

    /// @cond
    ~ActivationResponse();
    ActivationResponse(ActivationResponse const& other);
    ActivationResponse(ActivationResponse&& other);
    ActivationResponse& operator=(ActivationResponse const& other);
    ActivationResponse& operator=(ActivationResponse&& other);
    /// @endcond

    /**
    \brief Get activation status.
    \return activation status
    */
    ActivationResponse::Status status() const;

    /**
     \brief Attach arbitrary hints to this response.
     */
    void setHints(VariantMap const& hints);

    /**
     \brief Get hints attached to this response object.
     \return hints
     */
    VariantMap hints() const;

    Query query() const;

    /// @cond
    VariantMap serialize() const;
    /// @endcond

private:
    std::shared_ptr<internal::ActivationResponseImpl> p;
    ActivationResponse(std::shared_ptr<internal::ActivationResponseImpl> pimpl);
    friend class internal::ActivationResponseImpl;
};

} // namespace scopes

} // namespace unity

#endif
