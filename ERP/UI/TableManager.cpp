#include "TableManager.h"
#include <QHeaderView>
#include <QTableWidgetItem>

TableManager::TableManager(QTableWidget *table) : m_table(table) {
    if (m_table) {
        m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->verticalHeader()->setVisible(false);
    }
}

void TableManager::setupTable(const QStringList &headers) {
    if (!m_table) {
        return;
    }
    m_table->setColumnCount(headers.size());
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TableManager::displayData(const QVector<QStringList> &data) {
    if (!m_table) {
        return;
    }
    
    m_table->setRowCount(0);
    m_table->setUpdatesEnabled(false);
    
    for (int i = 0; i < data.size(); ++i) {
        m_table->insertRow(i);
        for (int j = 0; j < data[i].size(); ++j) {
            m_table->setItem(i, j, new QTableWidgetItem(data[i][j]));
        }
    }
    
    m_table->setUpdatesEnabled(true);
}
