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

#ifndef __SLES_GAME_SERVER_PARTY_ENTRY_HPP__
#define __SLES_GAME_SERVER_PARTY_ENTRY_HPP__

#include <types.hpp>
#include <string>

/**
 *
 *	@brief Party Match entry object.
 *
 **/

class PartyEntry
{
public:

	PartyEntry ();

	PartyEntry (const uint32_t pt_number, const uint8_t type, const uint8_t purpose, const uint8_t min_lv,
		const uint8_t max_lv, const std::string& description);

	uint32_t number () const;

	uint8_t type () const;

	uint8_t purpose () const;

	uint8_t min_level () const;

	uint8_t max_level () const;

	const std::string& description () const;

private:

	uint32_t m_number;
	uint8_t m_type;
	uint8_t m_purpose;
	uint8_t m_min_lv;
	uint8_t m_max_lv;
	std::string m_description;
};

#endif //__SLES_GAME_SERVER_PARTY_ENTRY_HPP__
