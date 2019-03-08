
#include "Config.h"

using namespace pallette;

Config::Config(std::string filename,
    std::string delimiter, std::string comment)
    : filename_(filename)
    , delimiter_(delimiter)
    , comment_(comment)
{
    // Construct a Config, getting keys and values from given file  

    std::ifstream in(filename_.c_str());
    if (!in)
    {
        std::ofstream out(filename_.c_str());
        out.close();
    }
    else
    {
        in >> (*this);
        in.close();
    }
}

bool Config::keyExists(const std::string& key) const
{
    // Indicate whether key is found  
    Mapci p = contents_.find(key);
    return (p != contents_.end());
}

/* static */
void Config::Trim(std::string& inoutS)
{
    // remove leading and trailing whitespace  
    static const char whitespace[] = " \n\t\v\r\f";
    inoutS.erase(0, inoutS.find_first_not_of(whitespace));
    inoutS.erase(inoutS.find_last_not_of(whitespace) + 1U);
}

void Config::remove(const std::string& key)
{
    // remove key and its value  
    contents_.erase(contents_.find(key));
}

void Config::save()
{
    std::ofstream out(filename_.c_str(), std::ios::app);
    out << *this;
    out.close();
}

std::ostream& pallette::operator<<(std::ostream& os, const Config& cf)
{
    // Save a Config to os  
    for (Config::Mapci p = cf.contents_.begin();
        p != cf.contents_.end(); ++p)
    {
        os << p->first << " " << cf.delimiter_ << " ";
        os << p->second << std::endl;
    }
    return os;
}

std::istream& pallette::operator>>(std::istream& is, Config& cf)
{
    // Load a Config from is  
    // read in keys and values, keeping internal whitespace  
    typedef std::string::size_type pos;
    const std::string& delim = cf.delimiter_;  // separator  
    const std::string& comm = cf.comment_;    // comment  
    const pos skip = delim.length();        // length of separator  

    std::string nextline = "";  // might need to read ahead to see where value ends  

    while (is || nextline.length() > 0)
    {
        // read an entire line at a time  
        std::string line;
        if (nextline.length() > 0)
        {
            line = nextline;  // we read ahead; use it now  
            nextline = "";
        }
        else
        {
            std::getline(is, line);
        }

        // Ignore comments  
        line = line.substr(0, line.find(comm));

        // Parse the line if it contains a delimiter  
        pos delimPos = line.find(delim);
        if (delimPos < std::string::npos)
        {
            // Extract the key  
            std::string key = line.substr(0, delimPos);
            line.replace(0, delimPos + skip, "");

            // See if value continues on the next line  
            // Stop at blank line, next line with a key, end of stream,  
            // or end of file sentry  
            bool terminate = false;
            while (!terminate && is)
            {
                std::getline(is, nextline);
                terminate = true;

                std::string nlcopy = nextline;
                Config::Trim(nlcopy);
                if (nlcopy == "") continue;

                nextline = nextline.substr(0, nextline.find(comm));
                if (nextline.find(delim) != std::string::npos)
                    continue;

                nlcopy = nextline;
                Config::Trim(nlcopy);
                if (nlcopy != "") line += "\n";
                line += nextline;
                terminate = false;
            }

            // Store key and value  
            Config::Trim(key);
            Config::Trim(line);
            cf.contents_[key] = line;  // overwrites if key is repeated  
        }
    }

    return is;
}

bool Config::fileExist(std::string filename)
{
    bool exist = false;
    std::ifstream in(filename.c_str());
    if (in)
    {
        exist = true;
    }
    return exist;
}