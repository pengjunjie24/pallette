
#ifndef PALLETTE_CONFIG_H
#define PALLETTE_CONFIG_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

namespace pallette
{
    class Config;
    std::ostream& operator<<(std::ostream& os, const Config& cf);
    std::istream& operator>>(std::istream& is, Config& cf);

    //简单的配置文件类
    class Config
    {
    public:
        typedef std::map<std::string, std::string>::iterator Mapi;
        typedef std::map<std::string, std::string>::const_iterator Mapci;

        explicit Config(std::string filename,
            std::string delimiter = "=", std::string comment = "#");

        template<class T>
        T read(const std::string& inKey, const T& inValue) const;
        template<class T>
        bool readInto(T& outVar, const std::string& inKey) const;
        template<class T>
        bool readInto(T& outVar, const std::string& inKey, const T& inValue) const;

        bool fileExist(std::string filename);

        // Check whether key exists in configuration
        bool keyExists(const std::string& inKey) const;

        // Modify keys and values
        template<class T> void add(const std::string& inKey, const T& inValue);
        void remove(const std::string& inKey);

        //Save to file
        void save();
        static void Trim(std::string& inoutS);

        // Write or read configuration
        friend std::ostream& operator<<(std::ostream& os, const Config& cf);
        friend std::istream& operator >> (std::istream& is, Config& cf);

    protected:
        template<class T> static std::string T_as_string(const T& t);
        template<class T> static T string_as_T(const std::string& s);

        std::string filename_;
        std::string delimiter_;  //!< separator between key and value
        std::string comment_;    //!< separator between value and comments
        std::map<std::string, std::string> contents_;  //!< extracted keys and values
    };

    /* static */
    template<class T>
    std::string Config::T_as_string(const T& t)
    {
        // Convert from a T to a string
        // Type T must support << operator
        std::ostringstream ost;
        ost << t;
        return ost.str();
    }

    /* static */
    template<class T>
    T Config::string_as_T(const std::string& s)
    {
        // Convert from a string to a T
        // Type T must support >> operator
        T t;
        std::istringstream ist(s);
        ist >> t;
        return t;
    }


    /* static */
    template<>
    inline std::string Config::string_as_T<std::string>(const std::string& s)
    {
        // Convert from a string to a string
        // In other words, do nothing
        return s;
    }


    /* static */
    template<>
    inline bool Config::string_as_T<bool>(const std::string& s)
    {
        // Convert from a string to a bool
        // Interpret "false", "F", "no", "n", "0" as false
        // Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
        bool b = true;
        std::string sup = s;
        for (std::string::iterator p = sup.begin(); p != sup.end(); ++p)
            *p = toupper(*p);  // make string all caps
        if (sup == std::string("FALSE") || sup == std::string("F") ||
            sup == std::string("NO") || sup == std::string("N") ||
            sup == std::string("0") || sup == std::string("NONE"))
            b = false;
        return b;
    }

    template<class T>
    T Config::read(const std::string& key, const T& value) const
    {
        // Return the value corresponding to key or given default value
        // if key is not found
        Mapci p = contents_.find(key);
        if (p == contents_.end()) return value;
        return string_as_T<T>(p->second);
    }

    template<class T>
    bool Config::readInto(T& var, const std::string& key) const
    {
        // Get the value corresponding to key and store in var
        // Return true if key is found
        // Otherwise leave var untouched
        Mapci p = contents_.find(key);
        bool found = (p != contents_.end());
        if (found) var = string_as_T<T>(p->second);
        return found;
    }

    template<class T>
    bool Config::readInto(T& var, const std::string& key, const T& value) const
    {
        // Get the value corresponding to key and store in var
        // Return true if key is found
        // Otherwise set var to given default
        Mapci p = contents_.find(key);
        bool found = (p != contents_.end());
        if (found)
            var = string_as_T<T>(p->second);
        else
            var = value;
        return found;
    }

    template<class T>
    void Config::add(const std::string& inKey, const T& value)
    {
        // add a key with given value
        std::string v = T_as_string(value);
        std::string key = inKey;
        Trim(key);
        Trim(v);
        contents_[key] = v;
    }
}

#endif