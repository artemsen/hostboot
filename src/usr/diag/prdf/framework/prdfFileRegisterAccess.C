/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/diag/prdf/framework/prdfFileRegisterAccess.C $        */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2019                        */
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

#include "prdfFileRegisterAccess.H"
#include <prdfTrace.H>
#include <prdfPnorFirDataReader.H>
#include <prdfBitString.H>

using namespace TARGETING;

namespace PRDF
{

uint32_t FileScomAccessor::Access(
                            TargetHandle_t i_target,
                            BitString & bs,
                            uint64_t registerId,
                            RegisterAccess::Operation operation) const
{
    #define PRDF_FUNC "[FileScomAccessor::Access()] "

    uint32_t rc = SUCCESS;

    PnorFirDataReader & firData = PnorFirDataReader::getPnorFirDataReader();
    uint64_t data = 0;

    switch (operation)
    {
        case RegisterAccess::WRITE:
            // TODO: RTC 62076 move BitString class to 64-bit
            data = (((uint64_t)bs.getFieldJustify( 0, 32)) << 32) |
                    ((uint64_t)bs.getFieldJustify(32, 32));
            firData.putScom( i_target, registerId, data);
            break;
        case RegisterAccess::READ:
            firData.getScom( i_target, registerId, data);
            // TODO: RTC 62076 move BitString class to 64-bit
            bs.setFieldJustify( 0, 32, data >> 32);
            bs.setFieldJustify(32, 32, data      );
            break;
        default:
            PRDF_ERR(PRDF_FUNC "Wrong Operation:%u", operation);
    }

    return rc;

    #undef PRDF_FUNC
}

} // End namespace PRDF
