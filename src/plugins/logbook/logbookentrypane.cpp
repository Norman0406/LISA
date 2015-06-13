#include "logbookentrypane.h"
#include "ui_logbookentrywidget.h"
#include "database.h"
#include "logbookentrydelegate.h"
#include "logbookmode.h"

#include <QMessageBox>

using namespace Logbook::Internal;

LogbookEntryPane::LogbookEntryPane(LogbookMode* mode, QWidget* parent)
    : Core::IOutputPane(parent),
      m_qsoEntryCopy(0),
      m_profile(0),
      m_mode(mode)
{
    m_widget = new QWidget(parent);
    m_ui = new Ui::LogbookEntryWidget();
    m_ui->setupUi(m_widget);

    m_itemDelegate = new LogbookEntryDelegate(m_ui, this);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->setOrientation(Qt::Horizontal);

    m_profileList = new QComboBox(parent);
    m_profileList->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_toolbarWidgets.push_back(m_profileList);

    m_buttonAdd = new QPushButton(tr("Add / Modify"), m_widget);
    connect(m_buttonAdd, &QPushButton::clicked, this, &LogbookEntryPane::addQso);
    m_toolbarWidgets.push_back(m_buttonAdd);

    m_buttonClear = new QPushButton(tr("Clear"), m_widget);
    connect(m_buttonClear, &QPushButton::clicked, this, &LogbookEntryPane::clearQso);
    m_toolbarWidgets.push_back(m_buttonClear);

    connect(m_profileList, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedProfileChanged(int)));
    connect(mode, &LogbookMode::profilesChanged, this, &LogbookEntryPane::updateProfiles);
}

LogbookEntryPane::~LogbookEntryPane()
{
    foreach (QWidget* widget, m_toolbarWidgets)
        delete widget;

    delete m_mapper;
    delete m_itemDelegate;
    delete m_widget;
    delete m_ui;
}

QWidget* LogbookEntryPane::outputWidget(QWidget* parent)
{
    Q_UNUSED(parent)
    return m_widget;
}

QList<QWidget*> LogbookEntryPane::toolBarWidgets() const
{
    return m_toolbarWidgets;
}

void LogbookEntryPane::visibilityChanged(bool visible)
{
}

void LogbookEntryPane::setFocus()
{
}

bool LogbookEntryPane::hasFocus() const
{
}

bool LogbookEntryPane::canFocus() const
{
    return false;
}

void LogbookEntryPane::rowSelected(QSqlRelationalTableModel* model, int row)
{
    // check if data has changed
//    bool isDirty = false;
//    for (int i = 0; i < model->columnCount(); i++) {
//        QWidget* mappedWidget = m_mapper->mappedWidgetAt(i);
//        if (mappedWidget){
//            QByteArray name = m_mapper->mappedPropertyName(mappedWidget);
//            QModelIndex index = model->index(m_mapper->currentIndex(), i);
//            if (index.data(Qt::EditRole) != mappedWidget->property(name)) {
//                isDirty = true;
//                break;
//            }
//        }
//    }

//    if (isDirty) {
//        if (QMessageBox::warning(m_widget, tr("Unsaved changes"),
//                                 tr("You have unsaved changes. Do you want to save the changed data?"),
//                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
//            addQso();
//        }
//    }

    m_mapper->setModel(model);
    m_mapper->setItemDelegate(m_itemDelegate);
    m_mapper->addMapping(m_ui->dateTimeEdit_QSO_Date, 1);
    m_mapper->addMapping(m_ui->dateTimeEdit_QSO_UTC, 1);
    m_mapper->addMapping(m_ui->lineEdit_QSO_CallsignTo, 3);
    m_mapper->addMapping(m_ui->lineEdit_Personal_Name, 4);
    m_mapper->addMapping(m_ui->lineEdit_QSO_Frequency, 5);
    m_mapper->addMapping(m_ui->lineEdit_QSO_RstSent, 7);
    m_mapper->addMapping(m_ui->spinBox_QSO_SentNumber, 8);
    m_mapper->addMapping(m_ui->lineEdit_QSO_RstRcvd, 9);
    m_mapper->addMapping(m_ui->spinBox_QSO_RcvdNumber, 10);
    m_mapper->addMapping(m_ui->plainTextEdit_QSO_Comment, 11);
    m_mapper->setCurrentIndex(row);
}

void LogbookEntryPane::addQso()
{
    m_mapper->submit();
}

void LogbookEntryPane::clearQso()
{
    m_mapper->revert();
}

void LogbookEntryPane::updateProfiles(const QList<ProfileData>& profiles)
{
    int selectedIndex = 0;

    if (m_selectedProfile.isNull())
        m_selectedProfile = profiles[selectedIndex].getUuid();

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

void LogbookEntryPane::selectedProfileChanged(int index)
{
    if (index >= 0) {
        m_selectedProfile = m_profileUuids[index];
        setProfile(m_mode->getProfile(m_selectedProfile));
    }
}

void LogbookEntryPane::setProfile(const ProfileData* profile)
{
    m_profile = profile;

    // a valid profile should at least have a callsign entry
    bool profileValid = profile && !profile->getCallsign().isEmpty();

    if (!profileValid)
        m_widget->setEnabled(false);
    else
        m_widget->setEnabled(true);
}
