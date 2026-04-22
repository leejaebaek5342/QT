#include "signupdialog.h"
#include "ui_signupdialog.h"
#include "Core/SessionManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

SignupDialog::SignupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignupDialog)
{
    ui->setupUi(this);
}

SignupDialog::~SignupDialog()
{
    delete ui;
}

void SignupDialog::on_btnCreateAccount_clicked()
{
    QString name = ui->editName->text().trimmed();
    QString id = ui->editUsername->text().trimmed();
    QString pw = ui->editPassword->text().trimmed();
    QString confirmpw = ui->editPasswordConfirm->text().trimmed();

    if (name.isEmpty() || id.isEmpty() || pw.isEmpty()) {
        QMessageBox::warning(this, "Error", "모든 항목을 입력해주세요.");
        return;
    }

    if (pw!=confirmpw){
        QMessageBox::warning(this, "Error", "비밀번호가 일치하지않습니다.");
        return;
    }

    if (isIdDuplicate(id)) {
        QMessageBox::warning(this, "Error", "이미 존재하는 아이디입니다.");
        return;
    }


    if (saveMember(name, id, pw)) {
        accept();
    }
}

void SignupDialog::on_btnCancel_clicked()
{
    reject();
}

bool SignupDialog::isIdDuplicate(const QString &id)
{
    QFile file(SessionManager::getInstance().getMembersJsonPath());
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        return false;
    }
    QJsonArray array = doc.array();

    for (int i = 0; i < array.size(); ++i) {
        if (array[i].toObject()["id"].toString() == id) {
            return true;
        }
    }
    return false;
}

bool SignupDialog::saveMember(const QString &name, const QString &id, const QString &pw)
{
    QString path = SessionManager::getInstance().getMembersJsonPath();
    QFile file(path);
    
    QJsonArray array;
    if (file.open(QIODevice::ReadOnly)) {
        array = QJsonDocument::fromJson(file.readAll()).array();
        file.close();
    }

    QJsonObject newUser;
    newUser["name"] = name;
    newUser["id"] = id;
    newUser["password"] = pw;
    array.append(newUser);

    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(QJsonDocument(array).toJson());
    file.close();
    return true;
}
