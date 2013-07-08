/*
 * Copyright (C) by Grant Foudree <grant907@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */
 #include "accesslog.h"

AccessLog::AccessLog()
{
}

bool AccessLog::logUsbDrive(logUSB &lUsb)
{
    Sql sql((char*)Paths::getDatabasePath().c_str());
    std::stringstream sqlStatement;
    sqlStatement << "INSERT INTO loggedDrives (accepted, date, user, driveLetter, driveName, driveLabel, ";
    sqlStatement << "driveSize, driveSerial, driveGUID, usbninjaSerial) VALUES (" << lUsb.accepted << ", \'";
    sqlStatement << lUsb.date << "\', \'" << lUsb.user << "\', \'" << lUsb.driveLetter << "\', \'" << lUsb.driveName;
    sqlStatement << "\', \'" << lUsb.driveLabel << "\', " << lUsb.driveSize << ", \'" << lUsb.driveSerial;
    sqlStatement << "\', \'" << lUsb.driveGUID << "\', \'" << lUsb.usbninjaSerial << "\');";
    sql.dbExecSql((char*)sqlStatement.str().c_str());

    return true;
}

void AccessLog::createLogStruct(logUSB *lUsb, char drvLtr)
{
    lUsb->date = ErrorLog::dateAndTime();
    lUsb->user = AccessLog::userName();
    lUsb->driveLetter = drvLtr;

    std::string devId;
    UsbDevice::GetDriveDeviceId(drvLtr, &devId);
    UsbDevice::GetFriendlyName(devId, &lUsb->driveName);
    UsbDevice::GetVolumeSize(drvLtr, &lUsb->driveSize);
    UsbDevice::GetVolumeSerial(drvLtr, &lUsb->driveSerial);
    UsbDevice::GetVolumeGUID(drvLtr, &lUsb->driveGUID);
    UsbDevice::GetVolumeLabel(drvLtr, &lUsb->driveLabel);
}

std::string AccessLog::userName()
{
    char username[UNLEN+1];
    DWORD unLen = UNLEN;
    if (!GetUserNameA(username, &unLen))
        ErrorLog::logErrorToFile("Unable to get username: ", ErrorLog::winErrToStr(GetLastError()));
    return username;
}
