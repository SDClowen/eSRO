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

#include "party_entry.hpp"

PartyEntry::PartyEntry ()
	: m_number(0), m_type(255), m_purpose(255), m_min_lv(0),m_max_lv(0)
{
}

PartyEntry::PartyEntry(const uint32_t pt_number, const uint8_t type, const uint8_t purpose, const uint8_t minlv, const uint8_t maxlv,
	const std::string& description)
	: m_number(pt_number), m_type(type), m_purpose(purpose), m_min_lv(minlv), m_max_lv(maxlv),
	  m_description(description)
{
}

uint32_t PartyEntry::number () const
{
	return m_number;
}

uint8_t PartyEntry::type () const
{
	return m_type;
}

uint8_t PartyEntry::purpose () const
{
	return m_purpose;
}

uint8_t PartyEntry::min_level () const
{
	return m_min_lv;
}

uint8_t PartyEntry::max_level () const
{
	return m_max_lv;
}

const std::string& PartyEntry::description () const
{
	return m_description;
}
