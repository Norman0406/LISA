#include "countryfile.h"
#include "logbookplugin.h"
#include <QFile>

using namespace Logbook::Internal;

CountryFile::CountryFile()
{
}

CountryFile::~CountryFile()
{
    foreach (Country* country, m_countryList)
        delete country;
}

void CountryFile::open()
{
    // TODO: check for update:
    // each country file contains an entry "=VERSION". This entry will be assigned to a different country
    // for each version. So in order to check the necessity of an update, see if the VERSION prefix is
    // assigned to the same country.
    // The exact version is also written under country "Canada" in the form of "=VERyyyymmdd".

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

                if (!m_dxccs.contains(newCountry->DXCCPrefix))
                    m_dxccs << newCountry->DXCCPrefix;

                if (!m_continents.contains(newCountry->Continent))
                    m_continents << newCountry->Continent;

                if (!m_countries.contains(newCountry->Name))
                    m_countries << newCountry->Name;

                m_countryList.push_back(newCountry);
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

        Q_ASSERT(m_continents.size() == 6);
        Q_ASSERT(m_countryList.size() == m_dxccs.size());

        m_dxccs.sort();
        m_continents.sort();
        m_countries.sort();

        ctyFile.close();
    }
}

const Country* CountryFile::getCountryFromCallsign(QString callsign) const
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

const Country* CountryFile::getCountryFromName(QString name) const
{
    foreach (const Country* country, m_countryList) {
        if (country->Name == name)
            return country;
    }

    return 0;
}

const Country* CountryFile::checkPrefix(QString callsign) const
{
    Country* currentCountry = 0;
    QString currentPrefix;

    // We check for prefixes that are the beginning of the callsign. Whenever there is a matching
    // prefix that is longer, this one is preferred.
    foreach (Country* country, m_countryList) {
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

const QStringList& CountryFile::getContinents() const
{
    return m_continents;
}

const QStringList& CountryFile::getCountries() const
{
    return m_countries;
}

const QStringList& CountryFile::getDXCCs() const
{
    // UNDONE: load ADIFCnt.txt
    return m_dxccs;
}
