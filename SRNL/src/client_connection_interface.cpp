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

#include "packet.hpp"
#include "security.hpp"
#include "client_connection_interface.hpp"
#include "client_connection_state.hpp"

/// GLOBAL INCLUDES
#include <utility>
#include <boost/bind.hpp>

namespace clt
{

IConnection::IConnection ( boost::asio::io_service &io_service, const std::string &host, const std::string &port)
    :   m_host(host),
        m_port(port),
        m_closed(false),
        m_ping_timeout(5),
        m_is_encryption_active(false),
        m_locale(0),
        m_version(0),
        m_resolver(io_service),
        m_socket(io_service),
        m_byte_count(0),
        m_ping_timer(io_service)
{
}

IConnection::~IConnection ()
{
}

IConnection::socket_type& IConnection::socket ()
{
    return m_socket;
}

void IConnection::Execute ()
{
    using boost::asio::ip::tcp;

    tcp::resolver::query query(m_host,m_port);

    m_resolver.async_resolve(query,boost::bind(&IConnection::OnResolve,this,
        boost::asio::placeholders::error,m_resolver.resolve(query)));
}

void IConnection::KeepAlive ()
{
    m_ping_timer.expires_from_now(boost::posix_time::seconds(m_ping_timeout));
    m_ping_timer.async_wait(boost::bind(&IConnection::OnTimeout,this, boost::asio::placeholders::error));
}

std::string IConnection::local_endpoint () const
{
    return m_LocalIP;
}

std::string IConnection::remote_endpoint () const
{
    return m_RemoteIP;
}

void IConnection::Stop ()
{
    if (!m_closed)
    {
        try
        {
            if (m_socket.is_open())
            {
                m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);

                m_socket.close();
            }
        }
        catch (...)
        {
        }

        m_ping_timer.cancel();

        if (m_State)
        {
            m_State->Close();
            m_State.reset();
        }

    	m_state_table.clear();

        m_closed = true;
    }
}

bool IConnection::SetState (const uint8_t ID)
{
    state_iterator i = m_state_table.find(ID);

    if (i == m_state_table.end())
        return false;

    m_State = i->second;

    return true;
}

void IConnection::SetEncryption (bool activate)
{
    m_is_encryption_active = activate;
}

void IConnection::SetSeed (uint32_t seed_count, uint32_t crc)
{
    if( seed_count == 0 )
        seed_count = 0x9ABFB3B6;

    unsigned long mut = seed_count, mut1 = 0, mut2 = 0, mut3 = 0;

    mut1 = GenerateValue(mut);
    mut2 = GenerateValue(mut1);
    mut3 = GenerateValue(mut2);
    mut = GenerateValue(mut3);

    unsigned char byte1 = (unsigned char)((mut&0xFF)^(mut3&0xFF));
    unsigned char byte2 = (unsigned char)((mut1&0xFF)^(mut2&0xFF));

    if( !byte1 )
        byte1 = 1;

    if( !byte2 )
        byte2 = 1;

    m_count_seeds[0] = byte1^byte2;
    m_count_seeds[1] = byte2;
    m_count_seeds[2] = byte1;

    m_crc_seed = crc;
}

uint8_t IConnection::GenerateCountByte ()
{
    unsigned char result = (m_count_seeds[2]*(~m_count_seeds[0]+m_count_seeds[1]));
    result = result ^ ( result >> 4 );
    m_count_seeds[0] = result;

    return result;
}

bool IConnection::IsEncryptionActive () const
{
    return m_is_encryption_active;
}

void IConnection::Locale (const uint8_t locale)
{
    m_locale = locale;
}

uint8_t IConnection::Locale() const
{
    return m_locale;
}

void IConnection::Version(const uint32_t version)
{
    m_version = version;
}

uint32_t IConnection::Version () const
{
    return m_version;
}

void IConnection::Send (boost::shared_ptr<OPacket> packet, bool encrypt, bool clear)
{
    if (m_socket.is_open() && packet)
    {
        packet->WriteSize(encrypt & IsEncryptionActive());

        size_t size = packet->size();

        if (IsEncryptionActive())
        {
            GenerateCountByte();

            packet->WriteCountSecurity(m_count_seeds[0]);

            packet->WriteCrcSecurity(GenerateCheckByte((const char*)packet->buffer(),packet->size(),m_crc_seed));

            if (encrypt)
                size = Encode((packet->buffer()+2),packet->buffer()+2,packet->size()-2) + 2;
        }

        boost::asio::async_write(m_socket,boost::asio::buffer(packet->buffer(),size),
           boost::bind(&IConnection::OnWrite, this, packet, boost::asio::placeholders::error));
    }
}

void IConnection::InitializeKey (BYTE key[], int keybytes)
{
    m_blowfish.Initialize(key,keybytes);
}

uint32_t IConnection::Encode(uint8_t *pInput, uint8_t *pOutput, uint32_t lSize)
{
    return m_blowfish.Encode(pInput,pOutput,lSize);
}

void IConnection::Decode(uint8_t *pInput, uint8_t *pOutput, uint32_t lSize)
{
    m_blowfish.Decode(pInput,pOutput,lSize);
}

void IConnection::OnResolve (const boost::system::error_code &error,
						  resolver_iterator_type endpoint_iterator)
{
    if (!error)
    {
        boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
        m_socket.async_connect(endpoint, boost::bind(&IConnection::OnConnect, this,
            boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else
        throw boost::system::system_error(error);
}

void IConnection::OnConnect (const boost::system::error_code &error,
						  resolver_iterator_type endpoint_iterator)
{
    if (!error)
    {
        // Cache Local and Remote IP.

        m_LocalIP = m_socket.local_endpoint().address().to_string();

        m_RemoteIP = m_socket.remote_endpoint().address().to_string();

        // The connection was successful.
        m_socket.async_read_some(boost::asio::buffer(&m_buffer,8192),boost::bind(&IConnection::OnRead,this,
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else if (endpoint_iterator != resolver_iterator_type())
    {
      // The connection failed. Try the next endpoint in the list.
      m_socket.close();
      boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
      m_socket.async_connect(endpoint, boost::bind(&IConnection::OnConnect, this,
            boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else
        throw boost::system::system_error(error);
}

void IConnection::OnRead (const boost::system::error_code &error, std::size_t bytes_transferred)
{
    enum { CLIENT_PING = 0x2002 };

    if (!error)
    {
        m_byte_count += bytes_transferred;

        size_t offset = 0;

        while (m_byte_count >= 6 )
        {
            PacketHeader *header = (PacketHeader*)(m_buffer+offset);

            size_t real_size = 0;
            bool encrypted = header->size & 0x800;

            if (encrypted)
                real_size = header->size & 0x7FF;
            else
                real_size = header->size;

            size_t packet_size = real_size + 6;

            ///PARSE IF WE GOT COMPLETE PACKET
            if (m_byte_count >= packet_size)
            {
                if (encrypted)
                {
                    if (!IsEncryptionActive())
                    {
                        Stop();
                        return;
                    }
                }

                ///CHECK SECURITY BYTES
                if (header->securityCount)
                {
                    Stop();
                    return;
                }

                if (header->securityCRC)
                {
                    Stop();
                    return;
                }

                IPacket packet(m_buffer+offset,packet_size);

                if (m_State)
                {
                    int ret = m_State->Parse(packet);

                    switch(ret)
                    {
                    case MSG_SUCCESS:
                        break;
                    case MSG_ERROR:
                        syslog(LOG_ERR,"Service Client Op %X - Error while parsing packet.",packet.opcode());
                        break;
                    case MSG_ERROR_OPCODE:
                        break;
                    case MSG_ERROR_SIZE:
                        syslog(LOG_ERR,"Service Client Op %X -  Error while parsing packet. Packet size mismatch",
                            packet.opcode());
                        break;
                    case MSG_ERROR_ARG:
                        syslog(LOG_ERR,"Service Client Op %X - Error while parsing packet. Packet argument(s) invalid",
                            packet.opcode());
                        break;
                    case MSG_ERROR_ORDER:
                        syslog(LOG_ERR,"Service Client Op %X - Error while parsing packet. Packet invalid",
                            packet.opcode());
                        break;
                    default:
                        syslog(LOG_ERR,"Service Client Op %X - Error while parsing packet. Error number = %i",
                            ret,packet.opcode());
                        break;
                    }

                    if (ret)
                    {
                        Stop();
                        return;
                    }
                }

                m_byte_count -= packet_size;
                offset += packet_size;
            }
        }

        if (m_byte_count)
            memcpy(m_buffer,m_buffer+offset,m_byte_count);

        m_socket.async_read_some(boost::asio::buffer(m_buffer,8192-offset),boost::bind(&IConnection::OnRead,this,
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        if (error != boost::asio::error::operation_aborted && !m_closed)
            Stop();
    }
}

void IConnection::OnWrite (const boost::shared_ptr<OPacket> &pkt, const boost::system::error_code &error)
{
    if (error && error != boost::asio::error::operation_aborted)
        Stop();
}

void IConnection::OnTimeout (const boost::system::error_code &error)
{
    if (!error)
    {
        boost::shared_ptr<OPacket> p;

        p.reset(new OPacket);

        if (p)
        {
            p->WriteOpcode(0x2002);

            Send(p);

            m_ping_timer.expires_from_now(boost::posix_time::seconds(m_ping_timeout));
            m_ping_timer.async_wait(boost::bind(&IConnection::OnTimeout,this, boost::asio::placeholders::error));
        }
        else
            Stop();
    }
}

}

