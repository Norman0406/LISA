#include "logbookentrypane.h"
#include "ui_logbookentrywidget.h"

using namespace Logbook::Internal;

LogbookEntryPane::LogbookEntryPane(QWidget* parent)
    : Core::IOutputPane(parent)
{
    m_widget = new QWidget(parent);
    m_ui = new Ui::LogbookEntryWidget();
    m_ui->setupUi(m_widget);
}

LogbookEntryPane::~LogbookEntryPane()
{
}

QWidget* LogbookEntryPane::outputWidget(QWidget* parent)
{
    Q_UNUSED(parent)
    return m_widget;
}

QList<QWidget*> LogbookEntryPane::toolBarWidgets() const
{
    return QList<QWidget*>();
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
