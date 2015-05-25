#ifndef PROFILEDATA_H
#define PROFILEDATA_H

#include <QString>
#include <QUuid>

namespace Logbook {
namespace Internal {

class ProfileData
{
public:
    ProfileData(bool isRemovable, QString profileName);
    ProfileData(QUuid uuid, bool isRemovable, QString profileName);
    ProfileData(const ProfileData&);

    QUuid getUuid() const;
    bool isRemovable() const;

    QString getProfileName() const;
    QString getCallsign() const;
    QString getName() const;
    QString getStreet() const;
    QString getZipCode() const;
    QString getCity() const;

    void setProfileName(QString);
    void setCallsign(QString);
    void setName(QString);
    void setStreet(QString);
    void setZipCode(QString);
    void setCity(QString);

private:
    QUuid m_uuid;
    bool m_isRemovable;
    QString m_profileName;
    QString m_callsign;
    QString m_name;
    QString m_street;
    QString m_zipCode;
    QString m_city;
};

}
}


#endif // PROFILEDATA_H
