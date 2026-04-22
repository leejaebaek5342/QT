#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QString>
#include <QStringList>

// 품목 관련 정보 입력을 위한 전용 대화상자
class InputDialog : public QDialog {
    Q_OBJECT
public:
    // 입고, 출고에 사용할 입력 대화상자를 생성
    // - mode: "입고", "출고" 등 동작 모드
    // - branches: 선택 가능한 지점 목록
    explicit InputDialog(const QString &mode, const QStringList &branches = {}, QWidget *parent = nullptr);

    // 입력된 품목 코드를 반환
    QString getCode() const { return m_codeEdit->text(); }

    // 입력된 품목명을 반환
    QString getName() const { return m_nameEdit->text(); }

    // 품목코드와 품목명을 입력창에 미리 설정
    void setItemInfo(const QString &code, const QString &name);

    // 선택된 지점명을 반환
    QString getLocation() const;

    // 입력된 수량을 반환
    int getQuantity() const;

private:
    QLineEdit *m_codeEdit;                     // 품목 코드 입력란
    QLineEdit *m_nameEdit;                     // 품목명 입력란
    QComboBox *m_locCombo = nullptr;           // 지점 선택 콤보박스
    QSpinBox *m_qtySpin = nullptr;             // 수량 입력 스핀박스
};

#endif // INPUTDIALOG_H
