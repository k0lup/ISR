# ISR Qt 5.15 migration

Текущий статус переноса ISR на C++/Qt 5.15.

## Перенесено

- Отдельная Qt-сборка через CMake (`Qt5::Widgets`, `Qt5::Network`).
- Точка входа приложения и главное окно.
- Пакетный заголовок `POR` в бинарно-совместимом packed-формате.
- Сетевой клиент `NetPrisClient` на `QTcpSocket` для портов `0x4020` и `0x4024`.
- Выделенный `por_codec` для безопасного little-endian кодирования/декодирования заголовка `POR`.
- Расширенная маршрутизация входящих пакетов в `ProtocolProcessor` по ключевым сортам из legacy `ProcessInPortion` (`2/3/4/5/6/7/8/9/10`) и событию установки заголовка (`Rod=2, Sort=1, AtomType=6`), включая перенос post-result шага логирования реквизитов протокола.
- Менеджер диалогов `DialogManager` с предсозданием всех основных окон ISR и вызовом по ключу.
- Единый список ключей диалогов теперь задается в `DialogManager` и переиспользуется в меню.
- Каркас основных ISR-окон/диалогов (About, CheckSPO, End, List, Find, Request, SP, Stencil, Title, Variant, отчеты и т.д.) и подключение их к меню.
- Для `Find/Variant/Request/Title` вынесены отдельные Qt-диалоги с поведением, приближенным к legacy (`SetOKBtnEnabled`, выбор варианта, тип операции запроса, поля заголовка раздела).
- Добавлены специализированные `Settings/End/SP` диалоги с переносом ключевых сценариев и флагов состояния.
- Добавлен `CheckSpoDialog` с переносом базовой логики выбора проверок и файловых валидаций (adjustment/structure/CPKU/SSBVS).
- Добавлены `PartitionListDialog` и `StencilDialog` с переносом базовых сценариев выбора раздела/поиска и сохранения трафарета.
- Валидация трафарета вынесена в отдельный `stencil_rules` модуль и переиспользуется в диалоге.
- Добавлен `CloseReportDialog` для закрытия выбранного отчета из списка открытых отчетов.
- Добавлены `PartitionReportDialog` и `SelectionReportDialog` для загрузки отчетов по разделу и по выборке.
- Логика распределения/очистки слотов отчетов вынесена в `report_slots` и используется в загрузке/закрытии отчетов.
- Добавлены `AboutDialog` (включая автозакрытие по таймеру) и `CompletingDialog` с переносом базового сценария подтверждения.
- Добавлен `ChildWindowDialog` с табличным представлением строк секций/команд (первый перенос MDI-контента).
- Добавлено согласованное состояние `currentProtocolName` для `Title/SP/End` через общий модуль именования протокола.

## Структура

- `qt/src/mainwindow.*` — центральное окно, меню и связывание компонентов.
- `qt/src/net_pris_client.*` — транспортный слой обмена по TCP.
- `qt/src/core/protocol_processor.*` — прикладная обработка порций.
- `qt/src/core/protocol_events.h` — константы событий/сортов протокола ISR.
- `qt/src/core/por_codec.*` — сериализация/десериализация заголовка `POR`.
- `qt/src/core/protocol_naming.*` — единые правила формирования имени протокола и номера деления.
- `qt/src/core/report_slots.*` — централизованное управление 3 слотами открытых отчетов.
- `qt/src/core/stencil_rules.*` — централизованные правила валидации трафарета (legacy-совместимые ограничения).
- `qt/src/core/dialog_manager.*` — жизненный цикл/реестр диалогов ISR.
- `qt/src/dialogs/*` — Qt-диалоги (в т.ч. специализированные `find_dialog`, `variant_dialog`, `request_dialog`, `title_dialog`, `settings_dialog`, `end_dialog`, `sp_dialog`, `checkspo_dialog`, `partition_list_dialog`, `stencil_dialog`, `close_report_dialog`, `partition_report_dialog`, `selection_report_dialog`, `about_dialog`, `completing_dialog`, `child_window_dialog`) и каркас форм VCL.

## Следующий этап

- Перенос полного содержимого каждой VCL-формы (`*.dfm`, `*.cpp`) в полноценные Qt-формы (в т.ч. завершение логики `ChildWindow`).
- Пошаговый перенос всей бизнес-логики из `Source/Main.cpp`, `Common*.cpp`, `ChildWin.cpp` и связанных модулей.
- Замена Word/OLE интеграции на Qt-совместимый механизм.
