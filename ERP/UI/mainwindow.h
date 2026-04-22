#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMap>
#include "Core/DataManager.h"
#include "Core/SessionManager.h"
#include "InputDialog.h"
#include "TableManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// ERP 시스템의 메인 윈도우 클래스
// - 대시보드, 재고 현황, 품목 관리 등 주요 화면 전환 및 이벤트를 제어합니다.
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // 네비게이션 및 메뉴 이벤트
    void on_btnDashboardMenu_clicked();         // Dashboard Page 에서의 Dashboard 화면 전환 버튼
    void on_btnInventoryMenu_clicked();         // Dashboard Page 에서의 Item manage 화면 전환 버튼
    void on_btnDashboardMenu_2_clicked();       // Item manage Page 에서의 Dashboard 화면 전환 버튼
    void on_btnInventoryMenu_2_clicked();       // Item manage Page 에서의 Item manage 화면 전환 버튼
    void on_LogoutButton_clicked();             // 로그아웃 버튼
    void on_SignupButton_clicked();             // 회원가입 버튼
    void on_SigninButton_clicked();             // 로그인 버튼

    // 재고 관리 이벤트
    void on_btnInbound_clicked();               // 입고 버튼
    void on_btnOutbound_clicked();              // 출고 버튼
    void on_btnAddBranch_clicked();             // 지점 추가 버튼
    
    // 검색 및 수정 이벤트
    void updateSearchResults();                 // 검색 결과 업데이트
    void on_btnResetSearch_clicked();           // 검색 초기화 버튼
    void on_tableItemManageResult_cellClicked(int row, int column); // 테이블 셀 클릭 이벤트
    void on_btnSaveEdit_clicked();              // 수정내용 save 버튼
    void on_btnExport_clicked();                // Dashboard Page 에서의 내보내기 버튼
    void on_btnExport_2_clicked();              // Item manage Page 에서의 내보내기 버튼
    void on_btnSearchExecute_clicked();         // 검색 실행 버튼
    void on_btnDeleteSelectedCell_clicked();    // 선택된 셀 삭제 버튼

private:
    // 모든 뷰(대시보드, 테이블)를 최신 데이터로 갱신
    void refreshAllViews();

    // 대시보드 및 재고 현황 테이블을 갱신
    void refreshInventoryView();
    
    // 현재 사용자 맞춤 검색 설정을 로드하거나 저장
    void loadSearchSettings();
    void saveSearchSettings();

    // 로그인 인증 로직
    bool authenticateUser(const QString &id, const QString &pw);

    Ui::MainWindow *ui;                        // UI 인터페이스 포인터
    TableManager *m_inventoryTableManager;     // 재고 현황 테이블
    TableManager *m_logTableManager;           // 물류 로그 테이블
    TableManager *m_searchTableManager;        // 품목 관리 검색 결과 테이블
    
    bool m_isResetMode = false;                // 전체 데이터 검색 여부 플래그
};

#endif // MAINWINDOW_H
