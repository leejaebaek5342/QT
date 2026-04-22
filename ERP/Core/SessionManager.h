#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QString>
#include <QJsonObject>

// 사용자 세션 및 개인화 설정을 관리하는 싱글톤 클래스
class SessionManager {
public:
    // SessionManager의 싱글톤 인스턴스를 반환
    static SessionManager& getInstance() {
        static SessionManager instance;
        return instance;
    }

    // 현재 로그인한 사용자 ID를 저장
    void setUserId(const QString &id) { m_currentUserId = id; }

    // 현재 로그인한 사용자 ID를 반환
    QString getUserId() const { return m_currentUserId; }

    // 로그인 상태 여부를 반환
    bool isLoggedIn() const { return !m_currentUserId.isEmpty(); }

    // 현재 로그인한 사용자 정보를 초기화
    void logout() { m_currentUserId.clear(); }

    // 회원 정보 JSON 파일 경로를 반환
    QString getMembersJsonPath() const;

private:
    SessionManager() = default;
    ~SessionManager() = default;
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    QString m_currentUserId;                   // 현재 로그인한 사용자의 ID
};

#endif // SESSIONMANAGER_H
