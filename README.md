# Лабораторна робота 8. Робота з БД через ORM

Варіант 22: Система обліку результатів голосування (Кандидат / Голос).
Спрощена навчальна модель обліку голосів за кандидатами, без ідентифікації виборців.

Студент: Яроменко Владислав, група ФІТ 3-11.

## Предметна область

Основна сутність Candidate (кандидат) має кілька записів Vote: кожен запис фіксує кількість голосів (ballots), облікованих за кандидата на виборчій дільниці в певну дату і через певний канал (online або paper).

## Структура БД

Таблиця Candidate (основна):

| Поле | Тип | Опис |
|------|-----|------|
| id | INTEGER, PRIMARY KEY AUTOINCREMENT | ідентифікатор |
| fullName | TEXT NOT NULL | ПІБ кандидата |
| party | TEXT NOT NULL | партія |
| region | TEXT NOT NULL | регіон |
| status | TEXT NOT NULL | registered / withdrawn |

Таблиця Vote (залежна):

| Поле | Тип | Опис |
|------|-----|------|
| id | INTEGER, PRIMARY KEY AUTOINCREMENT | ідентифікатор |
| candidateId | INTEGER NOT NULL, FOREIGN KEY -> Candidate.id | кандидат |
| castDate | TEXT NOT NULL | дата |
| pollingStation | TEXT NOT NULL | виборча дільниця |
| channel | TEXT NOT NULL | online / paper |
| ballots | INTEGER NOT NULL | кількість голосів (числове поле для агрегатів) |

Зв'язок 1:N:

```
Candidate (1) ----< (N) Vote
Candidate.id  <----  Vote.candidateId
```

## Структура проєкту

- vcpkg.json, CMakeLists.txt, CMakePresets.json: збірка (залежності sqlite3, sqlite-orm)
- include/Candidate.h, Vote.h: моделі
- include/Database.h: ORM-мапінг (make_storage, make_table, foreign_key), sync_schema, PRAGMA foreign_keys = ON
- include/ICandidateRepository.h, SQLiteCandidateRepository.h, src/SQLiteCandidateRepository.cpp: Repository
- include/VoteDao.h, src/VoteDao.cpp: доступ до залежної сутності
- include/RawSqlite.h, src/RawSqlite.cpp: той самий доступ через SQLite C API
- src/ComparisonDemo.cpp, RelationshipDemo.cpp, RepositoryDemo.cpp, QueryDemos.cpp, TransactionDemos.cpp, TestData.cpp, main.cpp: демонстрації

## Збірка і запуск (Windows, x64 Native Tools Command Prompt for VS)

```
set VCPKG_ROOT=C:\vcpkg
cmake --preset default
cmake --build build --config Release
build\Release\VotingORM.exe
```

Демо-бази (*.db) створюються поруч із місцем запуску і видаляються на початку кожного сценарію, тому повторний запуск дає той самий результат.

## Порівняння SQLite C API та sqlite_orm

| Критерій | SQLite C API | sqlite_orm |
|----------|--------------|------------|
| Обсяг коду (додати + знайти за id) | близько 110 рядків (RawSqlite.cpp) | 2 виклики: insert, get_optional |
| Перетворення даних | вручну: bind_*, column_* | автоматично за мапінгом |
| Керування ресурсами | sqlite3_finalize, sqlite3_close (потрібні RAII-обгортки) | керує бібліотека |
| Обробка помилок | перевірка кожного коду повернення | винятки std::system_error |
| Підтримка | зміна полів змінює SQL і прив'язки у кількох місцях | зміна в одному місці мапінгу |

## Результат виконання

```

=== 1. SQLITE C API vs sqlite_orm ===

[SQLite C API]
Inserted ID: 1
Found: 1 | Oksana Tkachenko | Unity | Vinnytsia | registered

[sqlite_orm]
Inserted ID: 1
Found: 1 | Oksana Tkachenko | Unity | Vinnytsia | registered

=== 2. ORM MAPPING AND ONE-TO-MANY RELATIONSHIP ===

Tables created by sync_schema():
Candidate exists: true
  id INTEGER PRIMARY KEY
  fullName TEXT
  party TEXT
  region TEXT
  status TEXT
Vote exists: true
  id INTEGER PRIMARY KEY
  candidateId INTEGER
  castDate TEXT
  pollingStation TEXT
  channel TEXT
  ballots INTEGER

Candidate ID: 1

Vote records of Candidate 1:
1 | 1 | 2026-11-01 | PS-901 | online | 25
2 | 1 | 2026-11-02 | PS-902 | paper | 40

Foreign key constraint test:
Insertion rejected: FOREIGN KEY constraint failed: constraint failed
Vote records after the rejected insertion: 2

=== 3. REPOSITORY (CRUD) ===

CREATE
Inserted ID: 1

READ
1 | Test Candidate | Independent | Zhytomyr | registered

UPDATE
Updated: true
New status: withdrawn

READ ALL
1 | Test Candidate | Independent | Zhytomyr | withdrawn

DELETE
Removed: true
Record exists after deletion: false

MISSING RECORD (ID 9999)
findById: not found
update: false
remove: false

=== TEST DATA ===
Test database prepared: voting_queries.db
Candidate records: 5
Vote records: 15

=== 4. QUERIES: FILTERING, SORTING, LIMIT ===

4.1 Candidate: status = 'registered':
1 | Olena Kovalenko | Unity | Kyiv | registered
2 | Andriy Melnyk | Unity | Lviv | registered
3 | Iryna Shevchenko | Greens | Odesa | registered
4 | Taras Bondar | Civic Union | Kharkiv | registered

4.2 Vote: channel = 'online' AND ballots >= 100:
1 | 1 | 2026-11-01 | PS-101 | online | 120
3 | 1 | 2026-11-02 | PS-101 | online | 240
5 | 2 | 2026-11-02 | PS-201 | online | 150
7 | 3 | 2026-11-01 | PS-301 | online | 300
9 | 3 | 2026-11-03 | PS-301 | online | 180

4.3 Candidate sorted by fullName:
2 | Andriy Melnyk | Unity | Lviv | registered
3 | Iryna Shevchenko | Greens | Odesa | registered
5 | Maria Lysenko | Greens | Dnipro | withdrawn
1 | Olena Kovalenko | Unity | Kyiv | registered
4 | Taras Bondar | Civic Union | Kharkiv | registered

4.4 Top 3 Vote records by ballots:
7 | 3 | 2026-11-01 | PS-301 | online | 300
3 | 1 | 2026-11-02 | PS-101 | online | 240
9 | 3 | 2026-11-03 | PS-301 | online | 180

4.5 Candidate: party = 'Unity', sorted by fullName:
2 | Andriy Melnyk | Unity | Lviv | registered
1 | Olena Kovalenko | Unity | Kyiv | registered

=== 5. AGGREGATE QUERIES ===

5.1 COUNT: 15
5.2 SUM(ballots): 1600
5.3 AVG(ballots): 106.667
5.4 MAX(ballots): 300

5.5 GROUP BY candidateId: records and SUM(ballots):
Candidate ID: 1 | records: 3 | total ballots: 445
Candidate ID: 2 | records: 3 | total ballots: 305
Candidate ID: 3 | records: 3 | total ballots: 525
Candidate ID: 4 | records: 3 | total ballots: 235
Candidate ID: 5 | records: 3 | total ballots: 90

5.6 GROUP BY channel: records and AVG(ballots):
online | records: 9 | average ballots: 133.889
paper | records: 6 | average ballots: 65.8333

=== 6. JOIN QUERIES ===

6.1 Candidate + Vote (INNER JOIN):
Olena Kovalenko | 2026-11-01 | PS-101 | 120
Olena Kovalenko | 2026-11-01 | PS-102 | 85
Olena Kovalenko | 2026-11-02 | PS-101 | 240
Andriy Melnyk | 2026-11-01 | PS-201 | 60
Andriy Melnyk | 2026-11-02 | PS-201 | 150
Andriy Melnyk | 2026-11-03 | PS-202 | 95
Iryna Shevchenko | 2026-11-01 | PS-301 | 300
Iryna Shevchenko | 2026-11-02 | PS-302 | 45
Iryna Shevchenko | 2026-11-03 | PS-301 | 180
Taras Bondar | 2026-11-01 | PS-401 | 75
Taras Bondar | 2026-11-02 | PS-401 | 110
Taras Bondar | 2026-11-03 | PS-402 | 50
Maria Lysenko | 2026-11-01 | PS-501 | 30
Maria Lysenko | 2026-11-02 | PS-502 | 20
Maria Lysenko | 2026-11-03 | PS-501 | 40

6.2 INNER JOIN with filter ballots > 100:
Iryna Shevchenko | PS-301 | 300
Olena Kovalenko | PS-101 | 240
Iryna Shevchenko | PS-301 | 180
Andriy Melnyk | PS-201 | 150
Olena Kovalenko | PS-101 | 120
Taras Bondar | PS-401 | 110

6.3 Total ballots of every Candidate (JOIN + GROUP BY):
Olena Kovalenko | 445
Andriy Melnyk | 305
Iryna Shevchenko | 525
Taras Bondar | 235
Maria Lysenko | 90

=== 7. TRANSACTION: COMMIT ===

Before transaction:
Candidate: 5
Vote: 15

Candidate inserted inside transaction, ID: 6
Vote inserted inside transaction

Transaction committed: true

After transaction:
Candidate: 6
Vote: 16
Candidate 'Petro Hrytsenko' exists: true

=== 8. TRANSACTION: ROLLBACK ===

Before transaction:
Candidate: 6
Vote: 16

Candidate inserted inside transaction, ID: 7

Transaction failed: FOREIGN KEY constraint failed: constraint failed
Changes rolled back

After transaction:
Candidate: 6
Vote: 16
Candidate 'Dmytro Sydorenko' exists: false

Transaction returned false, committed: false
Candidate 'Anna Moroz' exists: false

After rollback requested by the function:
Candidate: 6
Vote: 16

=== 9. THE SAME OPERATIONS WITHOUT A TRANSACTION ===

Before operations:
Candidate: 0
Vote: 0

Candidate inserted, ID: 1

Operation failed: FOREIGN KEY constraint failed: constraint failed

After operations:
Candidate: 1
Vote: 0
Candidate 'Viktor Kravchuk' exists: true (the first operation was not rolled back)
```
