CREATE DATABASE OOP;
USE OOP;
CREATE TABLE oop_info(stu_id INT NOT NULL, stu_name CHAR, PRIMARY KEY(stu_id));
LOAD DATA INFILE '1_in_file' INTO TABLE oop_info(stu_id, stu_name);
SELECT * FROM oop_info;
DROP DATABASE OOP;
