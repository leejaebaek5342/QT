#ifndef CSVMANAGER_H
#define CSVMANAGER_H

#include <QString>
#include <QStringList>
#include <QVector>

// CSV 파일 입출력을 관리하는 클래스
class CsvManager {
public:
    // CSV 파일을 읽어 QVector<QStringList> 형태로 반환
    // - filePath: 파일 경로 (공백일 경우 기본 경로 사용)
    // - 리턴: 읽어온 데이터 목록
    static QVector<QStringList> readCsv(const QString &filePath);

    // 데이터를 CSV 파일로 저장
    // - filePath: 저장할 파일 경로
    // - data: 저장할 데이터 목록
    // - 리턴: 성공 여부
    static bool saveCsv(const QString &filePath, const QVector<QStringList> &data);
};

#endif // CSVMANAGER_H
