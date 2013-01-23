/**
 * pk2Reader.cpp
 *
 *
 * Credits :
 *           Drew Benton (Creator)
 *           npcdoom (linux port)
 *
 *
 **/

#include "pk2Reader.h"
#include "blowfish.hpp"

#include <cstring>

// Start 1 byte alignment
#pragma pack(push, 1)

	// The PK2 Header
	struct tPk2Header
	{
		char header[30];
		uint32_t version;
		uint32_t challenge;
		unsigned char reserved[218];
	};

	// The private PK2 File Entry Header
	struct tPk2EntryPrivate
	{
		uint8_t type;
		char name[81];
		uint64_t accessTime;
		uint64_t createTime;
		uint64_t modifyTime;
		uint32_t positionLow;
		uint32_t positionHigh;
		uint32_t size;
		uint32_t nextChain;
		uint16_t reserved1;
		uint16_t reserved2;
		uint16_t reserved3;
	};

// End 1 byte alignment
#pragma pack(pop)


struct tNode
{
	tNode * parent;
	tNode * sibling;
	tNode * child;
	tPk2EntryPrivate entry;
};

IPK2::IPK2 ()
{
    // Clear our private data
	pBlowFish = NULL;
	mHeader = NULL;
	bLoaded = false;
	allDone = false;
	bInit = false;
}

IPK2::~IPK2 ()
{
    deinitialize();
}

bool IPK2::initialize ()
{
    // Make sure we are not already initialized
    if(bInit)
    {
        m_error = "cPk2Reader::Initialize - The object is already initialized.";
        return false;
    }

    // Silkroad Pk2 key data (modified key, not algorithm)
    unsigned char keyData[] = {0x32, 0xCE, 0xDD, 0x7C, 0xBC, 0xA8};

    // Make sure our Pk2Header is actually 256 bytes at runtime.
    uint32_t dwSize = sizeof(tPk2Header);
    if(dwSize != 256)
    {
        m_error = "cPk2Reader::Initialize - The tPk2Header structure is not 256 bytes on this computer.";
        return false;
    }

    // Make sure our tPk2EntryPrivate is actually 128 bytes at runtime.
    dwSize = sizeof(tPk2EntryPrivate);
    if(dwSize != 128)
    {
        m_error = "cPk2Reader::Initialize - The tPk2EntryPrivate structure is not 128 bytes on this computer.";
        return false;
    }

    // Allocate new memory for the header
    mHeader = new tPk2Header;

    // Make sure the system has enough memory
    if(mHeader == NULL)
    {
        m_error = "cPk2Reader::Initialize - There is not enough memory to allocate a tPk2Header object.";
        return false;
    }

    // Zero out the memory
    memset(mHeader, 0, sizeof(tPk2Header));

    // Allocate memory for the blowfish object
    pBlowFish = new cBlowFish;

    // Make sure the system has enough memory
    if(pBlowFish == NULL)
    {
        delete mHeader;
        mHeader = 0;

        m_error = "cPk2Reader::Initialize - There is not enough memory to allocate a cBlowFish object.";
        return false;
    }

    // Initilize the blowfish algo
    pBlowFish->Initialize(keyData, 6);

    // We are now initialized
    bInit = true;

    // Success
    return true;
}

void IPK2::deinitialize ()
{
    if(mHeader)
    {
        // Deallocate memory
        delete mHeader;
        mHeader = 0;
    }

    if(pBlowFish)
    {
        // Deallocate memory
        delete pBlowFish;
        pBlowFish = 0;
    }

    // Clear our private data
    bLoaded = false;
    allDone = false;
    bInit = false;
}

bool IPK2::open (const std::string &filename)
{
    // Level of the files
    int level = 0;

    // make sure we are initilized
    if(!bInit)
    {
        // Log the error
        m_error = "IPK2::open() - This object has not been initialized yet.";

        // Error
        return false;
    }

    m_file.open(filename.c_str(), std::ios::out);

    if (m_file.fail())
    {
        m_error = "Unable to open file " + filename;
        return false;
    }

    m_filename = filename;

    // Build the name of the pk2 file itself
    //(assumes its in the right format of 'media.pk2' and not something weird like media.pk2.blah)
    size_t start = m_filename.find_last_of('\\') + 1;
    size_t count = m_filename.find_last_of('.');

    m_PK2Name = m_filename.substr(start, count - start);

    // Close the existing pk2 if we have one opened
    ///Close();

    // Try to read in the header
    m_file.read((char*)mHeader,sizeof(tPk2Header));

    if (m_file.fail())
    {
        m_error = "IPK2::open() - Error while reading PK2 header.";
        return false;
    }

    ///CHECK THAT THE PK2 VERSION IS CORRECT
    if (mHeader->version != PK2_VERSION)
    {
        m_error = "IPK2::open() - Invalid PK2 Version.";
        return false;
    }

    ///CHECK THAT THE MAGIC NUMBER IS CORRECT
    if (mHeader->challenge != PK2_CHALLENGE)
    {
        m_error = "IPK2::open() - Invalid PK2 magic number.";
        return false;
    }

    // The string to compare the first 30 bytes of the header to
    char headerVerify[30] = {0};

    // Build the verify string
    snprintf(headerVerify, 30, "%s%c", "JoyMax File Manager!", 0x0A);

    // Compare the strings
    if(strcmp(mHeader->header, headerVerify) != 0)
    {
        m_error = "IPK2::open() - Invalid file manager header string.";
        return false;
    }

    // Input buffer for reading the pk2
    unsigned char buffer[128] = {0};

    m_file.read((char*)buffer,sizeof(tPk2EntryPrivate));

    if (m_file.fail())
    {
        m_error = "IPK2::open() - Error reading PK2 entries.";
        return false;
    }

    // New root node memory
    tNode * mRoot = new tNode;

    // Make sure the system has enough memory
    if(!mRoot)
    {
            m_error = "IPK2::open() - There is not enough memory to allocate a tNode object.";
            return false;
    }

    // Decode the buffer into the root node
    pBlowFish->Decode(buffer, (uint8_t*)&mRoot->entry, sizeof(tPk2EntryPrivate));

    // Store the current node pointer for parsing
    tNode * current = mRoot;

    // Save the entry into the vector of entries
    tPK2Entry * e = new tPK2Entry;
    memset(e, 0, sizeof(tPK2Entry));
    snprintf(e->name, MAX_PATH, "%s", mRoot->entry.name);
    e->position = mRoot->entry.positionLow;
    e->size = mRoot->entry.size;
    e->createTime = mRoot->entry.createTime;
    e->modifyTime =  mRoot->entry.modifyTime;
    e->type = mRoot->entry.type;
    e->level = level;
    entries.insert(std::make_pair(e->name,e));

    // Clear this flag
    allDone = false;

    // Start the parse process and run it until it's done, when false is returned
    while(Parse(buffer, current, level));

    // Clear this flag
    allDone = false;

    m_file.close();

    // Deallocate memory from the root
    delete mRoot;
    mRoot = 0;

    // Prestore
    size_t size1 = memoryList.size();

    // Loop through all the memory entries
    for(size_t x = 0; x < size1; ++x)
    {
            // Get the pointer
            tNode * tmp = (tNode *)memoryList[x];

            // Free the memory
            delete tmp;
    }

    // Empty the list
    memoryList.clear();

    // Path for the entry
    char path[2048] = {0};

    // Current file directory depth
    int curLevel = 0;

    // Prestore
    size1 = entries.size();

    // Looop through all entries to build file path
    for (iterator it = entries.begin(); it != entries.end(); ++it)
    {
        // If this entry has a name
        if (it->first.length())
        {
            // If there is a sub directory to process
            if(curLevel < it->second->level)
            {
                // If it's a directory
                const_iterator prev = it--;
                it++;

                if(prev->second->type == 1)
                {
                    // Build the path
                    strcat(path, prev->second->name);
                    strcat(path, "\\");

                    // New level
                    curLevel++;
                }
            }

            // If the current level is deeper than the current level
            while(curLevel > it->second->level)
            {
                // Go up one level
                curLevel--;

                // If we are back at root level, just clear the whole path
                if(curLevel == 0)
                {
                    memset(path, 0, 2048);
                }
                // Otherwise
                else
                {
                    // Store the length of the current path
                    size_t olen = strlen(path);

                    // Only if there is a current path
                    if(olen)
                    {
                        // Take out the last path seperator
                        path[olen - 1] = 0;

                        // Reduce string length
                        olen--;

                        // Loop backwards looking for the last '\'
                        for(size_t x = olen - 1; x > 0; --x)
                        {
                            // If it matches a '\'
                            if(path[x] == '\\')
                                    break;

                            // Clear out the character
                            path[x] = 0;
                        }
                    }
                }
            }

            // Copy the path into the entry
            snprintf(it->second->path, MAX_PATH, "%s", path);
        }
    }

    // We have a file loaded now
    bLoaded = true;

    return true;
}

void IPK2::close ()
{
    // No longer a file loaded
    bLoaded = false;

    for(iterator it = entries.begin(); it != entries.end(); ++it)
    {
        if(it->second)
            delete it->second;
    }

    // Clear all the entries now
    entries.clear();

    m_filename.clear();
    m_PK2Name.clear();

}

bool IPK2::is_loaded () const
{
    return bLoaded;
}

const std::string& IPK2::get_filename () const
{
    return m_filename;
}

const std::string& IPK2::get_pk2_name () const
{
    return m_PK2Name;
}

const std::string& IPK2::get_last_error () const
{
    return m_error;
}

// Return a vector containing all of the entries in the PK2 file
const std::map<std::string,tPK2Entry *>& IPK2::get_all_entries() const
{
    return entries;
}

IPK2::const_iterator IPK2::find (const std::string &name) const
{
    return entries.find(name);
}

size_t IPK2::size () const
{
    return entries.size();
}

IPK2::const_iterator IPK2::begin () const
{
    return entries.begin();
}

IPK2::const_iterator IPK2::end () const
{
    return entries.end();
}

// Private internal parsing function
bool IPK2::Parse(unsigned char * buffer, tNode * &current, int level)
{
    // Are we in a NULL section
    static bool inClear = false;

    // Try to read in the header
    m_file.read((char*)buffer,sizeof(tPk2EntryPrivate));

    if (m_file.fail())
    {
        m_error = "IPK2::Parse() - Error reading PK2 entry";
        return false;
    }

    // If we are done processing all entries, return
    if(allDone)
    {
        // End this cycle
        return false;
    }

    // Holds the temp entry
    tPk2EntryPrivate tmpEntry;

    // Decode the buffer into the entry
    pBlowFish->Decode(buffer, (uint8_t*)&tmpEntry, sizeof(tPk2EntryPrivate));

    // End of the chain when these are not all 0's -- ASSUMPTION!
    if(!(tmpEntry.reserved1 == 0 && tmpEntry.reserved2 == 0 && tmpEntry.reserved3 == 0))
    {
        // All done
        inClear = false;

        // End this cycle
        return false;
    }

    // If there is no name, it is a NULL entry
    if(strlen(tmpEntry.name) == 0)
    {
        // We are in a filler space
        inClear = true;
    }
    else
    {
        // If we are in the filler space
        if(inClear)
        {
            // No longer in the filler space
            inClear = false;

            // Done with this chain
            return false;
        }
    }

    // If we get to this condition, we are done parsing the pk2
    if(level == 0 && strcmp(tmpEntry.name, ".") == 0)
    {
        // We do not need to process entries anymore
        allDone = true;

        // End this cycle
        return false;
    }

    // Save the entry
    current->sibling = new tNode;
    memset(current->sibling, 0, sizeof(tNode));

    // Track the memory entry to free at the end
    memoryList.push_back(current->sibling);

    // Save the parent before we move the current pointer
    current->sibling->parent = current->parent;

    // Advance the current entry pointer
    current = current->sibling;

    // Store the new data
    memcpy(&current->entry, &tmpEntry, sizeof(tPk2EntryPrivate));

    if (strlen(tmpEntry.name))
    {
        // Save the entry into the vector of entries
        tPK2Entry * e = new tPK2Entry;
        memset(e, 0, sizeof(tPK2Entry));
        snprintf(e->name, MAX_PATH, "%s", tmpEntry.name);
        e->position = tmpEntry.positionLow;
        e->size = tmpEntry.size;
        e->createTime = tmpEntry.createTime;
        e->modifyTime =  tmpEntry.modifyTime;
        e->type = tmpEntry.type;
        e->level = level;

        // Save the entry
        entries.insert(std::make_pair(e->name,e));
    }
    // Process the entry
    if(current->entry.type == 1 && strcmp(current->entry.name, ".") != 0 && strcmp(current->entry.name, "..") != 0)
    {
        // Current position to resume to
        uint32_t curPos = m_file.tellg();
        ///uint32_t curPos = SetFilePointer(hFile, 0, 0, FILE_CURRENT);

        // Change to the new location
        m_file.seekg(current->entry.positionLow, std::ios::beg);

        // Allocate memory for a child node
        current->child = new tNode;

        // Clear the memory
        memset(current->child, 0, sizeof(tNode));

        // Track the memory entry to free at the end
        memoryList.push_back(current->child);

        // Store the parent node
        current->child->parent = current;

        // Recurse
        while(Parse(buffer, current->child, level + 1));

        // Restore
        m_file.seekg(curPos, std::ios::beg);
    }

    // If we have a pointer to another set of data
    if(current->entry.nextChain)
    {
        // Current position to resume to
        uint32_t curPos = m_file.tellg();

        // Change to the new location
        m_file.seekg(current->entry.nextChain, std::ios::beg);

        // Recurse
        while(Parse(buffer, current, level));

        // Restore
        m_file.seekg(curPos, std::ios::beg);
    }

    // Success
    return true;
}
