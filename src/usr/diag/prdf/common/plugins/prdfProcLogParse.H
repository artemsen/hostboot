/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/common/plugins/prdfProcLogParse.H $         */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2014,2018                        */
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

/** @file  prdfProcLogParse.H
 *  @brief Error log parsing code specific to the processor subsystem.
 */

#ifndef __prdfProcLogParse_H
#define __prdfProcLogParse_H

#include <stdint.h>
#include <p9_pm_recovery_ffdc_defines.H>

class ErrlUsrParser;

namespace PRDF
{
#if defined(PRDF_HOSTBOOT_ERRL_PLUGIN)
namespace HOSTBOOT
{
#elif defined(PRDF_FSP_ERRL_PLUGIN)
namespace FSP
{
#endif

/**
 * @brief Parses the SLW FFDC data section.
 * @param i_buffer The data buffer.
 * @param i_buflen The buffer length.
 * @param i_parser The error log parser.
 */
bool parseSlwFfdcData( uint8_t * i_buffer, uint32_t i_buflen,
                       ErrlUsrParser & i_parser );

bool parseTodFfdcData( uint8_t * i_buffer, uint32_t i_buflen,
                       ErrlUsrParser & i_parser );

/**
 * @brief Parses the L2 Line Delete/Column Repair section.
 * @param i_buffer The data buffer.
 * @param i_buflen The buffer length.
 * @param i_parser The error log parser.
 */
bool parseL2LdCrFfdc( uint8_t * i_buffer, uint32_t i_buflen,
                      ErrlUsrParser & i_parser );

/**
 * @brief Parses the L3 Line Delete/Column Repair section.
 * @param i_buffer The data buffer.
 * @param i_buflen The buffer length.
 * @param i_parser The error log parser.
 */
bool parseL3LdCrFfdc( uint8_t * i_buffer, uint32_t i_buflen,
                      ErrlUsrParser & i_parser );

/**
 * @brief parser a user data section added by PRD in case of PM malfunction.
 * @param[in] i_buf     points to user data section
 * @param[in] i_length  length of the section
 * @param[in] i_parser  error log parser
 * @param[in] i_subsec  sub section id
 * @return    PARSE_SUCCESS if parsing succeeds, error code otherwise.
 */
bool parsePmFfdcData( void * i_buf, uint32_t i_length,
                      ErrlUsrParser & i_parser, errlver_t i_subsec );

#if defined(PRDF_HOSTBOOT_ERRL_PLUGIN) || defined(PRDF_FSP_ERRL_PLUGIN)
} // end namespace FSP/HOSTBOOT
#endif
} // end namespace PRDF

#endif // __prdfProcLogParse_H


