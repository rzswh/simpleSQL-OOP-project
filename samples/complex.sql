CREATE DATABASE OOP;
USE OOP;
CREATE TABLE poi(lkj INT, mnb INT, bvc INT, dfj CHAR, PRIMARY KEY(mnb));
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (1, 2, 3, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (2, 3, 4, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (3, 4, 5, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (4, 5, 6, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (5, 6, 7, "a");
UPDATE poi SET dfj="b" WHERE lkj>3;
SELECT dfj from poi;
UPDATE poi SET dfj="c" WHERE lkj>3 OR mnb>2 AND bvc<7 AND lkj=3 OR mnb=2;
SELECT * into outfile "result" from poi;
SELECT * from poi;
DELETE fROm poi where lkj=5;
select lkj,  dfj FROM poi;
DROP DATABASE OOP;
CREATE database OOP2;
USE OOP2; 
CREATE TABLE poi(lkj INT, mnb INT, bvc INT, dfj CHAR, PRIMARY KEY(mnb));
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (1, 2, 3, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (2, 3, 4, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (3, 4, 5, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (4, 5, 6, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (5, 6, 7, "a");
INSERT INTO poi(mnb, bvc, dfj) VALUES (7, 7, "a");
DELETE FROM poi  WHERE lkj>3 OR 7=mnb AND lkj=10;
SELECT * from poi;
CREATE TABLE poi2(lkj DOUBLE, mnb INT, bvc INT, dfj CHAR, PRIMARY KEY(bvc));
INSERT INTO poi2(lkj, bvc, dfj) VALUES (11, 3, "a");
INSERT INTO poi2 ( lkj , mnb ,bvc ,dfj) VALUES (12, 3, 4, "a")  ;
insert INTO poi2 (lkj, mnb, bvc, dfj) VALUES (13, 4, 5, "a");
INSERT INTO poi2(lkj, mnb, bvc, dfj) VALUES (14, 5, 6, "a");
SELECT * from poi2;
DELETE FROM poi2 WHERE 4.9>mnb;
SELECT * from poi2 where "a"=dfj;
SELECT * from poi2 where NULL>dfj;
SELECT * from poi2 where 'b'>dfj into outfile 'withnull' ;
DROP DATABASE OOP2;
