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

#ifndef __CONNECTION_INTERFACE_HPP__
#define __CONNECTION_INTERFACE_HPP__

#include <stdint.h>
#include <blowfish.hpp>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

class OPacket;

namespace srv
{

class IServer;
class ConnectionState;

class IConnection
{
public:

    typedef boost::asio::ip::tcp::socket socket_type;
    typedef std::map<uint8_t,boost::shared_ptr<ConnectionState> >::iterator state_iterator;
    typedef std::map<uint8_t,boost::shared_ptr<ConnectionState> >::const_iterator state_const_iterator;

    IConnection (const uint32_t ID, boost::asio::io_service &io_service, IServer *srv);

    virtual ~IConnection ();

    uint32_t ID () const;

    socket_type& socket ();

    std::string local_endpoint () const;

    std::string remote_endpoint () const;

    void Execute ();

    void Stop ();

    void SetMonitor (bool log, const std::string &path);

    bool IsMonitored () const;

    bool SetState (const uint8_t ID);

    uint8_t GetCurrentState () const;

    void SetupCountByte (unsigned long seed);

    uint8_t GenerateCountByte ();

    bool IsEncryptionActive () const;

    uint32_t ClientVersion () const;

    const std::string& SrvIDS () const;

    void Send (boost::shared_ptr<OPacket> pkt, bool encrypt = false, bool clear = true);

    void Send (boost::shared_ptr<OPacket> p1, boost::shared_ptr<OPacket> p2,
        boost::shared_ptr<OPacket> p3, bool encrypt = false, bool clear = true);

    /**
     *
     *  @brief: Initialize blowfish key for decoding and encoding packets.
     *  @key: Key to initialize with.
     *  @keybytes: Length of the key.
     *
     **/

    void InitializeKey (BYTE key[], int keybytes);

    uint32_t Encode(uint8_t *pInput, uint8_t *pOutput, uint32_t lSize);

    void Decode(uint8_t *pInput, uint8_t *pOutput, uint32_t lSize);

protected:

    void OnRead (const boost::system::error_code& error, std::size_t bytes_transferred);

    void OnWrite (const boost::shared_ptr<OPacket> &pkt, const boost::system::error_code& error);

    void OnTimeout (const boost::system::error_code& error);

protected:

    uint32_t m_ID;
    uint8_t m_current_state;

    size_t m_byte_count;
    uint8_t m_buffer[8192];
    uint8_t m_pkt_buffer[8192];
    uint8_t m_count_seeds[3];
    uint8_t m_crc_seed;

    cBlowFish m_blowfish;

    IServer *m_Server;
    boost::shared_ptr<ConnectionState> m_State;
    std::map<uint8_t,boost::shared_ptr<ConnectionState> > m_state_table;

    bool m_idle;
    bool m_closed;

    socket_type m_socket;
    boost::asio::deadline_timer m_idle_timer;

    boost::mutex m_mutex;

    std::string m_LocalIP;
    std::string m_RemoteIP;

    bool m_log;
    std::ofstream m_Logfile;
};

}

#endif //__CONNECTION_INTERFACE_HPP__
