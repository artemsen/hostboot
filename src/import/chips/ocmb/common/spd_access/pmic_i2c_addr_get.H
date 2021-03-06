/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/ocmb/common/spd_access/pmic_i2c_addr_get.H $ */
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
/// @file pmic_i2c_addr_get.H
/// @brief Function to get PMIC i2c addresses given SPD binary
///
// *HWP HWP Owner: Mark Pizzutillo <mark.pizzutillo@ibm.com>
// *HWP HWP Owner: Dan Crowell <dcrowell@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

///
/// @brief Get the pmic i2c addrs from the provided spd binary
///
/// @param[in] i_spd SPD binary
/// @param[out] o_pmic0_i2c_addr I2C Address from SPD
/// @param[out] o_pmic1_i2c_addr I2C Address from SPD
///

#ifndef __PMIC_I2C_ADDR_GET__
#define __PMIC_I2C_ADDR_GET__

#include <vector>

constexpr uint16_t SPD_PMIC0_I2C_BYTE = 260;
constexpr uint16_t SPD_PMIC1_I2C_BYTE = 261;

constexpr uint8_t PMIC0 = 0;
constexpr uint8_t PMIC1 = 1;

// Mapping from PMIC ID to SPD byte
static const std::vector<uint16_t> PMIC_I2C_ADDR_VECTOR =
{
    SPD_PMIC0_I2C_BYTE, // [0 == PMIC0]
    SPD_PMIC1_I2C_BYTE  // [1 == PMIC1]
};

///
/// @brief Get the pmic i2c address for the given PMIC ID
///
/// @param[in] i_spd SPD binary
/// @param[in] i_pmic_id PMIC ID (0,1)
/// @return uint8_t I2C address for the given PMIC ID
/// @note May need to be changed to support 4U SPD
///
inline uint8_t get_pmic_i2c_addr(const char* i_spd, const uint8_t i_pmic_id)
{
    // Out of range, just return 0. Failsafe so we don't segfault
    // The caller is responsible for calling this with a valid ID
    if (i_pmic_id >= PMIC_I2C_ADDR_VECTOR.size())
    {
        return 0;
    }

    const uint16_t l_byte = PMIC_I2C_ADDR_VECTOR[i_pmic_id];
    return uint8_t(i_spd[l_byte]);
}

#endif
