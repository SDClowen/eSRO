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

#include "server.hpp"
#include "server_connection.hpp"
#include "server_state_game.hpp"

#include "world.hpp"
#include "academy_manager.hpp"

#include <academy_query.hpp>

#include <packet_academy.hpp>

/*
void AcademyEstablish::operator() (IPacket &packet)
{
	enum
	{
		REQUIRED_LEVEL = 60,
		ESTABLISH_FEED = 100000
	};

	std::cout << "AcademyEstablish()" << std::endl;

	uint32_t npc_id = packet.Read<uint32_t>();

	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();

	if(!is_npc_storage(npc_id))
		throw Exception::InvalidPacketArg();

	if (m_player->level() < REQUIRED_LEVEL)
	{
		//send(packet::AcademyError(packet::ERROR_ACADEMY_ESTABLISH_LEVEL));
		return;
	}

	if (!m_player->spend_gold(ESTABLISH_FEED))
	{
		std::cout << "Insuficient gold." << std::endl;
		return;
	}

	//DB::ACADEMY::Check check_query;

	//if (check_query(m_conn,m_player->get_name()))
	//{
	//	send(packet::AcademyError(packet::ERROR_ACADEMY_ESTABLISHED));
	//	return;
	//}


	//TODO SEND NEW ACADEMY DATA AND ADD TO THE DB!!
	///DWORD ID = 0;

	///send(packet::AcademyUpdate(ID,packet::STATUS_LOGIN));
}

void AcademyDisband::operator() (IPacket &packet)
{
	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();

	//TODO: CHECK IF THERES AN ACADEMY TO DISBAND!! GET ACADEMY MEMBERS
	//UPDATE THE DB AND ADD PENALTY FOR GUILD ESTABLISHING!!

	std::vector<std::string> list;

	send_to_list(PacketACAD_Notify(ACADEMY_DISBAND),list);

	send(PacketACAD_Disband());
}
*/

int StateGame::OnAcademyList (IPacket &packet)
{
	uint8_t page = packet.Read<uint8_t>();

	if (!packet.EndOfStream())
        return MSG_ERROR_SIZE;

//	uint8_t max_pages = AcademyManager::instance().max_pages();
//
//	if (page > max_pages)
//        return MSG_SUCCESS;
//
//	std::vector<Academy> list = AcademyManager::instance().get_list(page);
//
//	send(PacketACAD_List(max_pages,page,list));

    return MSG_SUCCESS;
}

/*
void AcademyNotice::operator() (IPacket &packet)
{
	std::cout << "AcademyNotice()" << std::endl;

	std::string title = packet.Read();

	std::string notice = packet.Read();

	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();

	std::cout << "Title = " << title << std::endl;
	std::cout << "Notice = " << notice << std::endl;

	//DB::ACADEMY::Members member_query;
	//std::vector<std::string> list = member_query(m_conn,m_player->get_name());

	//send_to_list(packet::AcademyUpdate(title,notice),list);

	//DB::ACADEMY::Notice notice_query;
	//notice_query(m_conn,m_player->get_name(),title,notice);

	//send(packet::AcademyNotice());
}

void AcademyRemove::operator() (IPacket &packet)
{
	uint32_t ID = packet.Read<uint32_t>();

	if (!packet.EndOfStream())
		throw Exception::InvalidPacketSize();

	send(PacketACAD_Remove(ID));
}
*/

int StateGame::OnAcademyRank (IPacket &packet)
{
    return MSG_SUCCESS;
}
