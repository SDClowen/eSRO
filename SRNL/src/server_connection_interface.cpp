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

#include "server_connection_interface.hpp"
#include "server_connection_state.hpp"
#include "server_state_handshake_interface.hpp"
#include "server_interface.hpp"
#include "security.hpp"
#include "packet_authentication.hpp"

/// GLOBAL INCLUDES
#include <utility>
#include <boost/bind.hpp>

namespace srv
{

IConnection::IConnection (const uint32_t ID, boost::asio::io_service &io_service, IServer *srv)
    :   m_ID(ID),
        m_current_state(0),
        m_crc_seed(0),
        m_Server(srv),
        m_idle(false),
        m_closed(false),
        m_socket(io_service),
        m_idle_timer(io_service),
        m_log(false)
{
    m_byte_count = 0;
    memset(m_buffer,0,8192);
    memset(m_count_seeds,0,3);
}

IConnection::~IConnection ()
{
}

uint32_t IConnection::ID () const
{
    return m_ID;
}

IConnection::socket_type& IConnection::socket ()
{
	return m_socket;
}

void IConnection::Execute ()
{
    // Cache Local and Remote IP.

    m_LocalIP = m_socket.local_endpoint().address().to_string();

    m_RemoteIP = m_socket.remote_endpoint().address().to_string();

    boost::shared_ptr<OPacket> p;

    p.reset(new OPacket);

    if (!p)
    {
        Stop();
        return;
    }

    if (IsEncryptionActive())
    {
        uint32_t count_seed = rand()%255;
        m_crc_seed = rand()%255;

        SetupCountByte(count_seed);

        /// PERFORM A DIFFIE-HELLMAN KEY EXCHANGE

        /// GENERATE A PUBLIC KEY (GARBAGE DATA I THINK NOT SURE!!)
        uint32_t public_key[2] = { 0 };
        public_key[0] = rand();
        public_key[1] = rand();

        /// GENERATE BASE KEY
        uint32_t base_key[2] = { 0 };
        base_key[0] = rand();
        base_key[1] = rand();

        /// GENERATE PUBLIC DATA
        uint32_t a = rand();
        uint32_t G = rand();
        uint32_t P = rand();

        uint32_t A = ModExp(P,a,G);

        boost::shared_ptr<IStateHandshake> st = boost::shared_static_cast<IStateHandshake,srv::ConnectionState>(m_State);

        if (!st)
        {
            Stop();
            return;
        }

        st->SetBaseKey(base_key);
        st->SetSharedData(a,P,G,A);

        srv_pkt::WriteHandshake(p,public_key,count_seed,m_crc_seed,base_key,G,P,A);
    }
    else
    {
        srv_pkt::WriteHandshake(p);
    }

    Send(p);

	m_idle_timer.expires_from_now(boost::posix_time::seconds(m_Server->ConnectionTimeout()));
	m_idle_timer.async_wait(boost::bind(&IConnection::OnTimeout,this, boost::asio::placeholders::error));

	m_socket.async_read_some(boost::asio::buffer(&m_buffer,8192),boost::bind(&IConnection::OnRead,this,
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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

        m_idle_timer.cancel();

        if (m_State)
        {
            m_State->Close();
            m_State.reset();
        }

    	m_state_table.clear();

        m_closed = true;

        m_Server->RemoveConnection(m_ID);
    }
}

void IConnection::SetMonitor (bool log, const std::string &path)
{
    m_log = log;

    if (m_log)
    {
        if (!m_Logfile.is_open())
        {
            char s[30];
            char filename[200];
            struct tm tim;
            time_t now;
            now = time(NULL);
            tim = *(localtime(&now));
            strftime(s,30,"%H%M%S%d%m%Y",&tim);

            sprintf(filename,"%s/srv_conn_client_%u_%s",path.c_str(),m_ID,s);

            m_Logfile.open(filename,std::ios::binary | std::ios::app);
        }
    }
    else
    {
        m_Logfile.close();
    }
}

bool IConnection::IsMonitored () const
{
    return m_log;
}

bool IConnection::SetState (const uint8_t ID)
{
    state_iterator i = m_state_table.find(ID);

	if (i == m_state_table.end())
        return false;

    m_current_state = ID;
    m_State = i->second;

	return true;
}

uint8_t IConnection::GetCurrentState () const
{
    return m_current_state;
}

void IConnection::SetupCountByte (unsigned long seed)
{
    if( seed == 0 )
		seed = 0x9ABFB3B6;

	unsigned long mut = seed, mut1 = 0, mut2 = 0, mut3 = 0;

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
    return m_Server->IsEncryptionActive();
}

uint32_t IConnection::ClientVersion () const
{
    return m_Server->ClientVersion();
}

const std::string& IConnection::SrvIDS () const
{
    return m_Server->IDS();
}

void IConnection::Send (boost::shared_ptr<OPacket> pkt, bool encrypt, bool clear)
{
    boost::mutex::scoped_lock m_lock(m_mutex);

    if (m_socket.is_open() && pkt)
    {
        pkt->WriteSize();

        if (encrypt)
        {
        }

        boost::asio::async_write(m_socket,boost::asio::buffer(pkt->buffer(),pkt->size()),
           boost::bind(&IConnection::OnWrite, this, pkt,boost::asio::placeholders::error));

        if (m_log)
        {
            m_Logfile.write((const char*)pkt->buffer(),pkt->size());
            m_Logfile.flush();
        }
    }
}

void IConnection::Send (boost::shared_ptr<OPacket> p1, boost::shared_ptr<OPacket> p2,
        boost::shared_ptr<OPacket> p3, bool encrypt, bool clear)
{
    boost::mutex::scoped_lock m_lock(m_mutex);

    if (m_socket.is_open() && p1 && p2 && p3)
    {
        p1->WriteSize();

        p2->WriteSize();

        p3->WriteSize();

        if (encrypt)
        {
        }

        boost::asio::async_write(m_socket,boost::asio::buffer(p1->buffer(),p1->size()),
           boost::bind(&IConnection::OnWrite, this, p1,boost::asio::placeholders::error));

        boost::asio::async_write(m_socket,boost::asio::buffer(p2->buffer(),p2->size()),
           boost::bind(&IConnection::OnWrite, this, p2,boost::asio::placeholders::error));

        boost::asio::async_write(m_socket,boost::asio::buffer(p3->buffer(),p3->size()),
           boost::bind(&IConnection::OnWrite, this, p3,boost::asio::placeholders::error));

        if (m_log)
        {
            m_Logfile.write((const char*)p1->buffer(),p1->size());
            m_Logfile.write((const char*)p2->buffer(),p2->size());
            m_Logfile.write((const char*)p3->buffer(),p3->size());

            m_Logfile.flush();
        }
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

            size_t buffer_size = 0;
            size_t real_size = 0;
            bool encrypted = header->size & 0x8000;

	        if (encrypted)
	        {
                real_size = header->size & 0x7FF;
                buffer_size = m_blowfish.GetOutputLength(real_size+4)+2;
	        }
	        else
	        {
                real_size = header->size;
                buffer_size = real_size + 6;
	        }

            ///PARSE IF WE GOT COMPLETE PACKET
            if (m_byte_count >= buffer_size)
            {
                ///COPY THE OPCODE TO THE PARSING BUFFER
                memcpy(m_pkt_buffer,&real_size,2);

                if (encrypted)
                {
                    if (!IsEncryptionActive())
                    {
                        Stop();
                        return;
                    }

                    ///DECODE AND COPY THE PACKET TO THE PARSING BUFFER
                    m_blowfish.Decode(m_buffer+offset+2,m_pkt_buffer+2,m_blowfish.GetOutputLength(real_size+4));
                }
                else
                {
                    ///COPY THE PACKET TO THE PARSING BUFFER
                    memcpy(m_pkt_buffer+2,m_buffer+offset+2,buffer_size-2);
                }

                header = (PacketHeader*)(m_pkt_buffer);

                ///CHECK SECURITY BYTES
                if (IsEncryptionActive())
                {
                    GenerateCountByte();

                    if (m_count_seeds[0] != header->securityCount)
                    {
                        Stop();
                        return;
                    }

                    uint8_t clt_crc = header->securityCRC;

                    ///SET THE ENCRYPTED FLAG SO WE CAN CALCULATE THE PACKET CRC.
                    if (encrypted)
                        header->size = real_size | 0x8000;

                    ///SET CRC BYTE 0 TO CALCULATE THE PACKET CRC.
                    header->securityCRC = 0;

                    uint8_t srv_crc = GenerateCheckByte((const char*)(m_pkt_buffer),header->size+6,m_crc_seed);

                    ///RESTORE PAYLOAD SIZE
                    header->size = real_size;

                    if (clt_crc != srv_crc)
                    {
                        Stop();
                        return;
                    }
                }
                else
                {
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
                }

                if (m_log)
                {
                    boost::mutex::scoped_lock lock(m_mutex);

                    m_Logfile.write((const char*)m_pkt_buffer,header->size+6);
                }

                IPacket packet(m_pkt_buffer,header->size+6);

                if (!m_State)
                {
                    Stop();
                    return;
                }

                if (packet.opcode() == CLIENT_PING)
                {
                    m_idle = false;
                }
                else
                {
                    m_idle = false;

                    int ret = m_State->Parse(packet);

                    switch(ret)
                    {
                    case MSG_SUCCESS:
                        break;
                    case MSG_ERROR:
                        syslog(LOG_ERR,"SrvClient %i Op %X - Error while parsing packet.",m_ID,packet.opcode());
                        break;
                    case MSG_ERROR_OPCODE:
                        break;
                    case MSG_ERROR_SIZE:
                        syslog(LOG_ERR,"SrvClient %i Op %X -  Error while parsing packet. Packet size mismatch",
                            m_ID,packet.opcode());
                        break;
                    case MSG_ERROR_ARG:
                        syslog(LOG_ERR,"SrvClient %i Op %X- Error while parsing packet. Packet argument(s) invalid",
                            m_ID,packet.opcode());
                        break;
                    case MSG_ERROR_ORDER:
                        syslog(LOG_ERR,"SrvClient %i Op %X - Error while parsing packet. Packet invalid",
                            m_ID,packet.opcode());
                        break;
                    case MSG_ERROR_MEMORY:
                        break;
                    default:
                        syslog(LOG_ERR,"SrvClient %i Op %X - Error while parsing packet. Error number = %i",
                            m_ID,ret,packet.opcode());
                        break;
                    }

                    if (ret)
                    {
                        Stop();
                        return;
                    }
                }

                m_byte_count -= buffer_size;
                offset += buffer_size;
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
        if (m_idle)
        {
            Stop();
        }
        else
        {
            m_idle = true;

            m_idle_timer.expires_from_now(boost::posix_time::seconds(m_Server->ConnectionTimeout()));
            m_idle_timer.async_wait(boost::bind(&IConnection::OnTimeout,this, boost::asio::placeholders::error));
        }
    }
}

}
