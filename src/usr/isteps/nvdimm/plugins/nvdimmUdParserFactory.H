/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/nvdimm/plugins/nvdimmUdParserFactory.H $       */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2014,2019                        */
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
#if !defined(_NVDIMMUDPARSERFACTORY_H)
#define _NVDIMMUDPARSERFACTORY_H

#include "errludparserfactory.H"
#include "errludP_nvdimm.H"

namespace NVDIMM
{
    class UserDetailsParserFactory
        : public ERRORLOG::ErrlUserDetailsParserFactory
    {
        public:
            UserDetailsParserFactory()
            {
                registerParser<NVDIMM::UdParserNvdimmParms>
                  (NVDIMM_UDT_PARAMETERS);
                registerParser<NVDIMM::UdParserNvdimmOPParms>
                  (NVDIMM_OP_PARAMETERS);
            }

            UserDetailsParserFactory(const UserDetailsParserFactory &) = delete;
            UserDetailsParserFactory & operator=(UserDetailsParserFactory &) = delete;
    };

};

#endif

