/*
 *  Copyright (c) 2017-2021, The OpenThread Authors.
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

#ifndef DNS_CLIENT_HPP_
#define DNS_CLIENT_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE

#include <openthread/dns_client.h>

#include "common/as_core_type.hpp"
#include "common/clearable.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "net/dns_types.hpp"
#include "net/ip6.hpp"
#include "net/netif.hpp"

/**
 * @file
 *   This file includes definitions for the DNS client.
 */

#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE

#if !OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#error "DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE requires OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE"
#endif

#if !OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE
#error "DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE requires OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE"
#endif

#endif

#if !OPENTHREAD_CONFIG_TCP_ENABLE && OPENTHREAD_CONFIG_DNS_CLIENT_OVER_TCP_ENABLE
#error "OPENTHREAD_CONFIG_DNS_CLIENT_OVER_TCP_ENABLE requires OPENTHREAD_CONFIG_TCP_ENABLE"
#endif

/**
 * Represents an opaque (and empty) type for a response to an address resolution DNS query.
 */
struct otDnsAddressResponse
{
};

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

/**
 * Represents an opaque (and empty) type for a response to browse (service instance enumeration) DNS query.
 */
struct otDnsBrowseResponse
{
};

/**
 * Represents an opaque (and empty) type for a response to service inst resolution DNS query.
 */
struct otDnsServiceResponse
{
};

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE

/**
 * Represents an opaque (and empty) type for a response to arbitrary record DNS query.
 */
struct otDnsRecordResponse
{
};

#endif

namespace ot {

namespace Srp {
class Client;
}

namespace Dns {

/**
 * Implements DNS client.
 */
class Client : public InstanceLocator, private NonCopyable
{
    friend class ot::Srp::Client;

    typedef Message Query; // `Message` is used to save `Query` related info.

public:
    /**
     * Represents a DNS query configuration (e.g., server address, response wait timeout, etc).
     */
    class QueryConfig : public otDnsQueryConfig, public Clearable<QueryConfig>
    {
        friend class Client;

    public:
        /**
         * Type represents the "Recursion Desired" (RD) flag in a `otDnsQueryConfig`.
         */
        enum RecursionFlag : uint8_t
        {
            kFlagUnspecified      = OT_DNS_FLAG_UNSPECIFIED,       ///< The flag is not specified.
            kFlagRecursionDesired = OT_DNS_FLAG_RECURSION_DESIRED, ///< Server can resolve the query recursively.
            kFlagNoRecursion      = OT_DNS_FLAG_NO_RECURSION,      ///< Server can not resolve the query recursively.
        };

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        /**
         * Type represents the NAT64 mode.
         */
        enum Nat64Mode : uint8_t
        {
            kNat64Unspecified = OT_DNS_NAT64_UNSPECIFIED, ///< NAT64 mode is not specified. Use default NAT64 mode.
            kNat64Allow       = OT_DNS_NAT64_ALLOW,       ///< Allow NAT64 address translation.
            kNat64Disallow    = OT_DNS_NAT64_DISALLOW,    ///< Disallow NAT64 address translation.
        };
#endif

        /**
         * Type represents the service resolution mode.
         */
        enum ServiceMode : uint8_t
        {
            kServiceModeUnspecified    = OT_DNS_SERVICE_MODE_UNSPECIFIED,      ///< Unspecified. Use default.
            kServiceModeSrv            = OT_DNS_SERVICE_MODE_SRV,              ///< SRV record only.
            kServiceModeTxt            = OT_DNS_SERVICE_MODE_TXT,              ///< TXT record only.
            kServiceModeSrvTxt         = OT_DNS_SERVICE_MODE_SRV_TXT,          ///< SRV and TXT same msg.
            kServiceModeSrvTxtSeparate = OT_DNS_SERVICE_MODE_SRV_TXT_SEPARATE, ///< SRV and TXT separate msgs.
            kServiceModeSrvTxtOptimize = OT_DNS_SERVICE_MODE_SRV_TXT_OPTIMIZE, ///< Same msg first, if fail separate.
        };

        /**
         * Type represents the DNS transport protocol selection.
         */
        enum TransportProto : uint8_t
        {
            kDnsTransportUnspecified = OT_DNS_TRANSPORT_UNSPECIFIED, /// Dns transport is unspecified.
            kDnsTransportUdp         = OT_DNS_TRANSPORT_UDP,         /// Dns query should be sent via UDP.
            kDnsTransportTcp         = OT_DNS_TRANSPORT_TCP,         /// Dns query should be sent via TCP.
        };

        /**
         * This is the default constructor for `QueryConfig` object.
         */
        QueryConfig(void) = default;

        /**
         * Gets the server socket address (IPv6 address and port number).
         *
         * @returns The server socket address.
         */
        const Ip6::SockAddr &GetServerSockAddr(void) const
        {
            return static_cast<const Ip6::SockAddr &>(mServerSockAddr);
        }

        /**
         * Gets the wait time to receive response from server (in msec).
         *
         * @returns The timeout interval in msec.
         */
        uint32_t GetResponseTimeout(void) const { return mResponseTimeout; }

        /**
         * Gets the maximum number of query transmit attempts before reporting failure.
         *
         * @returns The maximum number of query transmit attempts.
         */
        uint8_t GetMaxTxAttempts(void) const { return mMaxTxAttempts; }

        /**
         * Gets the recursion flag indicating whether the server can resolve the query recursively or not.
         *
         * @returns The recursion flag.
         */
        RecursionFlag GetRecursionFlag(void) const { return static_cast<RecursionFlag>(mRecursionFlag); }

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        /**
         * Gets the NAT64 mode.
         *
         * @returns The NAT64 mode.
         */
        Nat64Mode GetNat64Mode(void) const { return static_cast<Nat64Mode>(mNat64Mode); }
#endif
        /**
         * Gets the service resolution mode.
         *
         * @returns The service resolution mode.
         */
        ServiceMode GetServiceMode(void) const { return static_cast<ServiceMode>(mServiceMode); }

        /**
         * Gets the transport protocol.
         *
         * @returns The transport protocol.
         */
        TransportProto GetTransportProto(void) const { return static_cast<TransportProto>(mTransportProto); };

    private:
        static constexpr uint32_t kDefaultResponseTimeout = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RESPONSE_TIMEOUT;
        static constexpr uint16_t kDefaultServerPort      = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_PORT;
        static constexpr uint8_t  kDefaultMaxTxAttempts   = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_MAX_TX_ATTEMPTS;
        static constexpr bool kDefaultRecursionDesired    = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RECURSION_DESIRED_FLAG;
        static constexpr ServiceMode kDefaultServiceMode =
            static_cast<ServiceMode>(OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVICE_MODE);

        static_assert(kDefaultServiceMode != kServiceModeUnspecified, "Invalid default service mode");

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        static constexpr bool kDefaultNat64Allowed = OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_NAT64_ALLOWED;
#endif

        enum InitMode : uint8_t
        {
            kInitFromDefaults,
        };

        static const char kDefaultServerAddressString[];

        explicit QueryConfig(InitMode aMode);

        Ip6::SockAddr &GetServerSockAddr(void) { return AsCoreType(&mServerSockAddr); }

        void SetResponseTimeout(uint32_t aResponseTimeout) { mResponseTimeout = aResponseTimeout; }
        void SetMaxTxAttempts(uint8_t aMaxTxAttempts) { mMaxTxAttempts = aMaxTxAttempts; }
        void SetRecursionFlag(RecursionFlag aFlag) { mRecursionFlag = static_cast<otDnsRecursionFlag>(aFlag); }
        void SetServiceMode(ServiceMode aMode) { mServiceMode = static_cast<otDnsServiceMode>(aMode); }
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        void SetNat64Mode(Nat64Mode aMode) { mNat64Mode = static_cast<otDnsNat64Mode>(aMode); }
#endif
        void SetTransportProto(TransportProto aTransportProto)
        {
            mTransportProto = static_cast<otDnsTransportProto>(aTransportProto);
        }

        void SetFrom(const QueryConfig *aConfig, const QueryConfig &aDefaultConfig);
    };

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    /**
     * Provides info for a DNS service instance.
     */
    typedef otDnsServiceInfo ServiceInfo;
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE
    /**
     * Provides info for a DNS record in a query response.
     */
    class RecordInfo : public otDnsRecordInfo
    {
    public:
        /**
         * Represents a section in a DNS response.
         */
        enum RecordSection : uint8_t
        {
            kSectionAnswer     = OT_DNS_SECTION_ANSWER,     ///< Answer section.
            kSectionAuthority  = OT_DNS_SECTION_AUTHORITY,  ///< Authority section.
            kSectionAdditional = OT_DNS_SECTION_ADDITIONAL, ///< Additional section.
        };
    };
#endif

    /**
     * Represents a DNS query response.
     */
    class Response : public otDnsAddressResponse,
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
                     public otDnsBrowseResponse,
                     public otDnsServiceResponse,
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE
                     public otDnsRecordResponse,
#endif
                     public Clearable<Response>
    {
        friend class Client;

    protected:
        enum Section : uint8_t
        {
            kAnswerSection,
            kAdditionalDataSection,
        };

        Response(void) { Clear(); }

        Error GetName(char *aNameBuffer, uint16_t aNameBufferSize) const;
        void  SelectSection(Section aSection, uint16_t &aOffset, uint16_t &aNumRecord) const;
        Error CheckForHostNameAlias(Section aSection, Name &aHostName) const;
        Error FindHostAddress(Section       aSection,
                              const Name   &aHostName,
                              uint16_t      aIndex,
                              Ip6::Address &aAddress,
                              uint32_t     &aTtl) const;
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        Error FindARecord(Section aSection, const Name &aHostName, uint16_t aIndex, ARecord &aARecord) const;
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
        void  InitServiceInfo(ServiceInfo &aServiceInfo) const;
        Error ReadServiceInfo(Section aSection, const Name &aName, ServiceInfo &aServiceInfo) const;
        Error ReadTxtRecord(Section aSection, const Name &aName, ServiceInfo &aServiceInfo) const;
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE
        Error ReadRecordInfo(uint16_t aIndex, RecordInfo &aRecordInfo) const;
#endif
        void PopulateFrom(const Message &aMessage);

        Instance      *mInstance;              // The OpenThread instance.
        Query         *mQuery;                 // The associated query.
        const Message *mMessage;               // The response message.
        Response      *mNext;                  // The next response when we have related queries.
        uint16_t       mAnswerOffset;          // Answer section offset in `mMessage`.
        uint16_t       mAnswerRecordCount;     // Number of records in answer section.
        uint16_t       mAuthorityOffset;       // Authority section offset in `mMessage`.
        uint16_t       mAuthorityRecordCount;  // Number of records in authority section.
        uint16_t       mAdditionalOffset;      // Additional data section offset in `mMessage`.
        uint16_t       mAdditionalRecordCount; // Number of records in additional data section.
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        // This flag is only used in an IPv6 address query response.
        // It indicates that the response does not contain any IPv6
        // addresses but server provided at least one IPv4 address
        // in the additional data section for NAT64 address synthesis.
        bool mIp6QueryResponseRequiresNat64;
#endif
    };

    /**
     * Represents the function pointer callback which is called when a DNS response for an address resolution
     * query is received.
     */
    typedef otDnsAddressCallback AddressCallback;

    /**
     * Represents an address resolution query DNS response.
     */
    class AddressResponse : public Response
    {
        friend class Client;

    public:
        /**
         * Gets the host name associated with an address resolution DNS response.
         *
         * MUST only be used from `AddressCallback`.
         *
         * @param[out] aNameBuffer       A buffer to char array to output the host name.
         * @param[in]  aNameBufferSize   The size of @p aNameBuffer.
         *
         * @retval kErrorNone    The host name was read successfully.
         * @retval kErrorNoBufs  The name does not fit in @p aNameBuffer.
         */
        Error GetHostName(char *aNameBuffer, uint16_t aNameBufferSize) const
        {
            return GetName(aNameBuffer, aNameBufferSize);
        }

        /**
         * Gets the IPv6 address associated with an address resolution DNS response.
         *
         * MUST only be used from `AddressCallback`.
         *
         * The response may include multiple IPv6 address records. @p aIndex can be used to iterate through the list of
         * addresses. Index zero gets the the first address and so on. When we reach end of the list, this method
         * returns `kErrorNotFound`.
         *
         * @param[in]  aIndex        The address record index to retrieve.
         * @param[out] aAddress      A reference to an IPv6 address to output the address.
         * @param[out] aTtl          A reference to a `uint32_t` to output TTL for the address.
         *
         * @retval kErrorNone          The address was read successfully.
         * @retval kErrorNotFound      No address record at @p aIndex.
         * @retval kErrorParse         Could not parse the records.
         * @retval kErrorInvalidState  No NAT64 prefix (applicable only when NAT64 is allowed).
         */
        Error GetAddress(uint16_t aIndex, Ip6::Address &aAddress, uint32_t &aTtl) const;

    private:
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        Error GetNat64Prefix(Ip6::Prefix &aPrefix) const;
#endif
    };

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

    /**
     * Represents the function pointer callback which is called when a response for a browse (service
     * instance enumeration) DNS query is received.
     */
    typedef otDnsBrowseCallback BrowseCallback;

    /**
     * Represents a browse (service instance enumeration) DNS response.
     */
    class BrowseResponse : public Response
    {
        friend class Client;

    public:
        /**
         * Gets the service name associated with a DNS browse response.
         *
         * MUST only be used from `BrowseCallback`.
         *
         * @param[out] aNameBuffer       A buffer to char array to output the host name.
         * @param[in]  aNameBufferSize   The size of @p aNameBuffer.
         *
         * @retval kErrorNone    The host name was read successfully.
         * @retval kErrorNoBufs  The name does not fit in @p aNameBuffer.
         */
        Error GetServiceName(char *aNameBuffer, uint16_t aNameBufferSize) const
        {
            return GetName(aNameBuffer, aNameBufferSize);
        }

        /**
         * Gets a service instance associated with a DNS browse (service instance enumeration) response.
         *
         * MUST only be used from `BrowseCallback`.
         *
         * A response may include multiple service instance records. @p aIndex can be used to iterate through the list.
         * Index zero gives the the first record. When we reach end of the list, `kErrorNotFound` is returned.
         *
         * Note that this method gets the service instance label and not the full service instance name which is of the
         * form `<Instance>.<Service>.<Domain>`.
         *
         * @param[in]  aIndex             The service instance record index to retrieve.
         * @param[out] aLabelBuffer       A char array to output the service instance label (MUST NOT be NULL).
         * @param[in]  aLabelBufferSize   The size of @p aLabelBuffer.
         *
         * @retval kErrorNone         The service instance was read successfully.
         * @retval kErrorNoBufs       The name does not fit in @p aNameBuffer.
         * @retval kErrorNotFound     No service instance record at @p aIndex.
         * @retval kErrorParse        Could not parse the records.
         */
        Error GetServiceInstance(uint16_t aIndex, char *aLabelBuffer, uint8_t aLabelBufferSize) const;

        /**
         * Gets info for a service instance from a DNS browse (service instance enumeration) response.
         *
         * MUST only be used from `BrowseCallback`.
         *
         * A browse DNS response should include the SRV, TXT, and AAAA records for the service instances that are
         * enumerated (note that it is a SHOULD and not a MUST requirement). This method tries to retrieve this info
         * for a given service instance.
         *
         * - If no matching SRV record is found, `kErrorNotFound` is returned.
         * - If a matching SRV record is found, @p aServiceInfo is updated returning `kErrorNone`.
         * - If no matching TXT record is found, `mTxtDataSize` in @p aServiceInfo is set to zero.
         * - If no matching AAAA record is found, `mHostAddress is set to all zero or unspecified address.
         * - If there are multiple AAAA records for the host name `mHostAddress` is set to the first one. The other
         *   addresses can be retrieved using `GetHostAddress()` method.
         *
         * @param[in]  aInstanceLabel     The service instance label (MUST NOT be `nullptr`).
         * @param[out] aServiceInfo       A `ServiceInfo` to output the service instance information.
         *
         * @retval kErrorNone         The service instance info was read. @p aServiceInfo is updated.
         * @retval kErrorNotFound     Could not find a matching SRV record for @p aInstanceLabel.
         * @retval kErrorNoBufs       The host name and/or the TXT data could not fit in given buffers.
         * @retval kErrorParse        Could not parse the records.
         */
        Error GetServiceInfo(const char *aInstanceLabel, ServiceInfo &aServiceInfo) const;

        /**
         * Gets the host IPv6 address from a DNS browse (service instance enumeration) response.
         *
         * MUST only be used from `BrowseCallback`.
         *
         * The response can include zero or more IPv6 address records. @p aIndex can be used to iterate through the
         * list of addresses. Index zero gets the first address and so on. When we reach end of the list, this method
         * returns `kErrorNotFound`.
         *
         * @param[in]  aHostName     The host name to get the address (MUST NOT be `nullptr`).
         * @param[in]  aIndex        The address record index to retrieve.
         * @param[out] aAddress      A reference to an IPv6 address to output the address.
         * @param[out] aTtl          A reference to a `uint32_t` to output TTL for the address.
         *
         * @retval kErrorNone       The address was read successfully.
         * @retval kErrorNotFound   No address record for @p aHostname at @p aIndex.
         * @retval kErrorParse      Could not parse the records.
         */
        Error GetHostAddress(const char *aHostName, uint16_t aIndex, Ip6::Address &aAddress, uint32_t &aTtl) const;

    private:
        Error FindPtrRecord(const char *aInstanceLabel, Name &aInstanceName) const;
    };

    /**
     * Represents the function pointer callback which is called when a response for a service instance
     * resolution DNS query is received.
     */
    typedef otDnsServiceCallback ServiceCallback;

    /**
     * Represents a service instance resolution DNS response.
     */
    class ServiceResponse : public Response
    {
        friend class Client;

    public:
        /**
         * Gets the service instance name associated with a DNS service instance resolution response.
         *
         * MUST only be used from `ServiceCallback`.
         *
         * @param[out] aLabelBuffer      A buffer to char array to output the service instance label (MUST NOT be NULL).
         * @param[in]  aLabelBufferSize  The size of @p aLabelBuffer.
         * @param[out] aNameBuffer       A buffer to char array to output the rest of service name (can be NULL if user
         *                               is not interested in getting the name).
         * @param[in]  aNameBufferSize   The size of @p aNameBuffer.
         *
         * @retval kErrorNone    The service instance name was read successfully.
         * @retval kErrorNoBufs  Either the label or name does not fit in the given buffers.
         */
        Error GetServiceName(char    *aLabelBuffer,
                             uint8_t  aLabelBufferSize,
                             char    *aNameBuffer,
                             uint16_t aNameBufferSize) const;

        /**
         * Gets info for a service instance from a DNS service instance resolution response.
         *
         * MUST only be used from `ServiceCallback`.
         *
         * - If no matching SRV record is found, `kErrorNotFound` is returned.
         * - If a matching SRV record is found, @p aServiceInfo is updated and `kErrorNone` is returned.
         * - If no matching TXT record is found, `mTxtDataSize` in @p aServiceInfo is set to zero.
         * - If no matching AAAA record is found, `mHostAddress is set to all zero or unspecified address.
         * - If there are multiple AAAA records for the host name, `mHostAddress` is set to the first one. The other
         *   addresses can be retrieved using `GetHostAddress()` method.
         *
         * @param[out] aServiceInfo       A `ServiceInfo` to output the service instance information
         *
         * @retval kErrorNone         The service instance info was read. @p aServiceInfo is updated.
         * @retval kErrorNotFound     Could not find a matching SRV record.
         * @retval kErrorNoBufs       The host name and/or TXT data could not fit in the given buffers.
         * @retval kErrorParse        Could not parse the records in the @p aResponse.
         */
        Error GetServiceInfo(ServiceInfo &aServiceInfo) const;

        /**
         * Gets the host IPv6 address from a DNS service instance resolution response.
         *
         * MUST only be used from `ServiceCallback`.
         *
         * The response can include zero or more IPv6 address records. @p aIndex can be used to iterate through the
         * list of addresses. Index zero gets the first address and so on. When we reach end of the list, this method
         * returns `kErrorNotFound`.
         *
         * @param[in]  aHostName     The host name to get the address (MUST NOT be `nullptr`).
         * @param[in]  aIndex        The address record index to retrieve.
         * @param[out] aAddress      A reference to an IPv6 address to output the address.
         * @param[out] aTtl          A reference to a `uint32_t` to output TTL for the address.
         *
         * @retval kErrorNone       The address was read successfully.
         * @retval kErrorNotFound   No address record for @p aHostname at @p aIndex.
         * @retval kErrorParse      Could not parse the records.
         */
        Error GetHostAddress(const char *aHostName, uint16_t aIndex, Ip6::Address &aAddress, uint32_t &aTtl) const;
    };

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE

    /**
     * Represents the function pointer callback which is called when a DNS response for a DNS query response
     * is received.
     */
    typedef otDnsRecordCallback RecordCallback;

    /**
     * Represents a record query response.
     */
    class RecordResponse : public Response
    {
        friend class Client;

    public:
        /**
         * Gets the query name associated with a record DNS query.
         *
         * MUST only be used from `RecordCallback`.
         *
         * @param[out] aNameBuffer       A buffer to char array to output the name.
         * @param[in]  aNameBufferSize   The size of @p aNameBuffer.
         *
         * @retval kErrorNone    The host name was read successfully.
         * @retval kErrorNoBufs  The name does not fit in @p aNameBuffer.
         */
        Error GetQueryName(char *aNameBuffer, uint16_t aNameBufferSize) const
        {
            return GetName(aNameBuffer, aNameBufferSize);
        }

        /**
         * Gets the record data from a DNS query response.
         *
         * MUST only be used from `RecordCallback`.
         *
         * The response may include multiple records. @p aIndex can be used to iterate through the list. Index zero gets
         * the first record and so on. When we reach the end of the list, `kErrorNotFound` is returned.
         *
         * Upon successful retrieval (`kErrorNone`):
         * - `mRecordLength` is set to the actual length of the record's data.
         * - The data is copied into `mDataBuffer` (if not `NULL`) up to its capacity specified by `mDataBufferSize`.
         * - `mDataBufferSize` is then updated to reflect the number of bytes actually written into `mDataBuffer`.
         *
         * If the retrieved record type is PTR (12), CNAME (5), DNAME (39), NS (2), or SRV (33), the record data in the
         * received response contains a DNS name which may use DNS name compression. For these specific record types,
         * the record data is first decompressed such that it contains the full uncompressed DNS name. This
         * decompressed data is then provided in `mDataBuffer`, and `mRecordDataLength` will indicate the length of
         * this decompressed data. For all other record types, the record data is read and provided as it appears in
         * the received response message.
         *
         * @param[in]  aIndex            The record index to retrieve.
         * @param[out] aRecordInfo       A reference to a `RecordInfo` struct to populate from the read record.
         *
         * @retval kErrorNone            The record data was read successfully.
         * @retval kErrorNotFound        No record in @p aResponse at @p aIndex.
         * @retval kErrorParse           Could not parse the records.
         * @retval kErrorNoBufs          The name does not fit in the provided `mNameBufferSize` in @p aRecordInfo, or
         *                               failed to allocate buffer to decompress a DNS name (for PTR, SRV, CNAME).
         */
        Error GetRecordInfo(uint16_t aIndex, RecordInfo &aRecordInfo) const
        {
            return ReadRecordInfo(aIndex, aRecordInfo);
        }
    };

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE

    /**
     * Initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     */
    explicit Client(Instance &aInstance);

    /**
     * Starts the DNS client.
     *
     * @retval kErrorNone     Successfully started the DNS client.
     * @retval kErrorAlready  The socket is already open.
     */
    Error Start(void);

    /**
     * Stops the DNS client.
     */
    void Stop(void);

    /**
     * Gets the current default query config being used by DNS client.
     *
     * @returns The current default query config.
     */
    const QueryConfig &GetDefaultConfig(void) const { return mDefaultConfig; }

    /**
     * Sets the default query config.
     *
     * @param[in] aQueryConfig   The new default query config.
     */
    void SetDefaultConfig(const QueryConfig &aQueryConfig);

    /**
     * Resets the default config to the config used when the OpenThread stack starts.
     *
     * When OpenThread stack starts, the default DNS query config is determined from a set of OT config options such as
     * `OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_IP6_ADDRESS`, `_DEFAULT_SERVER_PORT`, or `_DEFAULT_RESPONSE_TIMEOUT`
     * etc. (see `config/dns_client.h` for all related config options).
     */
    void ResetDefaultConfig(void);

    /**
     * Sends an address resolution DNS query for AAAA (IPv6) record for a given host name.
     *
     * The @p aConfig can be nullptr. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-nullptr @p aConfig, some of the fields can be left unspecified (value zero).
     * The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in]  aHostName        The host name for which to query the address (MUST NOT be `nullptr`).
     * @param[in]  aCallback        A callback function pointer to report the result of query.
     * @param[in]  aContext         A pointer to arbitrary context information passed to @p aCallback.
     * @param[in]  aConfig          The config to use for this query.
     *
     * @retval kErrorNone           Successfully sent DNS query.
     * @retval kErrorNoBufs         Failed to allocate retransmission data.
     * @retval kErrorInvalidArgs    The host name is not valid format.
     * @retval kErrorInvalidState   Cannot send query since Thread interface is not up.
     */
    Error ResolveAddress(const char        *aHostName,
                         AddressCallback    aCallback,
                         void              *aContext,
                         const QueryConfig *aConfig = nullptr);

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    /**
     * Sends an address resolution DNS query for A (IPv4) record for a given host name.
     *
     * When a successful response is received, the addresses are returned from @p aCallback as NAT64 IPv6 translated
     * versions of the IPv4 addresses from the query response.
     *
     * The @p aConfig can be nullptr. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-nullptr @p aConfig, some of the fields can be left unspecified (value zero).
     * The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in]  aHostName        The host name for which to query the address (MUST NOT be `nullptr`).
     * @param[in]  aCallback        A callback function pointer to report the result of query.
     * @param[in]  aContext         A pointer to arbitrary context information passed to @p aCallback.
     * @param[in]  aConfig          The config to use for this query.
     *
     * @retval kErrorNone           Successfully sent DNS query.
     * @retval kErrorNoBufs         Failed to allocate retransmission data.
     * @retval kErrorInvalidArgs    The host name is not valid format or NAT64 is not enabled in config.
     * @retval kErrorInvalidState   Cannot send query since Thread interface is not up, or there is no NAT64 prefix.
     */
    Error ResolveIp4Address(const char        *aHostName,
                            AddressCallback    aCallback,
                            void              *aContext,
                            const QueryConfig *aConfig = nullptr);
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

    /**
     * Sends a browse (service instance enumeration) DNS query for a given service name.
     *
     * The @p aConfig can be nullptr. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-nullptr @p aConfig, some of the fields can be left unspecified (value zero).
     * The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in]  aServiceName     The service name to query for (MUST NOT be `nullptr`).
     * @param[in]  aCallback        The callback to report the response or errors (such as time-out).
     * @param[in]  aContext         A pointer to arbitrary context information.
     * @param[in]  aConfig          The config to use for this query.
     *
     * @retval kErrorNone       Query sent successfully. @p aCallback will be invoked to report the status.
     * @retval kErrorNoBufs     Insufficient buffer to prepare and send query.
     */
    Error Browse(const char        *aServiceName,
                 BrowseCallback     aCallback,
                 void              *aContext,
                 const QueryConfig *aConfig = nullptr);

    /**
     * Starts a DNS service instance resolution for a given service instance.
     *
     * The @p aConfig can be `nullptr`. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-`nullptr` @p aConfig, some of the fields can be left unspecified (value
     * zero). The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in]  aInstanceLabel     The service instance label.
     * @param[in]  aServiceName       The service name (together with @p aInstanceLabel form full instance name).
     * @param[in]  aCallback          A function pointer that shall be called on response reception or time-out.
     * @param[in]  aContext           A pointer to arbitrary context information.
     * @param[in]  aConfig            The config to use for this query.
     *
     * @retval kErrorNone         Query sent successfully. @p aCallback will be invoked to report the status.
     * @retval kErrorNoBufs       Insufficient buffer to prepare and send query.
     * @retval kErrorInvalidArgs  @p aInstanceLabel is `nullptr`.
     */
    Error ResolveService(const char          *aInstanceLabel,
                         const char          *aServiceName,
                         otDnsServiceCallback aCallback,
                         void                *aContext,
                         const QueryConfig   *aConfig = nullptr);

    /**
     * Starts a DNS service instance resolution for a given service instance, with a potential follow-up
     * host name resolution (if the server/resolver does not provide AAAA/A records for the host name in the response
     * to SRV query).
     *
     * The @p aConfig can be `nullptr`. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-`nullptr` @p aConfig, some of the fields can be left unspecified (value
     * zero). The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in]  aInstanceLabel     The service instance label.
     * @param[in]  aServiceName       The service name (together with @p aInstanceLabel form full instance name).
     * @param[in]  aCallback          A function pointer that shall be called on response reception or time-out.
     * @param[in]  aContext           A pointer to arbitrary context information.
     * @param[in]  aConfig            The config to use for this query.
     *
     * @retval kErrorNone         Query sent successfully. @p aCallback will be invoked to report the status.
     * @retval kErrorNoBufs       Insufficient buffer to prepare and send query.
     * @retval kErrorInvalidArgs  @p aInstanceLabel is `nullptr` or the @p aConfig is invalid.
     */
    Error ResolveServiceAndHostAddress(const char        *aInstanceLabel,
                                       const char        *aServiceName,
                                       ServiceCallback    aCallback,
                                       void              *aContext,
                                       const QueryConfig *aConfig = nullptr);

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE
    /**
     * Sends a DNS query for a given record type and name.
     *
     * The @p aConfig can be `nullptr`. In this case the default config (from `GetDefaultConfig()`) will be used as
     * the config for this query. In a non-NULL @p aConfig, some of the fields can be left unspecified (value zero).
     * The unspecified fields are then replaced by the values from the default config.
     *
     * @param[in] aRecordType   The resource record type (RRType) to query.
     * @param[in] aFirstLabel   The first label of the name to be queried (can be NULL if not needed).
     * @param[in] aNextLabels   The next labels of the name to be queried (MUST NOT be NULL).
     * @param[in] aCallback     A function pointer that shall be called on response reception or time-out.
     * @param[in] aContext      A pointer to arbitrary context information used with @p aCallback.
     * @param[in] aConfig       A pointer to the config to use for this query (or NULL for default config).
     *
     * @retval kErrorNone          Query sent successfully. @p aCallback will be invoked to report the outcome.
     * @retval kErrorNoBufs        Insufficient buffer to prepare and send query.
     * @retval kErrorInvalidState  Cannot send query since Thread interface is not up.
     */
    Error QueryRecord(uint16_t           aRecordType,
                      const char        *aFirstLabel,
                      const char        *aNextLabels,
                      RecordCallback     aCallback,
                      void              *aContext,
                      const QueryConfig *aConfig = nullptr);
#endif

private:
    static constexpr uint16_t kMaxCnameAliasNameChanges     = 40;
    static constexpr uint8_t  kLimitedQueryServersArraySize = 3;

    enum QueryType : uint8_t
    {
        kIp6AddressQuery, // IPv6 Address resolution.
#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
        kIp4AddressQuery, // IPv4 Address resolution
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
        kBrowseQuery,        // Browse (service instance enumeration).
        kServiceQuerySrvTxt, // Service instance resolution both SRV and TXT records.
        kServiceQuerySrv,    // Service instance resolution SRV record only.
        kServiceQueryTxt,    // Service instance resolution TXT record only.
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE
        kRecordQuery,
#endif
        kNoQuery,
    };

#if OPENTHREAD_CONFIG_DNS_CLIENT_OVER_TCP_ENABLE
    enum TcpState : uint8_t
    {
        kTcpUninitialized = 0,
        kTcpConnecting,
        kTcpConnectedIdle,
        kTcpConnectedSending,
    };
#endif

    union Callback
    {
        AddressCallback mAddressCallback;
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
        BrowseCallback  mBrowseCallback;
        ServiceCallback mServiceCallback;
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE
        RecordCallback mRecordCallback;
#endif
    };

    typedef MessageQueue QueryList; // List of queries.

    struct QueryInfo : public Clearable<QueryInfo> // Query related Info
    {
        void ReadFrom(const Query &aQuery) { IgnoreError(aQuery.Read(0, *this)); }

        QueryType   mQueryType;
        uint16_t    mMessageId;
        Callback    mCallback;
        void       *mCallbackContext;
        TimeMilli   mRetransmissionTime;
        QueryConfig mConfig;
        uint8_t     mTransmissionCount;
        bool        mShouldResolveHostAddr;
#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE
        uint16_t mRecordType; // Used only when `mQueryType == kRecordQuery`
#endif
        Query   *mMainQuery;
        Query   *mNextQuery;
        Message *mSavedResponse;
        // Followed by the name (service, host, instance) encoded as a `Dns::Name`.
    };

    static constexpr uint16_t kNameOffsetInQuery = sizeof(QueryInfo);

    Error       StartQuery(QueryInfo &aInfo, const char *aLabel, const char *aName, QueryType aSecondType = kNoQuery);
    Error       AllocateQuery(const QueryInfo &aInfo, const char *aLabel, const char *aName, Query *&aQuery);
    void        FreeQuery(Query &aQuery);
    void        UpdateQuery(Query &aQuery, const QueryInfo &aInfo) { aQuery.Write(0, aInfo); }
    Query      &FindMainQuery(Query &aQuery);
    Error       SendQuery(Query &aQuery, QueryInfo &aInfo, bool aUpdateTimer);
    uint16_t    DetermineQuestionRecordType(const QueryInfo &aInfo) const;
    void        FinalizeQuery(Query &aQuery, Error aError);
    void        FinalizeQuery(Response &Response, Error aError);
    static void GetQueryTypeAndCallback(const Query &aQuery, QueryType &aType, Callback &aCallback, void *&aContext);
    Error       AppendNameFromQuery(const Query &aQuery, Message &aMessage);
    Query      *FindQueryById(uint16_t aMessageId);
    void        HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMsgInfo);
    void        ProcessResponse(const Message &aResponseMessage);
    Error       ParseResponse(const Message &aResponseMessage, Query *&aQuery, Error &aResponseError);
    bool        CanFinalizeQuery(Query &aQuery);
    void        SaveQueryResponse(Query &aQuery, const Message &aResponseMessage);
    Query      *PopulateResponse(Response &aResponse, Query &aQuery, const Message &aResponseMessage);
    void        PrepareResponseAndFinalize(Query &aQuery, const Message &aResponseMessage, Response *aPrevResponse);
    void        HandleTimer(void);

#if OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
    Error ReplaceWithIp4Query(Query &aQuery, const Message &aResponseMessage);
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
    Error Resolve(const char        *aInstanceLabel,
                  const char        *aServiceName,
                  ServiceCallback    aCallback,
                  void              *aContext,
                  const QueryConfig *aConfig,
                  bool               aShouldResolveHostAddr);
    void  CheckAndUpdateServiceMode(QueryConfig &aConfig, const QueryConfig *aRequestConfig) const;
    void  RecordServerAsLimitedToSingleQuestion(const Ip6::Address &aServerAddress);
    void  RecordServerAsCapableOfMultiQuestions(const Ip6::Address &aServerAddress);
    Error ReplaceWithSeparateSrvTxtQueries(Query &aQuery);
    void  ResolveHostAddressIfNeeded(Query &aQuery, const Message &aResponseMessage);
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
    void UpdateDefaultConfigAddress(void);
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_OVER_TCP_ENABLE
    static void HandleTcpEstablishedCallback(otTcpEndpoint *aEndpoint);
    static void HandleTcpSendDoneCallback(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData);
    static void HandleTcpDisconnectedCallback(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason);
    static void HandleTcpReceiveAvailableCallback(otTcpEndpoint *aEndpoint,
                                                  size_t         aBytesAvailable,
                                                  bool           aEndOfStream,
                                                  size_t         aBytesRemaining);

    void  HandleTcpEstablished(otTcpEndpoint *aEndpoint);
    void  HandleTcpSendDone(otTcpEndpoint *aEndpoint, otLinkedBuffer *aData);
    void  HandleTcpDisconnected(otTcpEndpoint *aEndpoint, otTcpDisconnectedReason aReason);
    void  HandleTcpReceiveAvailable(otTcpEndpoint *aEndpoint,
                                    size_t         aBytesAvailable,
                                    bool           aEndOfStream,
                                    size_t         aBytesRemaining);
    Error InitTcpSocket(void);
    Error ReadFromLinkBuffer(const otLinkedBuffer *&aLinkedBuffer,
                             size_t                &aOffset,
                             Message               &aMessage,
                             uint16_t               aLength);
    void  PrepareTcpMessage(Message &aMessage);
#endif // OPENTHREAD_CONFIG_DNS_CLIENT_OVER_TCP_ENABLE

    static constexpr uint16_t kUdpQueryMaxSize = 512;

    using RetryTimer   = TimerMilliIn<Client, &Client::HandleTimer>;
    using ClientSocket = Ip6::Udp::SocketIn<Client, &Client::HandleUdpReceive>;

    ClientSocket mSocket;

#if OPENTHREAD_CONFIG_DNS_CLIENT_OVER_TCP_ENABLE
    Ip6::Tcp::Endpoint mEndpoint;

    otLinkedBuffer mSendLink;
    uint8_t        mSendBufferBytes[OPENTHREAD_CONFIG_DNS_CLIENT_OVER_TCP_QUERY_MAX_SIZE];
    uint8_t        mReceiveBufferBytes[OPENTHREAD_CONFIG_DNS_CLIENT_OVER_TCP_QUERY_MAX_SIZE];

    TcpState mTcpState;
#endif

    QueryList   mMainQueries;
    RetryTimer  mTimer;
    QueryConfig mDefaultConfig;
#if OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
    bool mUserDidSetDefaultAddress;
#endif
    Array<Ip6::Address, kLimitedQueryServersArraySize> mLimitedQueryServers;
};

} // namespace Dns

DefineCoreType(otDnsQueryConfig, Dns::Client::QueryConfig);
DefineCoreType(otDnsAddressResponse, Dns::Client::AddressResponse);
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
DefineCoreType(otDnsBrowseResponse, Dns::Client::BrowseResponse);
DefineCoreType(otDnsServiceResponse, Dns::Client::ServiceResponse);
DefineCoreType(otDnsServiceInfo, Dns::Client::ServiceInfo);
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_ARBITRARY_RECORD_QUERY_ENABLE
DefineCoreType(otDnsRecordResponse, Dns::Client::RecordResponse);
DefineCoreType(otDnsRecordInfo, Dns::Client::RecordInfo);
DefineMapEnum(otDnsRecordSection, Dns::Client::RecordInfo::RecordSection);
#endif

} // namespace ot

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE

#endif // DNS_CLIENT_HPP_
