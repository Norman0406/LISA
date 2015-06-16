#ifndef COUNTRYFILE_H
#define COUNTRYFILE_H

#include <QString>
#include <QList>

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
};

class CountryFile
{
public:
    CountryFile();

    void open();

private:
    QList<Country*> m_countries;
};

}
}

#endif // COUNTRYFILE_H
