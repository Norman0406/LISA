#include "logbookform.h"
#include "logbookformdialog.h"
#include "logbookwindow.h"
#include <QLabel>

using namespace Logbook::Internal;

LogbookForm::LogbookForm(QWidget* parent, LogbookWindow* window)
{
    m_dialog = new LogbookFormDialog(parent, window);
}

LogbookForm::~LogbookForm()
{
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
    return QList<QWidget*>();
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
