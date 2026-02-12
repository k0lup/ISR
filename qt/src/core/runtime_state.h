#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

struct RuntimeState {
    QString appTitle = QStringLiteral("ISR");
    QString versionString = QStringLiteral("1.0");
    int aboutAutoCloseMs = 0;

    // Find dialog
    QString findAbbr;

    // Variant dialog
    int variantCount = 0;
    int chosenVariant = -1;

    // Request dialog
    enum class OperationType { Report, Command, Message, DipolOperation, ISROperation };
    OperationType currentType = OperationType::Report;
    QString reportName = QStringLiteral("Отчет");
    QString commandName = QStringLiteral("Команда");
    QString messageName = QStringLiteral("Сообщение");
    QString dipolOperationName = QStringLiteral("Диполь-операция");
    QString isrOperationName = QStringLiteral("ISR-операция");
    QStringList requestOperationLines;
    bool makeOperation = false;

    // Title dialog
    bool titleAccepted = false;
    QString partitionAbbr;
    QString maxProtocolNumber = QStringLiteral("001");
    QString partitionTitle;
    QString protocolNamePart1;
    QString protocolNamePart2;
    QString currentProtocolName;
    QString rapName;
    QString operatorPUAISName;
    QString operatorAISName;
    QString operatorTKCKName;
    QString operatorNTKSIName;
    QString operatorBSName;

    // Settings dialog
    bool allowBackCrossing = false;
    bool menuReconfigureRequested = false;

    // SP dialog
    int bisectNumber = 1;
    int maxBisectCount = 99;
    bool spAccepted = false;

    // End dialog
    enum class EndResult { None, WithSave, WithoutSave };
    EndResult endResult = EndResult::None;

    // External directories from protocol events
    QString edDirectory;
    QString nasrDirectory;

    // CheckSPO dialog
    bool checkSpoAdjustmentEnabled = true;
    bool checkSpoStructureEnabled = true;
    bool checkSpoCpkuEnabled = true;
    bool checkSpoSsbvsEnabled = false;
    QString adjustmentFilePath;
    QString structureFilePath;
    QString cpkuProgramsPath;
    QString ssbvsProgramsPath;
    int checkSpoErrorCount = 0;

    // Partition list dialog
    QStringList allPartitions;
    QStringList hiddenPartitions;
    int partitionCount = 0;
    QString selectedPartitionAbbr;
    enum class PartitionListResult { Cancel, Choose, Load };
    PartitionListResult partitionListResult = PartitionListResult::Cancel;

    // Stencil dialog
    bool stencilExists = false;
    QString stencil;
    QString stencilFilePath;

    // Close report dialog
    QStringList openReports;
    QString lastClosedReport;
    int lastClosedReportSlot = -1;

    // Partition/selection reports
    QStringList reportCandidateSections;
    QString partisionRepName;
    QString secondRepName;
    QString thirdRepName;
    QString lastLoadedReport;
    int lastLoadedReportSlot = -1;
    QStringList selectionReportItems;
    QString lastSelectionReportItem;

    // Completing dialog
    bool completingAccepted = false;

    // Child window dialog (table-like MDI content preview)
    QVector<QStringList> childWindowRows;
};
