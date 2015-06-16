#include "countryfile.h"
#include "logbookplugin.h"
#include <QFile>

using namespace Logbook::Internal;

CountryFile::CountryFile()
{
}

void CountryFile::open()
{
    // TODO: check for update
    QString ctyFilename = LogbookPlugin::resourcePath() + QLatin1String("cty.dat");
    QFile ctyFile(ctyFilename);

    if (ctyFile.isOpen()) {
        QRegExp countryDesc(QLatin1String("[(\S+)\s*:]+"));

        while (!ctyFile.atEnd()) {
            QByteArray lineByteArray = ctyFile.readLine();
            QString line(lineByteArray);

            int pos = 0;
            while ((pos = countryDesc.indexIn(line, pos)) != -1)  {
                qDebug() << countryDesc.cap(1);
                pos += countryDesc.matchedLength();
            }
        }
    }
}

