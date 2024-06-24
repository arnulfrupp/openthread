/*
 *  Copyright (c) 2024, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "rcp_caps_diag.hpp"

#if OPENTHREAD_POSIX_CONFIG_RCP_CAPS_DIAG_ENABLE
namespace ot {
namespace Posix {

#define SPINEL_ENTRY(aCategory, aCommand, aKey)                                      \
    {                                                                                \
        aCategory, aCommand, aKey, &RcpCapsDiag::HandleSpinelCommand<aCommand, aKey> \
    }

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_CAPS>(void)
{
    static constexpr uint8_t kCapsBufferSize = 100;
    uint8_t                  capsBuffer[kCapsBufferSize];
    spinel_size_t            capsLength = sizeof(capsBuffer);

    return mRadioSpinel.Get(SPINEL_PROP_CAPS, SPINEL_DATATYPE_DATA_S, capsBuffer, &capsLength);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PROTOCOL_VERSION>(void)
{
    unsigned int versionMajor;
    unsigned int versionMinor;

    return mRadioSpinel.Get(SPINEL_PROP_PROTOCOL_VERSION, (SPINEL_DATATYPE_UINT_PACKED_S SPINEL_DATATYPE_UINT_PACKED_S),
                            &versionMajor, &versionMinor);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RADIO_CAPS>(void)
{
    unsigned int radioCaps;

    return mRadioSpinel.Get(SPINEL_PROP_RADIO_CAPS, SPINEL_DATATYPE_UINT_PACKED_S, &radioCaps);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_API_VERSION>(void)
{
    unsigned int rcpApiVersion;

    return mRadioSpinel.Get(SPINEL_PROP_RCP_API_VERSION, SPINEL_DATATYPE_UINT_PACKED_S, &rcpApiVersion);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_NCP_VERSION>(void)
{
    static constexpr uint16_t kVersionStringSize = 128;
    char                      mVersion[kVersionStringSize];

    return mRadioSpinel.Get(SPINEL_PROP_NCP_VERSION, SPINEL_DATATYPE_UTF8_S, mVersion, sizeof(mVersion));
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_CHAN>(void)
{
    static constexpr uint8_t kPhyChannel = 22;

    return mRadioSpinel.Set(SPINEL_PROP_PHY_CHAN, SPINEL_DATATYPE_UINT8_S, kPhyChannel);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_ENABLED>(void)
{
    return mRadioSpinel.Set(SPINEL_PROP_PHY_ENABLED, SPINEL_DATATYPE_BOOL_S, true /* aEnable*/);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_15_4_PANID>(void)
{
    static constexpr uint16_t kPanId = 0x1234;

    return mRadioSpinel.SetPanId(kPanId);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_15_4_LADDR>(void)
{
    static constexpr otExtAddress kExtAddress = {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}};

    return mRadioSpinel.Set(SPINEL_PROP_MAC_15_4_LADDR, SPINEL_DATATYPE_EUI64_S, kExtAddress.m8);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_15_4_SADDR>(void)
{
    static constexpr uint16_t kShortAddress = 0x1100;

    return mRadioSpinel.Set(SPINEL_PROP_MAC_15_4_SADDR, SPINEL_DATATYPE_UINT16_S, kShortAddress);
}

template <>
otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_RAW_STREAM_ENABLED>(void)
{
    return mRadioSpinel.Set(SPINEL_PROP_MAC_RAW_STREAM_ENABLED, SPINEL_DATATYPE_BOOL_S, true);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SCAN_MASK>(void)
{
    static constexpr uint8_t kScanChannel = 20;

    return mRadioSpinel.Set(SPINEL_PROP_MAC_SCAN_MASK, SPINEL_DATATYPE_DATA_S, &kScanChannel, sizeof(uint8_t));
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SCAN_PERIOD>(void)
{
    static constexpr uint16_t kScanDuration = 1;

    return mRadioSpinel.Set(SPINEL_PROP_MAC_SCAN_PERIOD, SPINEL_DATATYPE_UINT16_S, kScanDuration);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SCAN_STATE>(void)
{
    return mRadioSpinel.Set(SPINEL_PROP_MAC_SCAN_STATE, SPINEL_DATATYPE_UINT8_S, SPINEL_SCAN_STATE_ENERGY);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SRC_MATCH_ENABLED>(void)
{
    return mRadioSpinel.Set(SPINEL_PROP_MAC_SRC_MATCH_ENABLED, SPINEL_DATATYPE_BOOL_S, true);
}

template <>
otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES>(void)
{
    return mRadioSpinel.Set(SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES, nullptr);
}

template <>
otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES>(void)
{
    return mRadioSpinel.Set(SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES, nullptr);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_HWADDR>(void)
{
    otExtAddress ieeeEui64;

    return mRadioSpinel.Get(SPINEL_PROP_HWADDR, SPINEL_DATATYPE_EUI64_S, ieeeEui64.m8);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_CHAN_PREFERRED>(void)
{
    static constexpr uint8_t kChannelMaskBufferSize = 32;
    uint8_t                  maskBuffer[kChannelMaskBufferSize];
    spinel_size_t            maskLength = sizeof(maskBuffer);

    return mRadioSpinel.Get(SPINEL_PROP_PHY_CHAN_PREFERRED, SPINEL_DATATYPE_DATA_S, maskBuffer, &maskLength);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_CHAN_SUPPORTED>(void)
{
    static constexpr uint8_t kChannelMaskBufferSize = 32;
    uint8_t                  maskBuffer[kChannelMaskBufferSize];
    spinel_size_t            maskLength = sizeof(maskBuffer);

    return mRadioSpinel.Get(SPINEL_PROP_PHY_CHAN_SUPPORTED, SPINEL_DATATYPE_DATA_S, maskBuffer, &maskLength);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_RSSI>(void)
{
    int8_t rssi;

    return mRadioSpinel.Get(SPINEL_PROP_PHY_RSSI, SPINEL_DATATYPE_INT8_S, &rssi);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_RX_SENSITIVITY>(void)
{
    int8_t rxSensitivity;

    return mRadioSpinel.Get(SPINEL_PROP_PHY_RX_SENSITIVITY, SPINEL_DATATYPE_INT8_S, &rxSensitivity);
}

template <>
otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_INSERT, SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES>(void)
{
    static constexpr uint16_t kShortAddress = 0x1122;

    return mRadioSpinel.Insert(SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES, SPINEL_DATATYPE_UINT16_S, kShortAddress);
}

template <>
otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_INSERT, SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES>(
    void)
{
    static constexpr otExtAddress kExtAddress = {{0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}};

    return mRadioSpinel.Insert(SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES, SPINEL_DATATYPE_EUI64_S, kExtAddress.m8);
}

template <>
otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_REMOVE, SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES>(void)
{
    static constexpr uint16_t kShortAddress = 0x1122;

    return mRadioSpinel.Remove(SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES, SPINEL_DATATYPE_UINT16_S, kShortAddress);
}

template <>
otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_REMOVE, SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES>(
    void)
{
    static constexpr otExtAddress extAddress = {{0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}};

    return mRadioSpinel.Remove(SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES, SPINEL_DATATYPE_EUI64_S, extAddress.m8);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_RCP_ENH_ACK_PROBING>(void)
{
    static constexpr uint16_t     kShortAddress = 0x1122;
    static constexpr otExtAddress kExtAddress   = {{0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}};
    static constexpr uint8_t      kFlags        = SPINEL_THREAD_LINK_METRIC_PDU_COUNT | SPINEL_THREAD_LINK_METRIC_LQI |
                                      SPINEL_THREAD_LINK_METRIC_LINK_MARGIN | SPINEL_THREAD_LINK_METRIC_RSSI;

    return mRadioSpinel.Set(SPINEL_PROP_RCP_ENH_ACK_PROBING,
                            SPINEL_DATATYPE_UINT16_S SPINEL_DATATYPE_EUI64_S SPINEL_DATATYPE_UINT8_S, kShortAddress,
                            kExtAddress.m8, kFlags);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_RCP_MAC_FRAME_COUNTER>(void)
{
    static constexpr uint32_t kMacFrameCounter = 1;

    return mRadioSpinel.Set(SPINEL_PROP_RCP_MAC_FRAME_COUNTER, SPINEL_DATATYPE_UINT32_S SPINEL_DATATYPE_BOOL_S,
                            kMacFrameCounter, true /*aSetIfLarger*/);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_RCP_MAC_KEY>(void)
{
    static constexpr uint8_t keyIdMode1 = 1 << 3;
    static constexpr uint8_t keyId      = 100;
    otMacKeyMaterial         prevKey;
    otMacKeyMaterial         curKey;
    otMacKeyMaterial         nextKey;

    memset(prevKey.mKeyMaterial.mKey.m8, 0x11, OT_MAC_KEY_SIZE);
    memset(curKey.mKeyMaterial.mKey.m8, 0x22, OT_MAC_KEY_SIZE);
    memset(nextKey.mKeyMaterial.mKey.m8, 0x33, OT_MAC_KEY_SIZE);
    return mRadioSpinel.SetMacKey(keyIdMode1, keyId, &prevKey, &curKey, &nextKey);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_CSL_ACCURACY>(void)
{
    uint8_t accuracy;

    return mRadioSpinel.Get(SPINEL_PROP_RCP_CSL_ACCURACY, SPINEL_DATATYPE_UINT8_S, &accuracy);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_CSL_UNCERTAINTY>(void)
{
    uint8_t uncertainty;

    return mRadioSpinel.Get(SPINEL_PROP_RCP_CSL_UNCERTAINTY, SPINEL_DATATYPE_UINT8_S, &uncertainty);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_TIMESTAMP>(void)
{
    uint64_t       remoteTimestamp = 0;
    uint8_t        buffer[sizeof(remoteTimestamp)];
    spinel_ssize_t packed;

    packed = spinel_datatype_pack(buffer, sizeof(buffer), SPINEL_DATATYPE_UINT64_S, remoteTimestamp);
    return mRadioSpinel.GetWithParam(SPINEL_PROP_RCP_TIMESTAMP, buffer, static_cast<spinel_size_t>(packed),
                                     SPINEL_DATATYPE_UINT64_S, &remoteTimestamp);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_PROMISCUOUS_MODE>(void)
{
    return mRadioSpinel.SetPromiscuous(false /* aEnable*/);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_CCA_THRESHOLD>(void)
{
    int8_t ccaThreshold;

    return mRadioSpinel.Get(SPINEL_PROP_PHY_CCA_THRESHOLD, SPINEL_DATATYPE_INT8_S, &ccaThreshold);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_FEM_LNA_GAIN>(void)
{
    int8_t gain;

    return mRadioSpinel.Get(SPINEL_PROP_PHY_FEM_LNA_GAIN, SPINEL_DATATYPE_INT8_S, &gain);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_REGION_CODE>(void)
{
    uint16_t regionCode;

    return mRadioSpinel.Get(SPINEL_PROP_PHY_REGION_CODE, SPINEL_DATATYPE_UINT16_S, &regionCode);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_TX_POWER>(void)
{
    int8_t power;

    return mRadioSpinel.Get(SPINEL_PROP_PHY_TX_POWER, SPINEL_DATATYPE_INT8_S, &power);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RADIO_COEX_ENABLE>(void)
{
    bool enabled;

    return mRadioSpinel.Get(SPINEL_PROP_RADIO_COEX_ENABLE, SPINEL_DATATYPE_BOOL_S, &enabled);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RADIO_COEX_METRICS>(void)
{
    otRadioCoexMetrics coexMetrics;

    return mRadioSpinel.GetCoexMetrics(coexMetrics);
}

template <>
otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_MIN_HOST_API_VERSION>(void)
{
    unsigned int minHostRcpApiVersion;

    return mRadioSpinel.Get(SPINEL_PROP_RCP_MIN_HOST_API_VERSION, SPINEL_DATATYPE_UINT_PACKED_S, &minHostRcpApiVersion);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_CCA_THRESHOLD>(void)
{
    static constexpr int8_t kCcaThreshold = -75;

    return mRadioSpinel.Set(SPINEL_PROP_PHY_CCA_THRESHOLD, SPINEL_DATATYPE_INT8_S, kCcaThreshold);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_CHAN_MAX_POWER>(void)
{
    static constexpr uint8_t kChannel  = 20;
    static constexpr uint8_t kMaxPower = 10;

    return mRadioSpinel.Set(SPINEL_PROP_PHY_CHAN_MAX_POWER, SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_INT8_S, kChannel,
                            kMaxPower);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_CHAN_TARGET_POWER>(void)
{
    static constexpr uint8_t  kChannel     = 20;
    static constexpr uint16_t kTargetPower = 1000;

    return mRadioSpinel.Set(SPINEL_PROP_PHY_CHAN_TARGET_POWER, SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_INT16_S,
                            kChannel, kTargetPower);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_FEM_LNA_GAIN>(void)
{
    static constexpr int8_t kFemLnaGain = 0;

    return mRadioSpinel.Set(SPINEL_PROP_PHY_FEM_LNA_GAIN, SPINEL_DATATYPE_INT8_S, kFemLnaGain);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_REGION_CODE>(void)
{
    static constexpr uint16_t kRegionCode = 0x5757;

    return mRadioSpinel.Set(SPINEL_PROP_PHY_REGION_CODE, SPINEL_DATATYPE_UINT16_S, kRegionCode);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_TX_POWER>(void)
{
    static constexpr int8_t kTransmitPower = 10;

    return mRadioSpinel.Set(SPINEL_PROP_PHY_TX_POWER, SPINEL_DATATYPE_INT8_S, kTransmitPower);
}

template <> otError RcpCapsDiag::HandleSpinelCommand<SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_RADIO_COEX_ENABLE>(void)
{
    return mRadioSpinel.Set(SPINEL_PROP_RADIO_COEX_ENABLE, SPINEL_DATATYPE_BOOL_S, true /* aEnabled*/);
}

const struct RcpCapsDiag::SpinelEntry RcpCapsDiag::sSpinelEntries[] = {
    //  Basic Spinel commands
    SPINEL_ENTRY(kCategoryBasic, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_CAPS),
    SPINEL_ENTRY(kCategoryBasic, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PROTOCOL_VERSION),
    SPINEL_ENTRY(kCategoryBasic, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RADIO_CAPS),
    SPINEL_ENTRY(kCategoryBasic, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_API_VERSION),
    SPINEL_ENTRY(kCategoryBasic, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_NCP_VERSION),

    // Thread Version >= 1.1
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_CHAN),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_ENABLED),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_15_4_PANID),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_15_4_LADDR),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_15_4_SADDR),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_RAW_STREAM_ENABLED),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SCAN_MASK),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SCAN_PERIOD),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SCAN_STATE),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SRC_MATCH_ENABLED),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_HWADDR),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_CHAN_PREFERRED),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_CHAN_SUPPORTED),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_RSSI),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_RX_SENSITIVITY),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_INSERT, SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_INSERT, SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_REMOVE, SPINEL_PROP_MAC_SRC_MATCH_SHORT_ADDRESSES),
    SPINEL_ENTRY(kCategoryThread1_1, SPINEL_CMD_PROP_VALUE_REMOVE, SPINEL_PROP_MAC_SRC_MATCH_EXTENDED_ADDRESSES),

    // Thread Version >= 1.2
    SPINEL_ENTRY(kCategoryThread1_2, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_RCP_ENH_ACK_PROBING),
    SPINEL_ENTRY(kCategoryThread1_2, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_RCP_MAC_FRAME_COUNTER),
    SPINEL_ENTRY(kCategoryThread1_2, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_RCP_MAC_KEY),
    SPINEL_ENTRY(kCategoryThread1_2, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_CSL_ACCURACY),
    SPINEL_ENTRY(kCategoryThread1_2, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_CSL_UNCERTAINTY),
    SPINEL_ENTRY(kCategoryThread1_2, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_TIMESTAMP),

    // Utils
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_MAC_PROMISCUOUS_MODE),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_CCA_THRESHOLD),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_FEM_LNA_GAIN),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_REGION_CODE),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_PHY_TX_POWER),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RADIO_COEX_ENABLE),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RADIO_COEX_METRICS),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_GET, SPINEL_PROP_RCP_MIN_HOST_API_VERSION),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_CCA_THRESHOLD),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_CHAN_MAX_POWER),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_CHAN_TARGET_POWER),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_FEM_LNA_GAIN),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_REGION_CODE),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_PHY_TX_POWER),
    SPINEL_ENTRY(kCategoryUtils, SPINEL_CMD_PROP_VALUE_SET, SPINEL_PROP_RADIO_COEX_ENABLE),
};

otError RcpCapsDiag::DiagProcess(char *aArgs[], uint8_t aArgsLength)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(aArgsLength == 2, error = OT_ERROR_INVALID_ARGS);

    if (strcmp(aArgs[1], "spinel") == 0)
    {
        ProcessSpinel();
    }
    else
    {
        error = OT_ERROR_INVALID_COMMAND;
    }

exit:
    return error;
}

void RcpCapsDiag::ProcessSpinel(void)
{
    for (uint8_t i = 0; i < kNumCategories; i++)
    {
        TestSpinelCommands(static_cast<Category>(i));
    }
}

void RcpCapsDiag::TestSpinelCommands(Category aCategory)
{
    otError error;

    Output("\r\n%s :\r\n", CategoryToString(aCategory));

    for (const SpinelEntry &entry : sSpinelEntries)
    {
        if (entry.mCategory != aCategory)
        {
            continue;
        }

        error = (this->*entry.mHandler)();
        OutputResult(entry, error);
    }
}

void RcpCapsDiag::SetDiagOutputCallback(otPlatDiagOutputCallback aCallback, void *aContext)
{
    mOutputCallback = aCallback;
    mOutputContext  = aContext;
}

void RcpCapsDiag::OutputResult(const SpinelEntry &aEntry, otError error)
{
    static constexpr uint8_t  kSpaceLength            = 1;
    static constexpr uint8_t  kMaxCommandStringLength = 20;
    static constexpr uint8_t  kMaxKeyStringLength     = 35;
    static constexpr uint16_t kMaxLength              = kMaxCommandStringLength + kMaxKeyStringLength + kSpaceLength;
    static const char         kPadding[]              = "----------------------------------------------------------";
    const char               *commandString           = spinel_command_to_cstr(aEntry.mCommand);
    const char               *keyString               = spinel_prop_key_to_cstr(aEntry.mKey);
    uint16_t actualLength  = static_cast<uint16_t>(strlen(commandString) + strlen(keyString) + kSpaceLength);
    uint16_t paddingOffset = (actualLength > kMaxLength) ? kMaxLength : actualLength;

    static_assert(kMaxLength < sizeof(kPadding), "Padding bytes are too short");

    Output("%.*s %.*s %s %s\r\n", kMaxCommandStringLength, commandString, kMaxKeyStringLength, keyString,
           &kPadding[paddingOffset], otThreadErrorToString(error));
}

void RcpCapsDiag::Output(const char *aFormat, ...)
{
    va_list args;

    va_start(args, aFormat);

    if (mOutputCallback != nullptr)
    {
        mOutputCallback(aFormat, args, mOutputContext);
    }

    va_end(args);
}

const char *RcpCapsDiag::CategoryToString(Category aCategory)
{
    static const char *const kCategoryStrings[] = {
        "Basic",                 // (0) kCategoryBasic
        "Thread Version >= 1.1", // (1) kCategoryThread1_1
        "Thread Version >= 1.2", // (2) kCategoryThread1_2
        "Utils",                 // (3) kCategoryUtils
    };

    static_assert(kCategoryBasic == 0, "kCategoryBasic value is incorrect");
    static_assert(kCategoryThread1_1 == 1, "kCategoryThread1_1 value is incorrect");
    static_assert(kCategoryThread1_2 == 2, "kCategoryThread1_2 value is incorrect");
    static_assert(kCategoryUtils == 3, "kCategoryUtils value is incorrect");

    return (aCategory < OT_ARRAY_LENGTH(kCategoryStrings)) ? kCategoryStrings[aCategory] : "invalid";
}

} // namespace Posix
} // namespace ot
#endif // OPENTHREAD_POSIX_CONFIG_RCP_CAPS_DIAG_ENABLE