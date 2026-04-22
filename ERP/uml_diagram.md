classDiagram
    direction TB

    class MainWindow {
        -Ui::MainWindow *ui
        -TableManager *m_inventoryTableManager
        -TableManager *m_logTableManager
        -TableManager *m_searchTableManager
        -bool m_isResetMode
        +MainWindow(parent)
        +~MainWindow()
        +on_btnDashboardMenu_clicked()
        +on_btnInventoryMenu_clicked()
        +on_btnDashboardMenu_2_clicked()
        +on_btnInventoryMenu_2_clicked()
        +on_LogoutButton_clicked()
        +on_SignupButton_clicked()
        +on_SigninButton_clicked()
        +on_btnInbound_clicked()
        +on_btnOutbound_clicked()
        +on_btnAddBranch_clicked()
        +updateSearchResults()
        +on_btnResetSearch_clicked()
        +on_tableItemManageResult_cellClicked(row, col)
        +on_btnSaveEdit_clicked()
        +on_btnExport_clicked()
        +on_btnExport_2_clicked()
        +on_btnSearchExecute_clicked()
        +on_btnDeleteSelectedCell_clicked()
        -refreshAllViews()
        -refreshInventoryView()
        -loadSearchSettings()
        -saveSearchSettings()
        -authenticateUser(id, pw)
    }

    class DataManager {
        <<Singleton>>
        +getInstance() DataManager&
        +calculateStocks(outBranches)
        +validateProductConsistency(code, name, errorMsg)
        +getBranchList()
        +loadAllLogs()
        +saveLog(logRow)
        +updateLogs(allLogs)
        -getInventoryFilePath()
        -DataManager()
        -~DataManager()
    }

    class SessionManager {
        <<Singleton>>
        +getInstance() SessionManager&
        +setUserId(id)
        +getUserId()
        +logout()
        +getMembersJsonPath()
        -SessionManager()
    }

    class TableManager {
        -QTableWidget *m_table
        +TableManager(table)
        +setupTable(headers)
        +displayData(data)
    }

    class InputDialog {
        -QLineEdit *m_codeEdit
        -QLineEdit *m_nameEdit
        -QComboBox *m_locCombo
        -QSpinBox *m_qtySpin
        +InputDialog(mode, branches, parent)
        +getCode()
        +getName()
        +getLocation()
        +getQuantity()
        +setItemInfo(code, name)
    }

    class CsvManager {
        <<Utility>>
        +readCsv(filePath)
        +saveCsv(filePath, data)
    }

    class SignupDialog {
        +SignupDialog(parent)
        +~SignupDialog()
        +on_btnCreateAccount_clicked()
        +on_btnCancel_clicked()
        -isIdDuplicate(id)
        -saveMember(name, id, pw)
    }

    %% Relationships
    MainWindow "1" *-- "3" TableManager : owns
    MainWindow ..> DataManager : uses
    MainWindow ..> SessionManager : uses
    MainWindow ..> InputDialog : creates
    MainWindow ..> SignupDialog : creates
    
    DataManager ..> CsvManager : uses
    DataManager ..> SessionManager : uses
    
    SignupDialog ..> SessionManager : uses
    
    InputDialog ..> DataManager : data lookup
    TableManager ..> MainWindow : manages widgets
