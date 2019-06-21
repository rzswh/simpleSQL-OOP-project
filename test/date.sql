CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(oop_date DATE NOT NULL, PRIMARY KEY(oop_date));
SHOW columns from oop_info;
INSERT INTO oop_info(oop_date) VALUES ('1998-09-05');
select * from oop_info;
DROP DATABASE OOP;