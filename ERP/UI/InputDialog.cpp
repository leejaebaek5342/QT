#include "InputDialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>

InputDialog::InputDialog(const QString &mode, const QStringList &branches, QWidget *parent) : QDialog(parent) {
    setWindowTitle(mode + " 정보 입력");
    QFormLayout *layout = new QFormLayout(this);

    m_codeEdit = new QLineEdit(this);
    m_nameEdit = new QLineEdit(this);

    layout->addRow("품목코드:", m_codeEdit);
    layout->addRow("품목명:", m_nameEdit);

    if (mode != "제품추가") {
        m_locCombo = new QComboBox(this);

        if (branches.isEmpty()) {
            m_locCombo->addItem("본사");
        } else {
            m_locCombo->addItems(branches);
        }

        m_qtySpin = new QSpinBox(this);
        m_qtySpin->setRange(0, 999999);

        layout->addRow("지점:", m_locCombo);
        layout->addRow("수량:", m_qtySpin);
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *okBtn = new QPushButton("확인", this);
    QPushButton *cancelBtn = new QPushButton("취소", this);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addWidget(okBtn);
    buttonLayout->addWidget(cancelBtn);

    layout->addRow(buttonLayout);
}

QString InputDialog::getLocation() const {
    return m_locCombo ? m_locCombo->currentText() : "";
}

int InputDialog::getQuantity() const {
    return m_qtySpin ? m_qtySpin->value() : 0;
}

void InputDialog::setItemInfo(const QString &code, const QString &name) {
    if (m_codeEdit) {
        m_codeEdit->setText(code);
    }
    if (m_nameEdit) {
        m_nameEdit->setText(name);
    }
}
