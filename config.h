#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <vector>
#include <map>

#include "json.hpp"


class Config
{
public:
    Config();

    bool createConfig(QString conf);
    bool parseConfig();

    nlohmann::json get_config(){return config;}

private:
    QString config_location;
    nlohmann::json config;

    QString loadConfig();
};

#endif // CONFIG_H
