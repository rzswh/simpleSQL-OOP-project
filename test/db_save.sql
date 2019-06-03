CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, stu_name CHAR, PRIMARY KEY(stu_id));
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011343, "a");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011344, "b");
INSERT INTO oop_info(stu_id, stu_name) VALUES (2018011345, "c");
SELECT * INTO OUTFILE 'output_file' FROM oop_info;
DROP DATABASE OOP;
