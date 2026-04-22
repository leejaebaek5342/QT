#include "CsvManager.h"
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

QVector<QStringList> CsvManager::readCsv(const QString &filePath) {
    QVector<QStringList> data;
    if (filePath.isEmpty()) {
        return data;
    }

    QFile file(filePath);

    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "Date,Type,Code,Name,Location,Quantity\n";
            file.close();
        }
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return data;
    }

    QTextStream in(&file);
    bool isHeader = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (isHeader) { 
            isHeader = false; 
            continue; 
        }
        if (!line.trimmed().isEmpty()) {
            data.append(line.split(","));
        }
    }
    file.close();
    return data;
}

bool CsvManager::saveCsv(const QString &filePath, const QVector<QStringList> &data) {
    if (filePath.isEmpty()) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "Date,Type,Code,Name,Location,Quantity\n";

    for (const QStringList &row : data) {
        out << row.join(",") << "\n";
    }

    file.close();
    return true;
}
