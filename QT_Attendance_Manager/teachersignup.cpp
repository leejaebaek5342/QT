#include "teachersignup.h"
#include "ui_teachersignup.h"
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStringList>

teachersignup::teachersignup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::teachersignup())
{
    ui->setupUi(this);
}

teachersignup::~teachersignup()
{
    delete ui;
}

// 회원가입 창을 닫는다.
void teachersignup::on_Closebutton_clicked()
{
    close();
}


// 입력값을 검사하고 새 선생 계정을 등록한다.
void teachersignup::on_Signupbutton_clicked()
{
    QString name = ui->Name->text().trimmed();
    QString id = ui->ID->text().trimmed();
    QString pw = ui->Password->text().trimmed();
    QString confirm = ui->Confirmpw->text().trimmed();

    if(name.isEmpty() || id.isEmpty() || pw.isEmpty() || confirm.isEmpty())
    {
        QMessageBox::warning(this,"Error","Please fill in all required fields.");
        return;
    }
    if(pw!=confirm)
    {
        QMessageBox::warning(this,"Error","The passwords do not match.");
        return;
    }

    int duplicateCheckResult = isDuplicateId(id);
    if (duplicateCheckResult == -1)
    {
        QMessageBox::warning(this,"Error","The file is not open.");
        return;
    }
    if (duplicateCheckResult == 1)
    {
        QMessageBox::warning(this,"Error","This ID already exists.");
        return;
    }

    if(!saveTeacher(name,id,pw))
    {
        QMessageBox::warning(this,"Error","The file is not open.");
        return;
    }
    QMessageBox::information(this,"Complete","Signup is complete");
    ui->Name->clear();
    ui->ID->clear();
    ui->Password->clear();
    ui->Confirmpw->clear();
}

// 회원가입에 성공한 선생 정보를 teachers.txt에 한 줄씩 저장한다.
bool teachersignup::saveTeacher(const QString &name, const QString &id, const QString &pw)
{
    QFile file("teachers.txt");
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        return false;
    }
    QTextStream out(&file);
    out << name << "," << id <<"," << pw<<'\n';
    file.close();
    return true;
}


/* 선생 ID 중복 여부를 확인하는 함수
    1: 이미 존재하는 ID
    0: 중복 없음
   -1: 파일을 열 수 없거나 현재 선생 정보가 없는 경우 */
int teachersignup::isDuplicateId(const QString &id)
{
    QFile file("teachers.txt");
    if(!file.exists())
        return 0;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if(line.isEmpty()) continue;
        QStringList parts = line.split(",");
        if(parts.size() >= 2 && parts[1]==id)
        {
            file.close();
            return 1;
        }
    }
    file.close();
    return 0;
}
