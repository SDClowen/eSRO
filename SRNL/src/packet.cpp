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

IPacket::IPacket(const uint8_t *buffer, size_t size)
{
    memcpy(&m_header,buffer,HEADER_LENGTH);
    memset(m_data,0,BODY_MAX_LENGTH);
    memcpy(m_data,buffer+HEADER_LENGTH,m_header.size);

    index = 0;
}

uint16_t IPacket::size () const
{
    return m_header.size;
}

uint16_t IPacket::opcode () const
{
    return m_header.opcode;
}

void IPacket::Skip (uint16_t bytes) const
{
    index += bytes;
}

std::string IPacket::Read() const
{
    uint16_t len = Read<uint16_t>();

    if (index+len > m_header.size)
        throw Exception::InvalidPacketSize();

    std::string str(m_data+index,m_data+index+len);
    index += len;

    return str;
}

std::string IPacket::ReadUString () const
{
    uint16_t len = Read<uint16_t>()*2;

    if (index+len > m_header.size)
        throw Exception::InvalidPacketSize();

    std::string str(m_data+index,m_data+index+len);
    index += len;

    return str;
}

bool IPacket::EndOfStream () const
{
    return index == m_header.size;
}

const uint8_t* IPacket::buffer() const
{
    return (const uint8_t*)m_data;
}

OPacket::OPacket ()
{
    Clear();
}

OPacket::OPacket (const uint16_t opcode)
{
    Clear();

    WriteOpcode(opcode);
}

uint16_t OPacket::size () const
{
    return m_size;
}

void OPacket::WriteOpcode(const uint16_t opcode)
{
    WritePosition(ENTRY_OPCODE_POSITION);
    Write<uint16_t>(opcode,false);
    WritePosition(m_size);
}

void OPacket::WriteOpcode (const uint16_t opcode, const ANSWER_TYPE answer)
{
    WriteOpcode(opcode);
    Write<uint8_t>(answer);
}

void OPacket::WriteOpcode (const uint16_t opcode, const ANSWER_TYPE answer, const uint8_t error)
{
    WriteOpcode(opcode);
    Write<uint8_t>(answer);
    Write<uint8_t>(error);
}

void OPacket::WriteCountSecurity(const uint8_t security)
{
	WritePosition(ENTRY_COUNT_POSITION);
	Write<uint8_t>(security,false);
	WritePosition(m_size);
}

void OPacket::WriteCrcSecurity(const uint8_t security)
{
	WritePosition(ENTRY_CRC_POSITION);
	Write<uint8_t>(security,false);
	WritePosition(m_size);
}

void OPacket::WriteSize(const bool encrypted)
{
	WritePosition(ENTRY_SIZE_POSITION);

	uint16_t size = m_size - 6;

	if(encrypted)
		size |= 0x8000;

	Write<uint16_t>(size,false);

	WritePosition(m_size);
}

uint8_t* OPacket::buffer()
{
	return (uint8_t*)data;
}

const uint8_t* OPacket::buffer () const
{
    return (const uint8_t*)data;
}

void OPacket::Write (float val)
{
    uint8_t* stream = m_wpos;
    (*(float*)stream) = val;

    m_size += sizeof(float);

    m_wpos += sizeof(float);
}

void OPacket::Write (double val)
{
    uint8_t* stream = m_wpos;
    (*(double*)stream) = val;

    m_size += sizeof(double);

    m_wpos += sizeof(double);
}

void OPacket::Write (const std::string& str, const STRING_TYPE type)
{
	if ( type == STRING_ASCII)
		Write<uint16_t>(str.length());
	else
		Write<uint16_t>(str.length()/2);

	Write(str.c_str(),str.length());
}

void OPacket::Write(const char * str,const uint16_t len)
{
	uint8_t* stream = (uint8_t*)m_wpos;
	strcpy((char*)stream, str);
	m_size += (uint16_t)len;
	WritePosition(m_size);
}

void OPacket::WritePosition (const uint16_t pos)
{
	assert(pos < MAX_PACKET_SIZE);

	m_wpos = data + pos;
}

void OPacket::Clear()
{
    memset(data, 0, MAX_PACKET_SIZE);
	WritePosition(6);
	m_size = 6;
}
