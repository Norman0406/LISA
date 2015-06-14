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
      m_mode(mode),
      m_model(0),
      m_editModel(0),
      m_selectedRow(-1),
      m_isDirty(false),
      m_dateTimerUpdate(false)
{
    m_widget = new QWidget(parent);
    m_ui = new Ui::LogbookEntryWidget();
    m_ui->setupUi(m_widget);

    connect(m_ui->lineEdit_QSO_CallsignTo, &QLineEdit::textChanged, this, &LogbookEntryPane::convertInputToUppercase);

    // connect to each widget, such that a signal is called whenever data changes
    QList<QWidget*> childWidgets = m_widget->findChildren<QWidget*>();
    foreach (QWidget* widget, childWidgets) {
        if (QLineEdit* w = qobject_cast<QLineEdit*>(widget))
            connect(w, SIGNAL(textChanged(QString)), this, SLOT(dirty()));
        else if (QDateTimeEdit* w = qobject_cast<QDateTimeEdit*>(widget))
            connect(w, SIGNAL(dateChanged(QDate)), this, SLOT(dirty()));
        else if (QSpinBox* w = qobject_cast<QSpinBox*>(widget))
            connect(w, SIGNAL(valueChanged(int)), this, SLOT(dirty()));
        else if (QComboBox* w = qobject_cast<QComboBox*>(widget))
            connect(w, SIGNAL(currentIndexChanged(int)), this, SLOT(dirty()));
        else if (QPlainTextEdit* w = qobject_cast<QPlainTextEdit*>(widget))
            connect(w, SIGNAL(textChanged()), this, SLOT(dirty()));
        else if (QCheckBox* w = qobject_cast<QCheckBox*>(widget))
            connect(w, SIGNAL(stateChanged(int)), this, SLOT(dirty()));
    }

    m_itemDelegate = new LogbookEntryDelegate(m_ui, this);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->setOrientation(Qt::Horizontal);

    m_profileList = new QComboBox(parent);
    m_profileList->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_toolbarWidgets.push_back(m_profileList);

    m_buttonAdd = new QPushButton(tr("Add"), m_widget);
    connect(m_buttonAdd, &QPushButton::clicked, this, &LogbookEntryPane::addQso);
    m_toolbarWidgets.push_back(m_buttonAdd);

    m_buttonClear = new QPushButton(tr("Clear"), m_widget);
    connect(m_buttonClear, &QPushButton::clicked, this, &LogbookEntryPane::clearQso);
    m_toolbarWidgets.push_back(m_buttonClear);

    m_buttonDelete = new QPushButton(tr("Delete"), m_widget);
    connect(m_buttonDelete, &QPushButton::clicked, this, &LogbookEntryPane::deleteQso);
    m_toolbarWidgets.push_back(m_buttonDelete);

    connect(m_profileList, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedProfileChanged(int)));
    connect(mode, &LogbookMode::profilesChanged, this, &LogbookEntryPane::updateProfiles);

    m_dateTimer = new QTimer(this);
    connect(m_dateTimer, &QTimer::timeout, this, &LogbookEntryPane::updateDateTime);

    m_dateTimer->setInterval(1000);
    m_dateTimer->setSingleShot(false);
    m_dateTimer->start();
    updateDateTime();
}

LogbookEntryPane::~LogbookEntryPane()
{
    foreach (QWidget* widget, m_toolbarWidgets)
        delete widget;

    delete m_mapper;
    delete m_itemDelegate;
    delete m_widget;
    delete m_ui;

    clearModel();
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

void LogbookEntryPane::setModel(QSqlRelationalTableModel* model)
{
    m_model = model;

    // create a model with the same number of columns
    delete m_editModel;
    m_editModel = new QStandardItemModel();
    m_editModel->insertColumns(0, m_model->columnCount());
    m_editModel->submit();

    m_mapper->setModel(m_editModel);
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

    // when first loading a new model, a new empty qso is added
    newQso();
}

void LogbookEntryPane::clearModel()
{
    m_model = 0;
    delete m_editModel;
    m_editModel = 0;
}

void LogbookEntryPane::rowSelected(int row)
{
    // check if data has changed
    if (checkDirty())
        addQso();

    if (m_model) {
        m_selectedRow = row;

        // revert previous rows inserted
        m_editModel->revert();
        m_editModel->insertRow(0);
        for (int i = 1; i < m_model->columnCount(); i++) {
             QModelIndex editIndex = m_editModel->index(0, i);
             QModelIndex modelIndex = m_model->index(row, i);
             QVariant data = m_model->data(modelIndex);
             m_editModel->setData(editIndex, data);
        }

        m_mapper->setCurrentIndex(0);

        m_buttonDelete->setEnabled(true);
        m_buttonAdd->setText(tr("Modify"));

        resetDirtyFlag();
    }
}

void LogbookEntryPane::addQso()
{
    if (m_model) {
        if (m_ui->lineEdit_QSO_CallsignTo->text().isEmpty()) {
            QMessageBox::warning(m_widget, tr("Missing data"),
                                 tr("Please enter a valid callsign."));
        }
        else {
            m_mapper->submit();

            int rowIndex = m_selectedRow;

            if (rowIndex < 0) {
                rowIndex = m_model->rowCount();
                m_model->insertRow(rowIndex);
            }

            // update model (starting from 1 to skip the ID)
            for (int i = 1; i < m_model->columnCount(); i++) {
                QModelIndex editIndex = m_editModel->index(0, i);
                QModelIndex modelIndex = m_model->index(rowIndex, i);
                QVariant data = m_editModel->data(editIndex);
                m_model->setData(modelIndex, data);
            }
            m_model->submitAll();
            m_model->select();

            newQso();
        }
    }
}

void LogbookEntryPane::newQso()
{
    if (m_model) {
        // clear entries
        if (m_editModel->rowCount() > 0)
            m_editModel->removeRow(0);

        m_buttonDelete->setEnabled(false);
        m_buttonAdd->setText(tr("Add"));

        m_editModel->insertRow(0);
        m_mapper->setCurrentIndex(0);

        m_selectedRow = -1;

        resetDirtyFlag();

        if (!m_dateTimer->isActive()) {
            updateDateTime();
            m_dateTimer->start();
        }
    }
}

void LogbookEntryPane::clearQso()
{
    if (m_model) {
        if (checkDirty())
            addQso();

        newQso();
    }
}

void LogbookEntryPane::deleteQso()
{
    if (m_model && m_selectedRow >= 0) {
        if (QMessageBox::warning(m_widget, tr("Really delete QSO?"),
                                 tr("Do you really want to delete this QSO? All entered data will be lost."),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            m_model->removeRow(m_selectedRow);
            m_model->submitAll();
            m_model->select();

            newQso();
        }
    }
}

void LogbookEntryPane::updateDateTime()
{
    QDateTime now = QDateTime::currentDateTimeUtc();

    QDate date = m_ui->dateTimeEdit_QSO_Date->date();
    QTime time = m_ui->dateTimeEdit_QSO_UTC->time();
    QDateTime current(date, time);

    if (current != now) {
        bool previousDirtyFlag = m_isDirty;
        m_dateTimerUpdate = true;

        m_ui->dateTimeEdit_QSO_Date->setDate(now.date());
        m_ui->dateTimeEdit_QSO_UTC->setTime(now.time());

        m_dateTimerUpdate = false;
        m_isDirty = previousDirtyFlag;
    }
}

bool LogbookEntryPane::checkDirty()
{
    if (m_isDirty) {
        if (QMessageBox::warning(m_widget, tr("Unsaved changes"),
                                 tr("You have unsaved changes. Do you want to save the changed data?"),
                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            return true;
        }
    }

    return false;
}

void LogbookEntryPane::dirty()
{
    if (!m_isDirty) {
        // stop the date timer when a manual input occurred
        if (!m_dateTimerUpdate)
            m_dateTimer->stop();

        m_isDirty = true;
    }
}

void LogbookEntryPane::resetDirtyFlag()
{
    m_isDirty = false;
}

void LogbookEntryPane::convertInputToUppercase()
{
    QLineEdit* sender = qobject_cast<QLineEdit*>(QObject::sender());
    if (sender) {
        QString text = sender->text();
        text = text.toUpper();
        sender->setText(text);
    }
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

    if (!profileValid) {
        m_widget->setEnabled(false);
        m_buttonAdd->setEnabled(false);
        m_buttonClear->setEnabled(false);
    }
    else {
        m_widget->setEnabled(true);
        m_buttonAdd->setEnabled(true);
        m_buttonClear->setEnabled(true);
    }
}
