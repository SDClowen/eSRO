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

#include "job.hpp"

Job::Job ()
	: m_Level(0), m_Type(Job::NONE),m_Experience(0),m_Contribution(0)
{
}

Job::~Job ()
{
}

void Job::set_alias (const std::string &alias)
{
	m_Alias = alias;
}

void Job::set_experience (uint64_t exp)
{
	m_Experience = exp;
}

void Job::set_type (uint8_t job)
{
	m_Type = job;
}

void Job::set_level (uint8_t lvl)
{
	m_Level = lvl;
}

void Job::set_contribution (uint32_t contribution)
{
	m_Contribution = contribution;
}

std::string Job::get_alias () const
{
	return m_Alias;
}

uint64_t Job::get_experience () const
{
	return m_Experience;
}

uint8_t Job::get_type () const
{
	return m_Type;
}

uint8_t Job::get_level () const
{
	return m_Level;
}

uint32_t Job::get_contribution () const
{
	return m_Contribution;
}
