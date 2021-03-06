/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/istep16/call_host_activate_slave_cores.C $     */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2019                        */
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

#include    <errl/errlentry.H>
#include    <errno.h>
#include    <initservice/isteps_trace.H>
#include    <isteps/hwpisteperror.H>
#include    <errl/errludtarget.H>

#include    <arch/pirformat.H>
#include    <console/consoleif.H>

//  targeting support
#include    <targeting/common/commontargeting.H>
#include    <targeting/common/utilFilter.H>
#include    <targeting/namedtarget.H>
#include    <fapi2/target.H>
#include    <errl/errlmanager.H>
#include    <sys/task.h>
#include    <sys/misc.h>

#include    <fapi2/plat_hwp_invoker.H>
#include    <p9_check_idle_stop_done.H>

#ifdef CONFIG_IPLTIME_CHECKSTOP_ANALYSIS
  #include <isteps/pm/occCheckstop.H>
#endif

#include    <scom/scomif.H>
#include    <errl/errludprintk.H>
#include    <intr/intr_reasoncodes.H>
#include    <initservice/istepdispatcherif.H>

using   namespace   ERRORLOG;
using   namespace   TARGETING;
using   namespace   ISTEP;
using   namespace   ISTEP_ERROR;
using   namespace   p9_check_idle_stop;

namespace ISTEP_16
{
void* call_host_activate_slave_cores (void *io_pArgs)
{
    IStepError  l_stepError;

    errlHndl_t  l_timeout_errl  =   NULL;
    errlHndl_t  l_errl          =   NULL;


    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
               "call_host_activate_slave_cores entry" );

    // @@@@@    CUSTOM BLOCK:   @@@@@

    //track master group/chip/core (no threads)
    uint64_t l_masterPIR_wo_thread = PIR_t(task_getcpuid()).word &
                                     ~PIR_t::THREAD_MASK;

    TargetHandleList l_cores;
    getAllChiplets(l_cores, TYPE_CORE);
    TARGETING::Target* sys = NULL;
    TARGETING::targetService().getTopLevelTarget(sys);
    assert( sys != NULL );
    uint32_t l_numCores = 0;

    // keep track of which cores started
    TargetHandleList l_startedCores;

    for(TargetHandleList::const_iterator
        l_core = l_cores.begin();
        l_core != l_cores.end();
        ++l_core)
    {
        TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                   "Iterating all cores in system - "
                   "This is core: %d", l_numCores);
        l_numCores += 1;

        ConstTargetHandle_t l_processor = getParentChip(*l_core);

        CHIP_UNIT_ATTR l_coreId =
                (*l_core)->getAttr<TARGETING::ATTR_CHIP_UNIT>();
        FABRIC_GROUP_ID_ATTR l_logicalGroupId =
          l_processor->getAttr<TARGETING::ATTR_FABRIC_GROUP_ID>();
        FABRIC_CHIP_ID_ATTR l_chipId =
          l_processor->getAttr<TARGETING::ATTR_FABRIC_CHIP_ID>();


        const fapi2::Target<fapi2::TARGET_TYPE_CORE> l_fapi2_coreTarget(
              const_cast<TARGETING::Target*> (*l_core));

        //Determine PIR and threads to enable for this core
        uint64_t pir = PIR_t(l_logicalGroupId, l_chipId, l_coreId).word;
        uint64_t en_threads = sys->getAttr<ATTR_ENABLED_THREADS>();
        TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                   "pir for this core is: %lx", pir);

        //If not the master core, skip
        if ((pir & ~PIR_t::THREAD_MASK) != l_masterPIR_wo_thread)
        {
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                       "call_host_activate_slave_cores: Waking %x.",
                       pir );

            int rc = cpu_start_core(pir, en_threads);

            // Workaround to handle some syncing issues with new cpus
            //  waking
            if (-ETIME == rc)
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                        "call_host_activate_slave_cores: "
                        "Time out rc from kernel %d on core 0x%x, resending doorbell",
                        rc,
                        pir);
                rc = cpu_wakeup_core(pir,en_threads);
            }

            // Handle time out error
            uint32_t l_checkidle_eid = 0;
            if (-ETIME == rc)
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                        "call_host_activate_slave_cores: "
                        "Time out rc from kernel %d on core 0x%x",
                        rc,
                        pir);

                // only called if the core doesn't report in
                const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>
                    l_fapi2ProcTarget(
                        const_cast<TARGETING::Target*>(l_processor) );

                TARGETING::ATTR_FAPI_NAME_type l_targName = {0};
                fapi2::toString( l_fapi2ProcTarget,
                                 l_targName,
                                 sizeof(l_targName) );

                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                    "Call p9_check_idle_stop_done on processor %s", l_targName );

                FAPI_INVOKE_HWP( l_timeout_errl,
                                 p9_check_idle_stop_done,
                                 l_fapi2_coreTarget );

                if (l_timeout_errl)
                {
                    TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                          "ERROR : p9_check_idle_stop_done" );

                    // Add chip target info
                    ErrlUserDetailsTarget(l_processor).addToLog(l_timeout_errl);

                    // Create IStep error log
                    l_stepError.addErrorDetails(l_timeout_errl);

                    l_checkidle_eid = l_timeout_errl->eid();

                    // Commit error
                    errlCommit( l_timeout_errl, HWPF_COMP_ID );
                }
            } // End of handle time out error

            // Check if this core failed last time
            ATTR_PREVIOUS_WAKEUP_FAIL_type l_prevFail =
              (*l_core)->getAttr<TARGETING::ATTR_PREVIOUS_WAKEUP_FAIL>();

            // Create predictive error log if this is the first failure
            //   AND the HWP didn't see a problem
            if( (0 != rc) && (l_prevFail == 0) && (l_checkidle_eid == 0) )
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                        "call_host_activate_slave_cores: "
                        "Error from kernel %d on core %x",
                        rc,
                        pir);
                /*@
                  * @errortype
                  * @reasoncode  RC_BAD_RC
                  * @severity    ERRORLOG::ERRL_SEV_UNRECOVERABLE
                  * @moduleid    MOD_HOST_ACTIVATE_SLAVE_CORES
                  * @userdata1   PIR of failing core.
                  * @userdata2[00:31]   EID from p9_check_idle_stop_done().
                  * @userdata2[32:63]   rc of cpu_start_core().
                  *
                  * @devdesc Kernel returned error when trying to activate
                  *          core.
                  */
                l_errl = new ERRORLOG::ErrlEntry(
                             ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                             MOD_HOST_ACTIVATE_SLAVE_CORES,
                             RC_BAD_RC,
                             pir,
                             TWO_UINT32_TO_UINT64(
                                 l_checkidle_eid,
                                 rc) );

                // Going to assume some kind of SW error unless it fails
                //  again
                l_errl->addProcedureCallout( HWAS::EPUB_PRC_HB_CODE,
                                             HWAS::SRCI_PRIORITY_HIGH);

                // Callout core that failed to wake up.
                l_errl->addHwCallout(*l_core,
                        HWAS::SRCI_PRIORITY_LOW,
                        HWAS::NO_DECONFIG,
                        HWAS::GARD_NULL);

                // Could be an interrupt issue
                l_errl->collectTrace(INTR_TRACE_NAME,256);

                // Throw printk in there too in case it is a kernel issue
                ERRORLOG::ErrlUserDetailsPrintk().addToLog(l_errl);

                // Add interesting ISTEP traces
                l_errl->collectTrace(ISTEP_COMP_NAME,256);

                // Choosing to ignore this intermittent error
                l_errl->setSev(ERRORLOG::ERRL_SEV_INFORMATIONAL);
                errlCommit( l_errl, HWPF_COMP_ID );

                // Remember that we failed so we can gard the core if it
                //  happens again on the reboot
                l_prevFail = 1;
                (*l_core)->
                  setAttr<TARGETING::ATTR_PREVIOUS_WAKEUP_FAIL>(l_prevFail);

#ifdef CONFIG_BMC_IPMI
                // Initiate a graceful power cycle
                CONSOLE::displayf(ISTEP_COMP_NAME, "System Rebooting To Retry Recoverable Error");
                CONSOLE::flush();
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,"call_host_activate_slave_cores: requesting power cycle");
                INITSERVICE::requestReboot();
#endif

                break;
            }
            // Create unrecoverable error log if this is a repeat
            //  OR if the HWP hit something
            else if( (0 != rc) &&
                     ((l_prevFail > 0) || (l_checkidle_eid != 0)) )
            {
                TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                           "call_host_activate_slave_cores: "
                           "Core errors during wakeup on core %x",
                           pir);
                /*@
                 * @errortype
                 * @reasoncode  RC_SLAVE_CORE_WAKEUP_ERROR
                 * @severity    ERRORLOG::ERRL_SEV_UNRECOVERABLE
                 * @moduleid    MOD_HOST_ACTIVATE_SLAVE_CORES
                 * @userdata1[00:31]   PIR of failing core.
                 * @userdata2[32:63]   Number of previous failures.
                 * @userdata2[00:31]   EID from p9_check_idle_stop_done().
                 * @userdata2[32:63]   rc of cpu_start_core().
                 *
                 * @devdesc Kernel returned error when trying to activate
                 *          core.
                 */
                l_errl = new ERRORLOG::ErrlEntry(
                               ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                               MOD_HOST_ACTIVATE_SLAVE_CORES,
                               RC_SLAVE_CORE_WAKEUP_ERROR,
                               TWO_UINT32_TO_UINT64(
                                   pir,
                                   l_prevFail),
                               TWO_UINT32_TO_UINT64(
                                   l_checkidle_eid,
                                   rc) );

                // Callout and gard core that failed to wake up.
                l_errl->addHwCallout(*l_core,
                                     HWAS::SRCI_PRIORITY_HIGH,
                                     HWAS::DECONFIG,
                                     HWAS::GARD_Predictive);

                // Could be an interrupt issue
                l_errl->collectTrace(INTR_TRACE_NAME,256);

                // Throw printk in there too in case it is a kernel issue
                ERRORLOG::ErrlUserDetailsPrintk().addToLog(l_errl);

                // Add interesting ISTEP traces
                l_errl->collectTrace(ISTEP_COMP_NAME,256);

                l_stepError.addErrorDetails( l_errl );
                errlCommit( l_errl, HWPF_COMP_ID );

                // We garded the core so we should zero out the fail
                //  counter so the replacement doesn't get blamed
                l_prevFail = 0;
                (*l_core)->
                  setAttr<TARGETING::ATTR_PREVIOUS_WAKEUP_FAIL>(l_prevFail);

                break;
            }
            // Zero out the counter if we passed 
            else if( l_prevFail > 0 )
            {
                // Add to the list of passing cores so we can
                //  clear ATTR_PREVIOUS_WAKEUP_FAIL later
                l_startedCores.push_back(*l_core);
            }
        }
    }

    // Clear out the wakeup_fail indicators only after every core has passed.
    //  Doing this outside the loop helps mitigate the (unlikely) case where
    //  a failure bounces between different cores on several consecutive boots.
    for(TargetHandleList::const_iterator
        l_core = l_startedCores.begin();
        l_core != l_startedCores.end();
        ++l_core)
    {
        TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                   "call_host_activate_slave_cores: "
                   "Resetting failure count for core %.8X",
                   TARGETING::get_huid(*l_core) );
        ATTR_PREVIOUS_WAKEUP_FAIL_type l_prevFail = 0;
        (*l_core)->
          setAttr<TARGETING::ATTR_PREVIOUS_WAKEUP_FAIL>(l_prevFail);
    }

#if defined(CONFIG_IPLTIME_CHECKSTOP_ANALYSIS) && !defined(__HOSTBOOT_RUNTIME)
    if( l_stepError.isNull() )
    {
        // update firdata inputs for OCC
        TARGETING::Target* masterproc = NULL;
        TARGETING::targetService().masterProcChipTargetHandle(masterproc);
        l_errl = HBOCC::loadHostDataToSRAM(masterproc,
                                            PRDF::ALL_HARDWARE);
        if (l_errl)
        {
            TRACFCOMP(ISTEPS_TRACE::g_trac_isteps_trace,
                    "Error returned from call to HBOCC::loadHostDataToSRAM");

            //Create IStep error log and cross reference error that occurred
            l_stepError.addErrorDetails(l_errl);

            // Commit Error
            errlCommit(l_errl, HWPF_COMP_ID);
        }
    }
#endif

    //Set SKIP_WAKEUP to false after all cores are powered on (16.2)
    //If this is not set false, PM_RESET will fail to enable special wakeup.
    // PM_RESET is expected to enable special_wakeup after all the cores powered on
    sys->setAttr<ATTR_SKIP_WAKEUP>(0);

    // Now that the slave cores are running, we need to include them in
    //  multicast scom operations
    SCOM::enableSlaveCoreMulticast();

    TRACDCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
               "call_host_activate_slave_cores exit" );

    // end task, returning any errorlogs to IStepDisp
    return l_stepError.getErrorHandle();
}

};
