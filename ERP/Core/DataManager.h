#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>

// 재고 및 로그 데이터를 관리하는 엔진 클래스
class DataManager {
public:
    // DataManager의 싱글톤 인스턴스를 반환
    static DataManager& getInstance() {
        static DataManager instance;
        return instance;
    }

    // 현재 로그인한 사용자의 재고 CSV 파일 경로를 반환
    QString getInventoryFilePath() const;
    
    // 로그 데이터를 기반으로 품목별, 지점별 재고를 계산
    // - outBranches: 재고 현황에 표시할 지점 목록
    // - 리턴: 품목별 지점 재고 목록
    QMap<QString, QMap<QString, int>> calculateStocks(QStringList &outBranches);

    // 로그 데이터를 기반으로 사용 가능한 지점 목록을 반환
    QStringList getBranchList() const;
    
    // 품목코드와 품목명이 1:1 관계를 유지하는지 확인
    // - 리턴: 데이터가 일관되면 true, 충돌이 있으면 false
    bool validateProductConsistency(const QString &code, const QString &name, QString &errorMsg);
    
    // 현재 사용자의 전체 재고 로그를 읽어옴
    QVector<QStringList> loadAllLogs();

    // 새로운 재고 로그 한 줄을 저장
    // - 리턴: 저장 성공 여부
    bool saveLog(const QStringList &logRow);

    // 전체 재고 로그를 전달받은 데이터로 다시 저장
    // - 리턴: 저장 성공 여부
    bool updateLogs(const QVector<QStringList> &allLogs);

private:
    DataManager() = default;
    ~DataManager() = default;
};

#endif // DATAMANAGER_H
