/****************************************************************************
 *
 * Copyright (C) 2018 Pinecone Inc. All rights reserved.
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include <QtAndroidExtras/QtAndroidExtras>
#include <QtAndroidExtras/QAndroidJniObject>
#include "QGCApplication.h"
#include "AndroidInterface.h"
#include <QAndroidJniObject>

QString AndroidInterface::getSdcardPath()
{
    QAndroidJniObject value = QAndroidJniObject::callStaticObjectMethod("org/mavlink/qgroundcontrol/QGCActivity", "getSdcardPath",
                            "()Ljava/lang/String;");
    return value.toString();
}

void AndroidInterface::triggerMediaScannerScanFile(QString& file_path)
{
    QAndroidJniObject path = QAndroidJniObject::fromString(file_path);
    QAndroidJniObject::callStaticMethod<void>("org/mavlink/qgroundcontrol/QGCActivity", "triggerMediaScannerScanFile",
                                               "(Ljava/lang/String;)V",
                                               path.object<jstring>());
}
