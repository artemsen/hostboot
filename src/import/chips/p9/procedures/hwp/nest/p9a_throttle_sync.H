/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/nest/p9a_throttle_sync.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2019                             */
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
/// ----------------------------------------------------------------------------
/// @file  p9a_throttle_sync.H
///
/// @brief p9a_throttle_sync HWP
///
/// The purpose of this procedure is to triggers sync command from a 'master'
/// MC to other MCs that have attached memory in a processor.
///
/// ----------------------------------------------------------------------------
/// *HWP HWP Owner   : Joe McGill <jmcgill@us.ibm.com>
/// *HWP HWP Backup  : Mark Pizzutillo <Mark.Pizzutillo@ibm.com>
/// *HWP FW Owner    : Thi Tran <thi@us.ibm.com>
/// *HWP Team        : Nest
/// *HWP Level       : 3
/// *HWP Consumed by : HB
/// ----------------------------------------------------------------------------
#ifndef _P9A_THROTTLE_SYNC_H_
#define _P9A_THROTTLE_SYNC_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <fapi2.H>
#include <p9a_mc_scom_addresses_fld.H>
#include <p9a_misc_scom_addresses_fld.H>
#include <p9a_misc_scom_addresses.H>
#include <explorer_scom_addresses.H>
#include <explorer_scom_addresses_fld.H>

// Function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*p9a_throttle_sync_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>&);

///
/// @brief Structure that holds the potential master MI for a MC side (MC01/MC23)
///
struct mc_side_info_t
{
    bool master_mc_found = false;
    fapi2::Target<fapi2::TARGET_TYPE_MI> master_mc;   // Master MC for this MC side
};

extern "C"
{
    ///
    /// @brief p9a_throttle_sync procedure
    ///
    /// @param[in] i_target TARGET_TYPE_PROC_CHIP target
    /// @return FAPI2_RC_SUCCESS if success, else error code.
    ///
    fapi2::ReturnCode p9a_throttle_sync(const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target);

} // extern "C"

#endif  // _P9A_THROTTLE_SYNC_H_
