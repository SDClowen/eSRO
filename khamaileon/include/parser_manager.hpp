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

#ifndef PARSER_MANAGER_
#define PARSER_MANAGER_

#include <map>
#include <string>
#include <vector>
#include <boost/function.hpp>

class ParserManager
{
public:

    typedef boost::function<void (const std::vector<std::string>&)> parser_callback_function;
    typedef std::map<std::string,parser_callback_function>::iterator parser_iterator;
    typedef std::map<std::string,parser_callback_function>::const_iterator parser_const_iterator;

    ParserManager (const std::string &path);

    bool attachParser (const std::string &filename, const parser_callback_function &func);

    bool deattachParser (const std::string &filename);

    bool loadMedia ();

    bool loadCharacterdata ();

    bool loadItemdata ();

    bool loadSkilldata ();

protected:

    bool ParseListFile (const std::string &filename);

private:

    std::string m_Path;
    std::map<std::string,parser_callback_function> m_Parsers;
};

#endif // PARSER_MANAGER_
