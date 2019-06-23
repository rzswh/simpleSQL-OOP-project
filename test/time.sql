SELECT ADDTIME("20:34:21", "2:10:5");
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(oop_time TIME NOT NULL, PRIMARY KEY(oop_time));
SHOW columns from oop_info;
INSERT INTO oop_info(oop_time) VALUES ('09:34:21');
INSERT INTO oop_info(oop_time) VALUES ('20:54:20');
select * from oop_info;
DROP DATABASE OOP;


