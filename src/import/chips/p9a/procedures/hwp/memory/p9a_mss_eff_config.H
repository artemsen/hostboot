/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9a/procedures/hwp/memory/p9a_mss_eff_config.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2018                             */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

///
/// @file p9a_mss_eff_config.H
/// @brief Command and Control for the memory subsystem - populate attributes
///
// *HWP HWP Owner: Louis Stermole <stermole@us.ibm.com>
// *HWP HWP Backup: Andre A. Marin <aamarin@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 1
// *HWP Consumed by: FSP:HB

#ifndef P9A_MSS_EFF_CONFIG_H_
#define P9A_MSS_EFF_CONFIG_H_

#include <fapi2.H>
#include <vector>

typedef fapi2::ReturnCode (*p9a_mss_eff_config_FP_t) (const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>&);

extern "C"
{

    ///
    /// @brief Configure the attributes for each controller
    /// @param[in] i_target port target
    /// @return FAPI2_RC_SUCCESS iff ok
    ///
    fapi2::ReturnCode p9a_mss_eff_config( const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_target );

}

#endif
