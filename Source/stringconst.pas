unit stringConst;
interface

//Stringconst_

resourcestring

sMustUsePartitionHomeKat = 'Файл настройки разделов должен располагаться в каталоге, определенном параметром';
sMustUsePartitionHomeKat2 = 'файла настройки пользователя';

sHandPartition = 'Р'; // H
sCommentString = 'К'; // C
sReferenceString = 'П'; // R
sSpecialFacilities = 'С'; // S

sPassportSymbol = 'П'; // P

sDIPOL_OPERATION_SYMBOL = 'О'; // O
sISR_OPERATION_SYMBOL = 'И'; // I
sVARIANT_SYMBOL = 'В'; // V
sCOMMAND_SYMBOL = 'К'; // C
sREPORT_SYMBOL = 'Д'; // R
sMESSAGE_SYMBOL = 'П'; // M
sLABEL_SYMBOL = 'М'; // L

// имя директивы ПРИСа, которая запускает раздел
sPartitionStartDirName = 'ПУСКРАЗД'; // PART_ST (!!!!!!!!!!!!!!)
// имя команды ИСР, которая запускает раздел
sISRStartDirName = 'НАЧРАЗД'; // PART_BEG
// имя метки секции приведения в исходное
sGoInitialLabel = 'ПРИ'; // RETURN_TO_INITIAL
// имя метки секции штатного проведения раздела
sNormalWorkLabel = 'ШТАТ'; // NORMAL
// имя метки секции НШС
sNShSWorkLabel = 'НШС'; // ABNORMAL

// точное название метки на запуск раздела в автоматическом режиме
sAutoStartPartitionLabel = 'АВТ'; // AUTO
// точное название метки на запуск раздела в шаговом режиме
sStepStartPartitionLabel = 'ШАГ'; // STEP

// метка блока НШС
sNShSLabel = 'НШС'; //  ABN
// метка конца блока НШС
sNShSEndLabel = 'К_'; // E_

// параметры из файлов настройки
sArticleAbnormal = 'НШС_ИЗД'; // ARTICLE_ABNORMAL
sArticleBreakdown = 'АВАР_ИЗД'; // ARTICLE_BREAKDOWN
sArticleGoInitialState = 'ПРИ_ИЗД'; // ARTICLE_RETURN_INITIAL
sIsd        = 'ИЗДЕЛИЕ';                    //ARTICLE
sProgram  = 'ПРОГРАММЫ';                      //PROGRAMS
sProtocol = 'ПРОТОКОЛ';                       //PROTOCOL
sFilesLocation = 'РАЗМЕЩЕНИЕ_ФАЙЛОВ'; // FILES_LOCATION
sAdjustmentsLocation = 'РАЗМЕЩЕНИЕ_РАЗДЕЛОВ'; //
sSPOLocation = 'РАЗМЕЩЕНИЕ_СПО'; // PARTITIONS_LOCATION
sAdjustments = 'РАЗДЕЛЫ'; // PARTITIONS
sPFKS = 'ПФКС'; // PFKS

// директива "КАТАЛОГ"
sKATALOG = 'КАТАЛОГ'; // CATALOG

// название операции сохранения протокола
sSPStament = 'СП'; // KP


// название типа ЭД НШС изделия
sEDNshSArticleName = 'НШС_ИЗД'; // ARTICLE_ABNORMAL
// название типа ЭД АВАР изделия
sEDDamageArticleName = 'АВАР_ИЗД'; // ARTICLE_BREAKDOWN
// название типа ЭД ПРИ изделия
sEDGoInitialArticleName = 'ПРИ_ИЗД'; // ARTICLE_RERURN_TO_INITIAL
// точное название директивы 'переход на структуру раздела'
sNASRDirName = 'НА_СР'; // TO_PS

sStructureFileNotLoaded = 'файл структуры не загружен';
sKOStartComment = 'ИСР: Директива';
sKOEndComment = 'ИСР: Конец директивы';
sTitleProt10 = 'Конец операции "Заголовок раздела"';
sTitleProt9 = 'Имя оператора БС: ';
sTitleProt8 = 'Имя оператора НТК СИ: ';
sTitleProt7 = 'Имя оператора ТКЦК: ';
sTitleProt6 = 'Имя оператора АИС: ';
sTitleProt5 = 'Имя оператора ПУ АИС: ';
sTitleProt4 = 'Имя РЭП: ';
sTitleProt3 = 'Имя протокола: ';
sTitleProt2 = 'Заголовок раздела: ';
sTitleProt1 = 'Операция "Заголовок раздела"';
sTypeHeader = 'Введите заголовок раздела';
sErrorWriting = 'Ошибка при записи в файл';
sStencilNotSaved = 'Трафарет не сохранен';
sSteps2 = 'Оператор установил режим работы ИСР, запрещающий делать переходы назад при перемещении по секции';
sSteps = 'Оператор установил режим работы ИСР, позволяющий делать переходы назад при перемещении по секции';
sRefusalEndPartition = 'ПРИС ответил отказом на запрос о завершении раздела';
sRefusalEndPartition2 = 'Возможно, в разделе есть незавершенные циклограммы, или ПРИС сейчас не может принимать команды от ИСР. Раздел не завершен.';
sEndOfIntDir = 'Конец внутренней директивы';
sBadDebug = 'Отладка с данным файлом настройки не может быть выполнена, т.к. количество разделов в нем не равно 1.';
sPFKSError = 'Ошибка при вызове ПФКС';
sPartitionStart = 'Запуск раздела';
sErrorFinded = 'В ней обнаружена ошибка';
sThirdPar2 = 'и не';
sThirdPar = 'третий параметр не';
sTooManyDelimters = 'разделителей параметров больше 2-х';
sTooManyParameters = 'у директивы слишком много параметров';
sIncorrectSymbol2 = '(начиная с первого символа строки параметров)';
sIncorrectSymbol = 'недопустимый символ в строке параметров директивы. Номер символа';
sOnlyFromISR = 'нельзя загрузить головной раздел из ПРИС. Головной раздел можно загрузить только из';
sNoParameters = 'отсутствуют параметры директивы';
sOpenFileDialog3 = 'не может выполнить директиву, так как открыт диалог выбора файла';
sOpenFileDialog2 = 'Загрузить раздел нельзя, так как открыт диалог выбора файла';
sOpenFileDialog = 'В данный момент загрузить раздел нельзя, так как открыт диалог выбора файла';
sNoLoadPartInDebug = 'Нельзя загружать вложенные разделы во время отладки раздела';
sDebugNow = 'Сейчас идет отладка раздела.Во время отладки раздела нельзя загружать вложенные разделы.';
sDebugNow2 = 'Если вы хотите, чтобы загружался вложенный раздел, не используйте отладку раздела. Настройте ПРИС так, чтобы в списке разделов ИСР оказались и головной, и вложенный разделы.';
sAllPartitions = 'Список разделов построен из ВСЕХ разделов, СПО которых находится в каталоге СПО.';
sAllPartitions2 = 'Поэтому добавить раздел в список разделов невозможно. Если вы добавили СПО раздела в каталог СПО и хотите, чтобы этот раздел оказался в списке разделов, выберите пункт "Перестроить список разделов".';
sIntError = 'В программе произошла внутренняя ошибка. Информация о ней будет сохранена в файле';
sIncorrectValue = 'Введенное значение некорректно. За информацией обращайтесь к эксплуатационной документации.';
sOutOfMemory = 'В программе произошла критическая ошибка, связанная с нехваткой памяти. Вероятно, Вам следует увеличить размер файла подкачки. Программа будет закрыта.';
sIntDir = 'В данный момент интерпретируется внутренняя директива ИСР';
sDirGoingFromISR = 'В ИСР из ПРИС пришла директива';
sPartitionNotLoaded = 'Раздел не загружен. Причина';
sNoOperations = 'В данном блоке не найдено ни одной операции. Невозможно встать на данный блок.';
sBlockNotFound2 = 'Данный блок не найден.';
sToBlock = 'с переходом в секцию НШС в блок';
sIncorrectLabel2 = 'с некорректной меткой';
sDirGoing = 'Пришла директива';
sMaxPartitionCount = 'количество загруженных разделов равно максимально возможному';
sPartitionError3 = 'у раздела штатно отсутствует файл структуры';
sPartitionError2 = 'раздел отсутвует в списке разделов';
sPartitionError1 = 'раздел уже загружен';
sCantClosePartition4 = 'так как ПРИС обрабатывает операцию из данного раздела.';
//sCantClosePartition3 = 'Нельзя завершить разделы так как ПРИС обрабатывает операцию (операции) из раздела (разделов).';
sCantClosePartition2 = 'так как в нем выполняется блок "Авто".';
sCantClosePartition = 'Нельзя завершить раздел';
sPartitionClose = 'Завершение раздела';
sCantLoadPartition = 'Вы не можете загрузить раздел, так как Вы находитесь не в текущем разделе.';
sPartitionLoad = 'Загрузка раздела';
sReadStencilError = 'Ошибка при чтении трафарета из файла';
sOnFileAccessError = 'Ошибка при попытке доступа к пользовательскому файлу настройки ПРИСа. Программа будет закрыта.';
sCfgFileAccessError = 'Ошибка при попытке доступа к системному файлу настройки ПРИСа. Программа будет закрыта.';
sProgramWillBeClosed = 'Программа будет закрыта';
sBadCommandLine = 'Неверный формат командной строки. Программа будет закрыта.';
sCritError = 'Критическая ошибка';
//sNoStencil = 'В системе отсутствует трафарет. Задание заголовка невозможно.';
sHeader = 'Задание заголовка';
sStructureNotLoaded = 'не загружен файл структуры. Переключится к данному разделу невозможно';
sNoPartitions = 'Разделов нет';
sNoStructureFile = 'файл структуры штатно отсутствует';
sNotOperations2 = 'не найдено ни одной операции. Невозможно встать на данный блок';
sNotOperations = 'В блоке';
sNotFound = 'не найден';
sBlock2 = 'Блок';
sLabelsError = 'Метки допустимы только в секции НШС';
sBadSection = 'Неверный вид секции';
sCantJump2 = 'Нельзя выполнить переход внутри раздела: структура не загружена';
sCantJump = 'Нельзя выполнить переход внутри раздела: нет загруженных разделов';
sNoBookmark = 'Закладка';
sNoBookmark2 = 'в файле ЭД отсутствует';
sEDFileNotFound = 'Файл ЭД на диске не найден';
sBadED = 'Неверный вид ЭД';
sNoDelimiter = 'Отсутствует разделитель между параметрами директивы';
sFileNotFound2 = 'Отсутствует файл';
sViewED = 'Просмотр ЭД';
sFileIsNotAdjoindred = 'Файл ЭД не подключен к ИСР';
sNoMemory = 'Невозможно открыть секцию файла структуры из-за нехватки памяти';
sStringOnBadPosition = 'Строка формата 5 находися на недопустимой позиции';
sBadStructure = 'Нарушена структура файла';
sOpenError = 'ошибка при попытке открытия файла структуры раздела';
sCRCError = 'Ошибка в контрольной сумме в файле структуры';
sCRCError2 = 'Загрузить структуру?';
sBadCRC = 'у файла структуры раздела не верна контрольная сумма. Оператор отказался от раздела';
sProcessTitle = 'Загрузка файла структуры';
sInPartition = 'В разделе';
sDirNotFound = 'в качестве заимствованной директории указана директория';
sDirNotFound2 = 'Данная директория в каталоге СПО не найдена';

sWriteError = 'Ошибка записи на диск';
sErrorOpenFile = 'Ошибка при попытке открыть файл';
sErrorCreateFile = 'Ошибка при попытке создать файл';
sSyntaxError = 'Синтаксическая ошибка в директиве ИСР: неизвестное имя директивы ИСР';
sInternalDirEnd = 'Конец внутренней директивы';
EndAutoBlock = 'Окончено выполнение блока "Авто"';
sIncorrectAutoBlock = 'В блок "Авто" могут входить только операции типов';
sAnd = 'и';
sIncorrectAutoBlock2 = 'Отметка блока невозможна';
sCantExecuteAutoBlock = 'Блок "Авто" не может быть выполнен, так как данный раздел не является текущим';
sAutoBlockStarted = 'Начато выполнение блока "Авто"';
sErrorIn = 'сбой в';
sNonAbnormalSection = 'не секция НШС'; 

// строковые константы
sAppTitle   = 'ИСР';
sCantClose  =  'не может быть закрыт. Если Вам он сейчас не нужен, Вы можете свернуть его на панель задач.';
sIncorrectPortion = 'Пришла некорректная порция. Инфомация о ней сохранена в файле';
sStructureFileNotFound = 'Не найден на диске файл структуры раздела';
sStructureLoaded = 'Загружена структура';
sCheckSum = 'КС';
sDocumentLoadedToWord = 'В Word загружен документ';
sPartitionNotSelected = 'раздел не выбран';
sWaitPRISLoaded = 'ждем загрузки ПРИС';
sPartitionSelected = 'выбран раздел';
sPartitionLength = 'Продолжительность раздела';
sEndPartiition = 'Завершить раздел';
sCloseApplication = 'Закрыть';
sWarning = 'Осторожно!';
sSheckSumOfSettingFile = 'Контрольная сумма файла настройки';
sProtocolCheckSPO = 'Протокол проведения контроля СПО раздела';
sPartitionSettingFileOK = 'Файл настройки раздела - ОК!';
sPartitionStructureFileOK = 'Файл структуры раздела - ОК!';
sCPKUFilesOK = 'Файлы программ на ЦПКУ - ОК!';
sGoBack1 = 'Совершен переход назад при перемещении по секции';
sGoBack2 = 'в разделе';
sGoBack3 = 'с операции';
sGoBack4 = 'на операцию';
sAnswerFromPris = 'Ответ от ПРИС';
sOperationWasMadeAbnormal = 'Операция выполнена ненормально';
sOperationWasMadeNormal = 'Операция выполнена нормально';
sOperationCantBeExecuted = 'Операция не может быть выполнена, так как идет обработка испытательной директивы';
sBlockNotFound = 'блок не найден';
sOutOfNShS = 'вне блоков НШС';
sBlock = 'блок';
sHeader00 = '№';
sHeader10 = '№';
sHeader20 = '№';
sHeader01 = 'Т';
sHeader11 = 'Т';
sHeader21 = 'Т';
sHeader02 = 'Команды, доклады, пояснения';
sHeader12 = 'Диагностика НШС';
sHeader22 = 'Команды, доклады, пояснения';
sHeader03 = 'Операции по управлению и контролю';
sHeader13 = 'Операции по управлению и контролю';
sHeader23 = 'Операции по управлению и контролю';
sSectionType0 = 'ШТАТ';
sSectionType1 = 'НШС';
sSectionType2 = 'ПРИ';
sAutoMessage = 'Автоматическое выполнение';

// точное название операции 'Доклад'
sReportName = 'Доклад';
// точное название операции 'Команда'
sCommandName = 'Команда';
// точное название операции 'Указание оператору'
sMessageName = 'Указание оператору';
// точное название операции 'Диполь-операция'
sDipolOperationName = 'Директива для ПРИС';
// точное название операции 'Директива ИСР'
sISROperationName = 'Директива ИСР';
// сообщения об ошибках при чтении файла
sFileReadErrorMessage0 = 'отсутсвует символ новой строки после символа возврата каретки';
sFileReadErrorMessage1 = 'длина строки превышает максимально допустимую';
// название операции вызова диполь-программы
sProgramCallStament = 'Вызвать';
sEndPartitions = 'Выход из разделов';
sEndPartition = 'Конец раздела';
sEndPartitionsWithSavingProt = 'Закончить разделы с сохранением протокола';
sEndPartitionsWithoutSavingProt = 'Закончить разделы без сохранения протокола';
sEndPartitionWithSavingProt = 'Закончить раздел с сохранением протокола';
sEndPartitionWithoutSavingProt = 'Закончить раздел без сохранения протокола';
sPartitionNotFound = 'Раздел не найден';
sInfoInMainPartition = 'Файл настройки раздела присутствует на диске, при этом информация о разделе прописана в головном файле настройки раздела';
sPartitionSettingFileNotFound = 'Файл настройки раздела';
sPartitionStructureFileNotFound = 'Файл структуры раздела';
sFileNotFound = 'не найден';
sBorrowedCatalogueNotFound = 'Заимствованный каталог с программами (на ЦПКУ)';
sCatalogueNotFound = 'Каталог с программами (на ЦПКУ)';
sCPKUProgramFile = 'Файл программы ЦПКУ';
sCPKUProgramFile2 = 'есть в списке СПО раздела, но не найден на диске';
sCPKUProgramFile3 = 'есть на диске, но отсутствует в списке СПО раздела';
sCheckSpoSuccess = 'Контроль СПО для раздела в соответствии с заданными критериями прошел успешно';
sCheckSpoErrors = 'В процессе проведения контроля СПО для раздела в соответствии с заданными критериями обнаружены ошибки. Количество ошибок';
sCheckSpoErrors2 = 'Подробности смотри в протоколе ПРИСа';
sEndProtocolCheckSPO = 'Конец протокола проведения контроля СПО раздела';
sCantOpenFile = 'Не могу открыть файл';
sError = 'Ошибка';
sStructureOfTheFileIsInfringed = 'Нарушена структура файла';
sInString = 'в строке';
sIncorrectProgramLocation1 = 'При анализе файла структуры выявлена ошибка: некорректно указано размещение программы. Ошибка обнаружена в строке';
sIncorrectProgramLocation2 = 'Указано размещение';
sIncorrectProgramLocation3 = 'Данная строка будет проигнорирована';
sAbnormalBlockCountIsAboveThanMaximum = 'Количество блоков НШС в секции превышает максимально допустимое';
sIncorrectLabel = 'Некорректно расставлены метки блоков НШС. Ошибка в строке';
sCantGoUp = 'Вы не можете перемещаться вверх по секции, так как это запрещено в текущих параметрах настройки';
sOperationWasMade = 'Эта операция уже выполнена. Выполнить ее еще раз?';
sAttention = 'Внимание!';
sTryMakeOperationInconsistently1 = 'Вы хотите выполнить операции непоследовательно. Вы сделали переход вперед и пропустили';
sTryMakeOperationInconsistently2 = 'операций. Вы уверены, что необходимо выполнить текущую операцию?';
sCantMakeOper = 'Операция из данного раздела не может быть выполнена, так как данный раздел не является текущим';
sCommandOper = 'Операция "Команда"';
sCommandOperEnd = 'Конец операции "Команда"';
sReportOper = 'Операция "Доклад"';
sReportOperEnd = 'Конец операции "Доклад"';
sMessageOper = 'Операция "Сообщение оператору"';
sMessageOperEnd = 'Конец операции "Сообщение оператору"';
sPartition = 'раздел';
sSection = 'секция';
sOperation = 'операция';
sInternalDir = 'Внутренняя директива';

//----------------------------------------------------------------------
implementation
begin
end.
