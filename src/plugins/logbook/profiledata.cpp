#include "profiledata.h"

using namespace Logbook::Internal;

ProfileData::ProfileData(bool isRemovable, QString profileName)
    : m_isRemovable(isRemovable),
      m_profileName(profileName)
{
    m_uuid = QUuid::createUuid();
}

ProfileData::ProfileData(QUuid uuid, bool isRemovable, QString profileName)
    : m_uuid(uuid),
      m_isRemovable(isRemovable),
      m_profileName(profileName)
{
}

ProfileData::ProfileData(const ProfileData& other)
    : m_uuid(other.m_uuid),
      m_isRemovable(other.m_isRemovable),
      m_profileName(other.m_profileName),
      m_callsign(other.m_callsign),
      m_name(other.m_name),
      m_street(other.m_street),
      m_zipCode(other.m_zipCode),
      m_city(other.m_city)
{
}

QUuid ProfileData::getUuid() const
{
    return m_uuid;
}

bool ProfileData::isRemovable() const
{
    return m_isRemovable;
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

void ProfileData::setProfileName(QString profileName)
{
    m_profileName = profileName;
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
