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

    const Country* getCountry(QString) const;
    QStringList getContinents() const;
    QStringList getDXCCs() const;

private:
    const Country* checkPrefix(QString) const;

    QList<Country*> m_countries;
    QStringList m_continents;
    QStringList m_dxccs;
};

}
}

#endif // COUNTRYFILE_H
