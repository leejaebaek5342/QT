#include "widget.h"
#include "./ui_widget.h"
#include <QMessageBox>
#include <QFile>
#include <QStringList>
#include <QComboBox>
#include <QTableWidget>
#include <QRegularExpression>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , ts(new teachersignup)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    resetTeacherSessionData();
}

Widget::~Widget()
{
    delete ts;
    delete ui;
}

// Teacher 로그인 화면으로 이동한다.
void Widget::on_TeacherButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// Student 조회 화면으로 이동한다.
void Widget::on_StudentButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

// 로그인 입력값을 비우고 시작 화면으로 돌아간다.
void Widget::on_backbutton_clicked()
{
    ui->ID->clear();
    ui->Password->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

// 학생 조회 결과 화면에서 Teacher 메인 화면으로 돌아간다.
void Widget::on_goback_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

// Teacher 화면에서 시작 화면으로 돌아간다.
void Widget::on_goback_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// 학생 조회 입력값을 비우고 시작 화면으로 돌아간다.
void Widget::on_gobackbutton_clicked()
{
    ui->Name->clear();
    ui->StudentID->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

// 로그아웃 여부를 확인한 뒤 교사 세션 상태를 초기화한다.
void Widget::on_logoutbutton_clicked()
{
    QMessageBox::StandardButton reply;
    reply=QMessageBox::question(this, "Logout", "Do you want to logout?",
                                  QMessageBox::Yes | QMessageBox::No);
    if(reply==QMessageBox::Yes)
    {
        resetTeacherSessionData();
        currentTeacherId.clear();
        ui->ID->clear();
        ui->Password->clear();
        ui->stackedWidget->setCurrentIndex(0);
    }
    else
    {
        return;
    }
}


// 회원가입 창을 연다.
void Widget::on_signupbutton_clicked()
{
    ui->ID->clear();
    ui->Password->clear();
    ts->show();
}

// 입력한 교사 계정 정보로 로그인 여부를 확인한다.
void Widget::on_loginbutton_clicked()
{
    QString id = ui->ID->text().trimmed();
    QString pw = ui->Password->text().trimmed();
    if(id.isEmpty() || pw.isEmpty())
    {
        QMessageBox::warning(this,"Error","Please fill in all required fields.");
        return;
    }
    QFile file("teachers.txt");

    if (!file.exists())
    {
        QMessageBox::warning(this, "Error", "There is no registered teacher information.");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"Error","The file is not open.");
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if(line.isEmpty()) continue;
        QStringList parts = line.split(",");
        if(parts.size() >= 3 && id==parts[1] && pw==parts[2])
        {
            file.close();
            QMessageBox::information(this,"Complete","Login success");
            resetTeacherSessionData();
            currentTeacherId = parts[1];
            ui->TeacherName->setText(parts[0]);

            loadStudentsToTable();
            loadStudentsToComboBox();

            ui->stackedWidget->setCurrentIndex(3);
            ui->ID->clear();
            ui->Password->clear();
            return;
        }
    }
    file.close();
    QMessageBox::warning(this,"Error","There is no matching member information.");
    ui->ID->clear();
    ui->Password->clear();
    return;
}

/* 로그아웃하거나 다른 선생님으로 로그인할 때
   이전 선생님의 학생 목록, 조회 결과, 입력값이 남지 않도록 화면 상태를 초기화한다. */
void Widget::resetTeacherSessionData()
{
    ui->TeacherName->clear();
    ui->stName->clear();
    ui->stID->clear();
    ui->viewStudentName->clear();
    ui->viewStudentName_2->clear();
    ui->viewTextEdit->clear();
    ui->viewTextEdit_2->clear();
    ui->studentcombo->clear();
    ui->studentTable->clearContents();
    ui->studentTable->setRowCount(0);
}

// 선생 ID를 파일명에 안전하게 사용할 수 있도록 특수문자를 정리한다.
QString Widget::sanitizeFileComponent(const QString &value) const
{
    QString sanitized = value.trimmed();
    sanitized.replace(QRegularExpression(R"([^A-Za-z0-9_-])"), "_");

    if (sanitized.isEmpty())
        return "default";

    return sanitized;
}

// 현재 로그인한 선생님의 학생 목록 파일 경로를 만든다.
QString Widget::teacherStudentsFilePath(const QString &teacherId) const
{
    return "studentslist_" + sanitizeFileComponent(teacherId) + ".txt";
}

// 현재 로그인한 선생님의 출석 기록 파일 경로를 만든다.
QString Widget::teacherAttendanceFilePath(const QString &teacherId) const
{
    return "attendance_" + sanitizeFileComponent(teacherId) + ".txt";
}

// 조회용 콤보박스에 함께 저장해둔 학생 학번을 꺼낸다.
QString Widget::selectedStudentId() const
{
    return ui->studentcombo->currentData().toString().trimmed();
}

/*  학생 학번 중복 여부를 확인하는 함수
    1: 이미 존재하는 학번
    0: 중복 없음
   -1: 파일을 열 수 없거나 현재 선생 정보가 없는 경우 */
int Widget::isDuplicateStudentId(const QString &studentId)
{
    if (currentTeacherId.isEmpty())
        return -1;

    QFile file(teacherStudentsFilePath(currentTeacherId));

    if (!file.exists())
        return 0;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        QStringList parts = line.split(",");

        if (parts.size() >= 2 && parts[1] == studentId)
        {
            file.close();
            return 1;
        }
    }

    file.close();
    return 0;
}


// 학생 등록 입력값을 검사하고 현재 교사의 학생 목록에 추가한다.
void Widget::on_regisbutton_clicked()
{
    QString stname = ui->stName->text().trimmed();
    QString stid = ui->stID->text().trimmed();

    if (stname.isEmpty() || stid.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please fill in all required fields.");
        return;
    }
    int duplicateResult = isDuplicateStudentId(stid);
    if (duplicateResult == -1)
    {
        QMessageBox::warning(this, "Error", "The file is not open.");
        return;
    }
    if (duplicateResult == 1)
    {
        QMessageBox::warning(this, "Error", "This student ID already exists.");
        return;
    }

    if (currentTeacherId.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Teacher information is missing.");
        return;
    }

    QFile file(teacherStudentsFilePath(currentTeacherId));
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        QMessageBox::warning(this,"Error","The file is not open.");
        return;
    }
    QTextStream out(&file);
    out<<stname<<","<<stid<<'\n';
    file.close();
    ui->stName->clear();
    ui->stID->clear();
    loadStudentsToTable();
    loadStudentsToComboBox();
}
/* 현재 로그인한 선생님의 학생 목록을 테이블에 불러오고,
   각 학생마다 출석 상태를 선택할 수 있는 콤보박스를 함께 만든다. */
void Widget::loadStudentsToTable()
{
    if (currentTeacherId.isEmpty())
    {
        ui->studentTable->clearContents();
        ui->studentTable->setRowCount(0);
        return;
    }

    QFile file(teacherStudentsFilePath(currentTeacherId));

    ui->studentTable->clearContents();
    ui->studentTable->setRowCount(0);

    if (!file.exists())
    {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error", "The file is not open.");
        return;
    }

    QTextStream in(&file);

    int row = 0;

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        QStringList parts = line.split(",");

        if (parts.size() >= 2)
        {
            ui->studentTable->insertRow(row);

            ui->studentTable->setItem(row, 0, new QTableWidgetItem(parts[0]));
            ui->studentTable->setItem(row, 1, new QTableWidgetItem(parts[1]));

            QComboBox *combo = new QComboBox();
            combo->addItem("출석");
            combo->addItem("지각");
            combo->addItem("결석");

            ui->studentTable->setCellWidget(row, 2, combo);

            row++;
        }
    }

    file.close();
}

/* 조회 화면에서 학생 목록을 콤보박스에 불러온다.
   콤보박스에는 학생 이름을 표시하고, 각 항목의 내부 데이터로 학번을 함께 저장한다. */
void Widget::loadStudentsToComboBox()
{
    if (currentTeacherId.isEmpty())
    {
        ui->studentcombo->clear();
        return;
    }

    QFile file(teacherStudentsFilePath(currentTeacherId));

    ui->studentcombo->clear();

    if (!file.exists())
    {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        QStringList parts = line.split(",");

        if (parts.size() >= 2)
        {
            ui->studentcombo->addItem(parts[0], parts[1]);
        }
    }

    file.close();
}

/* 선택한 날짜의 기존 기록은 제외하고 현재 테이블 상태를 다시 저장한다.
    1: 저장 성공
   -1: 현재 선생 정보가 없는 경우
   -2: 파일을 열 수 없는 경우 */
int Widget::saveAttendanceByDate()
{
    if (currentTeacherId.isEmpty())
        return -1;

    QString selectedDate = ui->dateEdit->date().toString("yyyy-MM-dd");
    QStringList linesToKeep;

    QFile file(teacherAttendanceFilePath(currentTeacherId));

    if (file.exists())
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return -2;

        QTextStream in(&file);

        while (!in.atEnd())
        {
            QString line = in.readLine().trimmed();

            if (line.isEmpty())
                continue;

            QStringList parts = line.split(",");

            if (parts.size() >= 4)
            {
                if (parts[0] != selectedDate)
                {
                    linesToKeep.append(line);
                }
            }
        }

        file.close();
    }

    for (int row = 0; row < ui->studentTable->rowCount(); row++)
    {
        QTableWidgetItem *nameItem = ui->studentTable->item(row, 0);
        QTableWidgetItem *idItem = ui->studentTable->item(row, 1);
        QComboBox *combo = qobject_cast<QComboBox*>(ui->studentTable->cellWidget(row, 2));

        if (!nameItem || !idItem || !combo)
            continue;

        QString name = nameItem->text();
        QString studentId = idItem->text();
        QString state = combo->currentText();

        QString newLine = selectedDate + "," + name + "," + studentId + "," + state;
        linesToKeep.append(newLine);
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return -2;

    QTextStream out(&file);

    for (const QString &line : linesToKeep)
    {
        out << line << '\n';
    }

    file.close();
    return 1;
}


// 현재 테이블 상태를 기준으로 출석 데이터를 저장한다.
void Widget::on_savebutton_clicked()
{
    int saveResult = saveAttendanceByDate();

    if (saveResult == -1)
    {
        QMessageBox::warning(this, "Error", "Teacher information is missing.");
        return;
    }

    if (saveResult == -2)
    {
        QMessageBox::warning(this, "Error", "The file is not open.");
        return;
    }

    QMessageBox::information(this, "Complete", "Attendance saved successfully.");
}


// 선택한 학생의 출석 조회 화면으로 이동한다.
void Widget::on_viewbutton_clicked()
{
    QString selectedStudent = ui->studentcombo->currentText().trimmed();
    QString studentId = selectedStudentId();

    if (selectedStudent.isEmpty() || studentId.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please select a student.");
        return;
    }

    ui->viewStudentName->setText(selectedStudent);
    loadStudentAttendanceforteach(selectedStudent, studentId);
    ui->stackedWidget->setCurrentIndex(4);
}

/* 현재 로그인한 선생님의 출석 파일에서
   선택한 학생의 이름과 학번이 모두 일치하는 기록만 조회해 출력한다. */
void Widget::loadStudentAttendanceforteach(const QString &studentName, const QString &studentId)
{
    if (currentTeacherId.isEmpty())
    {
        ui->viewTextEdit->setText("No attendance record found.");
        return;
    }

    QFile file(teacherAttendanceFilePath(currentTeacherId));
    ui->viewTextEdit->clear();

    if (!file.exists())
    {
        ui->viewTextEdit->setText("No attendance record found.");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error", "The file is not open.");
        return;
    }

    QTextStream in(&file);

    bool found = false;

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        QStringList parts = line.split(",");

        if (parts.size() >= 4 && parts[1] == studentName && parts[2] == studentId)
        {
            QString record = parts[0] + " / " + parts[1] + " / " + parts[2] + " / " + parts[3];
            ui->viewTextEdit->append(record);
            found = true;
        }
    }

    file.close();

    if (!found)
    {
        ui->viewTextEdit->setText("No attendance record found.");
    }
}
/* 모든 선생님의 학생 목록 파일을 순회하면서
   입력한 이름과 학번이 실제로 존재하는지 확인한다.
   학생 정보 존재 여부를 확인하는 함수
    1: 이름과 학번이 일치하는 학생 정보가 존재함
    0: 일치하는 학생 정보가 없음
   -1: teachers.txt 파일을 열 수 없는 경우 */
int Widget::checkStudentInfo(const QString &name, const QString &id)
{
    QFile teachersFile("teachers.txt");

    if (!teachersFile.exists())
        return 0;

    if (!teachersFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream teachersIn(&teachersFile);

    while (!teachersIn.atEnd())
    {
        QString teacherLine = teachersIn.readLine().trimmed();

        if (teacherLine.isEmpty())
            continue;

        QStringList teacherParts = teacherLine.split(",");

        if (teacherParts.size() < 2)
            continue;

        QFile studentFile(teacherStudentsFilePath(teacherParts[1]));

        if (!studentFile.exists())
            continue;

        if (!studentFile.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream studentIn(&studentFile);

        while (!studentIn.atEnd())
        {
            QString line = studentIn.readLine().trimmed();

            if (line.isEmpty())
                continue;

            QStringList parts = line.split(",");

            if (parts.size() >= 2 && parts[0] == name && parts[1] == id)
            {
                studentFile.close();
                teachersFile.close();
                return 1;
            }
        }

        studentFile.close();
    }

    teachersFile.close();
    return 0;
}

/* Student 화면에서 이름과 학번을 기준으로
   모든 선생님의 출석 파일을 확인해 일치하는 기록을 화면에 출력한다. */
void Widget::loadStudentAttendanceforst(const QString &name, const QString &id)
{
    ui->viewTextEdit_2->clear();
    bool found = false;
    QFile teachersFile("teachers.txt");

    if (!teachersFile.exists())
    {
        ui->viewTextEdit_2->setText("No attendance record found.");
        return;
    }

    if (!teachersFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error", "The file is not open.");
        return;
    }

    QTextStream teachersIn(&teachersFile);

    while (!teachersIn.atEnd())
    {
        QString teacherLine = teachersIn.readLine().trimmed();

        if (teacherLine.isEmpty())
            continue;

        QStringList teacherParts = teacherLine.split(",");

        if (teacherParts.size() < 2)
            continue;

        QFile attendanceFile(teacherAttendanceFilePath(teacherParts[1]));

        if (!attendanceFile.exists())
            continue;

        if (!attendanceFile.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream attendanceIn(&attendanceFile);

        while (!attendanceIn.atEnd())
        {
            QString line = attendanceIn.readLine().trimmed();

            if (line.isEmpty())
                continue;

            QStringList parts = line.split(",");

            if (parts.size() >= 4 && parts[1] == name && parts[2] == id)
            {
                QString record = parts[0] + " / " + teacherParts[0] + " : " + parts[3];
                ui->viewTextEdit_2->append(record);
                found = true;
            }
        }

        attendanceFile.close();
    }

    teachersFile.close();

    if (!found)
    {
        ui->viewTextEdit_2->setText("No attendance record found.");
    }
}


// 입력한 이름과 학번으로 학생 출석 조회를 실행한다.
void Widget::on_Checkbutton_clicked()
{
    QString name = ui->Name->text().trimmed();
    QString id = ui->StudentID->text().trimmed();
    int studentInfoResult;

    if (name.isEmpty() || id.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please fill in all required fields.");
        return;
    }
    studentInfoResult = checkStudentInfo(name, id);
    if (studentInfoResult == -1)
    {
        QMessageBox::warning(this, "Error", "The file is not open.");
        return;
    }

    if (studentInfoResult == 0)
    {
        ui->viewStudentName_2->setText(name);
        ui->viewTextEdit_2->setText("No matching student information found.");
        ui->stackedWidget->setCurrentIndex(5);
        return;
    }

    ui->viewStudentName_2->setText(name);
    loadStudentAttendanceforst(name, id);
    ui->stackedWidget->setCurrentIndex(5);
    ui->Name->clear();
    ui->StudentID->clear();
}
