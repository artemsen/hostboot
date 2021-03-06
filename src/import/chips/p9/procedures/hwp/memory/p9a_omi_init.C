/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/memory/p9a_omi_init.C $    */
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
/// @file p9a_omi_init.C
/// @brief Finalize the OMI
///
// *HWP HWP Owner: Benjamin Gass <bgass@us.ibm.com>
// *HWP HWP Backup: Daniel Crowell <dcrowell@us.ibm.com>
// *HWP Team:
// *HWP Level: 2
// *HWP Consumed by: HB

#include <p9a_omi_init.H>
#include <p9a_omi_init_scom.H>
#include <p9a_mc_scom_addresses.H>
#include <p9a_mc_scom_addresses_fld.H>

///
/// @brief Run initfile to enable templates and set pacing.
///
/// @param[in] i_target                 p9a channel to work on
///
/// @return fapi2:ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode p9a_omi_init_scominit(const fapi2::Target<fapi2::TARGET_TYPE_MCC>& i_target)
{
    fapi2::ReturnCode l_rc;
    const fapi2::Target<fapi2::TARGET_TYPE_SYSTEM> FAPI_SYSTEM;

    FAPI_EXEC_HWP(l_rc, p9a_omi_init_scom, i_target, FAPI_SYSTEM);

    if (l_rc)
    {
        FAPI_ERR("Error from p9a.omi_init.scom.initfile");
        fapi2::current_err = l_rc;
    }

    FAPI_DBG("Exiting with return code : %08X...", (uint64_t) fapi2::current_err);
    return fapi2::current_err;
}

///
/// @brief Check and enable supported templates
///
/// @param[in] i_target                 p9a channel to work on
///
/// @return fapi2:ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode p9a_omi_init_enable_templates(const fapi2::Target<fapi2::TARGET_TYPE_MCC>& i_target)
{
    fapi2::ATTR_PROC_ENABLE_DL_TMPL_1_Type l_enable_tmpl_1;
    fapi2::ATTR_PROC_ENABLE_DL_TMPL_4_Type l_enable_tmpl_4;
    fapi2::ATTR_PROC_ENABLE_DL_TMPL_7_Type l_enable_tmpl_7;
    fapi2::buffer<uint64_t> l_data;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_ENABLE_DL_TMPL_1,
                           i_target,
                           l_enable_tmpl_1),
             "Error from FAPI_ATTR_GET (ATTR_PROC_ENABLE_DL_TMPL_1)");

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_ENABLE_DL_TMPL_4,
                           i_target,
                           l_enable_tmpl_4),
             "Error from FAPI_ATTR_GET (ATTR_PROC_ENABLE_DL_TMPL_4)");

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_ENABLE_DL_TMPL_7,
                           i_target,
                           l_enable_tmpl_7),
             "Error from FAPI_ATTR_GET (ATTR_PROC_ENABLE_DL_TMPL_7)");

    FAPI_ASSERT(l_enable_tmpl_1 != 0,
                fapi2::PROC_DOWNSTREAM_TMPL1_REQUIRED_ERR()
                .set_TARGET(i_target),
                "Downstream template 1 is required.");

    FAPI_ASSERT(l_enable_tmpl_4 != 0 || l_enable_tmpl_7 != 0,
                fapi2::PROC_DOWNSTREAM_TMPL4OR7_REQUIRED_ERR()
                .set_TARGET(i_target),
                "Downstream template 4 and/or 7 is required.");

    //Turn off temp0_only
    FAPI_TRY(getScom(i_target, P9A_MCC_DSTLCFG, l_data));
    l_data.clearBit<P9A_MCC_DSTLCFG_TMPL0_ONLY>();
    FAPI_TRY(putScom(i_target, P9A_MCC_DSTLCFG, l_data));

fapi_try_exit:

    FAPI_DBG("Exiting with return code : %08X...", (uint64_t) fapi2::current_err);
    return fapi2::current_err;
}

///
/// @brief Enable ibm buffer chip low latency mode
///
/// @param[in] i_target                 p9a channel to work on
///
/// @return fapi2:ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode p9a_omi_init_enable_lol(const fapi2::Target<fapi2::TARGET_TYPE_MCC>& i_target)
{
    fapi2::buffer<uint64_t> l_data;
    std::vector<fapi2::Target<fapi2::TARGET_TYPE_OMI>> l_omi_targets;
    fapi2::ATTR_CHIP_UNIT_POS_Type l_omi_pos;


    l_omi_targets = i_target.getChildren<fapi2::TARGET_TYPE_OMI>();

    FAPI_TRY(getScom(i_target, P9A_MCC_USTLCFG, l_data));

    for (const auto l_omi_target : l_omi_targets)
    {
        FAPI_TRY(FAPI_ATTR_GET( fapi2::ATTR_CHIP_UNIT_POS,
                                l_omi_target,
                                l_omi_pos));

        if ((l_omi_pos % 2) == 0)
        {
            l_data.setBit<P9A_MCC_USTLCFG_IBM_BUFFER_CHIP_CHANA_ENABLE>();
        }
        else
        {
            l_data.setBit<P9A_MCC_USTLCFG_IBM_BUFFER_CHIP_CHANB_ENABLE>();
        }
    }

    FAPI_TRY(putScom(i_target, P9A_MCC_USTLCFG, l_data));

fapi_try_exit:

    FAPI_DBG("Exiting with return code : %08X...", (uint64_t) fapi2::current_err);
    return fapi2::current_err;
}

///
/// @brief Finalize the OMI
///
/// @param[in] i_target                 p9a channel to work on
///
/// @return fapi2:ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode p9a_omi_init(const fapi2::Target<fapi2::TARGET_TYPE_MCC>& i_target)
{
    FAPI_TRY(p9a_omi_init_scominit(i_target));
    FAPI_TRY(p9a_omi_init_enable_templates(i_target));
    FAPI_TRY(p9a_omi_init_enable_lol(i_target));

fapi_try_exit:

    FAPI_DBG("Exiting with return code : %08X...", (uint64_t) fapi2::current_err);
    return fapi2::current_err;
}
