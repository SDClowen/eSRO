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

#include "market_stock.hpp"

int16_t Profit (const MarketStock &stock, uint16_t Quantity, uint32_t QuantitySold, uint64_t MaxSold)
{
    if (!MaxSold)
        MaxSold = 1;

    if (!QuantitySold)
        QuantitySold = 1;

    return stock.MaxProfit - stock.MinProfit*(Quantity/QuantitySold)*((QuantitySold)/MaxSold);
}
