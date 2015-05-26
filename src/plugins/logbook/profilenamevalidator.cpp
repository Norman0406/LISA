#include "profilenamevalidator.h"

using namespace Logbook::Internal;

ProfileNameValidator::ProfileNameValidator(QObject* parent)
    : QValidator(parent)
{
}

ProfileNameValidator::~ProfileNameValidator()
{
}

void ProfileNameValidator::clear()
{
    m_names.clear();
}

void ProfileNameValidator::setNames(QStringList names)
{
    m_names = names;
}

QValidator::State ProfileNameValidator::validate(QString& input, int& pos) const
{
    Q_UNUSED(pos)

    if (m_names.contains(input))
        return QValidator::Invalid;

    return QValidator::Acceptable;
}
