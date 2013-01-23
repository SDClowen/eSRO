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

#ifndef __LIBSLES_UTILITY_FUNCTION_HPP__
#define __LIBSLES_UTILITY_FUNCTION_HPP__

#include <utility>

using namespace boost;

template<typename T>
T calculate (int percent, const std::pair<T,T>& attr)
{
	return attr.first+(attr.second-attr.first)*(T)(percent/100);
}

template<typename T>
T calculate (int percent, int lv, const T& attr_lv, const std::pair<T,T>& attr)
{
	return calculate<T>(percent,attr)+ lv*attr_lv;
}

#endif //__LIBSLES_UTILITY_FUNCTION_HPP__
