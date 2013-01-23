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

#ifndef __SRO_SECURITY_HPP__
#define __SRO_SECURITY_HPP__

#include <stdint.h>

#define HIBYTE(w)	((unsigned char)((w & 0xff00) >> 8))
#define LOBYTE(w)	((unsigned char)(w & 0x00ff))
#define LOWORD(w)	((unsigned short)(w))
#define HIWORD(w)	((unsigned short)(((w) >> 16) & 0xFFFF))

void Func_X_2(uint8_t *stream, uint32_t key, uint8_t keyByte);

uint32_t ModExp( uint32_t P, uint32_t X, uint32_t G );

// This function was written by jMerlin as part of the article "How to generate the security bytes for SRO"
uint32_t GenerateValue( uint32_t value );

uint8_t GenerateCheckByte(const char* packet, int length, uint32_t seed );

#endif
