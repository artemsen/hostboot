/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/ocmb/explorer/procedures/hwp/memory/lib/utils/explorer_pos.C $ */
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

///
/// @file explorer_pos.C
/// @brief Tools to return target's position from a fapi2 target
///
// *HWP HWP Owner: Andre Marin <aamarin@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: HB:FSP

#include <generic/memory/lib/utils/pos.H>

namespace mss
{

///
/// @brief Return a DIMM's relative position from a port
/// @param[in] i_target a target representing the target in question
/// @return The position relative to chiplet R
///
template<>
posTraits<fapi2::TARGET_TYPE_DIMM>::pos_type
relative_pos<fapi2::TARGET_TYPE_MEM_PORT>(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target)
{
    typedef mcTypeTraits<mc_type::EXPLORER> TT;
    return pos(i_target) % TT::DIMMS_PER_PORT;
}

///
/// @brief Return a DIMM's relative position from an OCMB
/// @param[in] i_target a target representing the target in question
/// @return The position relative to chiplet R
///
template<>
posTraits<fapi2::TARGET_TYPE_DIMM>::pos_type
relative_pos<fapi2::TARGET_TYPE_OCMB_CHIP>(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target)
{
    typedef mcTypeTraits<mc_type::EXPLORER> TT;
    return pos(i_target) % (TT::DIMMS_PER_PORT * TT::PORTS_PER_OCMB);
}

///
/// @brief Return an MEM_PORT's relative position from an OCMB
/// @param[in] i_target a target representing the target in question
/// @return The position relative to chiplet R
///
template<>
posTraits<fapi2::TARGET_TYPE_MEM_PORT>::pos_type
relative_pos<fapi2::TARGET_TYPE_OCMB_CHIP>(const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_target)
{
    typedef mcTypeTraits<mc_type::EXPLORER> TT;
    return pos(i_target) % TT::PORTS_PER_OCMB;
}

///
/// @brief Return an MEM_PORT's relative position from itself
/// @param[in] i_target a target representing the target in question
/// @return The position relative to chiplet R
///
template<>
posTraits<fapi2::TARGET_TYPE_MEM_PORT>::pos_type
relative_pos<fapi2::TARGET_TYPE_MEM_PORT>(const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_target)
{
    return 0;
}

}// mss
