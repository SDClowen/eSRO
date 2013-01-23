/*********************************************************************************
 *
 * This file is part of eSRO.
 *
 * eSRO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * eSRO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright ⓒ 2013  Rafael Dominguez (npcdoom)
 *
 *********************************************************************************/

#ifndef __CLIENT_CONNECTION_INTERFACE_HPP__
#define __CLIENT_CONNECTION_INTERFACE_HPP__

#include <blowfish.hpp>
#include <boost/asio.hpp>

class OPacket;

namespace clt
{

class ConnectionState;

class IConnection
{
public:

    typedef boost::asio::ip::tcp::socket socket_type;
    typedef boost::asio::ip::tcp::resolver resolver_type;
    typedef boost::asio::ip::tcp::resolver::iterator resolver_iterator_type;

    typedef std::map<uint8_t,boost::shared_ptr<ConnectionState> >::iterator state_iterator;
    typedef std::map<uint8_t,boost::shared_ptr<ConnectionState> >::const_iterator state_const_iterator;

    IConnection (boost::asio::io_service &io_service, const std::string &host, const std::string &port);

    virtual ~IConnection ();

    socket_type& socket ();

    std::string local_endpoint () const;

    std::string remote_endpoint () const;

    void Execute ();

    void KeepAlive ();

    void Stop ();

    bool SetState (const uint8_t ID);

    void SetEncryption (bool activate);

    void SetSeed (uint32_t count_seed, uint32_t crc);

    uint8_t GenerateCountByte ();

    bool IsEncryptionActive () const;

    void Locale (const uint8_t locale);

    uint8_t Locale () const;

    void Version (const uint32_t version);

    uint32_t Version () const;

    void Send (boost::shared_ptr<OPacket> packet, bool encrypt = false, bool clear = true);

    void InitializeKey (BYTE key[], int keybytes);

    uint32_t Encode(uint8_t *pInput, uint8_t *pOutput, uint32_t lSize);

    void Decode(uint8_t *pInput, uint8_t *pOutput, uint32_t lSize);

protected:

    void OnResolve (const boost::system::error_code &error,
        resolver_iterator_type endpoint_iterator);

    void OnConnect (const boost::system::error_code &error,
        resolver_iterator_type endpoint_iterator);

    void OnRead (const boost::system::error_code& error, std::size_t bytes_transferred);

    void OnWrite (const boost::shared_ptr<OPacket> &pkt, const boost::system::error_code& error);

    void OnTimeout (const boost::system::error_code& error);

protected:

    std::string m_host;
    std::string m_port;

    bool m_closed;
    uint32_t m_ping_timeout;
    bool m_is_encryption_active;
    uint8_t m_locale;
    uint32_t m_version;

    resolver_type m_resolver;
    socket_type m_socket;

    size_t m_byte_count;
    uint8_t m_buffer[8192];

    boost::shared_ptr<ConnectionState> m_State;
    std::map<uint8_t,boost::shared_ptr<ConnectionState> > m_state_table;

    boost::asio::deadline_timer m_ping_timer;

    cBlowFish m_blowfish;

    uint8_t m_count_seeds[3];
    uint32_t m_crc_seed;

    std::string m_LocalIP;
    std::string m_RemoteIP;
};

}

#endif //__CLIENT_CONNECTION_INTERFACE_HPP__
