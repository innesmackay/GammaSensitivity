#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <fstream>
#include <map>

#include "TString.h"

#include "Log.hpp"

class Settings {

  public:
    Settings(){}
    Settings(std::string filename){ m_log = Log("Settings"); m_filename = filename; m_log.empty(); }
    ~Settings(){};

    void read(){
        m_log.info(("Reading settings file from " + m_filename).c_str());
        std::ifstream infile; infile.open(m_filename);

        std::string line;
        while (std::getline(infile, line)){
            line = line.substr(0, line.find("*")); // ignore comments
            if (line[0] == '*') continue;
            std::string delimiter = " ";
            std::string var = line.substr(0, line.find(delimiter)); // token is "scott"
            std::string val = line.substr(line.find(delimiter) + 1, line.size()); // token is "scott"
            m_var_map.insert({var, val});
        }
        infile.close();
        return;
    }

    void update_value(std::string key, std::string value){
        m_var_map[key] = value;
        return;
    }

    bool key_exists(std::string key){
        bool found = false;
        if (m_var_map.count(key) > 0){ found = true; }
        return found;
    }

    bool getB(std::string key){
        bool val = false;
        if ( !key_exists(key) ){ m_log.warning( ("Can't read key: " + key + "\nReturning false").c_str() ); }
        else{
            if (m_var_map[key] == "true"){ val = true; }
        }
        return val;
    }

    std::string get(std::string key){ return m_var_map[key]; }
    TString getT(std::string key){ return m_var_map[key].c_str(); }
    double getD(std::string key){ return std::stod(m_var_map[key]); }
    double getI(std::string key){ return std::stoi(m_var_map[key]); }
    std::map<std::string, std::string> get_map(){ return m_var_map; }
    std::string getFilename(){ return m_filename; }

  private:
    std::string m_filename;
    std::map<std::string, std::string> m_var_map;
    Log m_log;

};

#endif //  Settings_H
