#include "SessionManager.h"
#include <QCoreApplication>

QString SessionManager::getMembersJsonPath() const {
#ifdef JAMBAEK_MAC
    return "/Users/jambaek/Desktop/VEDAQT/ERP/Data/members.json";
#else
    return QCoreApplication::applicationDirPath() + "/members.json";
#endif
}
