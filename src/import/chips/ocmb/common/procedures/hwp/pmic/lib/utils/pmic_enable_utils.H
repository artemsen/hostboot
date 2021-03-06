/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/ocmb/common/procedures/hwp/pmic/lib/utils/pmic_enable_utils.H $ */
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
/// @file pmic_enable_utils.H
/// @brief Utility functions for PMIC enable operation
///
// *HWP HWP Owner: Mark Pizzutillo <mark.pizzutillo@ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

#ifndef __PMIC_ENABLE_UTILS_H__
#define __PMIC_ENABLE_UTILS_H__

#include <fapi2.H>
#include <pmic_regs.H>
#include <pmic_regs_fld.H>
#include <lib/i2c/i2c_pmic.H>
#include <lib/utils/pmic_common_utils.H>
#include <lib/utils/pmic_consts.H>
#include <generic/memory/lib/utils/c_str.H>
#include <generic/memory/lib/utils/index.H>
#include <mss_pmic_attribute_getters.H>
#include <mss_pmic_attribute_setters.H>

namespace mss
{
namespace pmic
{
/// @brief pointer to PMIC attribute getters for DIMM target
typedef fapi2::ReturnCode (*pmic_attr_ptr)(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target, uint8_t& o_value);
typedef fapi2::ReturnCode (*pmic_attr_ptr_signed)(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
        int8_t& o_value);
// Pointers below allow for run-time attribute getter selection by PMIC ID (0,1)

// PMIC0/1 sequence order
static constexpr pmic_attr_ptr get_sequence[] =
{
    mss::attr::get_pmic0_sequence,
    mss::attr::get_pmic1_sequence
};

// Voltage Setting
static constexpr pmic_attr_ptr get_swa_voltage_setting[] =
{
    mss::attr::get_pmic0_swa_voltage_setting,
    mss::attr::get_pmic1_swa_voltage_setting
};
static constexpr pmic_attr_ptr get_swb_voltage_setting[] =
{
    mss::attr::get_pmic0_swb_voltage_setting,
    mss::attr::get_pmic1_swb_voltage_setting
};
static constexpr pmic_attr_ptr get_swc_voltage_setting[] =
{
    mss::attr::get_pmic0_swc_voltage_setting,
    mss::attr::get_pmic1_swc_voltage_setting
};
static constexpr pmic_attr_ptr get_swd_voltage_setting[] =
{
    mss::attr::get_pmic0_swd_voltage_setting,
    mss::attr::get_pmic1_swd_voltage_setting
};

// Voltage Range Select
static constexpr pmic_attr_ptr get_swa_voltage_range_select[] =
{
    mss::attr::get_pmic0_swa_voltage_range_select,
    mss::attr::get_pmic1_swa_voltage_range_select
};
static constexpr pmic_attr_ptr get_swb_voltage_range_select[] =
{
    mss::attr::get_pmic0_swb_voltage_range_select,
    mss::attr::get_pmic1_swb_voltage_range_select
};
static constexpr pmic_attr_ptr get_swc_voltage_range_select[] =
{
    mss::attr::get_pmic0_swc_voltage_range_select,
    mss::attr::get_pmic1_swc_voltage_range_select
};
static constexpr pmic_attr_ptr get_swd_voltage_range_select[] =
{
    mss::attr::get_pmic0_swd_voltage_range_select,
    mss::attr::get_pmic1_swd_voltage_range_select
};

// Voltage Offset
static constexpr pmic_attr_ptr_signed get_swa_voltage_offset[] =
{
    mss::attr::get_pmic0_swa_voltage_offset,
    mss::attr::get_pmic1_swa_voltage_offset
};
static constexpr pmic_attr_ptr_signed get_swb_voltage_offset[] =
{
    mss::attr::get_pmic0_swb_voltage_offset,
    mss::attr::get_pmic1_swb_voltage_offset
};
static constexpr pmic_attr_ptr_signed get_swc_voltage_offset[] =
{
    mss::attr::get_pmic0_swc_voltage_offset,
    mss::attr::get_pmic1_swc_voltage_offset
};
static constexpr pmic_attr_ptr_signed get_swd_voltage_offset[] =
{
    mss::attr::get_pmic0_swd_voltage_offset,
    mss::attr::get_pmic1_swd_voltage_offset
};

// Sequence Delay
static constexpr pmic_attr_ptr get_swa_sequence_delay[] =
{
    mss::attr::get_pmic0_swa_sequence_delay,
    mss::attr::get_pmic1_swa_sequence_delay
};
static constexpr pmic_attr_ptr get_swb_sequence_delay[] =
{
    mss::attr::get_pmic0_swb_sequence_delay,
    mss::attr::get_pmic1_swb_sequence_delay
};
static constexpr pmic_attr_ptr get_swc_sequence_delay[] =
{
    mss::attr::get_pmic0_swc_sequence_delay,
    mss::attr::get_pmic1_swc_sequence_delay
};
static constexpr pmic_attr_ptr get_swd_sequence_delay[] =
{
    mss::attr::get_pmic0_swd_sequence_delay,
    mss::attr::get_pmic1_swd_sequence_delay
};

// Sequence Order
static constexpr pmic_attr_ptr get_swa_sequence_order[] =
{
    mss::attr::get_pmic0_swa_sequence_order,
    mss::attr::get_pmic1_swa_sequence_order
};
static constexpr pmic_attr_ptr get_swb_sequence_order[] =
{
    mss::attr::get_pmic0_swb_sequence_order,
    mss::attr::get_pmic1_swb_sequence_order
};
static constexpr pmic_attr_ptr get_swc_sequence_order[] =
{
    mss::attr::get_pmic0_swc_sequence_order,
    mss::attr::get_pmic1_swc_sequence_order
};
static constexpr pmic_attr_ptr get_swd_sequence_order[] =
{
    mss::attr::get_pmic0_swd_sequence_order,
    mss::attr::get_pmic1_swd_sequence_order
};

// Phase Combination
static constexpr pmic_attr_ptr get_phase_comb[] =
{
    mss::attr::get_pmic0_phase_comb,
    mss::attr::get_pmic1_phase_comb
};

// EFD Fields

// Offset
static constexpr pmic_attr_ptr_signed get_efd_swa_voltage_offset[] =
{
    mss::attr::get_efd_pmic0_swa_voltage_offset,
    mss::attr::get_efd_pmic1_swa_voltage_offset
};

static constexpr pmic_attr_ptr_signed get_efd_swb_voltage_offset[] =
{
    mss::attr::get_efd_pmic0_swb_voltage_offset,
    mss::attr::get_efd_pmic1_swb_voltage_offset
};

static constexpr pmic_attr_ptr_signed get_efd_swc_voltage_offset[] =
{
    mss::attr::get_efd_pmic0_swc_voltage_offset,
    mss::attr::get_efd_pmic1_swc_voltage_offset
};

static constexpr pmic_attr_ptr_signed get_efd_swd_voltage_offset[] =
{
    mss::attr::get_efd_pmic0_swd_voltage_offset,
    mss::attr::get_efd_pmic1_swd_voltage_offset
};

// These arrays allow us to dynamically choose the right attribute getter at runtime based on the rail and mss::pmic::id
static const pmic_attr_ptr* get_volt_setting[] =
{
    mss::pmic::get_swa_voltage_setting,
    mss::pmic::get_swb_voltage_setting,
    mss::pmic::get_swc_voltage_setting,
    mss::pmic::get_swd_voltage_setting
};

static const pmic_attr_ptr* get_volt_range_select[] =
{
    mss::pmic::get_swa_voltage_range_select,
    mss::pmic::get_swb_voltage_range_select,
    mss::pmic::get_swc_voltage_range_select,
    mss::pmic::get_swd_voltage_range_select
};

static const pmic_attr_ptr_signed* get_volt_offset[] =
{
    mss::pmic::get_swa_voltage_offset,
    mss::pmic::get_swb_voltage_offset,
    mss::pmic::get_swc_voltage_offset,
    mss::pmic::get_swd_voltage_offset
};

// EFD Offset + Direction functions
static const pmic_attr_ptr_signed* get_efd_volt_offset[] =
{
    mss::pmic::get_efd_swa_voltage_offset,
    mss::pmic::get_efd_swb_voltage_offset,
    mss::pmic::get_efd_swc_voltage_offset,
    mss::pmic::get_efd_swd_voltage_offset
};

// For output traces
static const std::vector<const char*> PMIC_RAIL_NAMES = {"SWA", "SWB", "SWC", "SWD"};

// Attribute setter FP type
typedef fapi2::ReturnCode (*pmic_attr_setter_ptr)(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
        uint8_t i_value);
typedef fapi2::ReturnCode (*pmic_attr_setter_ptr_signed)(const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_target,
        int8_t i_value);

// Voltage Setting
static constexpr pmic_attr_setter_ptr set_swa_voltage_setting[] =
{
    mss::attr::set_pmic0_swa_voltage_setting,
    mss::attr::set_pmic1_swa_voltage_setting
};
static constexpr pmic_attr_setter_ptr set_swb_voltage_setting[] =
{
    mss::attr::set_pmic0_swb_voltage_setting,
    mss::attr::set_pmic1_swb_voltage_setting
};
static constexpr pmic_attr_setter_ptr set_swc_voltage_setting[] =
{
    mss::attr::set_pmic0_swc_voltage_setting,
    mss::attr::set_pmic1_swc_voltage_setting
};
static constexpr pmic_attr_setter_ptr set_swd_voltage_setting[] =
{
    mss::attr::set_pmic0_swd_voltage_setting,
    mss::attr::set_pmic1_swd_voltage_setting
};

// Voltage Range Select
static constexpr pmic_attr_setter_ptr set_swa_voltage_range_select[] =
{
    mss::attr::set_pmic0_swa_voltage_range_select,
    mss::attr::set_pmic1_swa_voltage_range_select
};
static constexpr pmic_attr_setter_ptr set_swb_voltage_range_select[] =
{
    mss::attr::set_pmic0_swb_voltage_range_select,
    mss::attr::set_pmic1_swb_voltage_range_select
};
static constexpr pmic_attr_setter_ptr set_swc_voltage_range_select[] =
{
    mss::attr::set_pmic0_swc_voltage_range_select,
    mss::attr::set_pmic1_swc_voltage_range_select
};
static constexpr pmic_attr_setter_ptr set_swd_voltage_range_select[] =
{
    mss::attr::set_pmic0_swd_voltage_range_select,
    mss::attr::set_pmic1_swd_voltage_range_select
};

// Voltage Offset
static constexpr pmic_attr_setter_ptr_signed set_swa_voltage_offset[] =
{
    mss::attr::set_pmic0_swa_voltage_offset,
    mss::attr::set_pmic1_swa_voltage_offset
};
static constexpr pmic_attr_setter_ptr_signed set_swb_voltage_offset[] =
{
    mss::attr::set_pmic0_swb_voltage_offset,
    mss::attr::set_pmic1_swb_voltage_offset
};
static constexpr pmic_attr_setter_ptr_signed set_swc_voltage_offset[] =
{
    mss::attr::set_pmic0_swc_voltage_offset,
    mss::attr::set_pmic1_swc_voltage_offset
};
static constexpr pmic_attr_setter_ptr_signed set_swd_voltage_offset[] =
{
    mss::attr::set_pmic0_swd_voltage_offset,
    mss::attr::set_pmic1_swd_voltage_offset
};

// Sequence Delay
static constexpr pmic_attr_setter_ptr set_swa_sequence_delay[] =
{
    mss::attr::set_pmic0_swa_sequence_delay,
    mss::attr::set_pmic1_swa_sequence_delay
};
static constexpr pmic_attr_setter_ptr set_swb_sequence_delay[] =
{
    mss::attr::set_pmic0_swb_sequence_delay,
    mss::attr::set_pmic1_swb_sequence_delay
};
static constexpr pmic_attr_setter_ptr set_swc_sequence_delay[] =
{
    mss::attr::set_pmic0_swc_sequence_delay,
    mss::attr::set_pmic1_swc_sequence_delay
};
static constexpr pmic_attr_setter_ptr set_swd_sequence_delay[] =
{
    mss::attr::set_pmic0_swd_sequence_delay,
    mss::attr::set_pmic1_swd_sequence_delay
};

// Sequence Order
static constexpr pmic_attr_setter_ptr set_swa_sequence_order[] =
{
    mss::attr::set_pmic0_swa_sequence_order,
    mss::attr::set_pmic1_swa_sequence_order
};
static constexpr pmic_attr_setter_ptr set_swb_sequence_order[] =
{
    mss::attr::set_pmic0_swb_sequence_order,
    mss::attr::set_pmic1_swb_sequence_order
};
static constexpr pmic_attr_setter_ptr set_swc_sequence_order[] =
{
    mss::attr::set_pmic0_swc_sequence_order,
    mss::attr::set_pmic1_swc_sequence_order
};
static constexpr pmic_attr_setter_ptr set_swd_sequence_order[] =
{
    mss::attr::set_pmic0_swd_sequence_order,
    mss::attr::set_pmic1_swd_sequence_order
};

// Phase Combination
static constexpr pmic_attr_setter_ptr set_phase_comb[] =
{
    mss::attr::set_pmic0_phase_comb,
    mss::attr::set_pmic1_phase_comb
};

// Offset
static constexpr pmic_attr_setter_ptr_signed set_efd_swa_voltage_offset[] =
{
    mss::attr::set_efd_pmic0_swa_voltage_offset,
    mss::attr::set_efd_pmic1_swa_voltage_offset
};

static constexpr pmic_attr_setter_ptr_signed set_efd_swb_voltage_offset[] =
{
    mss::attr::set_efd_pmic0_swb_voltage_offset,
    mss::attr::set_efd_pmic1_swb_voltage_offset
};

static constexpr pmic_attr_setter_ptr_signed set_efd_swc_voltage_offset[] =
{
    mss::attr::set_efd_pmic0_swc_voltage_offset,
    mss::attr::set_efd_pmic1_swc_voltage_offset
};

static constexpr pmic_attr_setter_ptr_signed set_efd_swd_voltage_offset[] =
{
    mss::attr::set_efd_pmic0_swd_voltage_offset,
    mss::attr::set_efd_pmic1_swd_voltage_offset
};

//-----------------------------------
// SPD Biasing functions
//-----------------------------------

///
/// @breif set VR enable bit for system startup via R32 (not broadcast)
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode start_vr_enable(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target);

///
/// @brief bias PMIC0 with spd settings for phase combination (SWA, SWB or SWA+SWB)
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_port - port target of PMIC
/// @param[in] i_dimm_index - DIMM index for PMIC (0,1)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no error
///
fapi2::ReturnCode bias_with_spd_phase_comb(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm,
    const mss::pmic::id i_id);

///
/// @brief bias with SPD settings for voltage ranges
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_port port target of PMIC
/// @param[in] i_dimm_index DIMM index for PMIC (0,1)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no error
///
fapi2::ReturnCode bias_with_spd_volt_ranges(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm,
    const mss::pmic::id i_id);

///
/// @brief bias with SPD settings for startup sequence
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_port port target of PMIC
/// @param[in] i_dimm_index DIMM index for PMIC (0,1)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no error
///
fapi2::ReturnCode bias_with_spd_startup_seq(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm,
    const mss::pmic::id i_id);

///
/// @brief Set the startup seq register with the given parameters
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to
/// @param[in] i_round sequence round 1-4
/// @param[in] i_delay delay after round
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no error
///
fapi2::ReturnCode set_startup_seq_register(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const uint8_t i_rail,
    const uint8_t i_round,
    const uint8_t i_delay);

//-----------------------------------
// Templated SPD Biasing functions
//-----------------------------------

///
/// @brief bias with spd settings for voltages
///
/// @tparam V mss::pmic::vendor (TI/IDT)
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_dimm_target DIMM target of PMIC
/// @param[in] i_dimm_index - DIMM index for PMIC (0,1)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no error
///
template <mss::pmic::vendor V>
fapi2::ReturnCode bias_with_spd_voltages(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm_target,
    const mss::pmic::id i_id);

///
/// @brief Calcuate target voltage for PMIC
///
/// @param[in] i_dimm_target DIMM target of PMIC (holds the attributes)
/// @param[in] i_id ID of pmic (0,1)
/// @param[in] i_rail RAIL to calculate voltage for
/// @param[out] o_volt_buffer output buffer
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
inline fapi2::ReturnCode calculate_voltage_write_buffer(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm_target,
    const mss::pmic::id i_id,
    const uint8_t i_rail,
    fapi2::buffer<uint8_t>& o_volt_buffer)
{
    uint8_t l_volt = 0;
    int8_t l_volt_offset = 0;
    int8_t l_efd_volt_offset = 0;

    // Get the attributes corresponding to the rail and PMIC indices
    FAPI_TRY(get_volt_setting[i_rail][i_id](i_dimm_target, l_volt));
    FAPI_TRY(get_volt_offset[i_rail][i_id](i_dimm_target, l_volt_offset));
    FAPI_TRY(get_efd_volt_offset[i_rail][i_id](i_dimm_target, l_efd_volt_offset));

    // Set output buffer
    o_volt_buffer = l_volt + l_volt_offset + l_efd_volt_offset;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Order PMICs by sequence defined in the SPD
///
/// @param[in] i_dimm DIMM target to pull SPD fields from
/// @param[in] i_dimm_index index of the DIMM target
/// @param[in] i_pmics_per_dimm number of PMICs per dimm
/// @param[in,out] io_pmics vector of PMICs that will be re-ordered in place
/// @return fapi2::ReturnCode
///
fapi2::ReturnCode order_pmics_by_sequence(
    const fapi2::Target<fapi2::TARGET_TYPE_DIMM> i_dimm,
    const uint8_t i_dimm_index,
    const uint8_t i_pmics_per_dimm,
    std::vector<fapi2::Target<fapi2::TARGET_TYPE_PMIC>>& io_pmics);

///
/// @brief Bias with spd voltages for IDT pmic
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_dimm_target DIMM target
/// @param[in] i_id relative ID of PMIC (0/1)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
template <>
inline fapi2::ReturnCode bias_with_spd_voltages<mss::pmic::vendor::IDT>(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm_target,
    const mss::pmic::id i_id)
{
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    for (uint8_t l_rail_index = mss::pmic::rail::SWA; l_rail_index <= mss::pmic::rail::SWD; ++l_rail_index)
    {
        fapi2::buffer<uint8_t> l_volt_buffer;
        FAPI_TRY(calculate_voltage_write_buffer(i_dimm_target, i_id, l_rail_index, l_volt_buffer));

        // Since we have unsigned integers, this should check both underflow and overflow
        FAPI_ASSERT(l_volt_buffer <= CONSTS::MAX_VOLT_BITMAP,
                    fapi2::PMIC_VOLTAGE_OUT_OF_RANGE()
                    .set_TARGET(i_pmic_target)
                    .set_VOLTAGE_BITMAP(l_volt_buffer)
                    .set_RAIL(mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail_index]),
                    "Voltage out of range as determined by SPD voltage +/- offset for %s of %s",
                    PMIC_RAIL_NAMES[l_rail_index], mss::c_str(i_pmic_target) );

        l_volt_buffer = l_volt_buffer << CONSTS::SHIFT_VOLTAGE_FOR_REG;
        FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail_index], l_volt_buffer),
                 "Error writing address 0x%02hhX of PMIC %s", mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail_index],
                 mss::c_str(i_pmic_target));

    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Bias with spd voltages for TI pmic
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_dimm_target DIMM target
/// @param[in] i_id relative ID of PMIC (0/1)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
template <>
inline fapi2::ReturnCode bias_with_spd_voltages<mss::pmic::vendor::TI>(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm_target,
    const mss::pmic::id i_id)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    for (uint8_t l_rail_index = mss::pmic::rail::SWA; l_rail_index <= mss::pmic::rail::SWD; ++l_rail_index)
    {
        fapi2::buffer<uint8_t> l_volt_buffer;
        FAPI_TRY(calculate_voltage_write_buffer(i_dimm_target, i_id, l_rail_index, l_volt_buffer));

        bool l_overflow = false;

        uint8_t l_volt_range_select = 0;
        FAPI_TRY(get_volt_range_select[l_rail_index][i_id](i_dimm_target, l_volt_range_select));

        // SWD supports a RANGE 1, but NOT SWA-C
        if (l_rail_index == mss::pmic::rail::SWD)
        {
            // Can set range and voltage directly
            fapi2::buffer<uint8_t> l_volt_range_buffer;

            // Read in what the register has, as to not overwrite any default values
            FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R2B, l_volt_range_buffer));

            l_volt_range_buffer.writeBit<FIELDS::SWD_VOLTAGE_RANGE>(l_volt_range_select);

            // Write to PMIC
            FAPI_TRY(mss::pmic::i2c::reg_write_reverse_buffer(i_pmic_target, REGS::R2B, l_volt_range_buffer));
        }
        else
        {
            // Check if the range is range 1, in which case we will need to convert to range 0 (thanks TI)
            if (l_volt_range_select == CONSTS::RANGE_1)
            {
                // Convert from RANGE1 -> RANGE0

                // Since both ranges are 5mV (at least they're supposed to be)
                // we can just subtract the difference between range 1 and 0
                // which is 600mV -> 800mV
                // 200mV / 5 = 40
                uint8_t l_old_voltage = uint8_t(l_volt_buffer);
                l_volt_buffer = l_volt_buffer - CONSTS::CONVERT_RANGE1_TO_RANGE0;

                // Check for overflow (the old voltage should be larger unless we rolled over)
                if (l_old_voltage < l_volt_buffer)
                {
                    // Not using an extra xml error for this as overflow implies PMIC_VOLTAGE_OUT_OF_RANGE anyway.
                    FAPI_ERR("Overflow ocurred when converting SPD Range 1 voltage to TI Range 0");
                    l_overflow = true;
                }
            }
        }

        // Check for overflow due to range conversion (SWA-C), but also due to overflow due to offset attributes
        FAPI_ASSERT( (!l_overflow) && (l_volt_buffer <= CONSTS::MAX_VOLT_BITMAP),
                     fapi2::PMIC_VOLTAGE_OUT_OF_RANGE()
                     .set_TARGET(i_pmic_target)
                     .set_VOLTAGE_BITMAP(l_volt_buffer)
                     .set_RAIL(mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail_index]),
                     "Voltage out of range as determined by SPD voltage +/- offset for %s of %s",
                     PMIC_RAIL_NAMES[l_rail_index], mss::c_str(i_pmic_target) );

        l_volt_buffer = l_volt_buffer << CONSTS::SHIFT_VOLTAGE_FOR_REG;
        FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail_index], l_volt_buffer),
                 "Error writing address 0x%02hhX of PMIC %s", mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail_index],
                 mss::c_str(i_pmic_target));
    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Bias PMIC from SPD settings per vendor
///
/// @tparam V mss::pmic::vendor (IDT/TI)
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_dimm_target DIMM target associated with PMIC
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
template <mss::pmic::vendor V>
fapi2::ReturnCode bias_with_spd_settings(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm_target);

///
/// @brief Bias IDT PMIC from SPD settings
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_dimm_target DIMM target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
template<>
inline fapi2::ReturnCode bias_with_spd_settings<mss::pmic::vendor::IDT>(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm_target)
{
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;
    // Unlock Vendor Region
    FAPI_TRY(mss::pmic::unlock_vendor_region(i_pmic_target),
             "Error unlocking vendor region on PMIC %s", mss::c_str(i_pmic_target));

    {
        // PMIC position/ID of the OCMB target. There could be 4 total, but we care about whether its PMIC0(2) or PMIC1(3)
        const mss::pmic::id l_relative_pmic_id = static_cast<mss::pmic::id>(
                    mss::index(i_pmic_target) % CONSTS::NUM_UNIQUE_PMICS);

        // Phase combination
        FAPI_TRY(mss::pmic::bias_with_spd_phase_comb(i_pmic_target, i_dimm_target, l_relative_pmic_id));

        // Voltage ranges
        FAPI_TRY(mss::pmic::bias_with_spd_volt_ranges(i_pmic_target, i_dimm_target, l_relative_pmic_id));

        // Voltages
        FAPI_TRY(mss::pmic::bias_with_spd_voltages<mss::pmic::IDT>(i_pmic_target, i_dimm_target, l_relative_pmic_id));

        // Startup sequence
        FAPI_TRY(mss::pmic::bias_with_spd_startup_seq(i_pmic_target, i_dimm_target, l_relative_pmic_id));
    }

fapi_try_exit:
    // Try to lock vendor region even in the case of an error in this function
    return mss::pmic::lock_vendor_region(i_pmic_target, fapi2::current_err);
}

///
/// @brief Bias TI PMIC from SPD settings
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_dimm_target DIMM target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
template<>
inline fapi2::ReturnCode bias_with_spd_settings<mss::pmic::vendor::TI>(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm_target)
{
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;
    // Unlock Vendor Region
    FAPI_TRY(mss::pmic::unlock_vendor_region(i_pmic_target),
             "Error unlocking vendor region on PMIC %s", mss::c_str(i_pmic_target));

    {
        // PMIC position/ID of the OCMB target. There could be 4 total, but we care about whether its PMIC0(2) or PMIC1(3)
        const mss::pmic::id l_relative_pmic_id = static_cast<mss::pmic::id>(
                    mss::index(i_pmic_target) % CONSTS::NUM_UNIQUE_PMICS);
        // Phase combination
        FAPI_TRY(mss::pmic::bias_with_spd_phase_comb(i_pmic_target, i_dimm_target, l_relative_pmic_id));

        // Voltages
        // TI pmic only has range 0 for SWA-C. We need to convert anything SPD that says range 1 --> range 0
        FAPI_TRY(mss::pmic::bias_with_spd_voltages<mss::pmic::TI>(i_pmic_target, i_dimm_target, l_relative_pmic_id));

        // Startup sequence
        FAPI_TRY(mss::pmic::bias_with_spd_startup_seq(i_pmic_target, i_dimm_target, l_relative_pmic_id));
    }

fapi_try_exit:
    // Try to lock vendor region even in the case of an error in this function
    return mss::pmic::lock_vendor_region(i_pmic_target, fapi2::current_err);
}

//------------------- ENABLE FUNCTIONS-----------------//

///
/// @brief Enable pmics using manual mode (direct VR enable, no SPD fields)
/// @param[in] i_pmics vector of PMICs to enable
///
fapi2::ReturnCode enable_manual(const std::vector<fapi2::Target<fapi2::TARGET_TYPE_PMIC>> i_pmics);

///
/// @brief Function to enable 1U and 2U pmics
///
/// @param[in] i_pmic_target - the pmic target
/// @param[in] i_dimm_target - the dimm target that the PMIC resides on
/// @param[in] i_vendor_id - the vendor ID of the PMIC to bias
/// @return fapi2::ReturnCode - FAPI2_RC_SUCCESS if successful
///
fapi2::ReturnCode enable_chip_1U_2U(const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
                                    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_DIMM>& i_dimm_target,
                                    const uint16_t i_vendor_id);

}
} // mss

#endif
