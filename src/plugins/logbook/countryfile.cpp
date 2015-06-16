#include "countryfile.h"
#include "logbookplugin.h"
#include <QFile>

using namespace Logbook::Internal;

CountryFile::CountryFile()
{
}

CountryFile::~CountryFile()
{
    foreach (Country* country, m_countries)
        delete country;
}

void CountryFile::open()
{
    // TODO: check for update

    QString ctyFilename = LogbookPlugin::resourcePath() + QLatin1String("cty.dat");
    QFile ctyFile(ctyFilename);
    ctyFile.open(QFile::ReadOnly);

    if (ctyFile.isOpen()) {
        Country* lastCountry = 0;
        while (!ctyFile.atEnd()) {
            QString line = QLatin1String(ctyFile.readLine());
            QStringList countries = line.split(QLatin1Char(':'));

            if (countries.count() >= 8) {
                Country* newCountry = new Country;

                newCountry->Name = countries[0].trimmed();
                newCountry->CQZone = countries[1].trimmed().toInt();
                newCountry->ITUZone = countries[2].trimmed().toInt();
                newCountry->Continent = countries[3].trimmed();
                newCountry->Latitude = countries[4].trimmed().toDouble();
                newCountry->Longitude = countries[5].trimmed().toDouble();
                newCountry->GMTOffset = countries[6].trimmed().toInt();
                newCountry->DXCCPrefix = countries[7].trimmed();

                m_countries.push_back(newCountry);
                lastCountry = newCountry;
            }
            else if (countries.count() == 1 && lastCountry) {
                QStringList prefixes = line.split(QLatin1Char(','));

                // remove a ';' which should only be at the last entry
                foreach (QString prefix, prefixes) {
                    prefix = prefix.trimmed().remove(QRegExp(QLatin1String("[;\r\n]")));

                    // TODO: check prefix attributes

                    lastCountry->Prefixes << prefix;
                }
            }
        }

        ctyFile.close();
    }
}

const Country* CountryFile::getCountry(QString callsign) const
{
    // TODO: create a regex for callsigns

    QStringList parts = callsign.split(QLatin1Char('/'));

    // there can be multiple parts
    if (parts.count() == 1) {
        // only the callsign
        return checkPrefix(parts[0]);
    }
    else if (parts.count() == 2) {
        // callsign with either prepended country or appendix (/p, /m etc)
    }
    else if (parts.count() == 3) {
        // callsign with prepended country and appendix
        return checkPrefix(parts[1]);
    }

    return 0;
}

const Country* CountryFile::checkPrefix(QString callsign) const
{
    Country* currentCountry = 0;
    QString currentPrefix;

    // We check for prefixes that are the beginning of the callsign. Whenever there is a matching
    // prefix that is longer, this one is preferred.
    foreach (Country* country, m_countries) {
        foreach (const QString& prefix, country->Prefixes) {
            bool prefixFound = callsign.startsWith(prefix);
            if (prefixFound && !currentCountry ||
                prefixFound && currentCountry && prefix.length() > currentPrefix.length()) {
                currentCountry = country;
                currentPrefix = prefix;

                // TODO: check prefix attributes
            }
        }
    }

    return currentCountry;
}
