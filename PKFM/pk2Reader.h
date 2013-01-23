/*
* pk2Reader.h
*
*
* Credits :
*           Drew Benton (Creator)
*           npcdoom (linux port)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation (version 2
* of the License).
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
*/

#ifndef PK2READER_H_
#define PK2READER_H_

#include <map>
#include <string>
#include <vector>
#include <fstream>

#define MAX_PATH 260

// Forward declarations
struct tPk2Header;
struct tNode;
class cBlowFish;

// Structure for a pk2 entry
struct tPK2Entry
{
	// Type of entry is it, 0 is NULL, 1 is a folder, 2 is a file
	int type;

	// Name of the entry
	char name[MAX_PATH + 1];

	// Path of the entry
	char path[MAX_PATH + 1];

	// Entry file times
	uint64_t createTime;
	uint64_t modifyTime;

	// Position of the entry in the pk2
	uint32_t position;

	// Size of the entry
	uint32_t size;

	// Directory depth of entry
	int level;
};

/**
 *
 *  This is a PK2 reading class. It will load a PK2, parse through the encrypted
 *  file entries and allow a user to extract files.
 *
 **/

class IPK2
{
public:

    enum
    {
        PK2_VERSION = 0x01000002,
        PK2_CHALLENGE = 0x30dad801  ///ISRO, KSRO
        //PK2_CHALLENGE = 0x5b63de01  ///SWSRO,ECSRO
    };

    typedef std::map<std::string,tPK2Entry *>::iterator iterator;
    typedef std::map<std::string,tPK2Entry *>::const_iterator const_iterator;

    IPK2 ();

    ~IPK2 ();

    /**
     *
     *  Create the PK2 access object.
     *
     **/


    bool initialize ();

    /**
     *
     *  Release the PK2 access object.
     *
     **/

    void deinitialize ();

    // Loads and parses a PK2 file
    bool open(const std::string &filename);

    // Closes the PK2 file
    void close();

    // Returns true if a PK2 file is currently loaded
    bool is_loaded() const;

    const std::string& get_filename () const;

    const std::string& get_pk2_name () const;

    const std::string& get_last_error () const;

    // Return a vector containing all of the entries in the PK2 file
    const std::map<std::string,tPK2Entry *>& get_all_entries() const;

    // Returns an entry at a specified index
    const_iterator find(const std::string &name) const;

    size_t size () const;

    const_iterator begin () const;

    const_iterator end () const;

private:

    // Private internal parsing function
    bool Parse(unsigned char * buffer, tNode * &current, int level);

private:

    cBlowFish *pBlowFish;
    std::string m_filename;
    std::string m_PK2Name;
    std::string m_error;

    tPk2Header * mHeader;   /// The private PK2 header structure

    // Initialized and loaded flag
    bool bInit;
    bool bLoaded;

    // Done parsing the pk2 flag
    bool allDone;

    // Vector of entries
    std::map<std::string,tPK2Entry*> entries;

    // A list of pointers to free from memory
    std::vector<void*> memoryList;

    std::ifstream m_file;
};

#endif
