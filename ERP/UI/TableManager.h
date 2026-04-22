#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QTableWidget>
#include <QStringList>
#include <QVector>

// QTableWidget의 데이터 관리 및 설정을 담당하는 보조 클래스
class TableManager {
public:
    // 관리할 QTableWidget을 전달받아 테이블 기본 설정을 적용
    // - table: 관리할 QTableWidget 포인터
    explicit TableManager(QTableWidget *table);

    // 테이블에 데이터를 출력
    // - data: 표시할 행별 문자열 리스트 목록
    void displayData(const QVector<QStringList> &data);

    // 테이블의 헤더를 설정
    // - headers: 헤더 이름 리스트
    void setupTable(const QStringList &headers);

private:
    QTableWidget *m_table;                     // 관리 대상이 되는 QTableWidget 포인터
};

#endif // TABLEMANAGER_H
