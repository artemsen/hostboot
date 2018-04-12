/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/plat/prdfPlatServices_ipl.C $               */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2018                        */
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
 * @file  prdfPlatServices_ipl.C
 * @brief Wrapper code for external interfaces used by PRD (IPL only).
 *
 * This file contains code that is strictly specific to Hostboot. All code that
 * is common between FSP and Hostboot should be in the respective common file.
 */

#include <prdfPlatServices.H>

#include <prdfGlobal.H>
#include <prdfErrlUtil.H>
#include <prdfTrace.H>

#include <prdfCenMbaDataBundle.H>
#include <prdfMemDqBitmap.H>
#include <prdfMemScrubUtils.H>
#include <prdfMfgThresholdMgr.H>

#include <diag/mdia/mdia.H>
#include <config.h>

using namespace TARGETING;

namespace PRDF
{

namespace PlatServices
{

//##############################################################################
//##                        Memory specific functions
//##############################################################################

bool isInMdiaMode()
{
    bool o_isInMdiaMode = false;
#ifndef CONFIG_VPO_COMPILE
    MDIA::waitingForMaintCmdEvents(o_isInMdiaMode);
#endif
    return o_isInMdiaMode;
}

//------------------------------------------------------------------------------

int32_t mdiaSendEventMsg( TargetHandle_t i_trgt,
                          MDIA::MaintCommandEventType i_eventType )
{
    #define PRDF_FUNC "[PlatServices::mdiaSendEventMsg] "

    int32_t o_rc = SUCCESS;

#ifndef CONFIG_VPO_COMPILE

    PRDF_ASSERT( nullptr != i_trgt );

    // Only MCBIST and MBA supported.
    TYPE trgtType = getTargetType( i_trgt );
    PRDF_ASSERT( TYPE_MCBIST == trgtType || TYPE_MBA == trgtType );

    // MDIA must be running.
    PRDF_ASSERT( isInMdiaMode() );

    // Send command complete to MDIA.
    MDIA::MaintCommandEvent mdiaEvent;
    mdiaEvent.target = i_trgt;
    mdiaEvent.type   = i_eventType;

    errlHndl_t errl = MDIA::processEvent( mdiaEvent );
    if ( NULL != errl )
    {
        PRDF_ERR( PRDF_FUNC "MDIA::processEvent() failed: i_target=0x%08x "
                  "i_eventType=%d", getHuid(i_trgt), i_eventType );
        PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
        o_rc = FAIL;
    }

#endif

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

bool rcdParityErrorReconfigLoop( TargetHandle_t i_trgt )
{
    TargetHandle_t top = getSystemTarget();

    // Get the current reconfig count and increment.
    uint8_t count = i_trgt->getAttr<ATTR_RCD_PARITY_RECONFIG_LOOP_COUNT>() + 1;

    // Get the reconfig threshold and check MNFG threshold, if needed.
    uint8_t th = top->getAttr<ATTR_RCD_PARITY_RECONFIG_LOOPS_ALLOWED>() + 1;
    if ( mfgMode() )
    {
        uint8_t mnfgTh = MfgThresholdMgr::getInstance()->
                                   getThreshold(ATTR_MNFG_TH_RCD_PARITY_ERRORS);
        if ( mnfgTh < th )
            th = mnfgTh;
    }

    // If the count is under threshold, trigger a reconfig loop.
    if ( count < th )
    {
        // Set the RCD parity error flag in the reconfig loop attribute. This
        // will trigger a reconfig loop at the end of the current istep.
        ATTR_RECONFIGURE_LOOP_type attr = top->getAttr<ATTR_RECONFIGURE_LOOP>();
        if ( 0 == (attr & RECONFIGURE_LOOP_RCD_PARITY_ERROR) )
        {
            attr |= RECONFIGURE_LOOP_RCD_PARITY_ERROR;
            top->setAttr<ATTR_RECONFIGURE_LOOP>(attr);
        }

        // Write the new count to the attribute.
        i_trgt->setAttr<ATTR_RCD_PARITY_RECONFIG_LOOP_COUNT>(count);

        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

template<>
uint32_t mssRestoreDramRepairs<TYPE_MCA>( TargetHandle_t i_target,
                                          uint8_t & o_repairedRankMask,
                                          uint8_t & o_badDimmMask )
{
    uint32_t o_rc = SUCCESS;

    errlHndl_t errl = NULL;


    fapi2::buffer<uint8_t> tmpRepairedRankMask, tmpBadDimmMask;
    FAPI_INVOKE_HWP( errl, mss::restore_repairs,
                     fapi2::Target<fapi2::TARGET_TYPE_MCA>( i_target ),
                     tmpRepairedRankMask, tmpBadDimmMask );

    if ( NULL != errl )
    {
        PRDF_ERR( "[PlatServices::mssRestoreDramRepairs] "
                  "restore_repairs() failed. HUID: 0x%08x",
                  getHuid(i_target) );
        PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
        o_rc = FAIL;
    }

    o_repairedRankMask = (uint8_t)tmpRepairedRankMask;
    o_badDimmMask = (uint8_t)tmpBadDimmMask;

    return o_rc;
}

//------------------------------------------------------------------------------

template<>
uint32_t mssRestoreDramRepairs<TYPE_MBA>( TargetHandle_t i_target,
                                          uint8_t & o_repairedRankMask,
                                          uint8_t & o_badDimmMask )
{
    uint32_t o_rc = SUCCESS;

    /* TODO RTC 178743
    errlHndl_t errl = NULL;

    FAPI_INVOKE_HWP( errl, mss_restore_DRAM_repairs,
                     fapi::Target(fapi::TARGET_TYPE_MCA_CHIPLET, i_target),
                     o_repairedRankMask, o_badDimmMask );

    if ( NULL != errl )
    {
        PRDF_ERR( "[PlatServices::mssRestoreDramRepairs] "
                  "mss_restore_dram_repairs() failed. HUID: 0x%08x",
                  getHuid(i_target) );
        PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
        o_rc = FAIL;
    }
    */

    return o_rc;
}


//------------------------------------------------------------------------------

/* TODO RTC 157888
int32_t mssIplUeIsolation( TargetHandle_t i_mba, const CenRank & i_rank,
                           CenDqBitmap & o_bitmap )
{
    #define PRDF_FUNC "[PlatServices::mssIplUeIsolation] "

    int32_t o_rc = SUCCESS;

    uint8_t data[MBA_DIMMS_PER_RANK][DIMM_DQ_RANK_BITMAP_SIZE];

    errlHndl_t errl = NULL;
    FAPI_INVOKE_HWP( errl, mss_IPL_UE_isolation, getFapiTarget(i_mba),
                     i_rank.getMaster(), data );
    if ( NULL != errl )
    {
        PRDF_ERR( PRDF_FUNC "mss_IPL_UE_isolation() failed: MBA=0x%08x "
                  "rank=%d", getHuid(i_mba), i_rank.getMaster() );
        PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
        o_rc = FAIL;
    }
    else
    {
        o_bitmap = CenDqBitmap ( i_mba, i_rank, data );
    }

    return o_rc;

    #undef PRDF_FUNC
}
*/

//##############################################################################
//##                    Nimbus Maintenance Command wrappers
//##############################################################################

template<>
bool isBroadcastModeCapable<TYPE_MCBIST>( ExtensibleChip * i_chip )
{
    PRDF_ASSERT( nullptr != i_chip );
    PRDF_ASSERT( TYPE_MCBIST == i_chip->getType() );

    fapi2::Target<fapi2::TARGET_TYPE_MCBIST> fapiTrgt ( i_chip->getTrgt() );

    mss::states l_ret = mss::states::NO;
    FAPI_CALL_HWP( l_ret, mss::mcbist::is_broadcast_capable, fapiTrgt );
    return ( mss::states::YES == l_ret );
}

//------------------------------------------------------------------------------

template<>
uint32_t startSfRead<TYPE_MCA>( ExtensibleChip * i_mcaChip,
                                const MemRank & i_rank )
{
    #define PRDF_FUNC "[PlatServices::startSfRead<TYPE_MCA>] "

    PRDF_ASSERT( isInMdiaMode() ); // MDIA must be running.

    PRDF_ASSERT( nullptr != i_mcaChip );
    PRDF_ASSERT( TYPE_MCA == i_mcaChip->getType() );

    uint32_t o_rc = SUCCESS;

    // Get the MCBIST fapi target
    ExtensibleChip * mcbChip = getConnectedParent( i_mcaChip, TYPE_MCBIST );
    fapi2::Target<fapi2::TARGET_TYPE_MCBIST> fapiTrgt ( mcbChip->getTrgt() );

    // Get the stop conditions.
    mss::mcbist::stop_conditions stopCond;
    stopCond.set_pause_on_mpe(mss::ON)
            .set_pause_on_ue(mss::ON)
            .set_pause_on_aue(mss::ON)
            .set_nce_inter_symbol_count_enable(mss::ON)
            .set_nce_soft_symbol_count_enable( mss::ON)
            .set_nce_hard_symbol_count_enable( mss::ON);

    // Stop on hard CEs if MNFG CE checking is enabled.
    if ( isMfgCeCheckingEnabled() ) stopCond.set_pause_on_nce_hard(mss::ON);

    do
    {
        // Get the first address of the given rank.
        mss::mcbist::address saddr, eaddr;
        o_rc = getMemAddrRange<TYPE_MCA>( i_mcaChip, i_rank, saddr, eaddr,
                                          SLAVE_RANK );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "getMemAddrRange(0x%08x,0x%2x) failed",
                      i_mcaChip->getHuid(), i_rank.getKey() );
            break;
        }

        // Clear all of the counters and maintenance ECC attentions.
        o_rc = prepareNextCmd<TYPE_MCBIST>( mcbChip );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "prepareNextCmd(0x%08x) failed",
                      mcbChip->getHuid() );
            break;
        }

        // Start the super fast read command.
        errlHndl_t errl;
        FAPI_INVOKE_HWP( errl, mss::memdiags::sf_read, fapiTrgt, stopCond,
                         saddr );
        if ( nullptr != errl )
        {
            PRDF_ERR( PRDF_FUNC "mss::memdiags::sf_read(0x%08x,%d) failed",
                      mcbChip->getHuid(), i_rank.getMaster() );
            PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
            o_rc = FAIL; break;
        }

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

// This specialization only exists to avoid a lot of extra code in some classes.
// The input chip must still be an MCA chip.
template<>
uint32_t startSfRead<TYPE_MCBIST>( ExtensibleChip * i_mcaChip,
                                   const MemRank & i_rank )
{
    return startSfRead<TYPE_MCA>( i_mcaChip, i_rank );
}

//------------------------------------------------------------------------------

template<>
uint32_t cleanupSfRead<TYPE_MCBIST>( ExtensibleChip * i_mcbChip )
{
    return SUCCESS; // Not needed for MCBIST commands.
}

//##############################################################################
//##                   Centaur Maintenance Command wrappers
//##############################################################################

template<>
bool isBroadcastModeCapable<TYPE_MBA>( ExtensibleChip * i_chip )
{
    return false; // Not supported on Centaur.
}

//------------------------------------------------------------------------------

template<>
uint32_t startSfRead<TYPE_MBA>( ExtensibleChip * i_mbaChip,
                                const MemRank & i_rank )
{
    #define PRDF_FUNC "[PlatServices::startSfRead<TYPE_MBA>] "

    PRDF_ASSERT( isInMdiaMode() ); // MDIA must be running.

    PRDF_ASSERT( nullptr != i_mbaChip );
    PRDF_ASSERT( TYPE_MBA == i_mbaChip->getType() );

    uint32_t o_rc = SUCCESS;

    // Get the MBA fapi target
    fapi2::Target<fapi2::TARGET_TYPE_MBA> fapiTrgt ( i_mbaChip->getTrgt() );

    // Get the stop conditions.
    uint32_t stopCond = mss_MaintCmd::STOP_END_OF_RANK              |
                        mss_MaintCmd::STOP_ON_MPE                   |
                        mss_MaintCmd::STOP_ON_UE                    |
                        mss_MaintCmd::STOP_ON_END_ADDRESS           |
                        mss_MaintCmd::ENABLE_CMD_COMPLETE_ATTENTION;

    // Stop on hard CEs if MNFG CE checking is enabled.
    if ( isMfgCeCheckingEnabled() )
        stopCond |= mss_MaintCmd::STOP_ON_HARD_NCE_ETE;

    do
    {
        fapi2::buffer<uint64_t> saddr, eaddr, junk;

        // Get the first address of the given rank.
        o_rc = getMemAddrRange<TYPE_MBA>( i_mbaChip, i_rank, saddr, junk,
                                          SLAVE_RANK );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "getMemAddrRange(0x%08x,0x%2x) failed",
                      i_mbaChip->getHuid(), i_rank.getKey() );
            break;
        }

        // Get the last address of the chip.
        o_rc = getMemAddrRange<TYPE_MBA>( i_mbaChip, junk, eaddr );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "getMemAddrRange(0x%08x) failed",
                      i_mbaChip->getHuid() );
            break;
        }

        // Clear all of the counters and maintenance ECC attentions.
        o_rc = prepareNextCmd<TYPE_MBA>( i_mbaChip );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "prepareNextCmd(0x%08x) failed",
                      i_mbaChip->getHuid() );
            break;
        }

        // Create the new command. Store a pointer to the command in the MBA
        // data bundle so that we can call the cleanup function after the
        // command has completed.
        MbaDataBundle * db = getMbaDataBundle( i_mbaChip );
        PRDF_ASSERT( nullptr == db->iv_sfCmd ); // Code bug.
        db->iv_sfCmd = new mss_SuperFastRead { fapiTrgt, saddr, eaddr,
                                               stopCond, false };

        // Start the super fast read command.
        errlHndl_t errl;
        FAPI_INVOKE_HWP( errl, db->iv_sfCmd->setupAndExecuteCmd );
        if ( nullptr != errl )
        {
            PRDF_ERR( PRDF_FUNC "setupAndExecuteCmd() on 0x%08x,0x%02x failed",
                      i_mbaChip->getHuid(), i_rank.getKey() );
            PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
            o_rc = FAIL; break;
        }

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

template<>
uint32_t cleanupSfRead<TYPE_MBA>( ExtensibleChip * i_mbaChip )
{
    #define PRDF_FUNC "[PlatServices::cleanupSfRead<TYPE_MBA>] "

    PRDF_ASSERT( isInMdiaMode() ); // MDIA must be running.

    PRDF_ASSERT( nullptr != i_mbaChip );
    PRDF_ASSERT( TYPE_MBA == i_mbaChip->getType() );

    uint32_t o_rc = SUCCESS;

    // Cleanup the super fast read command, if it exists.
    MbaDataBundle * db = getMbaDataBundle( i_mbaChip );
    if ( nullptr != db->iv_sfCmd )
    {
        errlHndl_t errl;
        FAPI_INVOKE_HWP( errl, db->iv_sfCmd->cleanupCmd );
        if ( nullptr != errl )
        {
            PRDF_ERR( PRDF_FUNC "cleanupCmd() on 0x%08x failed",
                      i_mbaChip->getHuid() );
            PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
            o_rc = FAIL;
        }

        // Delete the command because we don't need it any more.
        delete db->iv_sfCmd; db->iv_sfCmd = nullptr;
    }

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

template<>
uint32_t startVcmPhase1<TYPE_MBA>( ExtensibleChip * i_chip,
                                   const MemRank & i_rank )
{
    #define PRDF_FUNC "[PlatServices::startVcmPhase1<TYPE_MBA>] "

    PRDF_ASSERT( isInMdiaMode() ); // MDIA must be running.

    PRDF_ASSERT( nullptr != i_chip );
    PRDF_ASSERT( TYPE_MBA == i_chip->getType() );

    uint32_t o_rc = SUCCESS;

    // Get the MBA fapi target
    fapi2::Target<fapi2::TARGET_TYPE_MBA> fapiTrgt ( i_chip->getTrgt() );

    // Get the stop conditions.
    uint32_t stopCond = mss_MaintCmd::STOP_ON_RETRY_CE_ETE          |
                        mss_MaintCmd::STOP_ON_END_ADDRESS           |
                        mss_MaintCmd::ENABLE_CMD_COMPLETE_ATTENTION;

    // Note that we set the stop on RCE ETE flag. This requires us to set a
    // threshold in the MBSTR. Fortunately, MDIA sets the threshold for us when
    // it starts the first command on this MBA.

    do
    {
        // Get the address range of the master rank.
        fapi2::buffer<uint64_t> saddr, eaddr;
        o_rc = getMemAddrRange<TYPE_MBA>( i_chip, i_rank, saddr, eaddr,
                                          MASTER_RANK );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "getMemAddrRange(0x%08x,0x%2x) failed",
                      i_chip->getHuid(), i_rank.getKey() );
            break;
        }

        // Clear all of the counters and maintenance ECC attentions.
        o_rc = prepareNextCmd<TYPE_MBA>( i_chip );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "prepareNextCmd(0x%08x) failed",
                      i_chip->getHuid() );
            break;
        }

        // Start a steer cleanup command.
        mss_TimeBaseSteerCleanup cmd { fapiTrgt, saddr, eaddr,
                                       mss_MaintCmd::FAST_MAX_BW_IMPACT,
                                       stopCond, false };
        errlHndl_t errl;
        FAPI_INVOKE_HWP( errl, cmd.setupAndExecuteCmd );
        if ( nullptr != errl )
        {
            PRDF_ERR( PRDF_FUNC "setupAndExecuteCmd() on 0x%08x,0x%02x failed",
                      i_chip->getHuid(), i_rank.getKey() );
            PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
            o_rc = FAIL; break;
        }

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

template<>
uint32_t startVcmPhase2<TYPE_MBA>( ExtensibleChip * i_chip,
                                   const MemRank & i_rank )
{
    #define PRDF_FUNC "[PlatServices::startVcmPhase2<TYPE_MBA>] "

    PRDF_ASSERT( isInMdiaMode() ); // MDIA must be running.

    PRDF_ASSERT( nullptr != i_chip );
    PRDF_ASSERT( TYPE_MBA == i_chip->getType() );

    uint32_t o_rc = SUCCESS;

    // Get the MBA fapi target
    fapi2::Target<fapi2::TARGET_TYPE_MBA> fapiTrgt ( i_chip->getTrgt() );

    // Get the stop conditions.
    uint32_t stopCond = mss_MaintCmd::STOP_ON_END_ADDRESS           |
                        mss_MaintCmd::ENABLE_CMD_COMPLETE_ATTENTION;

    do
    {
        // Get the address range of the master rank.
        fapi2::buffer<uint64_t> saddr, eaddr;
        o_rc = getMemAddrRange<TYPE_MBA>( i_chip, i_rank, saddr, eaddr,
                                          MASTER_RANK );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "getMemAddrRange(0x%08x,0x%2x) failed",
                      i_chip->getHuid(), i_rank.getKey() );
            break;
        }

        // Clear all of the counters and maintenance ECC attentions.
        o_rc = prepareNextCmd<TYPE_MBA>( i_chip );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "prepareNextCmd(0x%08x) failed",
                      i_chip->getHuid() );
            break;
        }

        // Create the new command. Store a pointer to the command in the MBA
        // data bundle so that we can call the cleanup function after the
        // command has completed.
        MbaDataBundle * db = getMbaDataBundle( i_chip );
        PRDF_ASSERT( nullptr == db->iv_sfCmd ); // Code bug.
        db->iv_sfCmd = new mss_SuperFastRead { fapiTrgt, saddr, eaddr,
                                               stopCond, false };

        // Start the super fast read command.
        errlHndl_t errl;
        FAPI_INVOKE_HWP( errl, db->iv_sfCmd->setupAndExecuteCmd );
        if ( nullptr != errl )
        {
            PRDF_ERR( PRDF_FUNC "setupAndExecuteCmd() on 0x%08x,0x%02x failed",
                      i_chip->getHuid(), i_rank.getKey() );
            PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
            o_rc = FAIL; break;
        }

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

template<>
uint32_t startTpsPhase1<TYPE_MBA>( ExtensibleChip * i_chip,
                                   const MemRank & i_rank )
{
    #define PRDF_FUNC "[PlatServices::startTpsPhase1<TYPE_MBA>] "

    PRDF_ASSERT( isInMdiaMode() ); // MDIA must be running.

    PRDF_ASSERT( nullptr != i_chip );
    PRDF_ASSERT( TYPE_MBA == i_chip->getType() );

    uint32_t o_rc = SUCCESS;

    // Get the MBA fapi target
    fapi2::Target<fapi2::TARGET_TYPE_MBA> fapiTrgt ( i_chip->getTrgt() );

    // Get the stop conditions.
    uint32_t stopCond = mss_MaintCmd::STOP_ON_RETRY_CE_ETE          |
                        mss_MaintCmd::STOP_ON_END_ADDRESS           |
                        mss_MaintCmd::ENABLE_CMD_COMPLETE_ATTENTION;

    // Note that we set the stop on RCE ETE flag. This requires us to set a
    // threshold in the MBSTR. Fortunately, MDIA sets the threshold for us when
    // it starts the first command on this MBA.

    do
    {
        // Set up the per-symbol counters to capture soft CEs.
        ExtensibleChip * membChip = getConnectedParent( i_chip, TYPE_MEMBUF );
        const char * reg_str = (0 == i_chip->getPos()) ? "MBA0_MBSTR"
                                                       : "MBA1_MBSTR";
        SCAN_COMM_REGISTER_CLASS * mbstr = membChip->getRegister( reg_str );
        o_rc = mbstr->Read();
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "Read() failed on %s", reg_str );
            break;
        }

        //  Enable per-symbol error counters to count soft CEs
        mbstr->SetBit(55);
        mbstr->SetBit(56);
        // Disable per-symbol error counters to count hard CEs
        mbstr->ClearBit(57);

        o_rc = mbstr->Write();
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "Write() failed on %s", reg_str );
            break;
        }

        // Get the address range of the master rank.
        fapi2::buffer<uint64_t> saddr, eaddr;
        o_rc = getMemAddrRange<TYPE_MBA>( i_chip, i_rank, saddr, eaddr,
                                          SLAVE_RANK );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "getMemAddrRange(0x%08x,0x%2x) failed",
                      i_chip->getHuid(), i_rank.getKey() );
            break;
        }

        // Clear all of the counters and maintenance ECC attentions.
        o_rc = prepareNextCmd<TYPE_MBA>( i_chip );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "prepareNextCmd(0x%08x) failed",
                      i_chip->getHuid() );
            break;
        }

        // Start a steer cleanup command.
        mss_TimeBaseScrub cmd { fapiTrgt, saddr, eaddr,
                                mss_MaintCmd::FAST_MAX_BW_IMPACT,
                                stopCond, false };
        errlHndl_t errl;
        FAPI_INVOKE_HWP( errl, cmd.setupAndExecuteCmd );
        if ( nullptr != errl )
        {
            PRDF_ERR( PRDF_FUNC "setupAndExecuteCmd() on 0x%08x,0x%02x failed",
                      i_chip->getHuid(), i_rank.getKey() );
            PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
            o_rc = FAIL; break;
        }

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

template<>
uint32_t startTpsPhase2<TYPE_MBA>( ExtensibleChip * i_chip,
                                   const MemRank & i_rank )
{
    #define PRDF_FUNC "[PlatServices::startTpsPhase2<TYPE_MBA>] "

    PRDF_ASSERT( isInMdiaMode() ); // MDIA must be running.

    PRDF_ASSERT( nullptr != i_chip );
    PRDF_ASSERT( TYPE_MBA == i_chip->getType() );

    uint32_t o_rc = SUCCESS;

    // Get the MBA fapi target
    fapi2::Target<fapi2::TARGET_TYPE_MBA> fapiTrgt ( i_chip->getTrgt() );

    // Get the stop conditions.
    uint32_t stopCond = mss_MaintCmd::STOP_ON_RETRY_CE_ETE          |
                        mss_MaintCmd::STOP_ON_END_ADDRESS           |
                        mss_MaintCmd::ENABLE_CMD_COMPLETE_ATTENTION;

    // Note that we set the stop on RCE ETE flag. This requires us to set a
    // threshold in the MBSTR. Fortunately, MDIA sets the threshold for us when
    // it starts the first command on this MBA.

    do
    {
        // Set up the per-symbol counters to capture soft CEs.
        ExtensibleChip * membChip = getConnectedParent( i_chip, TYPE_MEMBUF );
        const char * reg_str = (0 == i_chip->getPos()) ? "MBA0_MBSTR"
                                                       : "MBA1_MBSTR";
        SCAN_COMM_REGISTER_CLASS * mbstr = membChip->getRegister( reg_str );
        o_rc = mbstr->Read();
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "Read() failed on %s", reg_str );
            break;
        }

        //  Disable per-symbol error counters to count soft CEs
        mbstr->ClearBit(55);
        mbstr->ClearBit(56);
        //  Enable per-symbol error counters to count hard CEs
        mbstr->SetBit(57);

        o_rc = mbstr->Write();
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "Write() failed on %s", reg_str );
            break;
        }

        // Get the address range of the master rank.
        fapi2::buffer<uint64_t> saddr, eaddr;
        o_rc = getMemAddrRange<TYPE_MBA>( i_chip, i_rank, saddr, eaddr,
                                          SLAVE_RANK );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "getMemAddrRange(0x%08x,0x%2x) failed",
                      i_chip->getHuid(), i_rank.getKey() );
            break;
        }

        // Clear all of the counters and maintenance ECC attentions.
        o_rc = prepareNextCmd<TYPE_MBA>( i_chip );
        if ( SUCCESS != o_rc )
        {
            PRDF_ERR( PRDF_FUNC "prepareNextCmd(0x%08x) failed",
                      i_chip->getHuid() );
            break;
        }

        // Start a steer cleanup command.
        mss_TimeBaseScrub cmd { fapiTrgt, saddr, eaddr,
                                mss_MaintCmd::FAST_MAX_BW_IMPACT,
                                stopCond, false };
        errlHndl_t errl;
        FAPI_INVOKE_HWP( errl, cmd.setupAndExecuteCmd );
        if ( nullptr != errl )
        {
            PRDF_ERR( PRDF_FUNC "setupAndExecuteCmd() on 0x%08x,0x%02x failed",
                      i_chip->getHuid(), i_rank.getKey() );
            PRDF_COMMIT_ERRL( errl, ERRL_ACTION_REPORT );
            o_rc = FAIL; break;
        }

    } while (0);

    return o_rc;

    #undef PRDF_FUNC
}

//------------------------------------------------------------------------------

} // end namespace PlatServices

} // end namespace PRDF
