CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, stu_name CHAR, PRIMARY KEY(stu_id));
SHOW COLUMNS from oop_info;
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011343, "a");
select * from oop_info;
UPDATE oop_info SET stu_name='b' WHERE stu_id=2018011343;
select * from oop_info;
DELETE FROM oop_info WHERE stu_id=2018011343;
select * from oop_info;
DROP DATABASE OOP;
