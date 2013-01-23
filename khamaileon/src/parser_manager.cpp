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

#include "parser_manager.hpp"
#include <dirent.h>
#include <fstream>
#include <boost/bind.hpp>

std::vector<std::string> split (const std::string &line, const char delim)
{
    std::vector<std::string> entries;

    size_t beg = 0, end = 0;
    do
    {
        end = line.find_first_of(delim,beg+1);

        entries.push_back(line.substr(beg,end-beg));

        beg = end +1;

    } while (end != std::string::npos);

    return entries;
}

int textfiles (const dirent *ep)
{
    return strstr(ep->d_name,".txt") != NULL;
}

ParserManager::ParserManager (const std::string &path)
    : m_Path(path)
{
}

bool ParserManager::loadMedia ()
{
    dirent **eps;
    int n = scandir(m_Path.c_str(),&eps,textfiles,alphasort);

    if (n < 0)
        return false;

    std::ifstream file;
    std::string line;
    std::string fullpath;
    parser_const_iterator iter;
    std::vector<std::string> entries;

    for (int i = 0; i < n; ++i)
    {
        if (strcmp(eps[i]->d_name,"characterdata.txt") == 0)
        {
            loadCharacterdata();
        }
        else if (strcmp(eps[i]->d_name,"itemdata.txt") == 0)
        {
            loadItemdata();
        }
        else if (strcmp(eps[i]->d_name,"skilldata.txt") == 0)
        {
            loadSkilldata();
        }
        else if (strstr(eps[i]->d_name,"characterdata_") != NULL)
        {
        }
        else if (strstr(eps[i]->d_name,"itemdata_") != NULL)
        {
        }
        else if (strstr(eps[i]->d_name,"skilldata_") != NULL)
        {
        }
        else
        {
            std::string filename(eps[i]->d_name);
            std::transform(filename.begin(),filename.end(),filename.begin(),boost::bind(&tolower,_1));

            iter = m_Parsers.find(filename);

            if (iter == m_Parsers.end())
                continue;

            fullpath = m_Path + "/" + filename;
            file.open(fullpath.c_str(),std::ios::in);

            while (!file.eof())
            {
                getline(file,line);

                if (!file.good())
                    break;

                if (line.empty())
                    continue;

                entries = split(line,'\t');

                iter->second(entries);

                entries.clear();
            }

            file.close();
        }
    }

    return true;
}

bool ParserManager::loadCharacterdata()
{
    return ParseListFile("characterdata.txt");
}

bool ParserManager::loadItemdata()
{
    return ParseListFile("itemdata.txt");
}

bool ParserManager::loadSkilldata()
{
    return ParseListFile("skilldata.txt");
}

bool ParserManager::ParseListFile (const std::string &filename)
{
    parser_const_iterator iter = m_Parsers.find(filename);

    if (iter == m_Parsers.end())
        return false;

    std::string fullpath = m_Path + "/"  + filename;
    std::ifstream file(fullpath.c_str());

    if (!file)
        return false;

    if (file.fail())
        return false;

    std::string line;
    std::vector<std::string> file_entries;

    while (!file.eof())
    {
        getline(file,line);

        if (!file.good())
            break;

        if (line.empty())
            continue;

        line.erase(std::remove_if(line.begin(),line.end(),boost::bind(&isspace,_1)));
        std::transform(line.begin(),line.end(),line.begin(),boost::bind(&tolower,_1));
        file_entries.push_back(line);

        line.clear();
    }

    file.close();

    std::vector<std::string> entries;
    for (size_t i = 0; i < file_entries.size(); ++i)
    {
        fullpath = m_Path + "/" + file_entries[i];
        file.open(fullpath.c_str(),std::ios::in);

        while (!file.eof())
        {
            getline(file,line);

            if (!file.good())
                break;

            if (line.empty())
                continue;

            entries = split(line,'\t');

            iter->second(entries);

            entries.clear();
            line.clear();
        }

        file.close();
    }

    return true;
}

bool ParserManager::attachParser (const std::string &filename, const parser_callback_function &func)
{
    if (filename.empty())
        return false;

    std::pair<parser_iterator,bool> ret = m_Parsers.insert(std::make_pair(filename,func));

    return ret.second;
}

bool ParserManager::deattachParser (const std::string &filename)
{
    return m_Parsers.erase(filename);
}
