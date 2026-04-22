#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>

namespace Ui {
class SignupDialog;
}

// 회원가입을 담당하는 대화상자 클래스
class SignupDialog : public QDialog
{
    Q_OBJECT

public:
    // 회원가입 대화상자를 생성
    explicit SignupDialog(QWidget *parent = nullptr);

    // 회원가입 대화상자에서 사용하는 UI 객체를 해제
    ~SignupDialog() override;

private slots:
    // 계정 생성 버튼 클릭 시 실행되는 슬롯
    void on_btnCreateAccount_clicked();
    
    // 취소 버튼 클릭 시 실행되는 슬롯
    void on_btnCancel_clicked();

private:
    // 입력한 ID가 이미 존재하는지 확인
    // - 리턴: 중복이면 true, 사용 가능하면 false
    bool isIdDuplicate(const QString &id);

    // 신규 회원 정보를 JSON 파일에 저장
    // - 리턴: 저장 성공 여부
    bool saveMember(const QString &name, const QString &id, const QString &pw);

    Ui::SignupDialog *ui;                      // 회원가입 UI 포인터
};

#endif // SIGNUPDIALOG_H
