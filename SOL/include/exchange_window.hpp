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

#ifndef __SLES_EXCHANGE_WINDOW_HPP__
#define __SLES_EXCHANGE_WINDOW_HPP__

#include "storage.hpp"

class ExchangeWnd : public Storage
{
public:

	ExchangeWnd()
		: Storage(12,STORAGE_EXCHANGE)
	{}

	void set_gold_amount (uint32_t amount) { m_GoldAmount = amount; }

	uint32_t get_gold_amount () const { return m_GoldAmount; }

protected:

	uint32_t m_GoldAmount;
};

#endif //__SLES_EXCHANGE_WINDOW_HPP__
