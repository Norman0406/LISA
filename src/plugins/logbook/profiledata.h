#ifndef PROFILEDATA_H
#define PROFILEDATA_H

#include <QString>

namespace Logbook {
namespace Internal {

class ProfileData
{
public:
    ProfileData(bool isRemovable, bool isRenamable, QString profileName);
    ProfileData(const ProfileData&);

    bool isRemovable() const;
    bool isRenamable() const;

    QString getProfileName() const;
    QString getCallsign() const;
    QString getName() const;
    QString getStreet() const;
    QString getZipCode() const;
    QString getCity() const;

    bool setProfileName(QString);
    void setCallsign(QString);
    void setName(QString);
    void setStreet(QString);
    void setZipCode(QString);
    void setCity(QString);

private:
    bool m_isRemovable;
    bool m_isRenamble;
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
