#include "DataManager.h"
#include "SessionManager.h"
#include "../Utils/CsvManager.h"
#include <QCoreApplication>
#include <QDateTime>

QString DataManager::getInventoryFilePath() const {
    QString userId = SessionManager::getInstance().getUserId();
#ifdef JAMBAEK_MAC
    return "/Users/jambaek/Desktop/VEDAQT/ERP/Data/" + userId + "_inventory_data.csv";
#else
    return QCoreApplication::applicationDirPath() + "/" + userId + "_inventory_data.csv";
#endif
}

QVector<QStringList> DataManager::loadAllLogs() {
    return CsvManager::readCsv(getInventoryFilePath());
}

bool DataManager::saveLog(const QStringList &logRow) {
    QVector<QStringList> allLogs = loadAllLogs();
    allLogs.append(logRow);
    return CsvManager::saveCsv(getInventoryFilePath(), allLogs);
}

bool DataManager::updateLogs(const QVector<QStringList> &allLogs) {
    return CsvManager::saveCsv(getInventoryFilePath(), allLogs);
}

QStringList DataManager::getBranchList() const {
    QStringList branches;
    branches << "본사";

    QVector<QStringList> allLogs = CsvManager::readCsv(getInventoryFilePath());

    for (const QStringList &row : allLogs) {
        if (row.size() < 6) {
            continue;
        }

        QString type = row[1].trimmed();
        QString location = row[4].trimmed();

        if ((type == "지점추가" || type == "입고" || type == "출고")
            && !location.isEmpty()
            && !branches.contains(location)) {
            branches << location;
        }
    }
    return branches;
}

QMap<QString, QMap<QString, int>> DataManager::calculateStocks(QStringList &outBranches) {
    QVector<QStringList> allLogs = loadAllLogs();
    QMap<QString, QMap<QString, int>> itemStockMap;
    
    outBranches.clear();
    outBranches << "본사";

    for (const QStringList &row : allLogs) {
        if (row.size() < 6) {
            continue;
        }

        QString type = row[1].trimmed();
        QString code = row[2].trimmed();
        QString name = row[3].trimmed();
        QString location = row[4].trimmed();
        int qty = row[5].toInt();

        if (!location.isEmpty() && !outBranches.contains(location)) {
            outBranches << location;
        }

        QString itemKey = code + "|" + name;

        if (type == "품목추가") {
            // 초기화
            if (!itemStockMap.contains(itemKey)) {
                for (const QString &branch : outBranches) {
                    itemStockMap[itemKey][branch] = 0;
                }
            }
        }
        else if (type == "입고") {
            itemStockMap[itemKey][location] += qty;
        }
        else if (type == "출고") {
            itemStockMap[itemKey][location] -= qty;
            if (itemStockMap[itemKey][location] < 0) {
                itemStockMap[itemKey][location] = 0;
            }
        }
    }
    return itemStockMap;
}

bool DataManager::validateProductConsistency(const QString &code, const QString &name, QString &errorMsg) {
    QVector<QStringList> allLogs = loadAllLogs();
    for (const QStringList &row : allLogs) {
        if (row.size() < 4) {
            continue;
        }
        QString exCode = row[2].trimmed();
        QString exName = row[3].trimmed();

        if (exCode.isEmpty() || exName.isEmpty()) {
            continue;
        }

        if (exCode == code && exName != name) {
            errorMsg = QString("이미 존재하는 코드('%1')에 다른 이름('%2')이 지정되어 있습니다.").arg(exCode).arg(exName);
            return false;
        }
        if (exName == name && exCode != code) {
            errorMsg = QString("이미 존재하는 이름('%1')에 다른 코드('%2')가 지정되어 있습니다.").arg(exName).arg(exCode);
            return false;
        }
    }
    return true;
}
