#include "profiledata.h"

using namespace Logbook::Internal;

ProfileData::ProfileData(bool isRemovable, bool isRenamble, QString profileName)
    : m_isRemovable(isRemovable),
      m_isRenamble(isRenamble),
      m_profileName(profileName)
{
}

ProfileData::ProfileData(const ProfileData& other)
    : m_isRemovable(other.m_isRemovable),
      m_isRenamble(other.m_isRenamble),
      m_profileName(other.m_profileName),
      m_callsign(other.m_callsign),
      m_name(other.m_name),
      m_street(other.m_street),
      m_zipCode(other.m_zipCode),
      m_city(other.m_city)
{

}

bool ProfileData::isRemovable() const
{
    return m_isRemovable;
}

bool ProfileData::isRenamable() const
{
    return m_isRenamble;
}

QString ProfileData::getProfileName() const
{
    return m_profileName;
}

QString ProfileData::getCallsign() const
{
    return m_callsign;
}

QString ProfileData::getName() const
{
    return m_name;
}

QString ProfileData::getStreet() const
{
    return m_street;
}

QString ProfileData::getZipCode() const
{
    return m_zipCode;
}

QString ProfileData::getCity() const
{
    return m_city;
}

bool ProfileData::setProfileName(QString profileName)
{
    if (m_isRenamble) {
        m_profileName = profileName;
        return true;
    }

    return false;
}

void ProfileData::setCallsign(QString callsign)
{
    m_callsign = callsign;
}

void ProfileData::setName(QString name)
{
    m_name = name;
}

void ProfileData::setStreet(QString street)
{
    m_street = street;
}

void ProfileData::setZipCode(QString zipCode)
{
    m_zipCode = zipCode;
}

void ProfileData::setCity(QString city)
{
    m_city = city;
}
