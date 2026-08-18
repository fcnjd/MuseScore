/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2023 MuseScore Limited and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "braillepreferencesmodel.h"

#include <QVariant>

#include "translation.h"

using namespace mu::preferences;
using namespace mu::braille;

BraillePreferencesModel::BraillePreferencesModel(QObject* parent)
    : QObject(parent), muse::Contextable(muse::iocCtxForQmlObject(this))
{
}

void BraillePreferencesModel::load()
{
    brailleConfiguration()->braillePanelEnabledChanged().onNotify(this, [this]() {
        emit braillePanelEnabledChanged(braillePanelEnabled());
    });

    brailleConfiguration()->intervalDirectionChanged().onNotify(this, [this]() {
        emit intervalDirectionChanged(intervalDirection());
    });

    brailleConfiguration()->brailleTableChanged().onNotify(this, [this]() {
        emit brailleTableChanged(brailleTable());
    });

    brailleConfiguration()->barsToShowChanged().onNotify(this, [this]() {
        emit barsToShowChanged(barsToShow());
    });
}

bool BraillePreferencesModel::braillePanelEnabled() const
{
    return brailleConfiguration()->braillePanelEnabled();
}

QString BraillePreferencesModel::brailleTable() const
{
    return brailleConfiguration()->brailleTable();
}

int BraillePreferencesModel::intervalDirection() const
{
    return static_cast<int>(brailleConfiguration()->intervalDirection());
}

int BraillePreferencesModel::barsToShow() const
{
    return static_cast<int>(brailleConfiguration()->barsToShow());
}

QStringList BraillePreferencesModel::brailleTables() const
{
    return brailleConfiguration()->brailleTableList();
}

QVariantList BraillePreferencesModel::intervalDirections() const
{
    return QVariantList {
        QVariantMap {
            //: Braille chord interval direction: automatic (based on clef)
            { "text", muse::qtrc("preferences", "Auto") },
            { "value", static_cast<int>(BrailleIntervalDirection::Auto) },
        },
        QVariantMap {
            //: Braille chord interval direction: up (ascending)
            { "text", muse::qtrc("preferences", "Up") },
            { "value", static_cast<int>(BrailleIntervalDirection::Up) },
        },
        QVariantMap {
            //: Braille chord interval direction: down (descending)
            { "text", muse::qtrc("preferences", "Down") },
            { "value", static_cast<int>(BrailleIntervalDirection::Down) },
        },
    };
}

QVariantList BraillePreferencesModel::barsToShowOptions() const
{
    return QVariantList {
        QVariantMap { { "text", QString::number(1) }, { "value", static_cast<int>(BrailleBarsToShow::One) } },
        QVariantMap { { "text", QString::number(2) }, { "value", static_cast<int>(BrailleBarsToShow::Two) } },
        QVariantMap { { "text", QString::number(3) }, { "value", static_cast<int>(BrailleBarsToShow::Three) } },
        QVariantMap { { "text", QString::number(4) }, { "value", static_cast<int>(BrailleBarsToShow::Four) } },
        QVariantMap { { "text", QString::number(5) }, { "value", static_cast<int>(BrailleBarsToShow::Five) } },
        QVariantMap { { "text", QString::number(6) }, { "value", static_cast<int>(BrailleBarsToShow::Six) } },
        QVariantMap {
            //: Braille panel: show all bars belonging to the current printed system
            { "text", muse::qtrc("preferences", "According to the print system") },
            { "value", static_cast<int>(BrailleBarsToShow::System) },
        },
    };
}

void BraillePreferencesModel::setBraillePanelEnabled(bool value)
{
    if (value == braillePanelEnabled()) {
        return;
    }

    brailleConfiguration()->setBraillePanelEnabled(value);
    emit braillePanelEnabledChanged(value);
}

void BraillePreferencesModel::setBrailleTable(QString table)
{
    if (table == brailleTable()) {
        return;
    }

    brailleConfiguration()->setBrailleTable(table);
    emit brailleTableChanged(table);
}

void BraillePreferencesModel::setIntervalDirection(int direction)
{
    if (direction == intervalDirection()) {
        return;
    }

    brailleConfiguration()->setIntervalDirection(static_cast<BrailleIntervalDirection>(direction));
    emit intervalDirectionChanged(direction);
}

void BraillePreferencesModel::setBarsToShow(int value)
{
    if (value == barsToShow()) {
        return;
    }

    brailleConfiguration()->setBarsToShow(static_cast<BrailleBarsToShow>(value));
    emit barsToShowChanged(value);
}
