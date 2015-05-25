#include "logbookform.h"
#include "logbookformdialog.h"
#include "logbookmode.h"
#include "logbookwindow.h"
#include <QLabel>

using namespace Logbook::Internal;

LogbookForm::LogbookForm(QWidget* parent, LogbookMode* mode, LogbookWindow* window)
    : Core::IOutputPane(parent),
      m_mode(mode),
      m_window(window)
{
    m_dialog = new LogbookFormDialog(parent, window);

    m_profileList = new QComboBox(m_dialog);
    m_profileList->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_toolbarWidgets.push_back(m_profileList);

    // fill profile list
    connect(m_profileList, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedProfileChanged(int)));
    connect(mode, &LogbookMode::profilesChanged, this, &LogbookForm::updateProfiles);
}

LogbookForm::~LogbookForm()
{
    foreach (QWidget* widget, m_toolbarWidgets)
        delete widget;

    delete m_dialog;
}

LogbookFormDialog* LogbookForm::getDialog() const
{
    return m_dialog;
}

QWidget* LogbookForm::outputWidget(QWidget* parent)
{
    return getDialog();
}

QList<QWidget*> LogbookForm::toolBarWidgets() const
{
    return m_toolbarWidgets;
}

void LogbookForm::visibilityChanged(bool visible)
{
}

void LogbookForm::setFocus()
{
}

bool LogbookForm::hasFocus() const
{
}

bool LogbookForm::canFocus() const
{
    return false;
}

void LogbookForm::updateProfiles(const QList<ProfileData>& profiles)
{
    int selectedIndex = 0;

    QUuid selectedProfile = m_selectedProfile;
    m_profileUuids.clear();
    m_profileList->clear();
    foreach (const ProfileData& profile, profiles) {
        m_profileUuids.push_back(profile.getUuid());
        m_profileList->addItem(profile.getProfileName());

        if (profile.getUuid() == selectedProfile)
            selectedIndex = m_profileUuids.count() - 1;
    }

    m_profileList->setCurrentIndex(selectedIndex);
}

void LogbookForm::selectedProfileChanged(int index)
{
    if (index >= 0) {
        m_selectedProfile = m_profileUuids[index];
        m_dialog->setProfile(m_mode->getProfile(m_selectedProfile));
    }
}
