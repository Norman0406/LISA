#include "lineeditfrequency.h"
#include <QFontDatabase>
#include <QRegExpValidator>
#include <QKeyEvent>

using namespace Logbook::Internal;

LineEditFrequency::LineEditFrequency(QWidget* parent)
    : QLineEdit(parent),
      m_frequency(0)
{
    // set a font with fixed character width
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    // set a validator that accepts only numbers and a dot
    setValidator(new QRegExpValidator(QRegExp(QLatin1String("\\d*.\\d+"))));

    setAlignment(Qt::AlignRight);
    setFrequency(m_frequency);

    connect(this, &LineEditFrequency::textChanged, this, &LineEditFrequency::valueChanged);
}

void LineEditFrequency::keyPressEvent(QKeyEvent* event)
{
    if (event->text() == QLatin1String(".")) {
        int dotPos = text().indexOf(QLatin1Char('.'));
        int curPosition = cursorPosition();

        if (dotPos > curPosition) {
            // remove all text between the cursor position and the dot position
            QString frontText = text().mid(0, curPosition);
            QString backText = text().mid(dotPos + 1, text().length());

            setText(frontText + QLatin1Char('.') + backText);
            dotPos = text().indexOf(QLatin1Char('.'));
            setCursorPosition(dotPos + 1);
        }
        else if (dotPos == curPosition)
            setCursorPosition(dotPos + 1);
    }
    else
        QLineEdit::keyPressEvent(event);
}

void LineEditFrequency::valueChanged(const QString& text)
{
    if (text.isEmpty()) {
        setText(m_prevText);
        return;
    }

    if (text == QString(m_prevText).remove(QLatin1Char('.'))) {
        // restore the previous text if the dot has been deleted
        setText(m_prevText);
        int dotPos = m_prevText.indexOf(QLatin1Char('.'));
        setCursorPosition(dotPos);
    }
    else {
        QString frqText = text;

        // if the first part of the frequency is zero, do not append
        int prevDotPos = m_prevText.indexOf(QLatin1Char('.'));
        if (prevDotPos == 1) {
            QString prevFrontText = m_prevText.mid(0, prevDotPos);
            if (prevFrontText == QLatin1String("0")) {
                int dotPos = frqText.indexOf(QLatin1Char('.'));

                if (dotPos == 2) {
                    QString frontText = frqText.mid(0, dotPos - 1);
                    QString backText = frqText.mid(frqText.indexOf(QLatin1Char('.')) + 1, frqText.length());
                    frqText = frontText + QLatin1Char('.') + backText;
                }
            }
        }

        int position = cursorPosition();

        bool ok = false;
        double frequency = frqText.toDouble(&ok);
        if (!ok)
            frequency = 0;

        setFrequency(frequency);

        setCursorPosition(position);
    }
}

void LineEditFrequency::setFrequency(double frequency)
{
    QString number = QString::number(frequency, 'f', 4);
    m_prevText = number;
    setText(number);

    if (frequency != m_frequency)
        emit frequencyChanged(frequency);

    m_frequency = frequency;
}

double LineEditFrequency::getFrequency() const
{
    return m_frequency;
}
