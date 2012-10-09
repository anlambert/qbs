/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Build Suite.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "scannerpluginmanager.h"
#include "logger.h"

#include <tools/hostosinfo.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>
#include <QLibrary>

namespace qbs {
ScannerPluginManager *ScannerPluginManager::instance()
{
    static ScannerPluginManager *i = new ScannerPluginManager;
    return i;
}

ScannerPluginManager::ScannerPluginManager()
{
}

QList<ScannerPlugin *> ScannerPluginManager::scannersForFileTag(const QString &fileTag)
{
    return instance()->m_scannerPlugins.value(fileTag);
}

void ScannerPluginManager::loadPlugins(const QStringList &pluginPaths)
{
    QStringList filters;

    if (HostOsInfo::isWindowsHost())
        filters << "*.dll";
    else if (HostOsInfo::isMacHost())
        filters << "*.dylib";
    else
        filters << "*.so";

    foreach (const QString &pluginPath, pluginPaths) {
        qbsTrace("pluginmanager: loading plugins from '%s'.", qPrintable(QDir::toNativeSeparators(pluginPath)));
        QDirIterator it(pluginPath, filters, QDir::Files);
        while (it.hasNext()) {
            const QString fileName = it.next();
            QScopedPointer<QLibrary> lib(new QLibrary(fileName));
            if (!lib->load()) {
                qbsWarning("pluginmanager: couldn't load '%s'.",
                           qPrintable(QDir::toNativeSeparators(fileName)));
                continue;
            }

            getScanners_f getScanners = reinterpret_cast<getScanners_f>(lib->resolve("getScanners"));
            if (!getScanners) {
                qbsWarning("pluginmanager: couldn't resolve symbol in '%s'.",
                           qPrintable(QDir::toNativeSeparators(fileName)));
                continue;
            }

            ScannerPlugin **plugins = getScanners();
            if (plugins == 0) {
                qbsWarning("pluginmanager: no scanners returned from '%s'.",
                           qPrintable(QDir::toNativeSeparators(fileName)));
                continue;
            }

            qbsTrace("pluginmanager: scanner plugin '%s' loaded.",
                     qPrintable(QDir::toNativeSeparators(fileName)));

            for (int i = 0; plugins[i] != 0; ++i)
                m_scannerPlugins[QString::fromLocal8Bit(plugins[i]->fileTag)] += plugins[i];
            m_libs.append(lib.take());
        }
    }
}

} // namespace qbs
