#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "signupdialog.h"
#include "Core/DataManager.h"
#include "Core/SessionManager.h"
#include "Utils/CsvManager.h"

#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDate>
#include <QTableWidgetItem>
#include <QApplication>
#include <QInputMethod>
#include <QDir>
#include <QInputDialog>
#include <QLineEdit>
#include <algorithm>

// MainWindow 생성자
// - UI 초기화 및 위젯 매니저 설정을 수행합니다.
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    m_inventoryTableManager = new TableManager(ui->tableInventory);
    m_inventoryTableManager->setupTable({"품목코드", "품목명", "합계"});

    m_logTableManager = new TableManager(ui->tableLog);
    m_logTableManager->setupTable({"시간", "구분", "품목코드", "품목명", "지점명", "수량"});

    m_searchTableManager = new TableManager(ui->tableItemManageResult);
    m_searchTableManager->setupTable({"시간", "구분", "품목코드", "품목명", "지점명", "수량"});

    connect(ui->CloseButton, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->CloseButton_2, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->CloseButton_3, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->LogoutButton_2, &QPushButton::clicked, this, &MainWindow::on_LogoutButton_clicked);
}

MainWindow::~MainWindow()
{
    delete m_inventoryTableManager;
    delete m_logTableManager;
    delete m_searchTableManager;
    delete ui;
}

// --- 네비게이션 이벤트 ---

void MainWindow::on_btnDashboardMenu_clicked()
{
    ui->btnDashboardMenu->setChecked(false);
    ui->btnDashboardMenu_2->setChecked(true);
    ui->stackedWidget->setCurrentIndex(1);
    refreshInventoryView();
}

void MainWindow::on_btnInventoryMenu_clicked()
{
    ui->btnInventoryMenu->setChecked(false);
    ui->btnInventoryMenu_2->setChecked(true);
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btnDashboardMenu_2_clicked()
{
    ui->btnDashboardMenu->setChecked(true);
    ui->btnDashboardMenu_2->setChecked(false);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_btnInventoryMenu_2_clicked()
{
    ui->btnInventoryMenu->setChecked(true);
    ui->btnInventoryMenu_2->setChecked(false);
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_LogoutButton_clicked()
{
    if (QMessageBox::question(this, "Logout", "로그아웃 하시겠습니까?") != QMessageBox::Yes) {
        return;
    }

    SessionManager::getInstance().logout();
    ui->IDedit->clear();
    ui->PWedit->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

// --- 사용자 인증 이벤트 ---

void MainWindow::on_SignupButton_clicked()
{
    ui->IDedit->clear();
    ui->PWedit->clear();
    SignupDialog signup(this);
    if (signup.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Success", "회원가입이 완료되었습니다.");
    }
}

void MainWindow::on_SigninButton_clicked()
{
    QString id = ui->IDedit->text().trimmed();
    QString pw = ui->PWedit->text().trimmed();

    if (id.isEmpty() || pw.isEmpty()) {
        QMessageBox::warning(this, "Error", "아이디와 비밀번호를 입력해주세요.");
        return;
    }

    if (authenticateUser(id, pw)) {
        SessionManager::getInstance().setUserId(id);
        QMessageBox::information(this, "Login", "로그인 성공");
        
        ui->btnDashboardMenu->setChecked(true);
        ui->stackedWidget->setCurrentIndex(1);
        
        loadSearchSettings();
        refreshAllViews();
    }
}

bool MainWindow::authenticateUser(const QString &id, const QString &pw)
{
    QFile file(SessionManager::getInstance().getMembersJsonPath());
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "등록된 사용자 정보 파일이 없습니다. 회원가입을 먼저 해주세요.");
        ui->IDedit->clear();
        ui->PWedit->clear();
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        QMessageBox::warning(this, "Error", "사용자 데이터 형식이 올바르지 않습니다.");
        ui->IDedit->clear();
        ui->PWedit->clear();
        return false;
    }
    QJsonArray array = doc.array();

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        if (obj["id"].toString() == id && obj["password"].toString() == pw) {
            return true;
        }
    }
    QMessageBox::warning(this, "Error", "로그인 정보가 일치하지 않습니다.");
    ui->IDedit->clear();
    ui->PWedit->clear();
    return false;
}

// --- 재고 관리 비즈니스 로직 ---

void MainWindow::refreshAllViews()
{
    refreshInventoryView();
    updateSearchResults();
}

void MainWindow::refreshInventoryView()
{
    QStringList branches;
    QMap<QString, QMap<QString, int>> itemStockMap = DataManager::getInstance().calculateStocks(branches);
    QVector<QStringList> allLogs = DataManager::getInstance().loadAllLogs();

    QStringList headers = {"품목코드", "품목명", "합계"};
    headers.append(branches);
    m_inventoryTableManager->setupTable(headers);

    QVector<QStringList> displayData;
    int totalStock = 0;
    int todayInbound = 0;
    int todayOutbound = 0;
    QString todayStr = QDate::currentDate().toString("yyyy-MM-dd");

    for (QMap<QString, QMap<QString, int>>::iterator it = itemStockMap.begin(); it != itemStockMap.end(); ++it) {
        QStringList keyParts = it.key().split("|");
        int itemTotal = 0;
        QStringList rowData;
        rowData << keyParts.value(0) << keyParts.value(1);

        for (const QString &branch : branches) {
            int branchQty = it.value().value(branch, 0);
            itemTotal += branchQty;
        }
        totalStock += itemTotal;
        rowData << QString::number(itemTotal);
        
        for (const QString &branch : branches) {
            rowData << QString::number(it.value().value(branch, 0));
        }
        displayData.append(rowData);
    }

    for (const QStringList &log : allLogs) {
        if (log.size() < 6) {
            continue;
        }
        if (log[0].startsWith(todayStr)) {
            if (log[1] == "입고") {
                todayInbound += log[5].toInt();
            } else if (log[1] == "출고") {
                todayOutbound += log[5].toInt();
            }
        }
    }

    m_inventoryTableManager->displayData(displayData);
    
    QVector<QStringList> logDisplay = allLogs;
    std::reverse(logDisplay.begin(), logDisplay.end());
    m_logTableManager->displayData(logDisplay);

    ui->labelTotalStock->setText(QString::number(totalStock));
    ui->labelInboundCount->setText(QString::number(todayInbound));
    ui->labelOutboundCount->setText(QString::number(todayOutbound));
    ui->labelUser->setText(SessionManager::getInstance().getUserId());
    ui->labelUser_2->setText(SessionManager::getInstance().getUserId());
}

void MainWindow::on_btnInbound_clicked()
{
    QString code, name;
    InputDialog dlg("입고", DataManager::getInstance().getBranchList(), this);
    
    if (dlg.exec() == QDialog::Accepted) {
        QString errorMsg;
        if (!DataManager::getInstance().validateProductConsistency(dlg.getCode(), dlg.getName(), errorMsg)) {
            QMessageBox::warning(this, "입고 오류", errorMsg);
            return;
        }

        QStringList log;
        log << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm")
            << "입고" << dlg.getCode() << dlg.getName() << dlg.getLocation() << QString::number(dlg.getQuantity());
        
        DataManager::getInstance().saveLog(log);
        refreshInventoryView();
    }
}

void MainWindow::on_btnOutbound_clicked()
{
    InputDialog dlg("출고", DataManager::getInstance().getBranchList(), this);
    if (dlg.exec() == QDialog::Accepted) {
        QStringList branches;
        QMap<QString, QMap<QString, int>> stocks = DataManager::getInstance().calculateStocks(branches);
        QString key = dlg.getCode() + "|" + dlg.getName();
        
        if (stocks.value(key).value(dlg.getLocation(), 0) < dlg.getQuantity()) {
            QMessageBox::warning(this, "재고 부족", "해당 지점의 재고가 부족합니다.");
            return;
        }

        QStringList log;
        log << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm")
            << "출고" << dlg.getCode() << dlg.getName() << dlg.getLocation() << QString::number(dlg.getQuantity());
        
        DataManager::getInstance().saveLog(log);
        refreshInventoryView();
    }
}

void MainWindow::on_btnAddBranch_clicked()
{
    bool ok;
    QString branchName = QInputDialog::getText(this, "지점 추가", "지점명:", QLineEdit::Normal, "", &ok).trimmed();
    if (ok && !branchName.isEmpty()) {
        QStringList log;
        log << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm")
            << "지점추가" << "" << "" << branchName << "0";
        DataManager::getInstance().saveLog(log);
        refreshAllViews();
    }
}

// --- 검색 및 수정 관련 ---

void MainWindow::updateSearchResults()
{
    QString searchDate = ui->dateSearch->date().toString("yyyy-MM-dd");
    QString searchItem = ui->editSearchItem->text().trimmed();
    QString searchType = ui->comboLogType->currentText();

    QVector<QStringList> allLogs = DataManager::getInstance().loadAllLogs();
    QVector<QStringList> results;

    for (const QStringList &row : allLogs) {
        if (row.size() < 6) {
            continue;
        }
        if (!m_isResetMode && !row[0].startsWith(searchDate)) {
            continue;
        }
        if (searchType != "전체" && row[1] != searchType) {
            continue;
        }
        
        bool matchItem = searchItem.isEmpty() || row[2].contains(searchItem, Qt::CaseInsensitive) || row[3].contains(searchItem, Qt::CaseInsensitive);
        if (matchItem) {
            results.append(row);
        }
    }

    std::reverse(results.begin(), results.end());
    m_searchTableManager->displayData(results);
    
    if (!m_isResetMode) {
        saveSearchSettings();
    }
}

void MainWindow::on_btnResetSearch_clicked()
{
    ui->editSearchItem->clear();
    ui->comboLogType->setCurrentIndex(0);
    ui->dateSearch->setDate(QDate::currentDate());
    
    m_isResetMode = true;
    updateSearchResults();
    m_isResetMode = false;
    saveSearchSettings();
}

void MainWindow::on_tableItemManageResult_cellClicked(int row, int column)
{
    QStringList headers = {"시간", "구분", "품목코드", "품목명", "지점명", "수량"};
    if (column < 0 || column >= headers.size()) {
        return;
    }

    ui->editSelectedColumn->setText(headers[column]);
    
    if (column == 0 || column == 1) {
        ui->editCurrentValue->clear();
        ui->editNewValue->clear();
        ui->btnSaveEdit->setEnabled(false);
        ui->editNewValue->setPlaceholderText("수정 불가 컬럼");
        return;
    }

    ui->btnSaveEdit->setEnabled(true);
    QTableWidgetItem *item = ui->tableItemManageResult->item(row, column);
    if (item) {
        ui->editCurrentValue->setText(item->text());
        ui->editNewValue->setText(item->text());
        ui->editNewValue->setFocus();
    }
}

void MainWindow::on_btnSaveEdit_clicked()
{
    int row = ui->tableItemManageResult->currentRow();
    int col = ui->tableItemManageResult->currentColumn();
    if (row < 0 || col < 0) {
        return;
    }

    QApplication::inputMethod()->commit();
    QString newValue = ui->editNewValue->text().trimmed();
    QVector<QStringList> allLogs = DataManager::getInstance().loadAllLogs();
    
    QStringList targetRow;
    for (int i = 0; i < 6; ++i) {
        QTableWidgetItem *item = ui->tableItemManageResult->item(row, i);
        targetRow << (item ? item->text().trimmed() : "");
    }

    QString currentValue = targetRow.value(col).trimmed();
    if (newValue.isEmpty()) {
        QMessageBox::warning(this, "수정 오류", "수정할 값을 입력해주세요.");
        return;
    }

    if (newValue != currentValue && (col == 2 || col == 3 || col == 4)) {
        QString proposedCode = (col == 2) ? newValue : targetRow.value(2).trimmed();
        QString proposedName = (col == 3) ? newValue : targetRow.value(3).trimmed();

        for (const QStringList &log : allLogs) {
            if (log.size() < 6) {
                continue;
            }

            bool isTargetRow = true;
            for (int i = 0; i < 6; ++i) {
                if (log[i].trimmed() != targetRow.value(i).trimmed()) {
                    isTargetRow = false;
                    break;
                }
            }
            if (isTargetRow) {
                continue;
            }

            QString existingCode = log[2].trimmed();
            QString existingName = log[3].trimmed();
            QString existingLocation = log[4].trimmed();

            if ((col == 2 || col == 3) && existingCode == proposedCode) {
                QMessageBox::warning(this, "수정 오류", "이미 존재하는 품목코드입니다.");
                return;
            }
            if ((col == 2 || col == 3) && existingName == proposedName) {
                QMessageBox::warning(this, "수정 오류", "이미 존재하는 품목명입니다.");
                return;
            }
            if (col == 4 && existingLocation == newValue) {
                QMessageBox::warning(this, "수정 오류", "이미 존재하는 지점명입니다.");
                return;
            }
        }
    }
    
    for (QStringList &r : allLogs) {
        bool match = true;
        for (int i = 0; i < 6; ++i) {
            if (r[i] != targetRow[i]) {
                match = false;
            }
        }
        if (match) {
            r[col] = newValue;
            break;
        }
    }
    
    if (DataManager::getInstance().updateLogs(allLogs)) {
        QMessageBox::information(this, "Success", "수정 완료");
        refreshAllViews();
    }
}

void MainWindow::on_btnExport_clicked()
{
    QString userId = SessionManager::getInstance().getUserId();
    QString defaultName = QString("%1_export_%2.csv").arg(userId).arg(QDate::currentDate().toString("yyyyMMdd"));
    QString fileName = QFileDialog::getSaveFileName(this, "Export", QDir::homePath() + "/" + defaultName, "CSV (*.csv)");
    
    if (!fileName.isEmpty()) {
        if (CsvManager::saveCsv(fileName, DataManager::getInstance().loadAllLogs())) {
            QMessageBox::information(this, "Success", "내보내기 성공");
        }
    }
}

void MainWindow::on_btnExport_2_clicked()
{
    on_btnExport_clicked();
}

// --- 설정 관련 ---

void MainWindow::loadSearchSettings()
{
    QFile file(SessionManager::getInstance().getMembersJsonPath());
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonArray array = doc.array();
    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        if (obj["id"].toString() == SessionManager::getInstance().getUserId()) {
            QJsonObject settings = obj["search_settings"].toObject();
            if (settings.contains("date")) {
                ui->dateSearch->setDate(QDate::fromString(settings["date"].toString(), "yyyy-MM-dd"));
            }
            if (settings.contains("item")) {
                ui->editSearchItem->setText(settings["item"].toString());
            }
            if (settings.contains("log_type")) {
                ui->comboLogType->setCurrentText(settings["log_type"].toString());
            }
            break;
        }
    }
}


void MainWindow::saveSearchSettings()
{
    QString userId = SessionManager::getInstance().getUserId();
    if (userId.isEmpty()) {
        return;
    }

    QString path = SessionManager::getInstance().getMembersJsonPath();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        if (obj["id"].toString() == userId) {
            QJsonObject settings;
            settings["date"] = ui->dateSearch->date().toString("yyyy-MM-dd");
            settings["item"] = ui->editSearchItem->text().trimmed();
            settings["log_type"] = ui->comboLogType->currentText();
            obj["search_settings"] = settings;
            array[i] = obj;
            break;
        }
    }

    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(array).toJson());
        file.close();
    }
}
void MainWindow::on_btnSearchExecute_clicked()
{
    updateSearchResults();
}

void MainWindow::on_btnDeleteSelectedCell_clicked()
{
    int row = ui->tableItemManageResult->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "삭제할 셀을 선택해주세요.");
        return;
    }

    if (QMessageBox::question(this, "Confirm", "선택한 데이터를 삭제하시겠습니까?") != QMessageBox::Yes) {
        return;
    }

    QStringList targetRow;
    for (int i = 0; i < 6; ++i) {
        QTableWidgetItem *item = ui->tableItemManageResult->item(row, i);
        targetRow << (item ? item->text() : "");
    }

    QVector<QStringList> allLogs = DataManager::getInstance().loadAllLogs();
    bool found = false;
    QString targetType = targetRow.value(1);
    QString targetLocation = targetRow.value(4);

    if (targetType == "지점추가" && !targetLocation.isEmpty()) {
        for (QVector<QStringList>::iterator it = allLogs.begin(); it != allLogs.end(); ) {
            bool isSameBranchLog = it->size() >= 5 && it->value(4) == targetLocation;

            if (isSameBranchLog) {
                it = allLogs.erase(it);
                found = true;
            } else {
                ++it;
            }
        }
    } else {
        for (QVector<QStringList>::iterator it = allLogs.begin(); it != allLogs.end(); ++it) {
            bool match = true;
            for (int i = 0; i < 6; ++i) {
                if ((*it)[i] != targetRow[i]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                allLogs.erase(it);
                found = true;
                break;
            }
        }
    }

    if (found && DataManager::getInstance().updateLogs(allLogs)) {
        QMessageBox::information(this, "Success", "삭제되었습니다.");
        refreshAllViews();
    }
}
