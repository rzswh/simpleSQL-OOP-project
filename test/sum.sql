CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, PRIMARY KEY(stu_id), grade INT);
INSERT INTO oop_info(stu_id, grade) VALUES (2018011243, 98);
INSERT INTO oop_info(stu_id, grade) VALUES (2018011344, 77);
INSERT INTO oop_info(stu_id, grade) VALUES (2018011445, 55);
INSERT INTO oop_info(stu_id, grade) VALUES (2018011449, 62);
SELECT SUM(grade) from oop_info;
DROP DATABASE OOP;
