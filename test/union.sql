CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, PRIMARY KEY(stu_id), stu_name CHAR);
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011243, "a");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011344, "b");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011445, "c");
CREATE TABLE oop_info1(stu_id INT NOT NULL, PRIMARY KEY(stu_id), stu_name CHAR);
INSERT INTO oop_info1(stu_id, stu_name) VALUES (2018011243, "a");
INSERT INTO oop_info1(stu_id, stu_name) VALUES (2018011777, "b");
INSERT INTO oop_info1(stu_id, stu_name) VALUES (2018011666, "c");
SELECT stu_id, stu_name from oop_info union SELECT stu_id, stu_name from oop_info1;
SELECT stu_name from oop_info union SELECT stu_name from oop_info1;
SELECT stu_id from oop_info union all SELECT stu_id from oop_info1;
DROP DATABASE OOP;
