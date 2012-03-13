/**************************************************************************
**
** This file is part of the Qt Build Suite
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file.
** Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**************************************************************************/

#include "settings.h"
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <algorithm>

namespace qbs {

Settings::Settings()
    : m_globalSettings(0),
      m_localSettings(0)
{
    m_globalSettings = new QSettings(QSettings::UserScope,
                                     QLatin1String("Nokia"),
                                     QLatin1String("qbs"));
    m_globalSettings->setFallbacksEnabled(false);
}

Settings::~Settings()
{
    delete m_globalSettings;
    delete m_localSettings;
}

void Settings::loadProjectSettings(const QString &projectFileName)
{
    delete m_localSettings;
    m_localSettings = new QSettings(QFileInfo(projectFileName).path()
            + "/.qbs/config", QSettings::IniFormat);
    m_localSettings->setFallbacksEnabled(false);
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    if (m_localSettings && m_localSettings->contains(key))
        return m_localSettings->value(key, defaultValue);
    return m_globalSettings->value(key, defaultValue);
}

QVariant Settings::value(Scope scope, const QString &key, const QVariant &defaultValue) const
{
    QSettings *s = (scope == Global ? m_globalSettings : m_localSettings);
    return s ? s->value(key, defaultValue) : defaultValue;
}

QStringList Settings::allKeys() const
{
    QStringList keys;
    if (m_localSettings)
        keys = m_localSettings->allKeys();
    keys.append(m_globalSettings->allKeys());
    keys.sort();
    std::unique(keys.begin(), keys.end());
    return keys;
}

QStringList Settings::allKeys(Scope scope) const
{
    QSettings *s = (scope == Global ? m_globalSettings : m_localSettings);
    return s ? s->allKeys() : QStringList();
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    setValue(Global, key, value);
}

void Settings::setValue(Scope scope, const QString &key, const QVariant &value)
{
    QSettings *s = (scope == Global ? m_globalSettings : m_localSettings);
    Q_CHECK_PTR(s);
    s->setValue(key, value);
}

void Settings::remove(Settings::Scope scope, const QString &key)
{
    QSettings *s = (scope == Global ? m_globalSettings : m_localSettings);
    if (s)
        s->remove(key);
}

} // namespace qbs
