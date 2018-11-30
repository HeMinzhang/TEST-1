/**
 *  \file provider_status.h
 *  \author Nicola Ferri
 *  \brief Provider status enumeration
 */

#pragma once

namespace selink { namespace provider {

    enum status : int {
        logged_in = 0,  ///< Logged in successfully
        wait_config = 1,  ///< Wait for configuration
        error_notfound = 100,  ///< Device not found: the specified device is not connected
        error_userpin = 200,  ///< The provided user pin is wrong
        error_device = 201,  ///< An error status was received from the device
        error_unknown = 302  ///< Unknown Error
    };

} }  // namespace selink::provider
