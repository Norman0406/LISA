#ifndef COUNTRYFILE_H
#define COUNTRYFILE_H

#include <QString>
#include <QList>
#include <QMap>

namespace Logbook {
namespace Internal {

struct Country
{
    QString Name;
    int CQZone;
    int ITUZone;
    QString Continent;
    double Latitude;
    double Longitude;
    double GMTOffset;
    QString DXCCPrefix;
    QStringList Prefixes;
};

class CountryFile
{
public:
    CountryFile();
    ~CountryFile();

    void open();

    const Country* getCountry(QString callsign) const;

private:
    QList<Country*> m_countries;
    QMap<QString, QList<Country*>> m_prefixMap;
};

}
}

#endif // COUNTRYFILE_H
