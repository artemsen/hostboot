/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/sbeio/sbe_getCapabilities.C $                         */
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
/**
* @file sbe_getCapabilities.C
* @brief Get the capabilities from the SBE
*/

#include <errl/errlmanager.H> // errlHndl_t
#include <sbeio/sbe_psudd.H>  // SbeFifo::psuCommand
#include "sbe_fifodd.H"       // SbeFifo::fifoGetCapabilitiesRequest
#include <sbeio/sbe_utils.H>  // sbeCapabilities_t
#include <sbeio/sbeioreasoncodes.H> // SBEIO_PSU, SBEIO_FIFO,
#include <targeting/common/commontargeting.H>  // get_huid
#include <util/align.H>            // ALIGN_X
#include <sys/mm.h>                // mm_virt_to_phys

extern trace_desc_t* g_trac_sbeio;

namespace SBEIO
{
using namespace TARGETING;
using namespace ERRORLOG;

 /**
 * @brief Apply the SBE capabilities to the given target
 *
 * @param[in]  i_target Target to apply the SBE capabilities on
 *
 * @param[in]  i_capabilities The SBE capabilities themselves
 *
 */
void applySbeCapabilities(TargetHandle_t i_target,
                          sbeCapabilities_t &i_capabilities)
{
    // Get the SBE Version from the SBE Capabilities and set the
    // attribute associated with SBE Version
    ATTR_SBE_VERSION_INFO_type l_sbeVersionInfo =
                  TWO_UINT16_TO_UINT32(i_capabilities.majorVersion,
                                       i_capabilities.minorVersion);
    i_target->setAttr<ATTR_SBE_VERSION_INFO>(l_sbeVersionInfo);
    TRACFCOMP(g_trac_sbeio,"applySbeCapabilities: "
              "Retrieved SBE Version: 0x%X", l_sbeVersionInfo);

    // Get the SBE Commit ID from the SBE Capabilities and set the
    // attribute associated with SBE Commit ID
    ATTR_SBE_COMMIT_ID_type l_sbeCommitId = i_capabilities.commitId;
    i_target->setAttr<ATTR_SBE_COMMIT_ID>(l_sbeCommitId);
    TRACFCOMP(g_trac_sbeio,"applySbeCapabilities: "
              "Retrieved SBE Commit ID: 0x%X", l_sbeCommitId);

    // Get the SBE Release Tag from the SBE Capabilities and set the
    // attribute associated with SBE Release Tag
    ATTR_SBE_RELEASE_TAG_type l_sbeReleaseTagString = {0};

    // Make sure the sizes are compatible.
    static_assert(SBE_RELEASE_TAG_MAX_CHARS <= ATTR_SBE_RELEASE_TAG_max_chars,
    "Copy error - size of source is greater than size of destination.");

    // Copy the release tags over into a more compatible type and set
    // the SBE Release Tags attribute
    strncpy(l_sbeReleaseTagString,
            i_capabilities.releaseTag,
            SBE_RELEASE_TAG_MAX_CHARS);
    i_target->setAttr<ATTR_SBE_RELEASE_TAG>(l_sbeReleaseTagString);
    TRACFCOMP(g_trac_sbeio,"applySbeCapabilities: "
              "Retrieved SBE Release Tag: %s", l_sbeReleaseTagString);
}

/**
 * getPsuSbeCapabilities
 */
errlHndl_t getPsuSbeCapabilities(TargetHandle_t i_target)
{
    errlHndl_t l_errl(nullptr);

    TRACDCOMP(g_trac_sbeio, ENTER_MRK "getPsuSbeCapabilities");

    // Cache the SBE Capabilities' size for future uses
    size_t l_sbeCapabilitiesSize = sizeof(sbeCapabilities_t);

    // The buffer must be a multiple of 128 bytes in size, and it must
    // be aligned to a 128 byte boundary.
    const size_t totalAlignedSize = ALIGN_X(l_sbeCapabilitiesSize,
          SbePsu::SBE_CAPABILITIES_ALIGNMENT_SIZE_IN_BYTES)
        + (SbePsu::SBE_CAPABILITIES_ALIGNMENT_SIZE_IN_BYTES - 1);

    // Set up the buffer which the SBE will copy the capabilities info to.
    // Create buffer with enough size to be properly aligned
    uint8_t * l_sbeCapabilitiesReadBuffer = static_cast<uint8_t*>(
        malloc(totalAlignedSize));

    // Align the buffer
    uint64_t l_sbeCapabilitiesReadBufferAligned =
        ALIGN_X(reinterpret_cast<uint64_t>(l_sbeCapabilitiesReadBuffer),
                SbePsu::SBE_CAPABILITIES_ALIGNMENT_SIZE_IN_BYTES);

    // Clear buffer up to the size of the capabilities
    memset(reinterpret_cast<uint8_t*>(l_sbeCapabilitiesReadBufferAligned),
           0, l_sbeCapabilitiesSize );

    // Create a PSU request message and initialize it
    SbePsu::psuCommand l_psuCommand(
                SbePsu::SBE_REQUIRE_RESPONSE |
                SbePsu::SBE_REQUIRE_ACK,                 //control flags
                SbePsu::SBE_PSU_GENERIC_MESSAGE,         //command class
                SbePsu::SBE_PSU_MSG_GET_CAPABILITIES);   //command
    l_psuCommand.cd7_getSbeCapabilities_CapabilitiesSize =
                ALIGN_X(l_sbeCapabilitiesSize,
                        SbePsu::SBE_CAPABILITIES_ALIGNMENT_SIZE_IN_BYTES);
    l_psuCommand.cd7_getSbeCapabilities_CapabilitiesAddr =
                 mm_virt_to_phys(
                   reinterpret_cast<void*>(l_sbeCapabilitiesReadBufferAligned));

    // Create a PSU response message
    SbePsu::psuResponse l_psuResponse;

    do
    {
        // Make the call to perform the PSU Chip Operation
        l_errl = SbePsu::getTheInstance().performPsuChipOp(
                        i_target,
                        &l_psuCommand,
                        &l_psuResponse,
                        SbePsu::MAX_PSU_SHORT_TIMEOUT_NS,
                        SbePsu::SBE_GET_CAPABILITIES_REQ_USED_REGS,
                        SbePsu::SBE_GET_CAPABILITIES_RSP_USED_REGS);

        // Before continuing, make sure this request is honored
        if (SBE_PRI_INVALID_COMMAND == l_psuResponse.primaryStatus &&
            SBE_SEC_COMMAND_NOT_SUPPORTED == l_psuResponse.secondaryStatus)
        {
            TRACFCOMP(g_trac_sbeio, "getPsuSbeCapabilities: SBE firmware does "
                                    "not support PSU get capabilities request");

            // Do not pass back any errors
            delete l_errl;
            l_errl = nullptr;
            break;
        }


        if (l_errl)
        {
            TRACFCOMP(g_trac_sbeio,
                      "getPsuSbeCapabilities: "
                      "Call to performPsuChipOp failed, error returned");

            TRACDBIN(g_trac_sbeio,
              "getPsuSbeCapabilities: capabilities data",
              reinterpret_cast<uint8_t*>(l_sbeCapabilitiesReadBufferAligned),
              l_sbeCapabilitiesSize);

            break;
        }

        // Sanity check - are HW and HB communications in sync?
        if ((SbePsu::SBE_PSU_GENERIC_MESSAGE !=
                               l_psuResponse.sbe_commandClass) ||
            (SbePsu::SBE_PSU_MSG_GET_CAPABILITIES !=
                               l_psuResponse.sbe_command))
        {
            TRACFCOMP(g_trac_sbeio,
                      "Call to performPsuChipOp returned an unexpected "
                      "message type; "
                      "command class returned:0x%X, "
                      "expected command class:0x%X, "
                      "command returned:0x%X, "
                      "expected command:0x%X",
                      l_psuResponse.sbe_commandClass,
                      SbePsu::SBE_PSU_GENERIC_MESSAGE,
                      l_psuResponse.sbe_command,
                      SbePsu::SBE_PSU_MSG_GET_CAPABILITIES);

            /*@
             * @errortype
             * @moduleid          SBEIO_PSU
             * @reasoncode        SBEIO_RECEIVED_UNEXPECTED_MSG
             * @userdata1         Target HUID
             * @userdata2[0:15]   Requested command class
             * @userdata2[16:31]  Requested command
             * @userdata2[32:47]  Returned command class
             * @userdata2[48:63]  Returned command
             * @devdesc           Call to PSU Chip Op returned an
             *                    unexpected message type.
             */
            l_errl = new ErrlEntry(
                ERRL_SEV_INFORMATIONAL,
                SBEIO_PSU,
                SBEIO_RECEIVED_UNEXPECTED_MSG,
                get_huid(i_target),
                TWO_UINT32_TO_UINT64(
                  TWO_UINT16_TO_UINT32(SbePsu::SBE_PSU_GENERIC_MESSAGE,
                                       SbePsu::SBE_PSU_MSG_GET_CAPABILITIES),
                  TWO_UINT16_TO_UINT32(l_psuResponse.sbe_commandClass,
                                       l_psuResponse.sbe_command) ));

            l_errl->collectTrace(SBEIO_COMP_NAME, 256);

            break;
        }

        // Check for any difference in sizes (expected vs returned)
        // This may happen but it is not a show stopper, just note it
        if (l_psuResponse.sbe_capabilities_size != l_sbeCapabilitiesSize)
        {
            TRACFCOMP(g_trac_sbeio, "getPsuSbeCapabilities:"
                     "Call to performPsuChipOp returned an unexpected size: "
                     "capabilities size returned:%d, "
                     "expected capabilities size:%d, ",
                     l_psuResponse.sbe_capabilities_size,
                     l_sbeCapabilitiesSize);
        }

        // Create an SBE Capabilities structure to make it easy to pull data;
        // clear memory before use
        sbeCapabilities_t l_sbeCapabilities;
        memset(&l_sbeCapabilities, 0, sizeof(sbeCapabilities_t));

        // If the returned size is greater than or equal to the needed size,
        // then copy all of the SBE capabilities
        if (l_psuResponse.sbe_capabilities_size >= l_sbeCapabilitiesSize)
        {
            memcpy (&l_sbeCapabilities,
                   reinterpret_cast<void*>(l_sbeCapabilitiesReadBufferAligned),
                   l_sbeCapabilitiesSize);
        }
        // If the returned size is less than the needed size and is not zero
        // then copy what was given
        else if (l_psuResponse.sbe_capabilities_size)
        {
            memcpy (&l_sbeCapabilities,
                   reinterpret_cast<void*>(l_sbeCapabilitiesReadBufferAligned),
                   l_psuResponse.sbe_capabilities_size);
        }

        // If data returned, retrieve it
        if (l_psuResponse.sbe_capabilities_size)
        {
            applySbeCapabilities(i_target, l_sbeCapabilities);
        }
    } while (0);

    // Free the buffer
    if (l_sbeCapabilitiesReadBuffer)
    {
        free(l_sbeCapabilitiesReadBuffer);
        l_sbeCapabilitiesReadBuffer = nullptr;
    }

    TRACFCOMP(g_trac_sbeio, EXIT_MRK "getPsuSbeCapabilities");

    return l_errl;
};


/**
 *  getFifoSbeCapabilities
 */
errlHndl_t getFifoSbeCapabilities(TargetHandle_t i_target)
{
    errlHndl_t l_errl(nullptr);

    TRACDCOMP(g_trac_sbeio, ENTER_MRK "getFifoSbeCapabilities");

    do
    {
        // Get the needed structures to make the FIFO call
        // Create a FIFO request message. Default ctor initializes correctly
        SbeFifo::fifoGetCapabilitiesRequest l_fifoRequest;

        // Create a FIFO response message.  No need to iniitilaize
        SbeFifo::fifoGetCapabilitiesResponse l_fifoResponse;

        // Make the call to perform the FIFO Chip Operation
        l_errl = SbeFifo::getTheInstance().performFifoChipOp(
                        i_target,
                        reinterpret_cast<uint32_t *>(&l_fifoRequest),
                        reinterpret_cast<uint32_t *>(&l_fifoResponse),
                        sizeof(l_fifoResponse));

        if (l_errl)
        {
            TRACFCOMP(g_trac_sbeio,
                      "Call to performFifoChipOp failed, error returned");
            break;
        }

        // Sanity check - are HW and HB communications in sync?
        if ((SbeFifo::FIFO_STATUS_MAGIC != l_fifoResponse.status.magic)  ||
            (SbeFifo::SBE_FIFO_CLASS_GENERIC_MESSAGE !=
                                      l_fifoResponse.status.commandClass) ||
            (SbeFifo::SBE_FIFO_CMD_GET_CAPABILITIES !=
                                      l_fifoResponse.status.command))
        {
            TRACFCOMP(g_trac_sbeio,
                      "Call to performFifoChipOp returned an unexpected "
                      "message type; "
                      "magic code returned:0x%X, "
                      "expected magic code:0x%X, "
                      "command class returned:0x%X, "
                      "expected command class:0x%X, "
                      "command returned:0x%X, "
                      "expected command:0x%X",
                      l_fifoResponse.status.magic,
                      SbeFifo::FIFO_STATUS_MAGIC,
                      l_fifoResponse.status.commandClass,
                      SbeFifo::SBE_FIFO_CLASS_GENERIC_MESSAGE,
                      l_fifoResponse.status.command,
                      SbeFifo::SBE_FIFO_CMD_GET_CAPABILITIES);

            /*@
             * @errortype
             * @moduleid          SBEIO_FIFO
             * @reasoncode        SBEIO_RECEIVED_UNEXPECTED_MSG
             * @userdata1         Target HUID
             * @userdata2[0:15]   Requested command class
             * @userdata2[16:31]  Requested command
             * @userdata2[32:47]  Returned command class
             * @userdata2[48:63]  Returned command
             * @devdesc           Call to FIFO Chip Op returned an
             *                    unexpected message type.
             */
            l_errl = new ErrlEntry(
                ERRL_SEV_INFORMATIONAL,
                SBEIO_FIFO,
                SBEIO_RECEIVED_UNEXPECTED_MSG,
                get_huid(i_target),
                TWO_UINT32_TO_UINT64(
                  TWO_UINT16_TO_UINT32(SbeFifo::SBE_FIFO_CLASS_GENERIC_MESSAGE,
                                       SbeFifo::SBE_FIFO_CMD_GET_CAPABILITIES),
                  TWO_UINT16_TO_UINT32(l_fifoResponse.status.commandClass,
                                       l_fifoResponse.status.command) ));

            l_errl->collectTrace(SBEIO_COMP_NAME, 256);

            break;
        }

        applySbeCapabilities(i_target, l_fifoResponse.capabilities);
    }
    while(0);

    TRACDCOMP(g_trac_sbeio, EXIT_MRK "getFifoSbeCapabilities");

    return l_errl;
};

} //end namespace SBEIO

